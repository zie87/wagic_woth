#ifndef TARGETCHOOSER_H
#define TARGETCHOOSER_H

#define TARGET_NOK 0
#define TARGET_OK 1
#define TARGET_OK_FULL 2
#define TARGET_OK_NOT_READY 3

#include <JGE.h>
#include "TargetsList.h"
#include "ActionStack.h"

#include <string>
using std::string;

class MTGCardInstance;
class MTGGameZone;
class Player;
class Damageable;
class Targetable;
class CardDescriptor;

class TargetChooser : public TargetsList {
protected:
    int forceTargetListReady;

public:
    const static int UNLITMITED_TARGETS = 1000;
    enum { UNSET = 0, OPPONENT = -1, CONTROLLER = 1, TARGET_CONTROLLER = 2, OWNER = 3, TARGETED_PLAYER = 4 };
    bool other;
    bool withoutProtections;
    TargetChooser(GameObserver* observer,
                  MTGCardInstance* card = nullptr,
                  int _maxtargets       = UNLITMITED_TARGETS,
                  bool other            = false,
                  bool targetMin        = false);
    Player* Owner;
    GameObserver* observer;
    MTGCardInstance* source;
    MTGCardInstance* targetter;  // Optional, usually equals source, used for protection from...
    int maxtargets;
    bool done;
    bool targetMin;
    bool validTargetsExist(int maxTarget = 1);
    int attemptsToFill;
    string belongsToAbility;
    int countValidTargets();
    virtual int setAllZones() { return 0; }
    virtual bool targetsZone(MTGGameZone* z) { return false; }
    virtual bool targetsZone(MTGGameZone* z, MTGCardInstance* mSource) { return false; };
    int ForceTargetListReady();
    int targetsReadyCheck();
    int addTarget(Targetable* target) override;
    virtual bool canTarget(Targetable* _target, bool withoutProtections = false);

    // returns true if tc is equivalent to this TargetChooser
    // Two targetchoosers are equivalent if they target exactly the same cards
    virtual bool equals(TargetChooser* tc);

    virtual int full() {
        if ((maxtargets != UNLITMITED_TARGETS && (int(targets.size())) >= maxtargets) || done) {
            return 1;
        }
        return 0;
    };
    virtual int ready() { return (int)(targets.size()); };
    ~TargetChooser() override{};
    int targetListSet();
    virtual TargetChooser* clone() const = 0;
};

class TargetChooserFactory {
    GameObserver* observer;

public:
    TargetChooserFactory(GameObserver* observer) : observer(observer){};
    TargetChooser* createTargetChooser(string s, MTGCardInstance* card, MTGAbility* ability = nullptr);
    TargetChooser* createTargetChooser(MTGCardInstance* card);
};

