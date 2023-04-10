/*
  Filter-like system for determining if a card meets certain criteria, for this and thisforeach autos
*/

#ifndef THISDESCRIPTOR_H
#define THISDESCRIPTOR_H

#include "Counters.h"
#include "MTGGameZones.h"
#include "MTGCardInstance.h"
#include "CardDescriptor.h"

class ThisDescriptor {
public:
    int comparisonMode;
    int comparisonCriterion;
    virtual int match(MTGCardInstance* card) = 0;
    int matchValue(int value) const;
    virtual ~ThisDescriptor();
    virtual ThisDescriptor* clone() const = 0;
};

class ThisDescriptorFactory {
public:
    ThisDescriptor* createThisDescriptor(GameObserver* observer, string s);
};

class ThisTargetCompare : public ThisDescriptor {
public:
    TargetChooser* targetComp;
    int match(MTGCardInstance* card) override;
    ThisTargetCompare(TargetChooser* tcc = nullptr);
    ~ThisTargetCompare() override;
    ThisTargetCompare* clone() const override;
};

class ThisCounter : public ThisDescriptor {
public:
    Counter* counter;
    int match(MTGCardInstance* card) override;

    ThisCounter(Counter* _counter);
    ThisCounter(int power, int toughness, int nb, const char* name);
    ~ThisCounter() override;
    ThisCounter* clone() const override;
};

class ThisCounterAny : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisCounterAny(int nb);
    ThisCounterAny* clone() const override;
};

class ThisControllerlife : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisControllerlife(int life);
    ThisControllerlife* clone() const override;
};

class ThisOpponentlife : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisOpponentlife(int olife);
    ThisOpponentlife* clone() const override;
};

class ThisEquip : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisEquip(int equipment);
    ThisEquip* clone() const override;
};

class ThisAuras : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisAuras(int auras);
    ThisAuras* clone() const override;
};

class ThisOpponentDamageAmount : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisOpponentDamageAmount(int damagecount);
    ThisOpponentDamageAmount* clone() const override;
};

class ThisUntapped : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisUntapped(int untapped);
    ThisUntapped* clone() const override;
};

class ThisTapped : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisTapped(int tapped);
    ThisTapped* clone() const override;
};

class ThisAttacked : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisAttacked(int attack);
    ThisAttacked* clone() const override;
};

class ThisBlocked : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisBlocked(int block);
    ThisBlocked* clone() const override;
};

class ThisNotBlocked : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisNotBlocked(int unblocked);
    ThisNotBlocked* clone() const override;
};

class ThisDamaged : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisDamaged(int wasDealtDamage);
    ThisDamaged* clone() const override;
};

class ThisDualWield : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisDualWield(int dualWield);
    ThisDualWield* clone() const override;
};

class ThisPower : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisPower(int power);
    ThisPower* clone() const override;
};

class ThisToughness : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;

    ThisToughness(int toughness);
    ThisToughness* clone() const override;
};

class ThisX : public ThisDescriptor {
public:
    int match(MTGCardInstance* card) override;
    ThisX(int x);
    ThisX* clone() const override;
};

class ThisVariable : public ThisDescriptor {
public:
    string vWord;
    int match(MTGCardInstance* card) override;
    ThisVariable(int comp, string vWord = "");
    ThisVariable* clone() const override;
};

#endif
