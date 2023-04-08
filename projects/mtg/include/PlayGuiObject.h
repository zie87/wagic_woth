/*
 A class for all interactive objects in the play area (cards, avatars, etc...)
 */

#ifndef PLAYGUIOBJECT_H
#define PLAYGUIOBJECT_H

#define GUI_AVATAR 1
#define GUI_CARD 2
#define GUI_GRAVEYARD 3
#define GUI_LIBRARY 4
#define GUI_OPPONENTHAND 5

#include <JGui.h>
#include "Effects.h"
#include "WEvent.h"
#include "Pos.h"

class PlayGuiObject : public JGuiObject, public JGuiListener, public Pos {
protected:
public:
    int wave;
    float mHeight;
    float defaultHeight;
    bool mHasFocus;
    int type;
    void Entering() override {
        mHasFocus = true;
        zoom      = 1.4f;
    };
    bool Leaving(JButton key) override {
        mHasFocus = false;
        zoom      = 1.0;
        return true;
    };
    virtual bool CheckUserInput(JButton key) { return false; };
    bool ButtonPressed() override { return true; };
    void Render() override;
    void Update(float dt) override;

    PlayGuiObject(float desiredHeight, float x, float y, int inID, bool hasFocus);

    PlayGuiObject(float desiredHeight, const Pos& ref, int inID, bool hasFocus);

    void ButtonPressed(int controllerId, int controlId) override {}
    bool getTopLeft(float& top, float& left) override {
        top  = actY;
        left = actX;
        return true;
    };
    ~PlayGuiObject() override{};
    std::vector<Effect*> effects;
};

#endif
