#ifndef GUIBACKGROUND_H
#define GUIBACKGROUND_H

#include "GuiLayers.h"
#include "WEvent.h"

class GuiBackground : public GuiLayer {
protected:
    JQuad* quad;

public:
    GuiBackground(GameObserver* observer);
    ~GuiBackground() override;
    void Render() override;
};

#endif  // GUIBACKGROUND_H
