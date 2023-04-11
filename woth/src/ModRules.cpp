#include "PrecompiledHeader.h"

#include "ModRules.h"

#include <utility>
#include "utils.h"
#include "GameState.h"
#include "tinyxml/tinyxml.h"

ModRules gModRules;

bool ModRules::load(const std::string& filename) {
    std::string xmlBuffer;
    if (!JFileSystem::GetInstance()->readIntoString(filename, xmlBuffer)) {
        DebugTrace("FATAL: cannot find modrules.xml");
        return false;
    }

    TiXmlDocument doc;
    doc.Parse(xmlBuffer.c_str());

    for (TiXmlNode* node = doc.FirstChild(); node; node = node->NextSibling()) {
        TiXmlElement* element = node->ToElement();
        if (element != nullptr) {
            if (strcmp(element->Value(), "menu") == 0) {
                menu.parse(element);
            } else if (strcmp(element->Value(), "general") == 0) {
                general.parse(element);
            } else if (strcmp(element->Value(), "cards") == 0) {
                cards.parse(element);
            } else if (strcmp(element->Value(), "game") == 0) {
                game.parse(element);
            } else if (strcmp(element->Value(), "cardgui") == 0) {
                cardgui.parse(element);
            }
        }
    }
    return true;
}

int ModRulesMenuItem::strToAction(const std::string& str) {
    if (str == "playMenu") {
        return MENUITEM_PLAY;
    }
    if (str == "deckEditor") {
        return MENUITEM_DECKEDITOR;
    }
    if (str == "shop") {
        return MENUITEM_SHOP;
    }
    if (str == "options") {
        return MENUITEM_OPTIONS;
    }
    if (str == "quit") {
        return MENUITEM_EXIT;
    }
    if (str == "trophies") {
        return MENUITEM_TROPHIES;
    }

    return MENUITEM_PLAY;
}

ModRulesMenuItem::ModRulesMenuItem(const std::string& actionIdStr, std::string displayName)
    : mActionId(strToAction(std::move(actionIdStr)))
    , mDisplayName(std::move(displayName)) {}

int ModRulesMenuItem::getMatchingGameState() const { return getMatchingGameState(mActionId); }

int ModRulesMenuItem::getMatchingGameState(int actionId) {
    switch (actionId) {
    case MENUITEM_DECKEDITOR:
        return GAME_STATE_DECK_VIEWER;
    case MENUITEM_SHOP:
        return GAME_STATE_SHOP;
    case MENUITEM_OPTIONS:
        return GAME_STATE_OPTIONS;
    case MENUITEM_TROPHIES:
        return GAME_STATE_AWARDS;
    default:
        return GAME_STATE_NONE;
    }
}

ModRulesMainMenuItem::ModRulesMainMenuItem(const std::string& actionIdStr,
                                           std::string displayName,
                                           int iconId,
                                           std::string particleFile)
    : ModRulesMenuItem(std::move(actionIdStr), std::move(displayName))
    , mIconId(iconId)
    , mParticleFile(std::move(particleFile)) {}

JButton ModRulesOtherMenuItem::strToJButton(const string& str) {
    if (str == "btn_next") {
        return JGE_BTN_NEXT;
    }
    if (str == "btn_prev") {
        return JGE_BTN_PREV;
    }
    if (str == "btn_ctrl") {
        return JGE_BTN_CTRL;
    }
    if (str == "btn_menu") {
        return JGE_BTN_MENU;
    }
    if (str == "btn_cancel") {
        return JGE_BTN_CANCEL;
    }
    if (str == "btn_pri") {
        return JGE_BTN_PRI;
    }
    if (str == "btn_sec") {
        return JGE_BTN_SEC;
    }

    return JGE_BTN_NEXT;
}

ModRulesOtherMenuItem::ModRulesOtherMenuItem(const std::string& actionIdStr,
                                             std::string displayName,
                                             const std::string& keyStr)
    : ModRulesMenuItem(std::move(actionIdStr), std::move(displayName))
    , mKey(strToJButton(std::move(keyStr))) {}

