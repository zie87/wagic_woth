/* Default observers/Abilities that are added to the game for a standard Magic Game
 */

#ifndef MTGRULES_H
#define MTGRULES_H

#include <utility>

#include "MTGAbility.h"
#include "Counters.h"
#include "WEvent.h"
#include "CardSelector.h"
#include "ManaCost.h"

class PermanentAbility : public MTGAbility {
public:
    int testDestroy() override { return 0; };
    PermanentAbility(GameObserver* observer, int _id);
};

class OtherAbilitiesEventReceiver : public PermanentAbility {
public:
    int receiveEvent(WEvent* event) override;
    OtherAbilitiesEventReceiver(GameObserver* observer, int _id);
    OtherAbilitiesEventReceiver* clone() const override;
};

class MTGEventBonus : public PermanentAbility {
public:
    int textAlpha;
    string text;
    int army[2];
    bool army1[2];
    bool army2[2];
    bool army3[2];
    int toys[2];
    bool toybonusgranted[2];
    int chain[2];
    int highestChain[2];
    bool beastbonusgranted[2];
    int beast[2];
    bool zombiebonusgranted[2];
    int zombie[2];
    bool knightbonusgranted[2];
    int knight[2];
    bool insectbonusgranted[2];
    int insect[2];
    bool elementalbonusgranted[2];
    int elemental[2];
    bool vampirebonusgranted[2];
    int vampire[2];
    bool clericbonusgranted[2];
    int cleric[2];
    bool elfbonusgranted[2];
    int elf[2];
    bool Angelbonusgranted[2];
    int Angel[2];
    bool dragonbonusgranted[2];
    int dragon[2];

    int receiveEvent(WEvent* event) override;
    void grantAward(string awardName, int amount);
    void Update(float dt) override;
    void Render() override;
    MTGEventBonus(GameObserver* observer, int _id);
    MTGEventBonus* clone() const override;
};
class MTGPutInPlayRule : public PermanentAbility {
public:
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGPutInPlayRule(GameObserver* observer, int _id);
    const char* getMenuText() override { return "cast card normally"; }
    MTGPutInPlayRule* clone() const override;
};

class MTGKickerRule : public MTGPutInPlayRule {
public:
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGKickerRule(GameObserver* observer, int _id);
    const char* getMenuText() override { return "pay kicker"; }
    MTGKickerRule* clone() const override;
};

class MTGAlternativeCostRule : public PermanentAbility {
protected:
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana, ManaCost* alternateManaCost);
    int reactToClick(MTGCardInstance* card, ManaCost* alternateManaCost, int paymentType = ManaCost::MANA_PAID);
    string alternativeName;

public:
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGAlternativeCostRule(GameObserver* observer, int _id);
    const char* getMenuText() override {
        if (!alternativeName.empty()) {
            return alternativeName.c_str();
        }
        return "pay alternative cost";
    }
    MTGAlternativeCostRule* clone() const override;
};

class MTGBuyBackRule : public MTGAlternativeCostRule {
public:
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGBuyBackRule(GameObserver* observer, int _id);
    const char* getMenuText() override { return "cast and buy back"; }
    MTGBuyBackRule* clone() const override;
};

class MTGFlashBackRule : public MTGAlternativeCostRule {
public:
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGFlashBackRule(GameObserver* observer, int _id);
    const char* getMenuText() override { return "flash back"; }
    MTGFlashBackRule* clone() const override;
};

class MTGRetraceRule : public MTGAlternativeCostRule {
public:
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGRetraceRule(GameObserver* observer, int _id);
    const char* getMenuText() override { return "retrace"; }
    MTGRetraceRule* clone() const override;
};

class MTGMorphCostRule : public PermanentAbility {
public:
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGMorphCostRule(GameObserver* observer, int _id);
    const char* getMenuText() override { return "play morphed"; }
    MTGMorphCostRule* clone() const override;
};

