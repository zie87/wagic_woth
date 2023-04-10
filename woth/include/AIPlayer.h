/*
 *  Wagic, The Homebrew ?! is licensed under the BSD license
 *  See LICENSE in the Folder's root
 *  http://wololo.net/wagic/

 AIPlayer is the interface to represent a CPU Player.
 At its core, AIPlayer inherits from Player, and its children need to implement the function "Act" which
 pretty much handles all the logic.
 A sample implementation can be found in AIPlayerBaka.

 Ideally, mid-term, AIPlayer will handle all the mechanical tasks (clicking on cards, etc...) while its children are
 just in charge of the logic

 */

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "Player.h"
#include "config.h"

#include <queue>
#include <utility>
using std::queue;

class AIStats;
class AIPlayer;

class AIAction {
public:
    AIPlayer* owner;
    MTGAbility* ability;
    NestedAbility* nability;
    Player* player;
    int id;
    MTGCardInstance* click;
    MTGCardInstance* target;  // TODO Improve
    vector<Targetable*> mAbilityTargets;
    Targetable* playerAbilityTarget;
    // player targeting through abilities is handled completely seperate from spell targeting.

    AIAction(AIPlayer* owner, MTGAbility* a, MTGCardInstance* c, MTGCardInstance* t = nullptr)
        : owner(owner)
        , ability(a)
        , player(nullptr)
        , click(c)
        , target(t)
        , playerAbilityTarget(nullptr){};

    AIAction(AIPlayer* owner, MTGCardInstance* c, MTGCardInstance* t = nullptr);

    AIAction(AIPlayer* owner, Player* p)  // player targeting through spells
        : owner(owner)
        , ability(nullptr)
        , player(p)
        , click(nullptr)
        , target(nullptr)
        , playerAbilityTarget(nullptr){};

    AIAction(AIPlayer* owner, MTGAbility* a, MTGCardInstance* c, vector<Targetable*> targetCards)
        : owner(owner)
        , ability(a)
        , player(nullptr)
        , click(c)
        , mAbilityTargets(std::move(targetCards))
        , playerAbilityTarget(nullptr){};

    AIAction(AIPlayer* owner, MTGAbility* a, Player* p, MTGCardInstance* c)  // player targeting through abilities.
        : owner(owner)
        , ability(a)
        , click(c)
        , target(nullptr)
        , playerAbilityTarget(p){};
    int Act();
    int clickMultiAct(vector<Targetable*>& actionTargets) const;
};

class AIPlayer : public Player {
private:
    static int
        totalAIDecks;  // a cache that counts the number of AI deck files in the AI folder. see getTotalAIDecks() below.
    static int countTotalDecks(int lower, int higher, int current);

protected:
    bool mFastTimerMode;
    queue<AIAction*> clickstream;
    int clickMultiTarget(TargetChooser* tc, vector<Targetable*>& potentialTargets);
    int clickSingleTarget(TargetChooser* tc, vector<Targetable*>& potentialTargets,
                          MTGCardInstance* Choosencard = nullptr);
    RandomGenerator randomGenerator;

public:
    // These variables are used by TestSuite and Rules.cpp... TODO change that?
    int agressivity;
    bool forceBestAbilityUse;

    void End() override{};
    int displayStack() override { return 0; };
    int receiveEvent(WEvent* event) override;
    void Render() override;

    AIPlayer(GameObserver* observer, const string& deckFile, string deckFileSmall, MTGDeck* deck = nullptr);
    ~AIPlayer() override;

    virtual int chooseTarget(TargetChooser* tc = nullptr, Player* forceTarget = nullptr,
                             MTGCardInstance* Chosencard = nullptr, bool checkonly = false) = 0;
    virtual int affectCombatDamages(CombatStep)                                             = 0;
    int Act(float dt) override                                                              = 0;

    int isAI() override { return 1; };

    void setFastTimerMode(bool mode = true) { mFastTimerMode = mode; };
    RandomGenerator* getRandomGenerator() { return &randomGenerator; };

    bool parseLine(const string& s) override;

    static int getTotalAIDecks();
    static void invalidateTotalAIDecks();
};

class AIPlayerFactory {
public:
    AIPlayer* createAIPlayer(GameObserver* observer, MTGAllCards* collection, Player* opponent, int deckid = 0);
#ifdef AI_CHANGE_TESTING
    AIPlayer* createAIPlayerTest(GameObserver* observer, MTGAllCards* collection, Player* opponent, string folder);
#endif
};

#endif
