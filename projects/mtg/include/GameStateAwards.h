#ifndef GAMESTATEAWARDS_H
#define GAMESTATEAWARDS_H

#include <JGE.h>
#include "GameState.h"
#include "SimpleMenu.h"

class WGuiList;
class WGuiMenu;
class WSrcCards;

class GameStateAwards : public GameState, public JGuiListener {
private:
    WGuiList* listview;
    WGuiMenu* detailview;
    WSrcCards* setSrc;
    SimpleMenu* menu;
    bool showMenu;
    bool showAlt;
    bool saveMe;
    int mState;
    int mDetailItem;

public:
    GameStateAwards(GameApp* parent);
    bool enterSet(int setid);
    bool enterStats(int option);
    ~GameStateAwards() override;

    void Start() override;
    void End() override;
    void Create() override;
    void Destroy() override;
    void Update(float dt) override;
    void Render() override;
    void ButtonPressed(int controllerId, int controlId) override;
    void OnScroll(int inXVelocity, int inYVelocity) override;
};

#endif
