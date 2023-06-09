/*
 *  Wagic, The Homebrew ?! is licensed under the BSD license
 *  See LICENSE in the Folder's root
 *  http://wololo.net/wagic/
 */

#ifndef ACTIONSTACK_H
#define ACTIONSTACK_H

#define MAX_SPELL_TARGETS 10

#define ACTION_SPELL 10
#define ACTION_DAMAGE 11
#define ACTION_DAMAGES 12
#define ACTION_NEXTGAMEPHASE 13
#define ACTION_DRAW 14
#define ACTION_PUTINGRAVEYARD 15
#define ACTION_ABILITY 16

#define NOT_RESOLVED -2
#define RESOLVED_OK 1
#define RESOLVED_NOK -1

#include "PlayGuiObject.h"
#include "GuiLayers.h"
#include "TargetsList.h"
#include "Targetable.h"

#include "WResource_Fwd.h"

class GuiLayer;
class PlayGuiObject;
class MTGCardInstance;
class GameObserver;
class Player;
class Damageable;
class MTGAbility;
class Targetable;
class DamageStack;
class ManaCost;
class TargetChooser;

#define ACTIONSTACK_STANDARD 0
#define ACTIONSTACK_TARGET 1

class Interruptible : public PlayGuiObject, public Targetable {
public:
    // TODO : remove these when they are back in PlayGuiObject
    float x, y;

    int state, display;
    MTGCardInstance* source;
    void Entering() override { mHasFocus = true; }

    bool Leaving(JButton key) override {
        mHasFocus = false;
        return true;
    }

    bool ButtonPressed() override { return true; }

    virtual int resolve() { return 0; }

    void Render() override {}

    Interruptible(GameObserver* observer, int inID = 0, bool hasFocus = false)
        : PlayGuiObject(40, x, y, inID, hasFocus)
        , Targetable(observer)
        , state(NOT_RESOLVED)
        , display(0)
        , source(nullptr) {}

    const string getDisplayName() const override;
    void Render(MTGCardInstance* source,
                JQuad* targetQuad,
                const string& alt1,
                const string& alt2,
                string action,
                bool bigQuad = false);

    virtual int receiveEvent(WEvent* event) { return 0; }

    virtual void Dump();

protected:
    float GetVerticalTextOffset() const;
};

class NextGamePhase : public Interruptible {
public:
    int resolve() override;
    bool extraDamagePhase();
    void Render() override;
    std::ostream& toString(std::ostream& out) const override;
    const string getDisplayName() const override;
    NextGamePhase(GameObserver* observer, int id);
};

class Spell : public Interruptible {
protected:
public:
    MTGGameZone* from;
    TargetChooser* tc;
    ManaCost* cost;
    int payResult;
    int computeX(MTGCardInstance* card) const;
    Spell(GameObserver* observer, MTGCardInstance* _source);
    Spell(GameObserver* observer, int id, MTGCardInstance* _source, TargetChooser* _tc, ManaCost* _cost, int payResult);
    ~Spell() override;
    int resolve() override;
    void Render() override;
    bool FullfilledAlternateCost(const int& costType) const;
    const string getDisplayName() const override;
    std::ostream& toString(std::ostream& out) const override;
    MTGCardInstance* getNextCardTarget(MTGCardInstance* previous = nullptr) const;
    Player* getNextPlayerTarget(Player* previous = nullptr) const;
    Damageable* getNextDamageableTarget(Damageable* previous = nullptr) const;
    Interruptible* getNextInterruptible(Interruptible* previous, int type) const;
    Spell* getNextSpellTarget(Spell* previous = nullptr) const;
    Damage* getNextDamageTarget(Damage* previous = nullptr) const;
    Targetable* getNextTarget(Targetable* previous = nullptr) const;
    int getNbTargets() const;
};

class StackAbility : public Interruptible {
public:
    MTGAbility* ability;
    int resolve() override;
    void Render() override;
    std::ostream& toString(std::ostream& out) const override;
    const string getDisplayName() const override;
    StackAbility(GameObserver* observer, int id, MTGAbility* _ability);
};

class PutInGraveyard : public Interruptible {
public:
    MTGCardInstance* card;
    int removeFromGame;
    int resolve() override;
    void Render() override;
    std::ostream& toString(std::ostream& out) const override;
    PutInGraveyard(GameObserver* observer, int id, MTGCardInstance* _card);
};

class DrawAction : public Interruptible {
public:
    int nbcards;
    Player* player;
    int resolve() override;
    void Render() override;
    std::ostream& toString(std::ostream& out) const override;
    DrawAction(GameObserver* observer, int id, Player* _player, int _nbcards);
};

class ATutorialMessage;
class LifeAction : public Interruptible {
public:
    int amount;
    Damageable* target;
    int resolve() override;
    void Render() override;
    std::ostream& toString(std::ostream& out) const override;
    LifeAction(GameObserver* observer, int id, Damageable* _target, int amount);
};

class ActionStack : public GuiLayer {
public:
    typedef enum { NOT_DECIDED = 0, INTERRUPT = -1, DONT_INTERRUPT = 1, DONT_INTERRUPT_ALL = 2 } InterruptDecision;

protected:
    JQuadPtr pspIcons[8];
    InterruptDecision interruptDecision[2];
    float timer;
    int currentState;
    int mode;
    int checked;
    ATutorialMessage* currentTutorial;
    int interruptBtnXOffset, noBtnXOffset, noToAllBtnXOffset, interruptDialogWidth;

    JButton handleInterruptRequest(JButton inputKey, int& x, int& y) const;

public:
    Player* lastActionController;
    int setIsInterrupting(Player* player, bool log = true);
    int count(int type = 0, int state = 0, int display = -1);
    Interruptible* getActionElementFromCard(MTGCardInstance* card);
    Interruptible* getPrevious(Interruptible* next, int type = 0, int state = 0, int display = -1);
    int getPreviousIndex(Interruptible* next, int type = 0, int state = 0, int display = -1);
    Interruptible* getNext(Interruptible* previous, int type = 0, int state = 0, int display = -1);
    int getNextIndex(Interruptible* previous, int type = 0, int state = 0, int display = -1);
    void Fizzle(Interruptible* action);
    Interruptible* getAt(int id);
    void cancelInterruptOffer(InterruptDecision cancelMode = DONT_INTERRUPT, bool log = true);
    void endOfInterruption(bool log = true);
    Interruptible* getLatest(int state);
    Player* askIfWishesToInterrupt;
    int garbageCollect();
    int addAction(Interruptible* interruptible);
    Spell* addSpell(MTGCardInstance* card, TargetChooser* tc, ManaCost* mana, int payResult, int storm);
    int AddNextGamePhase();
    int AddNextCombatStep();
    int addPutInGraveyard(MTGCardInstance* card);
    int addDraw(Player* player, int nbcards = 1);
    int addLife(Damageable* _target, int amount = 0);
    int addDamage(MTGCardInstance* _source, Damageable* target, int _damage);
    int addAbility(MTGAbility* ability);
    void Update(float dt) override;
    bool CheckUserInput(JButton key) override;
    void Render() override;
    ActionStack(GameObserver* game);
    int resolve();
    int has(Interruptible* action);
    int has(MTGAbility* ability);
    int receiveEventPlus(WEvent* event) override;
    void Dump();
    void setCurrentTutorial(ATutorialMessage* message) { currentTutorial = message; };
    ATutorialMessage* getCurrentTutorial() { return currentTutorial; };
    bool isCalm() { return interruptDecision[0] == NOT_DECIDED && interruptDecision[1] == NOT_DECIDED; };
};

#endif
