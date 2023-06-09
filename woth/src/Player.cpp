#include "PrecompiledHeader.h"

#include "Player.h"

#include <utility>
#include "GameObserver.h"
#include "DeckStats.h"
#include "ManaCost.h"
#include "DeckMetaData.h"
#include "DeckManager.h"

#ifdef TESTSUITE
#include "TestSuiteAI.h"
#endif

Player::Player(GameObserver* observer, const string& file, string fileSmall, MTGDeck* deck)
    : Damageable(observer, 20)
    , offerInterruptOnPhase(MTG_PHASE_DRAW) {
    if (deck == nullptr && file != "testsuite" && file != "remote" && !file.empty()) {
        deck = NEW MTGDeck(file.c_str(), MTGCollection());
    }

    premade            = false;
    game               = nullptr;
    deckFile           = file;
    deckFileSmall      = std::move(fileSmall);
    handsize           = 0;
    manaPool           = NEW ManaPool(this);
    nomaxhandsize      = false;
    poisonCount        = 0;
    damageCount        = 0;
    preventable        = 0;
    mAvatarTex         = nullptr;
    type_as_damageable = DAMAGEABLE_PLAYER;
    playMode           = MODE_HUMAN;
    skippingTurn       = 0;
    extraTurn          = 0;
    if (deck != nullptr) {
        game = NEW MTGPlayerCards(deck);
        // This automatically sets the observer pointer on all the deck cards
        game->setOwner(this);
        deckName = deck->meta_name;
    } else {
        game = new MTGPlayerCards();
        game->setOwner(this);
    }
    mDeck = deck;
}

void Player::setObserver(GameObserver* g) {
    observer = g;
    // fix card instances direct pointer
    game->setOwner(this);
}

/*Method to call at the end of a game, before all objects involved in the game are destroyed */
void Player::End() { DeckStats::GetInstance()->saveStats(this, opponent(), observer); }

Player::~Player() {
    SAFE_DELETE(manaPool);
    SAFE_DELETE(game);
    if (mAvatarTex && observer->getResourceManager()) {
        observer->getResourceManager()->Release(mAvatarTex);
    }
    mAvatarTex = nullptr;
    SAFE_DELETE(mDeck);
}

bool Player::loadAvatar(const string& file, string resName) {
    WResourceManager* rm = observer->getResourceManager();
    if (!rm) {
        return false;
    }

    if (mAvatarTex) {
        rm->Release(mAvatarTex);
        mAvatarTex = nullptr;
    }
    mAvatarTex = rm->RetrieveTexture(file, RETRIEVE_LOCK, TEXTURE_SUB_AVATAR);
    if (mAvatarTex) {
        mAvatar = rm->RetrieveQuad(file, 0, 0, 35, 50, std::move(resName), RETRIEVE_NORMAL, TEXTURE_SUB_AVATAR);
        return true;
    }

    return false;
}

const string Player::getDisplayName() const {
    if (this == observer->players[0]) {
        return "Player 1";
    }
    return "Player 2";
}

MTGInPlay* Player::inPlay() const { return game->inPlay; }

int Player::getId() {
    for (int i = 0; i < 2; i++) {
        if (observer->players[i] == this) {
            return i;
        }
    }
    return -1;
}

JQuadPtr Player::getIcon() {
    if (!mAvatarTex) {
        loadAvatar(mAvatarName);
    }

    return mAvatar;
}

Player* Player::opponent() {
    if (!observer || (observer->players.size() < 2)) {
        return nullptr;
    }
    return this == observer->players[0] ? observer->players[1] : observer->players[0];
}

HumanPlayer::HumanPlayer(GameObserver* observer, const string& file, string fileSmall, bool isPremade, MTGDeck* deck)
    : Player(observer, std::move(file), std::move(fileSmall), deck) {
    mAvatarName = "avatar.jpg";
    playMode    = MODE_HUMAN;
    premade     = isPremade;
}

ManaPool* Player::getManaPool() { return manaPool; }

int Player::gainOrLoseLife(int value) {
    if (!value) {
        return 0;  // Don't do anything if there's no actual life change
    }

    thatmuch = abs(value);  // the value that much is a variable to be used with triggered abilities.
    // ie:when ever you gain life, draw that many cards. when used in a trigger draw:thatmuch, will return the value
    // that the triggered event stored in the card for "that much".
    life += value;
    if (value < 0) {
        lifeLostThisTurn += abs(value);
    }

    // Send life event to listeners
    WEvent* lifed = NEW WEventLife(this, value);
    observer->receiveEvent(lifed);

    return value;
}

int Player::gainLife(int value) {
    if (value < 0) {
        DebugTrace("PLAYER.CPP: don't call gainLife on a negative value, use loseLife instead");
        return 0;
    }
    return gainOrLoseLife(value);
}

int Player::loseLife(int value) {
    if (value < 0) {
        DebugTrace("PLAYER.CPP: don't call loseLife on a negative value, use gainLife instead");
        return 0;
    }
    return gainOrLoseLife(-value);
}

int Player::afterDamage() { return life; }

int Player::poisoned() { return poisonCount; }

int Player::damaged() { return damageCount; }

int Player::prevented() { return preventable; }