void ModRulesMenu::parse(TiXmlElement* element) {
    TiXmlNode* mainNode = element->FirstChild("main");
    if (mainNode) {
        for (TiXmlNode* node = mainNode->ToElement()->FirstChild("item"); node; node = node->NextSibling("item")) {
            TiXmlElement* element = node->ToElement();
            {
                main.push_back(NEW ModRulesMainMenuItem(element->Attribute("action"), element->Attribute("displayName"),
                                                        atoi(element->Attribute("iconId")),
                                                        element->Attribute("particleFile")));
            }
        }
    }

    TiXmlNode* otherNode = element->FirstChild("other");
    if (otherNode) {
        for (TiXmlNode* node = otherNode->ToElement()->FirstChild("item"); node; node = node->NextSibling("item")) {
            TiXmlElement* element = node->ToElement();
            if (element) {
                other.push_back(NEW ModRulesOtherMenuItem(
                    element->Attribute("action"), element->Attribute("displayName"), element->Attribute("key")));
            }
        }
    }
}

ModRulesMenu::~ModRulesMenu() {
    for (size_t i = 0; i < main.size(); ++i) {
        SAFE_DELETE(main[i]);
    }

    for (size_t i = 0; i < other.size(); ++i) {
        SAFE_DELETE(other[i]);
    }

    main.clear();
    other.clear();
}

// inGame rules
ModRulesGame::ModRulesGame() : mCanInterrupt(true) {}

void ModRulesGame::parse(TiXmlElement* element) {
    const int value = ModRules::getValueAsInt(element, "canInterrupt");
    if (value != -1) {
        mCanInterrupt = value > 0;
    }
}

// General Rules
ModRulesGeneral::ModRulesGeneral() : mHasDeckEditor(true), mHasShop(true) {}

void ModRulesGeneral::parse(TiXmlElement* element) {
    int value = ModRules::getValueAsInt(element, "hasDeckEditor");
    if (value != -1) {
        mHasDeckEditor = value > 0;
    }

    value = ModRules::getValueAsInt(element, "hasShop");
    if (value != -1) {
        mHasShop = value > 0;
    }
}

int ModRules::getValueAsInt(TiXmlElement* element, const std::string& childName) {
    TiXmlNode* node = element->FirstChild(childName.c_str());
    if (node) {
        const char* value = node->ToElement()->GetText();
        return atoi(value);
    }
    return -1;
}

ModRulesCards::ModRulesCards() : activateEffect(NEW SimpleCardEffectRotate(M_PI / 2)) {
    // Default activation effect
}

SimpleCardEffect* ModRulesCards::parseEffect(const std::string& s) {
    const size_t limiter = s.find('(');
    std::string function;
    std::string params;
    if (limiter != string::npos) {
        function = s.substr(0, limiter);
        params   = s.substr(limiter + 1, s.size() - 2 - limiter);
    } else {
        function = s;
    }

    if (function == "rotate") {
        return NEW SimpleCardEffectRotate(M_PI * atoi(params.c_str()) / 180);
    }

    if (function == "mask") {
        std::vector<std::string> argb = ::woth::split(params, ',');
        if (argb.size() < 4) {
            DebugTrace("not enough params in mask");
            return nullptr;
        }
        auto mask = ARGB(atoi(argb[0].c_str()), atoi(argb[1].c_str()), atoi(argb[2].c_str()), atoi(argb[3].c_str()));
        return NEW SimpleCardEffectMask(mask);
    }
    return nullptr;
}

void ModRulesCards::parse(TiXmlElement* element) {
    TiXmlNode* node = element->FirstChild("general");
    if (node) {
        TiXmlElement* generalElement = node->ToElement();
        TiXmlNode* nodeActivation    = generalElement->FirstChild("activate");
        if (nodeActivation) {
            TiXmlElement* activateElement = nodeActivation->ToElement();
            TiXmlNode* nodeUIEvent        = activateElement->FirstChild("uiEvent");
            if (nodeUIEvent) {
                const char* event = nodeUIEvent->ToElement()->GetText();
                SAFE_DELETE(activateEffect);
                activateEffect = parseEffect(event);
            }
        }
    }
}