class MTGSuspendRule : public MTGAlternativeCostRule {
public:
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int receiveEvent(WEvent* e) override;
    int reactToClick(MTGCardInstance* card) override;
    string suspendmenu;
    std::ostream& toString(std::ostream& out) const override;
    MTGSuspendRule(GameObserver* observer, int _id);
    const char* getMenuText() override;
    MTGSuspendRule* clone() const override;
};

class MTGAttackRule : public PermanentAbility, public Limitor {
public:
    bool select(Target*) override;
    bool greyout(Target*) override;
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGAttackRule(GameObserver* observer, int _id);
    const char* getMenuText() override { return "Attacker"; }
    int receiveEvent(WEvent* event) override;
    MTGAttackRule* clone() const override;
};

class MTGPlaneswalkerAttackRule : public PermanentAbility, public Limitor {
public:
    bool select(Target*) override;
    bool greyout(Target*) override;
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    MTGPlaneswalkerAttackRule(GameObserver* observer, int _id);
    const char* getMenuText() override { return "Attack Planeswalker"; }
    MTGPlaneswalkerAttackRule* clone() const override;
};
class AAPlaneswalkerAttacked : public InstantAbility {
public:
    string menuText;
    MTGCardInstance* attacker;
    AAPlaneswalkerAttacked(GameObserver* observer, int id, MTGCardInstance* source, MTGCardInstance* target);
    int resolve() override;
    const char* getMenuText() override;
    AAPlaneswalkerAttacked* clone() const override;
    ~AAPlaneswalkerAttacked() override;
};
/* handles combat trigger send recieve events*/
class MTGCombatTriggersRule : public PermanentAbility {
public:
    MTGCombatTriggersRule(GameObserver* observer, int _id);
    int receiveEvent(WEvent* event) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGCombatTriggersRule* clone() const override;
};

class MTGBlockRule : public PermanentAbility {
public:
    string blockmenu;
    TargetChooser* tcb;
    MTGAbility* blocker;
    MTGAbility* blockAbility;
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGBlockRule(GameObserver* observer, int _id);
    const char* getMenuText() override;
    MTGBlockRule* clone() const override;
    ~MTGBlockRule() override;
};

/* Persist Rule */
class MTGPersistRule : public PermanentAbility {
public:
    MTGPersistRule(GameObserver* observer, int _id);
    int receiveEvent(WEvent* event) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGPersistRule* clone() const override;
};
/* vampire Rule */
class MTGVampireRule : public PermanentAbility {
public:
    MTGVampireRule(GameObserver* observer, int _id);
    map<MTGCardInstance*, vector<MTGCardInstance*> > victims;
    int receiveEvent(WEvent* event) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGVampireRule* clone() const override;
};
// unearths destruction if leaves play effect
class MTGUnearthRule : public PermanentAbility {
public:
    MTGUnearthRule(GameObserver* observer, int _id);
    int receiveEvent(WEvent* event) override;
    std::ostream& toString(std::ostream& out) const override;
    MTGUnearthRule* clone() const override;
};
class MTGTokensCleanup : public PermanentAbility {
public:
    vector<MTGCardInstance*> list;
    MTGTokensCleanup(GameObserver* observer, int _id);
    int receiveEvent(WEvent* event) override;
    MTGTokensCleanup* clone() const override;
};

/*
 * Rule 420.5e (Legend Rule)
 * If two or more legendary permanents with the same name are in play, all are put into their
 * owners' graveyards. This is called the "legend rule." If only one of those permanents is
 * legendary, this rule doesn't apply.
 */
