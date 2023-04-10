#ifndef EXTRACOST_H
#define EXTRACOST_H

#include <vector>
#include "Counters.h"
#include "ObjectAnalytics.h"

using std::vector;

class TargetChooser;
class MTGCardInstance;
class MTGAbility;

class ExtraCost
#ifdef TRACK_OBJECT_USAGE
    : public InstanceCounter<ExtraCost>
#endif
{
public:
    TargetChooser* tc;
    MTGCardInstance* source;
    MTGCardInstance* target;
    std::string mCostRenderString;

    ExtraCost(const std::string& inCostRenderString, TargetChooser* _tc = nullptr);
    virtual ~ExtraCost();

    virtual int setPayment(MTGCardInstance* card);
    virtual int isPaymentSet() { return (target != nullptr); }

    virtual int canPay() { return 1; }
    virtual int doPay() = 0;
    virtual void Render();
    virtual int setSource(MTGCardInstance* _source);
    virtual ExtraCost* clone() const = 0;
};

class ExtraCosts {
public:
    vector<ExtraCost*> costs;
    MTGCardInstance* source;
    MTGAbility* action;

    ExtraCosts();
    ~ExtraCosts();
    void Render();
    int tryToSetPayment(MTGCardInstance* card);
    int isPaymentSet();
    int canPay();
    int doPay();
    int reset();
    int setAction(MTGAbility* _action, MTGCardInstance* _source);
    void Dump() const;
    ExtraCosts* clone() const;
};

class SacrificeCost : public ExtraCost {
public:
    SacrificeCost(TargetChooser* _tc = nullptr);
    int doPay() override;
    SacrificeCost* clone() const override;
};

// life cost
class LifeCost : public ExtraCost {
public:
    LifeCost(TargetChooser* _tc = nullptr);
    int canPay() override;
    int doPay() override;
    LifeCost* clone() const override;
};

// pyrhaixa mana
class LifeorManaCost : public ExtraCost {
public:
    LifeorManaCost(TargetChooser* _tc = nullptr, string manaType = "");
    string manaType;
    int canPay() override;
    int doPay() override;
    LifeorManaCost* clone() const override;
};

// Discard a random card cost
class DiscardRandomCost : public ExtraCost {
public:
    DiscardRandomCost(TargetChooser* _tc = nullptr);
    int canPay() override;
    int doPay() override;
    DiscardRandomCost* clone() const override;
};

// a choosen discard
class DiscardCost : public ExtraCost {
public:
    DiscardCost(TargetChooser* _tc = nullptr);
    int doPay() override;
    DiscardCost* clone() const override;
};

// cycle
class CycleCost : public ExtraCost {
public:
    CycleCost(TargetChooser* _tc = nullptr);
    int doPay() override;
    CycleCost* clone() const override;
};

// tolibrary cost
class ToLibraryCost : public ExtraCost {
public:
    ToLibraryCost(TargetChooser* _tc = nullptr);
    int doPay() override;
    ToLibraryCost* clone() const override;
};

// Millyourself cost
class MillCost : public ExtraCost {
public:
    MillCost(TargetChooser* _tc = nullptr);
    int canPay() override;
    int doPay() override;
    MillCost* clone() const override;
};

// Mill to exile yourself cost
class MillExileCost : public MillCost {
public:
    MillExileCost(TargetChooser* _tc = nullptr);
    int doPay() override;
};

// tap  cost
class TapCost : public ExtraCost {
public:
    TapCost();
    int isPaymentSet() override;
    int canPay() override;
    int doPay() override;
    TapCost* clone() const override;
};

// untap  cost
class UnTapCost : public ExtraCost {
public:
    UnTapCost();
    int isPaymentSet() override;
    int canPay() override;
    int doPay() override;
    UnTapCost* clone() const override;
};

// tap other cost
class TapTargetCost : public ExtraCost {
public:
    TapTargetCost(TargetChooser* _tc = nullptr);
    int isPaymentSet() override;
    int doPay() override;
    TapTargetCost* clone() const override;
};
// untap a target as cost
class UnTapTargetCost : public ExtraCost {
public:
    UnTapTargetCost(TargetChooser* _tc = nullptr);
    int isPaymentSet() override;
    int doPay() override;
    UnTapTargetCost* clone() const override;
};

// exile as cost
class ExileTargetCost : public ExtraCost {
public:
    ExileTargetCost(TargetChooser* _tc = nullptr);
    int doPay() override;
    ExileTargetCost* clone() const override;
};

// bounce cost
class BounceTargetCost : public ExtraCost {
public:
    BounceTargetCost(TargetChooser* _tc = nullptr);
    int doPay() override;
    BounceTargetCost* clone() const override;
};

// bounce cost
class Ninja : public ExtraCost {
public:
    Ninja(TargetChooser* _tc = nullptr);
    int canPay() override;
    int isPaymentSet() override;
    int doPay() override;
    Ninja* clone() const override;
};

class CounterCost : public ExtraCost {
public:
    Counter* counter;
    int hasCounters;
    CounterCost(Counter* _counter, TargetChooser* _tc = nullptr);
    ~CounterCost() override;
    int setPayment(MTGCardInstance* card) override;
    int isPaymentSet() override;
    int canPay() override;
    int doPay() override;
    CounterCost* clone() const override;
};

#endif
