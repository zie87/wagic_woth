#include "woth/string/algorithms.hpp"

#include "PrecompiledHeader.h"

#include "AIHints.h"
#include "AIPlayerBaka.h"
#include "utils.h"
#include "AllAbilities.h"

#include <sstream>
#include <utility>

AIHint::AIHint(string _line) {
    string line = std::move(_line);
    if (!line.length()) {
        DebugTrace("AIHINTS: line is empty");
        return;
    }
    std::transform(line.begin(), line.end(), line.begin(), ::tolower);
    vector<string> parameters = ::woth::split(line, ':');
    mCondition                = (parameters.size() == 1) ? "" : parameters[0];
    const string action       = parameters[parameters.size() - 1];

    vector<string> splitAction = parseBetween(action, "sourceid(", ")");
    if (!splitAction.empty()) {
        mAction   = splitAction[0];
        mSourceId = atoi(splitAction[1].c_str());
    } else {
        mAction   = action;
        mSourceId = 0;
    }

    vector<string> splitDontAttack = parseBetween(action, "dontattackwith(", ")");
    if (!splitDontAttack.empty()) {
        mCombatAttackTip = splitDontAttack[1];
    }

    vector<string> splitCastOrder = parseBetween(action, "castpriority(", ")");
    if (!splitCastOrder.empty()) {
        castOrder = ::woth::split(splitCastOrder[1], ',');
    }
}

AIHints::AIHints(AIPlayerBaka* player) : mPlayer(player) {}

void AIHints::add(string line) { hints.push_back(NEW AIHint(std::move(line))); }

AIHints::~AIHints() {
    for (size_t i = 0; i < hints.size(); ++i) {
        SAFE_DELETE(hints[i]);
    }
    hints.clear();
}

AIHint* AIHints::getByCondition(const string& condition) {
    if (condition.empty()) {
        return nullptr;
    }

    for (size_t i = 0; i < hints.size(); ++i) {
        if (hints[i]->mCondition == condition) {
            return hints[i];
        }
    }
    return nullptr;
}

bool AIHints::HintSaysDontAttack(GameObserver* observer, MTGCardInstance* card) {
    TargetChooserFactory tfc(observer);
    TargetChooser* hintTc = nullptr;
    for (unsigned int i = 0; i < hints.size(); i++) {
        if (!hints[i]->mCombatAttackTip.empty()) {
            hintTc = tfc.createTargetChooser(hints[i]->mCombatAttackTip, card);
            if (hintTc && hintTc->canTarget(card, true)) {
                SAFE_DELETE(hintTc);
                return true;
            }
            SAFE_DELETE(hintTc);
        }
    }
    return false;
}

vector<string> AIHints::mCastOrder() {
    for (unsigned int i = 0; i < hints.size(); i++) {
        if (!hints[i]->castOrder.empty()) {
            return hints[i]->castOrder;
        }
    }
    return vector<string>();
}

// return true if a given ability matches a hint's description
// Eventually this will look awfully similar to the parser...any way to merge them somehow ?
bool AIHints::abilityMatches(MTGAbility* ability, AIHint* hint) {
    const string s = hint->mAction;

    MTGAbility* a = AbilityFactory::getCoreAbility(ability);

    // Here we want to check that the card reacting to the MTGAbility is the one mentioned in the hint,
    //  to avoid mistaking the MTGAbility with a similar one.
    // Ideally we would find all cards with this ID, and see if the ability reacts to a click on one of these cards.
    //  This is the poor man's version, based on the fact that most cards are the source of their own abilities
    if (hint->mSourceId && ((!a->source) || a->source->getMTGId() != hint->mSourceId)) {
        return false;
    }

    if (auto* counterAbility = dynamic_cast<AACounter*>(a)) {
        vector<string> splitCounter = parseBetween(s, "counter(", ")");
        if (splitCounter.empty()) {
            return false;
        }

        string counterstring = counterAbility->name;
        std::transform(counterstring.begin(), counterstring.end(), counterstring.begin(), ::tolower);
        return (splitCounter[1] == counterstring);
    }

    if (auto* tokenAbility = dynamic_cast<ATokenCreator*>(a)) {
        vector<string> splitToken = parseBetween(s, "token(", ")");
        if (splitToken.empty()) {
            return false;
        }
        return (tokenAbility->tokenId && tokenAbility->tokenId == atoi(splitToken[1].c_str()));
    }

    return false;
}

