#ifndef GUIAVATARS_H
#define GUIAVATARS_H

#include "GuiLayers.h"

struct GuiAvatar;
class GuiGraveyard;
class GuiLibrary;
class GuiOpponentHand;
class GuiAvatars : public GuiLayer {
protected:
    GuiAvatar *self, *opponent;
    GuiGraveyard *selfGraveyard, *opponentGraveyard;
    GuiLibrary *selfLibrary, *opponentLibrary;
    GuiOpponentHand* opponentHand;
    GuiAvatar* active;

public:
    GuiAvatars(GameObserver* observer);
    ~GuiAvatars() override;

    GuiAvatar* GetSelf();
    GuiAvatar* GetOpponent();
    void Update(float dt) override;
    void Render() override;
    void Activate(PlayGuiObject* c);
    void Deactivate(PlayGuiObject* c);
    int receiveEventPlus(WEvent*) override;
    int receiveEventMinus(WEvent*) override;
    bool CheckUserInput(JButton key) override;
    float LeftBoundarySelf();
};

#endif  // GUIAVATARS_H
