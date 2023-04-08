#ifndef GUIFRAME_H
#define GUIFRAME_H

#include "GuiLayers.h"

class GuiFrame : public GuiLayer {
protected:
    JQuadPtr wood;
    JQuadPtr gold1, gold2, goldGlow;
    float step;

public:
    GuiFrame(GameObserver* observer);
    ~GuiFrame() override;
    void Render() override;
    void Update(float dt) override;
};

#endif  // GUIFRAME_H
