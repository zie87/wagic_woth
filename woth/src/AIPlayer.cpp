#include "PrecompiledHeader.h"

#include "AIPlayer.h"

#include <utility>
#include <algorithm>

#include "GameStateDuel.h"
#include "DeckManager.h"
#include "CardSelector.h"

// Instances for Factory
#include "AIPlayerBaka.h"

#ifdef AI_CHANGE_TESTING
#include "AIPlayerBakaB.h"
#endif

int AIPlayer::totalAIDecks = -1;

const char* const MTG_LAND_TEXTS[] = {"artifact", "forest", "island", "mountain", "swamp", "plains", "other lands"};

AIAction::AIAction(AIPlayer* owner, MTGCardInstance* c, MTGCardInstance* t)
    : owner(owner)
    , ability(nullptr)
    , player(nullptr)
    , click(c)
    , target(t) {
    // useability tweak - assume that the user is probably going to want to see the full res card,
    // so prefetch it. The idea is that we do it here as we want to start the prefetch before it's time to render,
    // and waiting for it to actually go into play is too late, as we start drawing the card during the interrupt
    // window. This is a good intercept point, as the AI has committed to using this card.

    // if we're not in text mode, always get the thumb
    if (owner->getObserver()->getCardSelector()->GetDrawMode() != DrawMode::kText) {
        // DebugTrace("Prefetching AI card going into play: " << c->getImageName());
        if (owner->getObserver()->getResourceManager()) {
            owner->getObserver()->getResourceManager()->RetrieveCard(c, RETRIEVE_THUMB);
        }

        // also cache the large image if we're using kNormal mode
        if (owner->getObserver()->getCardSelector()->GetDrawMode() == DrawMode::kNormal) {
            if (owner->getObserver()->getResourceManager()) {
                owner->getObserver()->getResourceManager()->RetrieveCard(c);
            }
        }
    }
}

int AIAction::Act() {
    GameObserver* g = owner->getObserver();
    if (player && !playerAbilityTarget) {
        g->cardClick(nullptr, player);
        return 1;
    }
    if (ability) {
        g->cardClick(click, ability);
        if (target && mAbilityTargets.empty()) {
            g->cardClick(target);
            return 1;
        }
        if (playerAbilityTarget && mAbilityTargets.empty()) {
            g->cardClick(nullptr, dynamic_cast<Player*>(playerAbilityTarget));
            return 1;
        }
        if (!mAbilityTargets.empty()) {
            return clickMultiAct(mAbilityTargets);
        }
    } else if (!mAbilityTargets.empty()) {
        return clickMultiAct(mAbilityTargets);
    } else if (click) {  // Shouldn't be used, really...
        g->cardClick(click, click);
        if (target) {
            g->cardClick(target);
        }
        return 1;
    }
    return 0;
}

int AIAction::clickMultiAct(vector<Targetable*>& actionTargets) const {
    GameObserver* g   = owner->getObserver();
    TargetChooser* tc = g->getCurrentTargetChooser();
    if (!tc) {
        return 0;
    }
    auto ite = actionTargets.begin();
    while (ite != actionTargets.end()) {
        MTGCardInstance* card = (dynamic_cast<MTGCardInstance*>(*ite));
        if (card == (MTGCardInstance*)tc->source)  // click source first.
        {
            g->cardClick(card);
            ite = actionTargets.erase(ite);
            continue;
        }
        ite++;
    }

    // shuffle to make it less predictable, otherwise ai will always seem to target from right to left. making it very
    // obvious.
    owner->getRandomGenerator()->random_shuffle(actionTargets.begin(), actionTargets.end());

    for (int k = 0; k < int(actionTargets.size()) && k < tc->maxtargets; k++) {
        if (auto* card = dynamic_cast<MTGCardInstance*>(actionTargets[k])) {
            if (k + 1 == int(actionTargets.size())) {
                tc->done = true;
            }
            g->cardClick(card);
        }
    }
    tc->attemptsToFill++;
    return 1;
}