ModRulesCards::~ModRulesCards() { SAFE_DELETE(activateEffect); }

ModRulesBackGroundCardGuiItem::ModRulesBackGroundCardGuiItem(const string& ColorId,
                                                             string ColorName,
                                                             string DisplayImg,
                                                             string DisplayThumb,
                                                             const string& MenuIcon)
    : mColorId(atoi(ColorId.c_str()))
    , MColorName(std::move(ColorName))
    , mDisplayImg(std::move(DisplayImg))
    , mDisplayThumb(std::move(DisplayThumb))
    , mMenuIcon(atoi(MenuIcon.c_str())) {}

ModRulesRenderCardGuiItem::ModRulesRenderCardGuiItem(string name,
                                                     int posX,
                                                     int posY,
                                                     string formattedData,
                                                     string filter,
                                                     bool font,
                                                     int fontSize,
                                                     PIXEL_TYPE fontColor,
                                                     int SizeIcon,
                                                     int IconPosX,
                                                     int IconPosY,
                                                     string FileName)
    : mName(std::move(name))
    , mFileName(std::move(FileName))
    , mFilter(std::move(filter))
    , mFont(font)
    , mFontColor(fontColor)
    , mFontSize(fontSize)
    , mFormattedData(std::move(formattedData))
    , mIconPosX(IconPosX)
    , mIconPosY(IconPosY)
    , mPosX(posX)
    , mPosY(posY)
    , mSizeIcon(SizeIcon) {}

