#ifndef MTGABILITY_H
#define MTGABILITY_H

class MTGCardInstance;
class Spell;
class Damageable;
class PlayGuiObject;
class ManaCost;
class MTGGameZone;
class Player;
class AManaProducer;
class WEvent;
class Counter;

#include "GameObserver.h"
#include "ActionElement.h"
#include <string>
#include <map>
#include <hge/hgeparticle.h>
#include "Damage.h"
#include "TargetChooser.h"
using std::map;
using std::string;

// stupid variables used to give a hint to the AI:
//  Should I cast a spell on an enemy or friendly unit ?
#define BAKA_EFFECT_GOOD 1
#define BAKA_EFFECT_BAD -1
#define BAKA_EFFECT_DONTKNOW 0
#define MODE_PUTINTOPLAY 1
#define MODE_ABILITY 2
#define MODE_TARGET 3

#define COUNT_POWER 1

#define PARSER_LORD 1
#define PARSER_FOREACH 2
#define PARSER_ASLONGAS 3

class MTGAbility : public ActionElement {
private:
    ManaCost* mCost;

protected:
    char menuText[50];

    GameObserver* game;

    // returns target itself if it is a player, or its controller if it is a card
    static Player* getPlayerFromTarget(Targetable* target);

    // returns target itself if it is a player, or its controller if it is a card
    static Player* getPlayerFromDamageable(Damageable* target);

public:
    enum {
        NO_RESTRICTION     = 0,
        PLAYER_TURN_ONLY   = 1,
        AS_SORCERY         = 2,
        MY_BEFORE_BEGIN    = 3,
        MY_UNTAP           = 4,
        MY_UPKEEP          = 5,
        MY_DRAW            = 6,
        MY_FIRSTMAIN       = 7,
        MY_COMBATBEGIN     = 8,
        MY_COMBATATTACKERS = 9,
        MY_COMBATBLOCKERS  = 10,
        MY_COMBATDAMAGE    = 11,
        MY_COMBATEND       = 12,
        MY_SECONDMAIN      = 13,
        MY_ENDOFTURN       = 14,
        MY_EOT             = 15,
        MY_CLEANUP         = 16,
        MY_AFTER_EOT       = 17,

        OPPONENT_BEFORE_BEGIN    = 23,
        OPPONENT_UNTAP           = 24,
        OPPONENT_UPKEEP          = 25,
        OPPONENT_DRAW            = 26,
        OPPONENT_FIRSTMAIN       = 27,
        OPPONENT_COMBATBEGIN     = 28,
        OPPONENT_COMBATATTACKERS = 29,
        OPPONENT_COMBATBLOCKERS  = 30,
        OPPONENT_COMBATDAMAGE    = 31,
        OPPONENT_COMBATEND       = 32,
        OPPONENT_SECONDMAIN      = 33,
        OPPONENT_ENDOFTURN       = 34,
        OPPONENT_EOT             = 35,
        OPPONENT_CLEANUP         = 36,
        OPPONENT_AFTER_EOT       = 37,

        BEFORE_BEGIN    = 43,
        UNTAP           = 44,
        UPKEEP          = 45,
        DRAW            = 46,
        FIRSTMAIN       = 47,
        COMBATBEGIN     = 48,
        COMBATATTACKERS = 49,
        COMBATBLOCKERS  = 50,
        COMBATDAMAGE    = 51,
        COMBATEND       = 52,
        SECONDMAIN      = 53,
        ENDOFTURN       = 54,
        EOT             = 55,
        CLEANUP         = 56,
        AFTER_EOT       = 57,

        OPPONENT_TURN_ONLY = 60,

    };

    bool oneShot;
    int forceDestroy;
    int forcedAlive;
    bool canBeInterrupted;
    ManaCost* alternative;
    ManaCost* BuyBack;
    ManaCost* FlashBack;
    ManaCost* Retrace;
    ManaCost* morph;
    ManaCost* suspend;

    Targetable* target;
    int aType;
    int naType;
    int abilitygranted;
    MTGCardInstance* source;

    int allowedToCast(MTGCardInstance* card, Player* player);
    int allowedToAltCast(MTGCardInstance* card, Player* player);
    MTGAbility(GameObserver* observer, int id, MTGCardInstance* card);
    MTGAbility(GameObserver* observer, int id, MTGCardInstance* _source, Targetable* _target);
    MTGAbility(const MTGAbility& copyFromMe);
    int testDestroy() override;
    ~MTGAbility() override;
    ManaCost* getCost() { return mCost; };
    void setCost(ManaCost* cost, bool forceDelete = 0);

    void Render() override {}

    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override { return 0; }

    int reactToClick(MTGCardInstance* card) override { return 0; }

    int receiveEvent(WEvent* event) override { return 0; }

    void Update(float dt) override {}

