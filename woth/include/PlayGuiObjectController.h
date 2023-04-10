#ifndef PLAYGUIOBJECTCONTROLLER_H
#define PLAYGUIOBJECTCONTROLLER_H

#define BIG_CARD_RENDER_TIME 0.4

#include "GuiLayers.h"

class PlayGuiObjectController : public GuiLayer {
protected:
    float last_user_move;
    int getClosestItem(int direction);
    int getClosestItem(int direction, float tolerance);
    static int showBigCards;  // 0 hide, 1 show, 2 show text
public:
    void Update(float dt) override;
    bool CheckUserInput(JButton key) override;
    PlayGuiObjectController(GameObserver* observer) : GuiLayer(observer), last_user_move(0){};
    void Render() override { GuiLayer::Render(); };
};

#endif
