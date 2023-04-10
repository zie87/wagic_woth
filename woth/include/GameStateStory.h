#ifndef GAMESTATESTORY_H
#define GAMESTATESTORY_H

#include "GameState.h"
#include <JGui.h>

#include <vector>
#include <string>

class StoryFlow;
class SimpleMenu;

class GameStateStory : public GameState, public JGuiListener {
private:
    StoryFlow* flow;
    SimpleMenu* menu;
    std::vector<std::string> stories;
    void loadStoriesMenu(const char* root);

public:
    GameStateStory(GameApp* parent);
    ~GameStateStory() override;
    void Start() override;
    void End() override;
    void Update(float dt) override;
    void Render() override;
    void ButtonPressed(int controllerId, int controlId) override;
};

#endif