class MTGLegendRule : public ListMaintainerAbility {
public:
    MTGLegendRule(GameObserver* observer, int _id);
    int canBeInList(MTGCardInstance* card) override;
    int added(MTGCardInstance* card) override;
    int removed(MTGCardInstance* card) override;
    int testDestroy() override;
    std::ostream& toString(std::ostream& out) const override;
    MTGLegendRule* clone() const override;
};
class MTGPlaneWalkerRule : public ListMaintainerAbility {
public:
    MTGPlaneWalkerRule(GameObserver* observer, int _id);
    int canBeInList(MTGCardInstance* card) override;
    int added(MTGCardInstance* card) override;
    int removed(MTGCardInstance* card) override;
    int testDestroy() override;
    std::ostream& toString(std::ostream& out) const override;
    MTGPlaneWalkerRule* clone() const override;
};
/* LifeLink */
class MTGPlaneswalkerDamage : public PermanentAbility {
public:
    MTGPlaneswalkerDamage(GameObserver* observer, int _id);

    int receiveEvent(WEvent* event) override;

    MTGPlaneswalkerDamage* clone() const override;
};
class MTGMomirRule : public PermanentAbility {
private:
    int genRandomCreatureId(int convertedCost);
    vector<int> pool[20];
    int initialized;

    int textAlpha;
    string text;

public:
    int alreadyplayed;
    MTGAllCards* collection;
    MTGCardInstance* genCreature(int id);
    void Update(float dt) override;
    void Render() override;
    MTGMomirRule(GameObserver* observer, int _id, MTGAllCards* _collection);
    int isReactingToClick(MTGCardInstance* card, ManaCost* mana = nullptr) override;
    int reactToClick(MTGCardInstance* card) override;
    int reactToClick(MTGCardInstance* card, int id);
    const char* getMenuText() override { return "Momir"; }
    std::ostream& toString(std::ostream& out) const override;
    MTGMomirRule* clone() const override;
};

// stone hewer gaint avatar mode
class MTGStoneHewerRule : public PermanentAbility {
private:
    int genRandomEquipId(int convertedCost);
    vector<int> pool[20];
    int initialized;

public:
    MTGAllCards* collection;
    MTGCardInstance* genEquip(int id);
    MTGStoneHewerRule(GameObserver* observer, int _id, MTGAllCards* _collection);
    int receiveEvent(WEvent* event) override;
    const char* getMenuText() override { return "Stone Hewer"; }
    std::ostream& toString(std::ostream& out) const override;
    MTGStoneHewerRule* clone() const override;
};
// Hermit Druid avatar mode
class MTGHermitRule : public PermanentAbility {
public:
    MTGHermitRule(GameObserver* observer, int _id);
    int receiveEvent(WEvent* event) override;
    const char* getMenuText() override { return "Hermit"; }
    MTGHermitRule* clone() const override;
};
//
/* LifeLink */
class MTGLifelinkRule : public PermanentAbility {
public:
    MTGLifelinkRule(GameObserver* observer, int _id);

    int receiveEvent(WEvent* event) override;

    std::ostream& toString(std::ostream& out) const override;

    MTGLifelinkRule* clone() const override;
};

/* Deathtouch */
class MTGDeathtouchRule : public PermanentAbility {
public:
    MTGDeathtouchRule(GameObserver* observer, int _id);

    int receiveEvent(WEvent* event) override;

    const char* getMenuText() override { return "Deathtouch"; }

    MTGDeathtouchRule* clone() const override;
};

/* handling parentchild */
class ParentChildRule : public PermanentAbility {
public:
    ParentChildRule(GameObserver* observer, int _id);
    int receiveEvent(WEvent* event) override;
    std::ostream& toString(std::ostream& out) const override;
    ParentChildRule* clone() const override;
};
/* HUD Display */

class HUDString {
public:
    string value;
    float timestamp;
    int quantity;
    HUDString(string s, float ts) : value(std::move(s)), timestamp(ts), quantity(1){};
};

class HUDDisplay : public PermanentAbility {
private:
    std::list<HUDString*> events;
    float timestamp;
    float popdelay;
    WFont* f;
    float maxWidth;
    int addEvent(const string& s);

public:
    int receiveEvent(WEvent* event) override;
    void Update(float dt) override;
    void Render() override;
    HUDDisplay(GameObserver* observer, int _id);
    ~HUDDisplay() override;
    HUDDisplay* clone() const override;
};

#endif