void Player::takeMulligan() const {
    MTGPlayerCards* currentPlayerZones = game;
    const int cardsinhand              = currentPlayerZones->hand->nb_cards;
    for (int i = 0; i < cardsinhand; i++) {  // Discard hand
        currentPlayerZones->putInZone(currentPlayerZones->hand->cards[0], currentPlayerZones->hand,
                                      currentPlayerZones->library);
    }

    currentPlayerZones->library->shuffle();  // Shuffle

    for (int i = 0; i < (cardsinhand - 1); i++) {
        game->drawFromLibrary();
    }
    // Draw hand with 1 less card penalty //almhum
}

// Cleanup phase at the end of a turn
void Player::cleanupPhase() const {
    game->inPlay->cleanupPhase();
    game->graveyard->cleanupPhase();
}

std::string Player::GetCurrentDeckStatsFile() const {
    std::ostringstream filename;
    filename << "stats/" << deckFileSmall << ".txt";
    return options.profileFile(filename.str());
}

bool Player::parseLine(const string& s) {
    if (((Damageable*)this)->parseLine(s)) {
        return true;
    }

    size_t limiter = s.find('=');
    if (limiter == string::npos) {
        limiter = s.find(':');
    }
    string areaS;
    if (limiter != string::npos) {
        areaS = s.substr(0, limiter);
        if (areaS == "manapool") {
            SAFE_DELETE(manaPool);
            manaPool = new ManaPool(this);
            ManaCost::parseManaCost(s.substr(limiter + 1), manaPool);
            return true;
        }
        if (areaS == "avatar") {
            mAvatarName = s.substr(limiter + 1);
            loadAvatar(mAvatarName, "bakaAvatar");
            return true;
        }
        if (areaS == "customphasering") {
            phaseRing = s.substr(limiter + 1);
            return true;
        }
        if (areaS == "premade") {
            premade = (atoi(s.substr(limiter + 1).c_str()) == 1);
            return true;
        }
        if (areaS == "deckfile") {
            deckFile = s.substr(limiter + 1);
            if (playMode == Player::MODE_AI) {
                sscanf(deckFile.c_str(), "ai/baka/deck%i.txt", &deckId);

                int deckSetting = EASY;
                if (opponent()) {
                    const bool isOpponentAI = opponent()->isAI() == 1;
                    DeckMetaData* meta =
                        observer->getDeckManager()->getDeckMetaDataByFilename(opponent()->deckFile, isOpponentAI);
                    if (meta && meta->getVictoryPercentage() >= 65) {
                        deckSetting = HARD;
                    }
                }

                SAFE_DELETE(mDeck);
                SAFE_DELETE(game);
                mDeck = NEW MTGDeck(deckFile.c_str(), MTGCollection(), 0, deckSetting);
                game  = NEW MTGPlayerCards(mDeck);
                // This automatically sets the observer pointer on all the deck cards
                game->setOwner(this);
                deckName = mDeck->meta_name;
            }
            return true;
        }
        if (areaS == "deckfilesmall") {
            deckFileSmall = s.substr(limiter + 1);
            return true;
        }
        if (areaS == "offerinterruptonphase") {
            for (int i = 0; i < NB_MTG_PHASES; i++) {
                const string phaseStr = Constants::MTGPhaseCodeNames[i];
                if (s.find(phaseStr) != string::npos) {
                    offerInterruptOnPhase = PhaseRing::phaseStrToInt(phaseStr);
                    return true;
                }
            }
        }
    }

    if (!game) {
        game = new MTGPlayerCards();
        game->setOwner(this);
    }

    if (game->parseLine(s)) {
        return true;
    }

    return false;
}

void HumanPlayer::End() {
    if (!premade && opponent() && (observer->gameType() == GAME_TYPE_CLASSIC)) {
        DeckStats::GetInstance()->saveStats(this, opponent(), observer);
    }
}

std::ostream& operator<<(std::ostream& out, const Player& p) {
    out << "mode=" << p.playMode << std::endl;
    out << *(Damageable*)&p;
    const string manapoolstring = p.manaPool->toString();
    if (!manapoolstring.empty()) {
        out << "manapool=" << manapoolstring << std::endl;
    }
    if (!p.mAvatarName.empty()) {
        out << "avatar=" << p.mAvatarName << std::endl;
    }
    if (!p.phaseRing.empty()) {
        out << "customphasering=" << p.phaseRing << std::endl;
    }
    out << "offerinterruptonphase=" << Constants::MTGPhaseCodeNames[p.offerInterruptOnPhase] << std::endl;
    out << "premade=" << p.premade << std::endl;
    if (!p.deckFile.empty()) {
        out << "deckfile=" << p.deckFile << std::endl;
    }
    if (!p.deckFileSmall.empty()) {
        out << "deckfilesmall=" << p.deckFileSmall << std::endl;
    }

    if (p.game) {
        out << *(p.game);
    }

    return out;
}

// Method comparing "this" to "aPlayer", each in their own gameObserver
bool Player::operator<(Player& aPlayer) {
    // if this is dead and aPlayer is not dead then this < aPlayer
    if (isDead() && !aPlayer.isDead()) {
        return true;
    }

    // heuristics for min-max

    // if this is more poisoined than aPlayer then this < aPlayer
    if (poisonCount > aPlayer.poisonCount) {
        return true;
    }

    // if this has less life than aPlayer then this < aPlayer
    if (life < aPlayer.life) {
        return true;
    }

    // if this has less parmanents in game that aPlayer then this < aPlayer
    if (game->battlefield->cards.size() < aPlayer.game->battlefield->cards.size()) {
        return true;
    }

    return false;
}

bool Player::isDead() {
    if (observer) {
        return observer->didWin(opponent());
    }
    return false;
};