void ModRulesCardGui::parse(TiXmlElement* element) {
    std::string _Name;
    int _Posx;
    int _Posy;
    std::string _FormattedText;
    std::string _Filter;
    int _FontSize;
    bool _Font;
    PIXEL_TYPE _FontColor;
    int _SizeIcon;
    int _IconPosX;
    int _IconPosY;
    std::string _FileName;

    TiXmlNode* mainNode = element->FirstChild("background");
    if (mainNode) {
        for (TiXmlNode* node = mainNode->ToElement()->FirstChild("card"); node; node = node->NextSibling("card")) {
            TiXmlElement* element = node->ToElement();
            {
                background.push_back(NEW ModRulesBackGroundCardGuiItem(
                    element->Attribute("id"), element->Attribute("color"), element->Attribute("img"),
                    element->Attribute("thumb"), element->Attribute("menuicon")));
            }
        }
    }
    mainNode = element->FirstChild("renderbig");
    if (mainNode) {
        TiXmlNode* ChildNode;
        for (TiXmlNode* node = mainNode->ToElement()->FirstChild("item"); node; node = node->NextSibling("item")) {
            _Name          = node->ToElement()->Attribute("name");
            _Posx          = 0;
            _Posy          = 0;
            _FormattedText = "";
            _Filter        = "";
            _FontSize      = 0;
            _Font          = false;
            _FontColor     = 0;
            _SizeIcon      = 0;
            _IconPosX      = 0;
            _IconPosY      = 0;
            _FileName      = "";

            TiXmlElement* ItemElement = node->ToElement();
            ChildNode                 = ItemElement->FirstChild("position");
            if (ChildNode) {
                _Posx = atoi(ChildNode->ToElement()->Attribute("x"));
                _Posy = atoi(ChildNode->ToElement()->Attribute("y"));
            }
            ChildNode = ItemElement->FirstChild("formattedtext");
            if (ChildNode) {
                _FormattedText = ChildNode->ToElement()->GetText();
            }
            ChildNode = ItemElement->FirstChild("filter");
            if (ChildNode) {
                _Filter = ChildNode->ToElement()->GetText();
            }

            ChildNode = ItemElement->FirstChild("font");
            if (ChildNode) {
                _Font                         = true;
                _FontSize                     = atoi(ChildNode->ToElement()->Attribute("size"));
                std::vector<std::string> argb = woth::split(ChildNode->ToElement()->Attribute("color"), ',');
                _FontColor =
                    ARGB(atoi(argb[0].c_str()), atoi(argb[1].c_str()), atoi(argb[2].c_str()), atoi(argb[3].c_str()));
            }
            ChildNode = ItemElement->FirstChild("iconposition");
            if (ChildNode) {
                _IconPosX = atoi(ChildNode->ToElement()->Attribute("x"));
                _IconPosY = atoi(ChildNode->ToElement()->Attribute("y"));
            }

            ChildNode = ItemElement->FirstChild("filename");
            if (ChildNode) {
                _FileName = ChildNode->ToElement()->GetText();
            }
            ChildNode = ItemElement->FirstChild("sizeicon");
            if (ChildNode) {
                _SizeIcon = atoi(ChildNode->ToElement()->GetText());
            }

            renderbig.push_back(NEW ModRulesRenderCardGuiItem(_Name, _Posx, _Posy, _FormattedText, _Filter, _Font,
                                                              _FontSize, _FontColor, _SizeIcon, _IconPosX, _IconPosY,
                                                              _FileName));
        }
    }
    mainNode = element->FirstChild("rendertinycrop");
    if (mainNode) {
        TiXmlNode* ChildNode;
        for (TiXmlNode* node = mainNode->ToElement()->FirstChild("item"); node; node = node->NextSibling("item")) {
            _Name          = node->ToElement()->Attribute("name");
            _Posx          = 0;
            _Posy          = 0;
            _FormattedText = "";
            _Filter        = "";
            _FontSize      = 0;
            _Font          = false;
            _FontColor     = 0;
            _SizeIcon      = 0;
            _IconPosX      = 0;
            _IconPosY      = 0;
            _FileName      = "";

            TiXmlElement* ItemElement = node->ToElement();
            ChildNode                 = ItemElement->FirstChild("position");
            if (ChildNode) {
                _Posx = atoi(ChildNode->ToElement()->Attribute("x"));
                _Posy = atoi(ChildNode->ToElement()->Attribute("y"));
            }
            ChildNode = ItemElement->FirstChild("formattedtext");
            if (ChildNode) {
                _FormattedText = ChildNode->ToElement()->GetText();
            }
            ChildNode = ItemElement->FirstChild("filter");
            if (ChildNode) {
                _Filter = ChildNode->ToElement()->GetText();
            }

            ChildNode = ItemElement->FirstChild("font");
            if (ChildNode) {
                _Font                         = true;
                _FontSize                     = atoi(ChildNode->ToElement()->Attribute("size"));
                std::vector<std::string> argb = ::woth::split(ChildNode->ToElement()->Attribute("color"), ',');
                _FontColor =
                    ARGB(atoi(argb[0].c_str()), atoi(argb[1].c_str()), atoi(argb[2].c_str()), atoi(argb[3].c_str()));
            }
            ChildNode = ItemElement->FirstChild("iconposition");
            if (ChildNode) {
                _IconPosX = atoi(ChildNode->ToElement()->Attribute("x"));
                _IconPosY = atoi(ChildNode->ToElement()->Attribute("y"));
            }

            ChildNode = ItemElement->FirstChild("filename");
            if (ChildNode) {
                _FileName = ChildNode->ToElement()->GetText();
            }
            ChildNode = ItemElement->FirstChild("sizeicon");
            if (ChildNode) {
                _SizeIcon = atoi(ChildNode->ToElement()->GetText());
            }

            rendertinycrop.push_back(NEW ModRulesRenderCardGuiItem(_Name, _Posx, _Posy, _FormattedText, _Filter, _Font,
                                                                   _FontSize, _FontColor, _SizeIcon, _IconPosX,
                                                                   _IconPosY, _FileName));
        }
    }
}

ModRulesCardGui::~ModRulesCardGui() {
    for (size_t i = 0; i < background.size(); ++i) {
        SAFE_DELETE(background[i]);
    }
    for (size_t i = 0; i < renderbig.size(); ++i) {
        SAFE_DELETE(renderbig[i]);
    }
    for (size_t i = 0; i < rendertinycrop.size(); ++i) {
        SAFE_DELETE(rendertinycrop[i]);
    }

    background.clear();
    renderbig.clear();
    rendertinycrop.clear();
}