AIPlayer::AIPlayer(GameObserver* observer, const string& file, string fileSmall, MTGDeck* deck)
    : Player(observer, std::move(file), std::move(fileSmall), deck)
    , agressivity(50)
    , forceBestAbilityUse(false)
    , mFastTimerMode(false) {
    playMode = Player::MODE_AI;
}

AIPlayer::~AIPlayer() {
    while (!clickstream.empty()) {
        AIAction* action = clickstream.front();
        SAFE_DELETE(action);
        clickstream.pop();
    }
}

int AIPlayer::Act(float dt) {
    if (observer->currentPlayer == this) {
        observer->userRequestNextGamePhase();
    }
    return 1;
}

int AIPlayer::clickMultiTarget(TargetChooser* tc, vector<Targetable*>& potentialTargets) {
    auto ite = potentialTargets.begin();
    while (ite != potentialTargets.end()) {
        auto* card = dynamic_cast<MTGCardInstance*>(*ite);
        if (card && card == tc->source)  // if the source is part of the targetting deal with it first. second click is
                                         // "confirming click".
        {
            clickstream.push(NEW AIAction(this, card));
            DebugTrace("Ai clicked source as a target: " << (card ? card->name : "None") << std::endl);
            ite = potentialTargets.erase(ite);
            continue;
        }
        if (auto* pTarget = dynamic_cast<Player*>(*ite)) {
            clickstream.push(NEW AIAction(this, pTarget));
            DebugTrace("Ai clicked Player as a target");
            ite = potentialTargets.erase(ite);
            continue;
        }
        ite++;
    }

    randomGenerator.random_shuffle(potentialTargets.begin(), potentialTargets.end());
    if (!potentialTargets.empty()) {
        clickstream.push(NEW AIAction(this, nullptr, tc->source, potentialTargets));
    }
    while (!clickstream.empty()) {
        AIAction* action = clickstream.front();
        action->Act();
        SAFE_DELETE(action);
        clickstream.pop();
    }
    return 1;
}

int AIPlayer::clickSingleTarget(TargetChooser* tc, vector<Targetable*>& potentialTargets, MTGCardInstance* chosenCard) {
    const int i = randomGenerator.random() % potentialTargets.size();

    if (auto* card = dynamic_cast<MTGCardInstance*>(potentialTargets[i])) {
        if (!chosenCard) {
            clickstream.push(NEW AIAction(this, card));
        }
    } else if (auto* player = dynamic_cast<Player*>(potentialTargets[i])) {
        clickstream.push(NEW AIAction(this, player));
    }

    return 1;
}

AIPlayer* AIPlayerFactory::createAIPlayer(GameObserver* observer,
                                          MTGAllCards* collection,
                                          Player* opponent,
                                          int deckid) {
    char deckFile[512];
    string avatarFilename;  // default imagename
    char deckFileSmall[512];

    if (deckid == GameStateDuel::MENUITEM_EVIL_TWIN) {  // Evil twin
        sprintf(deckFile, "%s", opponent->deckFile.c_str());
        DebugTrace("Evil Twin => " << opponent->deckFile);
        avatarFilename = "avatar.jpg";
        sprintf(deckFileSmall, "%s", "ai_baka_eviltwin");
    } else {
        if (!deckid) {
            // random deck
            const int nbdecks = std::min(AIPlayer::getTotalAIDecks(), options[Options::AIDECKS_UNLOCKED].number);
            if (!nbdecks) {
                return nullptr;
            }
            deckid = 1 + WRand() % (nbdecks);
        }
        sprintf(deckFile, "ai/baka/deck%i.txt", deckid);
        DeckMetaData* aiMeta = observer->getDeckManager()->getDeckMetaDataByFilename(deckFile, true);
        avatarFilename       = aiMeta->getAvatarFilename();
        sprintf(deckFileSmall, "ai_baka_deck%i", deckid);
    }

    int deckSetting = EASY;
    if (opponent) {
        const bool isOpponentAI = opponent->isAI() == 1;
        DeckMetaData* meta = observer->getDeckManager()->getDeckMetaDataByFilename(opponent->deckFile, isOpponentAI);
        if (meta && meta->getVictoryPercentage() >= 65) {
            deckSetting = HARD;
        }
    }

    // AIPlayerBaka will delete MTGDeck when it's time
    auto* baka   = NEW AIPlayerBaka(observer, deckFile, deckFileSmall, avatarFilename,
                                    NEW MTGDeck(deckFile, collection, 0, deckSetting));
    baka->deckId = deckid;
    return baka;
}

