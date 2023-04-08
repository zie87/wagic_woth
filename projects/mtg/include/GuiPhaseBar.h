#ifndef GUIPHASEBAR_H
#define GUIPHASEBAR_H

#include "GuiLayers.h"
#include "PhaseRing.h"
#include "WEvent.h"

class GuiPhaseBar : public GuiLayer, public PlayGuiObject {
protected:
    Phase* phase;
    float angle;
    float zoomFactor;

public:
    GuiPhaseBar(GameObserver* observer);
    ~GuiPhaseBar() override;
    void Update(float dt) override;
    void Render() override;
    int receiveEventMinus(WEvent* e) override;
    std::ostream& toString(std::ostream& out) const override;
    void Entering() override;
    bool Leaving(JButton key) override;
};

#endif  // GUIPHASEBAR_H
