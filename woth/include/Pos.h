#ifndef POS_H
#define POS_H

#include "JGE.h"

struct Pos {
    virtual ~Pos() = default;
    float actX, actY, actZ, actT, actA;
    float x, y, zoom, t, alpha;
    float width, height;
    PIXEL_TYPE mask;
    Pos(float, float, float, float, float);
    virtual void Update(float dt);
    void UpdateNow();
    virtual void Render();
    void Render(JQuad*);
};

#endif  // POS_H