int AIPlayer::receiveEvent(WEvent* event) { return 0; }

void AIPlayer::Render() {}

bool AIPlayer::parseLine(const string& s) {
    size_t limiter = s.find('=');
    if (limiter == string::npos) {
        limiter = s.find(':');
    }
    string areaS;
    if (limiter != string::npos) {
        areaS = s.substr(0, limiter);
        if (areaS == "rvalues") {
            randomGenerator.loadRandValues(s.substr(limiter + 1));
            return true;
        }
    }

    return Player::parseLine(s);
}

#ifdef AI_CHANGE_TESTING
AIPlayer* AIPlayerFactory::createAIPlayerTest(GameObserver* observer,
                                              MTGAllCards* collection,
                                              Player* opponent,
                                              string _folder) {
    char deckFile[512];
    string avatarFilename;  // default imagename
    char deckFileSmall[512];

    string folder = _folder.size() ? _folder : "ai/baka/";

    int deckid = 0;

    // random deck
    int nbdecks = 0;
    int found   = 1;
    while (found && nbdecks < options[Options::AIDECKS_UNLOCKED].number) {
        found = 0;
        char buffer[512];
        sprintf(buffer, "%sdeck%i.txt", folder.c_str(), nbdecks + 1);
        if (FileExists(buffer)) {
            found = 1;
            nbdecks++;
        }
    }
    if (!nbdecks) {
        if (_folder.size()) return createAIPlayerTest(observer, collection, opponent, "");
        return NULL;
    }
    deckid = 1 + WRand() % (nbdecks);

    sprintf(deckFile, "%sdeck%i.txt", folder.c_str(), deckid);
    DeckMetaData* aiMeta = observer->getDeckManager()->getDeckMetaDataByFilename(deckFile, true);
    avatarFilename       = aiMeta->getAvatarFilename();
    sprintf(deckFileSmall, "ai_baka_deck%i", deckid);

    int deckSetting = EASY;
    if (opponent) {
        bool isOpponentAI  = opponent->isAI() == 1;
        DeckMetaData* meta = observer->getDeckManager()->getDeckMetaDataByFilename(opponent->deckFile, isOpponentAI);
        if (meta->getVictoryPercentage() >= 65) deckSetting = HARD;
    }

    // AIPlayerBaka will delete MTGDeck when it's time
    AIPlayerBaka* baka = opponent ? NEW AIPlayerBakaB(observer, deckFile, deckFileSmall, avatarFilename,
                                                      NEW MTGDeck(deckFile, collection, 0, deckSetting))
                                  : NEW AIPlayerBaka(observer, deckFile, deckFileSmall, avatarFilename,
                                                     NEW MTGDeck(deckFile, collection, 0, deckSetting));
    baka->deckId       = deckid;
    baka->setObserver(observer);
    return baka;
}
#endif

int AIPlayer::getTotalAIDecks() {
    if (totalAIDecks == -1) {
        totalAIDecks = countTotalDecks(0, 0, 1);
    }
    return totalAIDecks;
}

int AIPlayer::countTotalDecks(int lower, int higher, int current) {
    char buffer[512];
    sprintf(buffer, "ai/baka/deck%i.txt", current);
    if (JFileSystem::GetInstance()->FileExists(buffer)) {
        if (higher == current + 1) {
            return current;
        }
        const int newlower   = current;
        const int newcurrent = higher ? (higher + newlower) / 2 : current * 2;
        return countTotalDecks(newlower, higher, newcurrent);
    }
    if (!lower) {
        return 0;
    }
    if (lower == current - 1) {
        return lower;
    }

    const int newhigher  = current;
    const int newcurrent = (lower + newhigher) / 2;
    return countTotalDecks(lower, newhigher, newcurrent);
}

void AIPlayer::invalidateTotalAIDecks() { totalAIDecks = -1; }