class TargetZoneChooser : public TargetChooser {
public:
    int zones[15];
    int nbzones;
    int init(int* _zones, int _nbzones);
    bool targetsZone(MTGGameZone* z) override;
    bool targetsZone(MTGGameZone* z, MTGCardInstance* mSource) override;
    bool withoutProtections;
    TargetZoneChooser(GameObserver* observer,
                      MTGCardInstance* card = nullptr,
                      int _maxtargets       = 1,
                      bool other            = false,
                      bool targetMin        = false);
    TargetZoneChooser(GameObserver* observer,
                      int* _zones,
                      int _nbzones,
                      MTGCardInstance* card = nullptr,
                      int _maxtargets       = 1,
                      bool other            = false,
                      bool targetMin        = false);
    bool canTarget(Targetable* _card, bool withoutProtections = false) override;
    int setAllZones() override;
    TargetZoneChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class CardTargetChooser : public TargetZoneChooser {
protected:
    MTGCardInstance* validTarget;

public:
    bool withoutProtections;
    CardTargetChooser(GameObserver* observer,
                      MTGCardInstance* card,
                      MTGCardInstance* source,
                      int* zones  = nullptr,
                      int nbzones = 0);
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    CardTargetChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class TypeTargetChooser : public TargetZoneChooser {
public:
    int nbtypes;
    int types[10];
    bool withoutProtections;
    TypeTargetChooser(GameObserver* observer,
                      const char* _type,
                      MTGCardInstance* card = nullptr,
                      int _maxtargets       = 1,
                      bool other            = false,
                      bool targetMin        = false);
    TypeTargetChooser(GameObserver* observer,
                      const char* _type,
                      int* _zones,
                      int nbzones,
                      MTGCardInstance* card = nullptr,
                      int _maxtargets       = 1,
                      bool other            = false,
                      bool targetMin        = false);
    void addType(int type);
    void addType(const char* type);
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    TypeTargetChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class DamageableTargetChooser : public TypeTargetChooser {
public:
    bool withoutProtections;
    DamageableTargetChooser(GameObserver* observer,
                            int* _zones,
                            int _nbzones,
                            MTGCardInstance* card = nullptr,
                            int _maxtargets       = 1,
                            bool other            = false,
                            bool targetMin        = false)
        : TypeTargetChooser(observer, "creature", _zones, _nbzones, card, _maxtargets, other, targetMin){};
    DamageableTargetChooser(GameObserver* observer,
                            MTGCardInstance* card = nullptr,
                            int _maxtargets       = 1,
                            bool other            = false,
                            bool targetMin        = false)
        : TypeTargetChooser(observer, "creature", card, _maxtargets, other, targetMin){};
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    DamageableTargetChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class PlayerTargetChooser : public TargetChooser {
protected:
    Player* p;  // In Case we can only target a specific player
public:
    bool withoutProtections;
    PlayerTargetChooser(GameObserver* observer,
                        MTGCardInstance* card = nullptr,
                        int _maxtargets       = 1,
                        Player* _p            = nullptr);
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    PlayerTargetChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class DescriptorTargetChooser : public TargetZoneChooser {
public:
    CardDescriptor* cd;
    bool withoutProtections;
    DescriptorTargetChooser(GameObserver* observer,
                            CardDescriptor* _cd,
                            MTGCardInstance* card = nullptr,
                            int _maxtargets       = 1,
                            bool other            = false,
                            bool targetMin        = false);
    DescriptorTargetChooser(GameObserver* observer,
                            CardDescriptor* _cd,
                            int* _zones,
                            int nbzones,
                            MTGCardInstance* card = nullptr,
                            int _maxtargets       = 1,
                            bool other            = false,
                            bool targetMin        = false);
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    ~DescriptorTargetChooser() override;
    DescriptorTargetChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class SpellTargetChooser : public TargetChooser {
public:
    int color;
    bool withoutProtections;
    SpellTargetChooser(GameObserver* observer,
                       MTGCardInstance* card = nullptr,
                       int _color            = -1,
                       int _maxtargets       = 1,
                       bool other            = false,
                       bool targetMin        = false);
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    SpellTargetChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class SpellOrPermanentTargetChooser : public TargetZoneChooser {
public:
    int color;
    bool withoutProtections;
    SpellOrPermanentTargetChooser(GameObserver* observer,
                                  MTGCardInstance* card = nullptr,
                                  int _color            = -1,
                                  int _maxtargets       = 1,
                                  bool other            = false,
                                  bool targetMin        = false);
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    SpellOrPermanentTargetChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class DamageTargetChooser : public TargetChooser {
public:
    int color;
    int state;
    bool withoutProtections;
    DamageTargetChooser(GameObserver* observer,
                        MTGCardInstance* card = nullptr,
                        int _color            = -1,
                        int _maxtargets       = 1,
                        int state             = NOT_RESOLVED);
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    DamageTargetChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

// Should only be used for triggered abilities.
class TriggerTargetChooser : public TargetChooser {
public:
    Targetable* target;
    int triggerTarget;
    bool withoutProtections;
    TriggerTargetChooser(GameObserver* observer, int _triggerTarget);
    bool targetsZone(MTGGameZone* z) override;
    bool canTarget(Targetable* _target, bool withoutProtections = false) override;
    TriggerTargetChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class ProliferateChooser : public TypeTargetChooser {
public:
    bool withoutProtections;
    ProliferateChooser(GameObserver* observer,
                       int* _zones,
                       int _nbzones,
                       MTGCardInstance* card = nullptr,
                       int _maxtargets       = 1,
                       bool other            = false,
                       bool targetMin        = false)
        : TypeTargetChooser(observer, "*", _zones, _nbzones, card, _maxtargets, other, targetMin){};
    ProliferateChooser(GameObserver* observer,
                       MTGCardInstance* card = nullptr,
                       int _maxtargets       = 1,
                       bool other            = false,
                       bool targetMin        = false)
        : TypeTargetChooser(observer, "*", card, _maxtargets, other, targetMin){};
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    ProliferateChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class BlockableChooser : public TypeTargetChooser {
public:
    bool withoutProtections;
    BlockableChooser(GameObserver* observer,
                     int* _zones,
                     int _nbzones,
                     MTGCardInstance* card = nullptr,
                     int _maxtargets       = 1,
                     bool other            = false,
                     bool targetMin        = false)
        : TypeTargetChooser(observer, "creature", _zones, _nbzones, card, _maxtargets, other, targetMin){};
    BlockableChooser(GameObserver* observer,
                     MTGCardInstance* card = nullptr,
                     int _maxtargets       = 1,
                     bool other            = false,
                     bool targetMin        = false)
        : TypeTargetChooser(observer, "creature", card, _maxtargets, other, targetMin){};
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    BlockableChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class myCursesChooser : public TypeTargetChooser {
public:
    bool withoutProtections;
    myCursesChooser(GameObserver* observer,
                    int* _zones,
                    int _nbzones,
                    MTGCardInstance* card = nullptr,
                    int _maxtargets       = 1,
                    bool other            = false,
                    bool targetMin        = false)
        : TypeTargetChooser(observer, "*", _zones, _nbzones, card, _maxtargets, other, targetMin){};
    myCursesChooser(GameObserver* observer,
                    MTGCardInstance* card = nullptr,
                    int _maxtargets       = 1,
                    bool other            = false,
                    bool targetMin        = false)
        : TypeTargetChooser(observer, "*", card, _maxtargets, other, targetMin){};
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    myCursesChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
};

class ParentChildChooser : public TypeTargetChooser {
public:
    bool withoutProtections;
    int type;
    TargetChooser* deeperTargeting;
    ParentChildChooser(GameObserver* observer,
                       int* _zones,
                       int _nbzones,
                       MTGCardInstance* card = nullptr,
                       int _maxtargets       = 1,
                       TargetChooser* deepTc = nullptr,
                       int type              = 1,
                       bool other            = false,
                       bool targetMin        = false)
        : TypeTargetChooser(observer, "*", _zones, _nbzones, card, _maxtargets, other, targetMin)
        , type(type)
        , deeperTargeting(deepTc){};
    ParentChildChooser(GameObserver* observer,
                       MTGCardInstance* card = nullptr,
                       int _maxtargets       = 1,
                       TargetChooser* deepTc = nullptr,
                       int type              = 1,
                       bool other            = false,
                       bool targetMin        = false)
        : TypeTargetChooser(observer, "*", card, _maxtargets, other, targetMin)
        , type(type)
        , deeperTargeting(deepTc){};
    bool canTarget(Targetable* target, bool withoutProtections = false) override;
    ParentChildChooser* clone() const override;
    bool equals(TargetChooser* tc) override;
    ~ParentChildChooser() override;
};
#endif