// Finds all mtgAbility matching the Hint description
//  For now we limit findings
vector<MTGAbility*> AIHints::findAbilities(AIHint* hint) {
    std::vector<MTGAbility*> elems;
    ActionLayer* al = mPlayer->getObserver()->mLayers->actionLayer();

    for (size_t i = 1; i < al->mObjects.size(); i++)  // 0 is not a mtgability...hackish
    {
        MTGAbility* a = (dynamic_cast<MTGAbility*>(al->mObjects[i]));
        if (abilityMatches(a, hint)) {
            elems.push_back(a);
        }
    }
    return elems;
}

// Finds a mtgAbility matching the Hint description, and returns a valid AIAction matching this mtgability
RankingContainer AIHints::findActions(AIHint* hint) {
    RankingContainer ranking;

    vector<MTGAbility*> abilities = findAbilities(hint);

    for (size_t i = 0; i < abilities.size(); ++i) {
        MTGAbility* a = abilities[i];

        for (int j = 0; j < mPlayer->game->inPlay->nb_cards; j++) {
            MTGCardInstance* card = mPlayer->game->inPlay->cards[j];
            if (a->isReactingToClick(card, a->getCost())) {
                mPlayer->createAbilityTargets(a, card, ranking);  // TODO make that function static?
                break;                                            // For performance... ?
            }
        }
    }

    return ranking;
}

// Returns true if a card with the given MTG ID exists
bool AIHints::findSource(int sourceId) {
    for (int i = 0; i < mPlayer->game->inPlay->nb_cards; i++) {
        MTGCardInstance* c = mPlayer->game->inPlay->cards[i];
        if (c->getMTGId() == sourceId) {
            return true;
        }
    }
    return false;
}

string AIHints::constraintsNotFulfilled(AIAction* action, AIHint* hint, ManaCost* potentialMana) {
    std::stringstream out;

    if (!action) {
        if (!hint->mCombatAttackTip.empty()) {
            out << "to see if this can attack[" << hint->mCombatAttackTip << "]";
            return out.str();
        }
        if (hint->mSourceId && !findSource(hint->mSourceId)) {
            out << "needcardinplay[" << hint->mSourceId << "]";
            return out.str();
        }
        out << "needability[" << hint->mAction << "]";
        return out.str();
    }

    MTGAbility* a = action->ability;
    if (!a) {
        return "not supported";
    }

    MTGCardInstance* card = action->click;
    if (!card) {
        return "not supported";
    }

    // dummy test: would the ability work if we were sure to fulfill its mana requirements?
    if (!a->isReactingToClick(card, a->getCost())) {
        DebugTrace("This shouldn't happen, this AIAction doesn't seem like a good choice");
        return "not supported";
    }

    if (!a->isReactingToClick(card, potentialMana)) {
        // Not enough Mana, try to find which mana we should get in priority
        ManaCost* diff = potentialMana->Diff(a->getCost());
        for (int i = 0; i < Constants::NB_Colors; i++) {
            if (diff->getCost(i) < 0) {
                out << "needmana[" << Constants::MTGColorChars[i] << "]";
                if (Constants::MTGColorChars[i] == 'r') DebugTrace("Got it");
                SAFE_DELETE(diff);
                return out.str();
            }
        }

        // TODO, handle more cases where the cost cannot be paid
        return "not supported, can't afford cost for some reason";
    }

    // No problem found, we believe this is a good action to perform
    return "";
}

AIAction* AIHints::findAbilityRecursive(AIHint* hint, ManaCost* potentialMana) {
    RankingContainer ranking = findActions(hint);

    AIAction* a = nullptr;
    if (!ranking.empty()) {
        a = NEW AIAction(ranking.begin()->first);
    }

    const string s = constraintsNotFulfilled(a, hint, potentialMana);
    if (!hint->mCombatAttackTip.empty() || !hint->castOrder.empty()) {
        return nullptr;
    }
    if (!s.empty()) {
        SAFE_DELETE(a);
        AIHint* nextHint = getByCondition(s);
        DebugTrace("**I Need " << s << ", this can be provided by " << (nextHint ? nextHint->mAction : "NULL")
                               << "\n\n");
        if (nextHint && nextHint != hint) {
            return findAbilityRecursive(nextHint, potentialMana);
        }
        return nullptr;
    }

    return a;
}

AIAction* AIHints::suggestAbility(ManaCost* potentialMana) {
    for (size_t i = 0; i < hints.size(); ++i) {
        // Don't suggest abilities that require a condition, for now
        if (!hints[i]->mCondition.empty()) {
            continue;
        }

        AIAction* a = findAbilityRecursive(hints[i], potentialMana);
        if (a) {
            DebugTrace("**I Decided that the best to fulfill " << hints[i]->mAction << " is to play "
                                                               << a->ability->getMenuText() << "\n\n");
            return a;
        }
    }
    return nullptr;
}
