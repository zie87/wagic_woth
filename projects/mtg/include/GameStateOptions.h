#ifndef GAMESTATEOPTIONS_H
#define GAMESTATEOPTIONS_H

#include <JGE.h>
#include <JGui.h>
#include "GameState.h"

class GameApp;
class WGuiTabMenu;
class SimpleMenu;
class SimplePad;

struct KeybGrabber {
    virtual ~KeybGrabber()               = default;
    virtual void KeyPressed(LocalKeySym) = 0;
};

class GameStateOptions : public GameState, public JGuiListener {
private:
    enum { SHOW_OPTIONS, SHOW_OPTIONS_MENU, SAVE };
    float timer;
    bool mReload;
    KeybGrabber* grabber;

public:
    SimpleMenu* optionsMenu;
    WGuiTabMenu* optionsTabs;
    int mState;

    GameStateOptions(GameApp* parent);
    ~GameStateOptions() override;

    void Start() override;
    void End() override;
    void Update(float dt) override;
    void Render() override;
    virtual void GrabKeyboard(KeybGrabber*);
    virtual void UngrabKeyboard(const KeybGrabber*);
    void ButtonPressed(int controllerId, int ControlId) override;

    string newProfile;
};

#endif
