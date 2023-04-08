#ifndef REPLACEMENTEFFECTS_H
#define REPLACEMENTEFFECTS_H

#include <list>

#include "Damage.h"
#include "WEvent.h"
#include "Counters.h"

class TargetChooser;
class MTGAbility;

class ReplacementEffect {
public:
    virtual WEvent* replace(WEvent* e) { return e; };
    virtual ~ReplacementEffect() {}
};

class REDamagePrevention : public ReplacementEffect {
protected:
    MTGAbility* source;
    TargetChooser* tcSource;
    TargetChooser* tcTarget;
    int damage;
    bool oneShot;
    int typeOfDamage;

public:
    REDamagePrevention(MTGAbility* _source, TargetChooser* _tcSource = nullptr, TargetChooser* _tcTarget = nullptr,
                       int _damage = -1, bool _oneShot = true, int typeOfDamage = DAMAGE_ALL_TYPES);
    WEvent* replace(WEvent* e) override;
    ~REDamagePrevention() override;
};

class RECountersPrevention : public ReplacementEffect {
protected:
    MTGAbility* source;
    MTGCardInstance* cardSource;
    MTGCardInstance* cardTarget;
    TargetChooser* TargetingCards;
    Counter* counter;

public:
    RECountersPrevention(MTGAbility* _source, MTGCardInstance* cardSource = nullptr,
                         MTGCardInstance* cardTarget = nullptr, TargetChooser* tc = nullptr,
                         Counter* counter = nullptr);
    WEvent* replace(WEvent* e) override;
    ~RECountersPrevention() override;
};

class ReplacementEffects {
protected:
    std::list<ReplacementEffect*> modifiers;

public:
    ReplacementEffects();
    WEvent* replace(WEvent* e);
    int add(ReplacementEffect* re);
    int remove(ReplacementEffect* re);
    ~ReplacementEffects();
};

#endif
