#ifndef GUIHAND_H
#define GUIHAND_H

#include "GuiLayers.h"
#include "WEvent.h"
#include "MTGGameZones.h"
#include "CardGui.h"
#include "GuiHand.h"

class GuiHand;

struct HandLimitor : public Limitor {
    ~HandLimitor() override = default;
    GuiHand* hand;
    bool select(Target*) override;
    bool greyout(Target*) override;

    HandLimitor(GuiHand* hand);
};

class GuiHand : public GuiLayer {
public:
    static const float ClosedRowX;
    static const float LeftRowX;
    static const float RightRowX;

    static const float OpenX;
    static const float ClosedX;
    static const float OpenY;
    static const float ClosedY;

protected:
    const MTGHand* hand;
    JQuadPtr back;
    vector<CardView*> cards;

public:
    GuiHand(GameObserver* observer, MTGHand* hand);
    ~GuiHand() override;
    void Update(float dt) override;
    bool isInHand(CardView*);

    friend struct HandLimitor;
};

class GuiHandOpponent : public GuiHand {
public:
    GuiHandOpponent(GameObserver* observer, MTGHand* hand);
    void Render() override;
    int receiveEventPlus(WEvent* e) override;
    int receiveEventMinus(WEvent* e) override;
};

class GuiHandSelf : public GuiHand {
protected:
    typedef enum { Open, Closed } HandState;
    HandState state;
    Pos backpos;

public:
    GuiHandSelf(GameObserver* observer, MTGHand* hand);
    ~GuiHandSelf() override;
    int receiveEventPlus(WEvent* e) override;
    int receiveEventMinus(WEvent* e) override;

    void Repos();
    bool CheckUserInput(JButton key) override;
    void Render() override;
    void Update(float dt) override;
    float LeftBoundary();

    HandState GetState() { return state; }

    HandLimitor* limitor;
};

#endif  // GUIHAND_H
