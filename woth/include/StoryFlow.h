#ifndef STORYFLOW_H
#define STORYFLOW_H

#include <string>
#include <map>
#include <vector>
#include "../../libs/jge/src/tinyxml/tinyxml.h"
#include <JGui.h>
class GameObserver;
class MTGDeck;
#define CAMPAIGNS_FOLDER "campaigns/"

class StoryDialogElement : public JGuiObject {
public:
    float mX;
    float mY;
    StoryDialogElement(float x, float y, int id = 0);
    void Entering() override{};
    bool Leaving(JButton key) override { return false; };
    bool ButtonPressed() override { return false; };
    bool hasFocus() { return false; };
    virtual float getHeight() = 0;
    bool getTopLeft(float& top, float& left) override {
        top  = mY;
        left = mX;
        return true;
    }
};

class StoryText : public StoryDialogElement {
public:
    std::string text;
    int align;
    int font;
    StoryText(std::string text, float mX, float mY, const std::string& align = "center", int font = 0, int id = 0);

    void Render() override;
    void Update(float dt) override;
    std::ostream& toString(std::ostream& out) const override;
    float getHeight() override;
};
class StoryImage : public StoryDialogElement {
public:
    std::string img;
    StoryImage(std::string img, float mX, float mY);
    void Render() override;
    void Update(float dt) override;
    std::ostream& toString(std::ostream& out) const override;
    float getHeight() override;
};

class StoryReward : public StoryText {
public:
    enum {
        STORY_REWARD_CREDITS,
        STORY_REWARD_SET,
        STORY_REWARD_CARD,
    };

    int rewardDone;
    std::string value;
    int type;

    StoryReward(const std::string& _type, std::string _value, std::string text, float _mX, float _mY,
                const std::string& align = "center", int font = 0, int id = 0);
    void Update(float dt) override;
    void Render() override;

    static bool rewardSoundPlayed;
    static bool rewardsEnabled;
    static MTGDeck* collection;
};

class StoryChoice : public StoryText {
public:
    std::string pageId;

    bool mHasFocus;
    float mScale;
    float mTargetScale;
    StoryChoice(std::string id, std::string text, int JGOid, float mX, float mY, const std::string& _align, int _font,
                bool hasFocus);
    void Render() override;
    void Update(float dt) override;

    void Entering() override;
    bool Leaving(JButton key) override;
    bool ButtonPressed() override;
    bool hasFocus() const;
    std::ostream& toString(std::ostream& out) const override;
    float getHeight() override;
};

class StoryFlow;
class StoryPage {
protected:
    std::string safeAttribute(TiXmlElement* element, const std::string& attribute);

public:
    StoryFlow* mParent;
    std::string musicFile;
    StoryPage(StoryFlow* mParent);
    virtual void Update(float dt) = 0;
    virtual void Render()         = 0;
    virtual ~StoryPage(){};
    int loadElement(TiXmlElement* element);
};

class StoryDialog : public StoryPage, public JGuiListener, public JGuiController {
private:
    std::vector<StoryDialogElement*> graphics;
    void RenderElement(StoryDialogElement* elmt);

public:
    StoryDialog(TiXmlElement* el, StoryFlow* mParent);
    ~StoryDialog() override;
    void Update(float dt) override;
    void Render() override;
    void ButtonPressed(int, int) override;

    static float currentY;
    static float previousY;
};

class Rules;
class StoryDuel : public StoryPage {
public:
    std::string pageId;
    std::string onWin, onLose;
    std::string bg;  // background file
    GameObserver* game;
    Rules* rules;
    StoryDuel(TiXmlElement* el, StoryFlow* mParent);
    ~StoryDuel() override;
    void Update(float dt) override;
    void Render() override;
    void init();
};

class StoryFlow {
private:
    std::map<std::string, StoryPage*> pages;
    bool parse(const std::string& filename);
    StoryPage* loadPage(TiXmlElement* element);
    bool _gotoPage(const std::string& id);

public:
    std::string currentPageId;
    std::string folder;
    StoryFlow(const std::string& folder);
    ~StoryFlow();

    bool gotoPage(const std::string& id);
    bool loadPageId(const std::string& id);
    void Update(float dt);
    void Render();
};

#endif