    virtual int fireAbility();
    int stillInUse(MTGCardInstance* card) override;

    virtual int resolve() { return 0; }

    MTGAbility* clone() const override = 0;
    std::ostream& toString(std::ostream& out) const override;
    virtual int addToGame();
    virtual int removeFromGame();

    /*Poor man's casting */
    /* Todo replace that crap with dynamic casting */
    enum {
        UNKNOWN                   = 0,
        MANA_PRODUCER             = 1,
        MTG_ATTACK_RULE           = 2,
        DAMAGER                   = 3,
        STANDARD_REGENERATE       = 4,
        PUT_INTO_PLAY             = 5,
        MOMIR                     = 6,
        MTG_BLOCK_RULE            = 7,
        ALTERNATIVE_COST          = 8,
        BUYBACK_COST              = 9,
        FLASHBACK_COST            = 10,
        RETRACE_COST              = 11,
        MTG_COMBATTRIGGERS_RULE   = 12,
        STANDARD_PREVENT          = 13,
        STANDARD_EQUIP            = 14,
        STANDARD_LEVELUP          = 15,
        FOREACH                   = 16,
        STANDARD_DRAW             = 17,
        STANDARD_PUMP             = 18,
        STANDARD_BECOMES          = 19,
        UPCOST                    = 20,
        STANDARDABILITYGRANT      = 21,
        UNTAPPER                  = 22,
        TAPPER                    = 23,
        LIFER                     = 24,
        CLONING                   = 25,
        STANDARD_TEACH            = 26,
        STANDARD_TOKENCREATOR     = 27,
        MORPH_COST                = 28,
        SUSPEND_COST              = 29,
        COUNTERS                  = 30,
        PUT_INTO_PLAY_WITH_KICKER = 31,
        STANDARD_FIZZLER          = 32,
    };
};

class NestedAbility {
public:
    MTGAbility* ability;
    NestedAbility(MTGAbility* _ability);
};

class TriggeredAbility : public MTGAbility {
public:
    TriggeredAbility(GameObserver* observer, int id, MTGCardInstance* card);
    TriggeredAbility(GameObserver* observer, int id, MTGCardInstance* _source, Targetable* _target);
    void Update(float dt) override;

    void Render() override {}

    virtual int trigger() { return 0; }

    virtual int triggerOnEvent(WEvent* e) { return 0; }

    int receiveEvent(WEvent* e) override;
    int resolve() override                   = 0;
    TriggeredAbility* clone() const override = 0;
    std::ostream& toString(std::ostream& out) const override;
    string castRestriction;
};

// Triggers are not "real" abilities. They don't resolve, they just "trigger" and are associated to other abilities that
// will be addedToGame when the Trigger triggers
class Trigger : public TriggeredAbility {
private:
    bool mOnce;
    bool mActiveTrigger;

public:
    Trigger(GameObserver* observer, int id, MTGCardInstance* source, bool once, TargetChooser* _tc = nullptr);
    int resolve() override {
        return 0;  // This is a trigger, this function should not be called
    }
    int triggerOnEvent(WEvent* event) override;
    virtual int triggerOnEventImpl(WEvent* event) = 0;
};

class ActivatedAbility : public MTGAbility {
public:
    ManaCost* abilityCost;
    int restrictions;
    int limitPerTurn;
    int counters;
    int needsTapping;
    string limit;
    MTGAbility* sideEffect;
    MTGAbility* sa;
    string usesBeforeSideEffects;
    int uses;
    string castRestriction;

    ActivatedAbility(GameObserver* observer,
                     int id,
                     MTGCardInstance* card,
                     ManaCost* _cost              = nullptr,
                     int _restrictions            = NO_RESTRICTION,
                     string limit                 = "",
                     MTGAbility* sideEffect       = nullptr,
                     string usesBeforeSideEffects = "",
                     string castRestriction       = "");
    ~ActivatedAbility() override;

    void Update(float dt) override {
        if (newPhase != currentPhase && newPhase == MTG_PHASE_AFTER_EOT) {
            counters = 0;
        }
        MTGAbility::Update(dt);
    }
    int reactToClick(MTGCardInstance* card) override;
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToTargetClick(Targetable* object) override;
    virtual int activateAbility();
    int resolve() override = 0;
    void activateSideEffect();
    ActivatedAbility* clone() const override = 0;
    std::ostream& toString(std::ostream& out) const override;
};

class TargetAbility : public ActivatedAbility, public NestedAbility {
public:
    TargetAbility(GameObserver* observer,
                  int id,
                  MTGCardInstance* card,
                  TargetChooser* _tc,
                  ManaCost* _cost        = nullptr,
                  int _playerturnonly    = 0,
                  string castRestriction = "");
    TargetAbility(GameObserver* observer,
                  int id,
                  MTGCardInstance* card,
                  ManaCost* _cost        = nullptr,
                  int _playerturnonly    = 0,
                  string castRestriction = "");
    ~TargetAbility() override;

