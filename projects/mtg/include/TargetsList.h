#ifndef TARGETSLIST_H
#define TARGETSLIST_H

class Targetable;
class MTGCardInstance;
class Player;
class Damageable;
class Spell;
class Interruptible;
class Damage;

#include <utility>
#include <vector>
using std::vector;

class TargetsList {
protected:
    size_t iterateTarget(Targetable* previous);
    vector<Targetable*> targets;

public:
    virtual ~TargetsList() = default;
    TargetsList();
    TargetsList(Targetable* _targets[], int nbtargets);
    int alreadyHasTarget(Targetable* target);
    int removeTarget(Targetable* _card);
    int toggleTarget(Targetable* _card);
    size_t getNbTargets() { return targets.size(); };
    virtual int addTarget(Targetable* _target);
    MTGCardInstance* getNextCardTarget(MTGCardInstance* previous = nullptr);
    Player* getNextPlayerTarget(Player* previous = nullptr);
    Damageable* getNextDamageableTarget(Damageable* previous = nullptr);
    Interruptible* getNextInterruptible(Interruptible* previous, int type);
    Spell* getNextSpellTarget(Spell* previous = nullptr);
    Damage* getNextDamageTarget(Damage* previous = nullptr);
    Targetable* getNextTarget(Targetable* previous = nullptr);
    vector<Targetable*> getTargetsFrom() { return targets; }
    void setTargetsTo(vector<Targetable*> targetTo) { targets = std::move(targetTo); }
    void initTargets() { targets.clear(); };
};

#endif
