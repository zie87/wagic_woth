#ifndef GAMESTATETRANSITIONS_H
#define GAMESTATETRANSITIONS_H

#include <JGE.h>
#include <JGui.h>
#include "GameState.h"

class TransitionBase : public GameState, public JGuiListener {
public:
    TransitionBase(GameApp* parent, GameState* _from, GameState* _to, float duration);
    ~TransitionBase() override;
    void Start() override;
    void End() override;

    virtual bool Finished() { return (mElapsed >= mDuration); };
    void Update(float dt) override;
    void Render() override = 0;
    void ButtonPressed(int controllerId, int controlId) override;

    float mElapsed;
    float mDuration;
    GameState* from;
    GameState* to;
    bool bAnimationOnly;  // Does not call start or end on subordinates.
};

class TransitionFade : public TransitionBase {
public:
    TransitionFade(GameApp* p, GameState* f, GameState* t, float dur, bool reversed);
    void Render() override;
    bool mReversed;
};

#endif