    int reactToClick(MTGCardInstance* card) override;
    int reactToTargetClick(Targetable* object) override;
    TargetAbility* clone() const override = 0;
    void Render() override;
    int resolve() override;
    const char* getMenuText() override;
    std::ostream& toString(std::ostream& out) const override;
};

class InstantAbility : public MTGAbility {
public:
    int init;
    void Update(float dt) override;
    int testDestroy() override;
    InstantAbility(GameObserver* observer, int _id, MTGCardInstance* source);
    InstantAbility(GameObserver* observer, int _id, MTGCardInstance* source, Targetable* _target);

    int resolve() override { return 0; }

    InstantAbility* clone() const override = 0;
    std::ostream& toString(std::ostream& out) const override;
};

/* State based effects. This class works ONLY for InPlay and needs to be extended for other areas of the game !!! */
class ListMaintainerAbility : public MTGAbility {
public:
    map<MTGCardInstance*, bool> cards;
    map<MTGCardInstance*, bool> checkCards;
    map<Player*, bool> players;
    ListMaintainerAbility(GameObserver* observer, int _id) : MTGAbility(observer, _id, nullptr) {}

    ListMaintainerAbility(GameObserver* observer, int _id, MTGCardInstance* _source)
        : MTGAbility(observer, _id, _source) {}

    ListMaintainerAbility(GameObserver* observer, int _id, MTGCardInstance* _source, Damageable* _target)
        : MTGAbility(observer, _id, _source, _target) {}

    void Update(float dt) override;
    void updateTargets();
    void checkTargets();
    virtual bool canTarget(MTGGameZone* zone);
    virtual int canBeInList(MTGCardInstance* card) = 0;
    virtual int added(MTGCardInstance* card)       = 0;
    virtual int removed(MTGCardInstance* card)     = 0;

    virtual int canBeInList(Player* p) { return 0; }

    virtual int added(Player* p) { return 0; }

    virtual int removed(Player* p) { return 0; }

    int destroy() override;
    ListMaintainerAbility* clone() const override = 0;
    std::ostream& toString(std::ostream& out) const override;
};

class TriggerAtPhase : public TriggeredAbility {
public:
    int phaseId;
    int who;
    bool sourceUntapped;
    bool sourceTap;
    bool lifelost;
    int lifeamount;
    bool once, activeTrigger;
    TriggerAtPhase(GameObserver* observer,
                   int id,
                   MTGCardInstance* source,
                   Targetable* target,
                   int _phaseId,
                   int who             = 0,
                   bool sourceUntapped = false,
                   bool sourceTap      = false,
                   bool lifelost       = false,
                   int lifeamount      = 0,
                   bool once           = false);
    int trigger() override;
    int resolve() override { return 0; };
    TriggerAtPhase* clone() const override;
};

class TriggerNextPhase : public TriggerAtPhase {
public:
    int destroyActivated;
    bool sourceUntapped;
    bool sourceTap;
    bool once, activeTrigger;
    TriggerNextPhase(GameObserver* observer,
                     int id,
                     MTGCardInstance* source,
                     Targetable* target,
                     int _phaseId,
                     int who             = 0,
                     bool sourceUntapped = false,
                     bool sourceTap      = false,
                     bool once           = false);
    TriggerNextPhase* clone() const override;
    int testDestroy() override;
};

class GenericTriggeredAbility : public TriggeredAbility, public NestedAbility {
public:
    TriggeredAbility* t;
    std::queue<Targetable*> targets;
    MTGAbility* destroyCondition;
    GenericTriggeredAbility(GameObserver* observer,
                            int id,
                            MTGCardInstance* _source,
                            TriggeredAbility* _t,
                            MTGAbility* a,
                            MTGAbility* dc      = nullptr,
                            Targetable* _target = nullptr);
    int trigger() override;
    int triggerOnEvent(WEvent* e) override;
    int resolve() override;
    int testDestroy() override;

    Targetable* getTriggerTarget(WEvent* e, MTGAbility* a);
    void setTriggerTargets(Targetable* ta, MTGAbility* a);

    void Update(float dt) override;
    GenericTriggeredAbility* clone() const override;
    const char* getMenuText() override;
    ~GenericTriggeredAbility() override;
};

