/*
 *  Wagic, The Homebrew ?! is licensed under the BSD license
 *  See LICENSE in the Folder's root
 *  http://wololo.net/wagic/
 */

/*
ModRules class describes global game rules used for a given Wagic Mod.
These rules describe some high level Game rules,
some graphical effects, what parts of the game are made accessible to the player, etc...
They are accessed through the global variable gModRules, and loaded from rules/modrules.xml
*/

#ifndef MODRULES_H
#define MODRULES_H

#include <string>
#include <vector>

#include "CardGui.h"

class TiXmlElement;

enum {
    SUBMENUITEM_CANCEL = kCancelMenuID,
    MENUITEM_PLAY,
    MENUITEM_DECKEDITOR,
    MENUITEM_SHOP,
    MENUITEM_OPTIONS,
    MENUITEM_EXIT,
    MENUITEM_TROPHIES,
    SUBMENUITEM_1PLAYER,
#ifdef NETWORK_SUPPORT
    SUBMENUITEM_2PLAYERS,
    SUBMENUITEM_HOST_GAME,
    SUBMENUITEM_JOIN_GAME,
#endif  // NETWORK_SUPPORT
    SUBMENUITEM_DEMO,
    SUBMENUITEM_TESTSUITE,
    SUBMENUITEM_TESTAI,
    SUBMENUITEM_END_OFFSET
};

class ModRulesMenuItem {
protected:
    static int strToAction(const std::string& str);

public:
    int mActionId;
    std::string mDisplayName;
    ModRulesMenuItem(const std::string& actionIdStr, std::string displayName);
    // most actionIds are associated to a game state. e.g. MENUITEM_DECKEDITOR <--> GAME_STATE_DECK_VIEWER
    // This function returns the game state that matches the actionId, if any
    int getMatchingGameState() const;
    static int getMatchingGameState(int actionId);
};

class ModRulesMainMenuItem : public ModRulesMenuItem {
public:
    int mIconId;
    std::string mParticleFile;
    ModRulesMainMenuItem(const std::string& actionIdStr, std::string displayName, int iconId, std::string particleFile);
};

class ModRulesOtherMenuItem : public ModRulesMenuItem {
public:
    JButton mKey;
    ModRulesOtherMenuItem(const std::string& actionIdStr, std::string displayName, const std::string& keyStr);
    static JButton strToJButton(const std::string& keyStr);
};

class ModRulesMenu {
public:
    std::vector<ModRulesMainMenuItem*> main;
    std::vector<ModRulesOtherMenuItem*> other;

    void parse(TiXmlElement* element);
    ~ModRulesMenu();
};

class ModRulesBackGroundCardGuiItem {
protected:
    static int strToint(std::string str);

public:
    int mColorId;
    std::string MColorName;
    std::string mDisplayImg;
    std::string mDisplayThumb;
    int mMenuIcon;
    ModRulesBackGroundCardGuiItem(const std::string& ColorId, std::string ColorName, std::string DisplayImg,
                                  std::string DisplayThumb, const std::string& MenuIcon);
};

class ModRulesRenderCardGuiItem {
public:
    std::string mName;
    int mPosX;
    int mPosY;
    std::string mFilter;
    std::string mFormattedData;
    int mFontSize;
    bool mFont;
    PIXEL_TYPE mFontColor;
    /*Icons attributes*/
    int mSizeIcon;
    int mIconPosX;
    int mIconPosY;
    std::string mFileName;
    ModRulesRenderCardGuiItem(std::string name, int posX, int posY, std::string formattedData, std::string filter,
                              bool font, int fontSize, PIXEL_TYPE fontColor, int SizeIcon, int IconPosX, int IconPosY,
                              std::string FileName);
};

class ModRulesCardGui {
public:
    std::vector<ModRulesBackGroundCardGuiItem*> background;
    std::vector<ModRulesRenderCardGuiItem*> renderbig;
    std::vector<ModRulesRenderCardGuiItem*> rendertinycrop;
    void parse(TiXmlElement* element);
    ~ModRulesCardGui();
};

class ModRulesGame {
public:
    bool mCanInterrupt;

    bool canInterrupt() const { return mCanInterrupt; };
    ModRulesGame();
    void parse(TiXmlElement* element);
};

class ModRulesGeneral {
protected:
    bool mHasDeckEditor;
    bool mHasShop;

public:
    bool hasDeckEditor() const { return mHasDeckEditor; };
    bool hasShop() const { return mHasShop; };
    ModRulesGeneral();
    void parse(TiXmlElement* element);
};

class ModRulesCards {
public:
    SimpleCardEffect* activateEffect;
    static SimpleCardEffect* parseEffect(const std::string& str);
    ModRulesCards();
    ~ModRulesCards();
    void parse(TiXmlElement* element);
};

class ModRules {
public:
    ModRulesGeneral general;
    ModRulesCards cards;
    ModRulesMenu menu;
    ModRulesGame game;
    ModRulesCardGui cardgui;
    bool load(const std::string& filename);
    static int getValueAsInt(TiXmlElement* element, const std::string& childName);
};

extern ModRules gModRules;

#endif
