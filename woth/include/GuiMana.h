#ifndef GUIMANA_H
#define GUIMANA_H

#include "string.h"
#include <vector>
#include <hge/hgeparticle.h>
#include "JGE.h"
#include "MTGDefinitions.h"
#include "GameApp.h"
#include "GuiLayers.h"

class ManaIcon : public Pos {
    hgeParticleSystem* particleSys;
    JQuadPtr icon;

    float zoomP1, zoomP2, zoomP3, zoomP4, zoomP5, zoomP6;
    float xP1, xP2, xP3;
    float yP1, yP2, yP3;
    float tP1;
    float f;
    float destx, desty;

public:
    enum { ALIVE, WITHERING, DROPPING, DEAD } mode;

    int color;
    void Render() override;
    void Update(float dt, float shift);
    void Wither();
    void Drop();
    ManaIcon(int color, float x, float y, float destx, float desty);
    ~ManaIcon() override;
};

class GuiMana : public GuiLayer {
protected:
    vector<ManaIcon*> manas;
    float x, y;
    Player* owner;
    void RenderStatic();

public:
    GuiMana(float x, float y, Player* p);
    ~GuiMana() override;
    void Render() override;
    void Update(float dt) override;
    int receiveEventPlus(WEvent* e) override;
    int receiveEventMinus(WEvent* e) override;
};

#endif