/* Ability Factory */
class AbilityFactory {
private:
    std::string storedString;
    std::string storedAbilityString;
    std::string storedAndAbility;
    int countCards(TargetChooser* tc, Player* player = nullptr, int option = 0);
    TriggeredAbility* parseTrigger(const string& s,
                                   const string& magicText,
                                   int id,
                                   Spell* spell,
                                   MTGCardInstance* card,
                                   Targetable* target);
    MTGAbility* getAlternateCost(string s, int id, Spell* spell, MTGCardInstance* card);
    MTGAbility* getManaReduxAbility(const string& s,
                                    int id,
                                    Spell* spell,
                                    MTGCardInstance* card,
                                    MTGCardInstance* target);
    TargetChooser* parseSimpleTC(const std::string& s,
                                 const std::string& starter,
                                 MTGCardInstance* card,
                                 bool forceNoTarget = true);
    GameObserver* observer;

public:
    AbilityFactory(GameObserver* observer) : observer(observer){};
    int parseRestriction(const string& s);
    int parseCastRestrictions(MTGCardInstance* card, Player* player, const string& restrictions);
    Counter* parseCounter(const string& s, MTGCardInstance* target, Spell* spell = nullptr);
    int parsePowerToughness(const string& s, int* power, int* toughness);
    int getAbilities(vector<MTGAbility*>* v,
                     Spell* spell,
                     MTGCardInstance* card = nullptr,
                     int id                = 0,
                     MTGGameZone* dest     = nullptr);
    MTGAbility* parseMagicLine(string s,
                               int id,
                               Spell* spell,
                               MTGCardInstance* card,
                               bool activated    = false,
                               bool forceUEOT    = false,
                               MTGGameZone* dest = nullptr);
    int abilityEfficiency(MTGAbility* a,
                          Player* p,
                          int mode           = MODE_ABILITY,
                          TargetChooser* tc  = nullptr,
                          Targetable* target = nullptr);
    int magicText(int id,
                  Spell* spell,
                  MTGCardInstance* card = nullptr,
                  int mode              = MODE_PUTINTOPLAY,
                  TargetChooser* tc     = nullptr,
                  MTGGameZone* dest     = nullptr);
    static int computeX(Spell* spell, MTGCardInstance* card);
    static MTGAbility* getCoreAbility(MTGAbility* a);
    int destroyAllInPlay(TargetChooser* tc, int bury = 0);
    int moveAll(TargetChooser* tc, string destinationZone);
    int damageAll(TargetChooser* tc, int damage);
    int TapAll(TargetChooser* tc);
    int UntapAll(TargetChooser* tc);
    void addAbilities(int _id, Spell* spell);
    MTGAbility* parseUpkeepAbility(const string& s       = "",
                                   MTGCardInstance* card = nullptr,
                                   Spell* spell          = nullptr,
                                   int restrictions      = 0,
                                   int id                = -1);
    MTGAbility* parsePhaseActionAbility(const string& s         = "",
                                        MTGCardInstance* card   = nullptr,
                                        Spell* spell            = nullptr,
                                        MTGCardInstance* target = nullptr,
                                        int restrictions        = 0,
                                        int id                  = -1);
    MTGAbility* parseChooseActionAbility(const string& s         = "",
                                         MTGCardInstance* card   = nullptr,
                                         Spell* spell            = nullptr,
                                         MTGCardInstance* target = nullptr,
                                         int restrictions        = 0,
                                         int id                  = -1);
};

class ActivatedAbilityTP : public ActivatedAbility {
public:
    int who;
    ActivatedAbilityTP(GameObserver* observer,
                       int id,
                       MTGCardInstance* card,
                       Targetable* _target = nullptr,
                       ManaCost* cost      = nullptr,
                       int who             = TargetChooser::UNSET);
    Targetable* getTarget();
};

class InstantAbilityTP : public InstantAbility {
public:
    int who;
    InstantAbilityTP(GameObserver* observer,
                     int id,
                     MTGCardInstance* card,
                     Targetable* _target = nullptr,
                     int who             = TargetChooser::UNSET);
    Targetable* getTarget();
};

class AbilityTP : public MTGAbility {
public:
    int who;
    AbilityTP(GameObserver* observer,
              int id,
              MTGCardInstance* card,
              Targetable* _target = nullptr,
              int who             = TargetChooser::UNSET);

    Targetable* getTarget();

    ~AbilityTP() override {}
};

class AManaProducer : public ActivatedAbilityTP {
protected:
    Player* controller;

public:
    string menutext;
    ManaCost* output;
    int tap;
    string Producing;
    AManaProducer(GameObserver* observer,
                  int id,
                  MTGCardInstance* card,
                  Targetable* t,
                  ManaCost* _output,
                  ManaCost* _cost  = nullptr,
                  int who          = TargetChooser::UNSET,
                  string producing = "");
    int isReactingToClick(MTGCardInstance* _card, ManaCost* mana = nullptr) override;
    int resolve() override;
    int reactToClick(MTGCardInstance* _card) override;
    const char* getMenuText() override;
    ~AManaProducer() override;
    AManaProducer* clone() const override;
};

#endif
