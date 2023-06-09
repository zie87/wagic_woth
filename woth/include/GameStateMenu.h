#ifndef GAMESTATEMENU_H
#define GAMESTATEMENU_H

#include <JGui.h>
#include <dirent.h>
#include "GameState.h"
#include "SimpleMenu.h"
#include "TextScroller.h"

#include <vector>
#include <string>

class GameStateMenu : public GameState, public JGuiListener {
private:
    TextScroller* scroller;
    int scrollerSet;
    int mPercentComplete;
    JGuiController* mGuiController;
    SimpleMenu* subMenuController;
    SimpleMenu* gameTypeMenu;
    bool hasChosenGameType;
    JQuadPtr mIcons[10];
    JTexture* bgTexture;
    JQuadPtr mBg;
    JTexture* splashTex;
    float mCreditsYPos;
    int currentState;
    int mVolume;
    char nbcardsStr[400];
    std::vector<std::string> langs;
    std::vector<std::string> primitives;

    size_t mCurrentSetFolderIndex;
    std::string mCurrentSetName;
    std::string mCurrentSetFileName;
    std::vector<std::string> setFolders;

    std::string wallpaper;
    int primitivesLoadCounter;

    int mReadConf;
    float timeIndex;
    void fillScroller();

    void setLang(int id);
    std::string getLang(std::string s);
    void loadLangMenu();
    bool langChoices;
    void runTest();  //!!
    void listPrimitives();
    void genNbCardsStr();               // computes the contents of nbCardsStr
    void ensureMGuiController();        // creates the MGuiController if it doesn't exist
    std::string loadRandomWallpaper();  // loads a list of string of textures that can be randolmy shown on the loading
                                        // screen

    void RenderTopMenu();
    int gamePercentComplete();

public:
    GameStateMenu(GameApp* parent);
    ~GameStateMenu() override;
    void Create() override;
    void Destroy() override;
    void Start() override;
    void End() override;
    void Update(float dt) override;
    void Render() override;
    void ButtonPressed(int controllerId, int controlId) override;

    int nextSetFolder(const string& root, const string& file);  // Retrieves the next directory to have matching file
    void createUsersFirstDeck(int setId);
    virtual std::ostream& toString(std::ostream& out) const;

    enum {
        MENU_CARD_PURCHASE      = 2,
        MENU_DECK_SELECTION     = 10,
        MENU_DECK_BUILDER       = 11,
        MENU_FIRST_DUEL_SUBMENU = 102,
        MENU_LANGUAGE_SELECTION = 103,
    };
};

#endif
