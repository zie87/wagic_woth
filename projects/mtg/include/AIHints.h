#ifndef AIHINTS_H
#define AIHINTS_H

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "AIPlayerBaka.h"

class ManaCost;
class MTGAbility;

class AIHint {
public:
    string mCondition;
    string mAction;
    string mCombatAttackTip;
    vector<string> castOrder;
    int mSourceId;
    AIHint(string line);
};

class AIHints {
protected:
    AIPlayerBaka* mPlayer;
    vector<AIHint*> hints;
    AIHint* getByCondition(const string& condition);
    AIAction* findAbilityRecursive(AIHint* hint, ManaCost* potentialMana);
    vector<MTGAbility*> findAbilities(AIHint* hint);
    RankingContainer findActions(AIHint* hint);
    string constraintsNotFulfilled(AIAction* a, AIHint* hint, ManaCost* potentialMana);
    bool findSource(int sourceId);
    bool abilityMatches(MTGAbility* a, AIHint* hint);

public:
    AIHints(AIPlayerBaka* player);
    AIAction* suggestAbility(ManaCost* potentialMana);
    bool HintSaysDontAttack(GameObserver* observer, MTGCardInstance* card = nullptr);
    vector<string> mCastOrder();
    void add(string line);
    ~AIHints();
};

#endif
