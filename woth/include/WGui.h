#ifndef WGUI_H
#define WGUI_H
/**
  @file WFilter.h
  Includes classes and functionality related to card filtering.
*/
#include <set>
#include <utility>

class hgeDistortionMesh;
class GameStateOptions;

/**
  @defgroup WGui Basic Gui
  @{
*/

/**
  Color definition groups. Used to group text and background areas of similar purpose,
  so that their color need only be defined once.
*/
class WGuiColor {
public:
    enum {
        SCROLLBAR,
        SCROLLBUTTON,
        // Foregrounds only after this
        TEXT,
        TEXT_HEADER,
        TEXT_FAIL,
        TEXT_TAB,
        TEXT_BODY,
        // Backgrounds only after this
        BACK,
        BACK_ALERT,
        BACK_HEADER,
        BACK_FAIL,
        BACK_TAB,
    };
};

/**
  Quad distortion structure. Stores the modified x and y positions for all four corners of a quad.
*/
struct WDistort {
    WDistort();
    WDistort(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    float& operator[](int p);

protected:
    float xy[8];
};

/**
  Base class for all GUI item classes.
*/
class WGuiBase : public JGuiListener {
public:
    typedef enum {
        CONFIRM_NEED,    ///< Still needs confirmation
        CONFIRM_OK,      ///< Is okay (no need to confirm, or has been confirmed)
        CONFIRM_CANCEL,  ///< Is not okay, must cancel save
    } CONFIRM_TYPE;

    WGuiBase(){};
    ~WGuiBase() override{};

    /**
     If false, the option will be skipped over when moving the selection cursor.
    */
    virtual bool Selectable() { return true; };
    /**
      If true, the item overrides the button handling of the WGuiMenu classes. See
      WGuiMenu::CheckUserInput() for an example of modality implementation.
    */
    virtual bool isModal() { return false; };
    /**
      If false, the item will not render, and any lists will contract as if it weren't there. Meant to be
      overridden in subclasses so that visibility is conditional on some function.
    */
    virtual bool Visible() { return true; };
    /**
      Returns true when the underlying data that this item represents has been changed by user interaction.
      This is used to help WDecoConfirm determine if a confirmation dialog is needed or not.
    */
    virtual bool Changed() { return false; };
    /**
      In cases where a WDecoConfirm dialog is used, it is meant to be overridden with an implementation
      that then applies the change to the underlying data. See the OptionProfile for an example.
      Note: This is ONLY called after the user presses "OK" on a WDecoConfirm dialog. See setData()
      for the standard method of changing underlying data.
    */
    virtual void confirmChange(bool confirmed){};
    /**
      Returns whether or not any changes to this item would require confirmation. Can also be used to
      validate those changes, or to perform necessary cleanup when a change fails.
    */
    virtual CONFIRM_TYPE needsConfirm();
    virtual bool yieldFocus();
    virtual PIXEL_TYPE getColor(int type);
    virtual float getMargin(int type) { return 4; };

    /**
      What to do when the selection cursor enters the item.
      @param key The key pressed to enter this item.
    */
    virtual void Entering(JButton key) = 0;
    /**
      Request permission to leave the item. If the return value is false, the item remains selected.
      @param key The key pressed to leave this item.
    */
    virtual bool Leaving(JButton key) = 0;

    virtual void Update(float dt) = 0;
    /**
      Called when the item is selected and the OK button is pressed. Generally used to
      change the visible notification of the selected item, but not to change the underlying data.
      For example, the OptionTheme class reacts to a button press by changing the selected theme,
      but does not actually apply the theme.
    */
    virtual void updateValue(){};
    virtual void Render() = 0;
    /**
      Used to change the underlying data this gui element represents.
    */
    virtual void setData() = 0;

    void ButtonPressed(int controllerId, int controlId) override{};
    /**
      Used when it is necessary to update some information. Often called from confirmChange(), but also called
      in other places, such as to reload the list of possible profiles after a new one is created. See OptionProfile
      for details.
    */
    virtual void Reload(){};

    /**
      Render something after (and thus on top of) the regular Render() call.
    */
    virtual void Overlay(){};
    /**
      Render something before (and thus under) the regular Render() call.
    */
    virtual void Underlay(){};

    /**
      Returns true if the object currently has the focus.
    */
    virtual bool hasFocus() = 0;
    /**
      Sets whether or not the object has the focus.
    */
    virtual void setFocus(bool bFocus) = 0;
    virtual float getX()               = 0;
    virtual float getY()               = 0;
    virtual float getWidth()           = 0;
    virtual float getHeight()          = 0;
    virtual int getId() { return INVALID_ID; };
    virtual string getDisplay() const { return ""; };
    virtual float minWidth() { return getWidth(); };
    virtual float minHeight() { return getHeight(); };

    /** Sets the modality of the item, if applicable. */
    virtual void setModal(bool val){};
    virtual void setDisplay(string s){};
    virtual void setX(float _x){};
    virtual void setY(float _y){};
    virtual void setWidth(float _w){};
    virtual void setHeight(float _h){};
    virtual void setId(int _id){};
    virtual void setHidden(bool bHidden){};
    virtual void setVisible(bool bVisisble){};
    virtual void renderBack(WGuiBase* it);
    virtual void subBack(WGuiBase* item){};

    virtual bool CheckUserInput(JButton key) { return false; };

protected:
    vector<WGuiBase*> items;
};

/**
  Base class for all GUI concrete implementation classes.
*/
class WGuiItem : public WGuiBase {
public:
    void Entering(JButton key) override;
    bool Leaving(JButton key) override;
    bool CheckUserInput(JButton key) override;
    void Update(float dt) override{};
    void Render() override;

    WGuiItem(string _display, u8 _mF = 0);
    ~WGuiItem() override{};

    string _(string input) const;  // Override global with our flag checker.

    void setData() override{};

    bool hasFocus() override { return mFocus; };
    void setFocus(bool bFocus) override { mFocus = bFocus; };

    string getDisplay() const override { return displayValue; };
    void setDisplay(string s) override { displayValue = s; };

    int getId() override { return INVALID_ID; };
    float getX() override { return x; };
    float getY() override { return y; };
    float getWidth() override { return width; };
    float getHeight() override { return height; };
    float minWidth() override;
    float minHeight() override;
    void setId(int _id) override{};
    void setX(float _x) override { x = _x; };
    void setY(float _y) override { y = _y; };
    void setWidth(float _w) override { width = _w; };
    void setHeight(float _h) override { height = _h; };
    enum {
        NO_TRANSLATE = (1 << 1),
    };

    u8 mFlags;

protected:
    bool mFocus;
    float x, y;
    float width, height;
    string displayValue;
};

/**
  An image drawn from the current position in a WDataSource.
*/
class WGuiImage : public WGuiItem {
public:
    WGuiImage(WDataSource* wds, float _w = 0, float _h = 0, int _margin = 0);
    bool Selectable() override { return false; };
    void Render() override;
    float getHeight() override;
    virtual void imageScale(float _w, float _h);
    virtual void setSource(WDataSource* s) { source = s; };

protected:
    int margin;
    float imgW, imgH;
    WDataSource* source;
};

/**
  A card image drawn from the current position in a WDataSource.
*/
class WGuiCardImage : public WGuiImage {
public:
    WGuiCardImage(WDataSource* wds, bool _thumb = false);
    void Render() override;
    WSyncable mOffset;

protected:
    bool bThumb;
};

/**
  A variation of the WGuiCardImage that is distorted.
*/
class WGuiCardDistort : public WGuiCardImage {
public:
    WGuiCardDistort(WDataSource* wds, bool _thumb = false, WDataSource* _distort = nullptr);
    ~WGuiCardDistort() override;
    void Render() override;
    WDistort xy;
    /* we assume first xy is the top left of the distorted card */
    float getX() override { return xy[0]; };
    float getY() override { return xy[1]; };

protected:
    hgeDistortionMesh* mesh;
    WDataSource* distortSrc;
};

/**
  Base decorator class, wraps all WGuiBase functionality and forwards it to the decorated item.
*/
class WGuiDeco : public WGuiBase {
public:
    WGuiDeco(WGuiBase* _it) : it(_it){};
    ~WGuiDeco() override { SAFE_DELETE(it); };

    bool Selectable() override { return it->Selectable(); };
    bool Visible() override { return it->Visible(); };
    bool Changed() override { return it->Changed(); };
    void confirmChange(bool confirmed) override { it->confirmChange(confirmed); };
    CONFIRM_TYPE needsConfirm() override { return it->needsConfirm(); };
    bool yieldFocus() override { return it->yieldFocus(); };

    void Entering(JButton key) override { it->Entering(key); };
    bool Leaving(JButton key) override { return it->Leaving(key); };
    void Update(float dt) override { it->Update(dt); };
    void updateValue() override { it->updateValue(); };
    void Reload() override { it->Reload(); };
    void Overlay() override { it->Overlay(); };
    void Underlay() override { it->Underlay(); };
    void Render() override { it->Render(); };
    void setData() override { it->setData(); };

    void ButtonPressed(int controllerId, int controlId) override { it->ButtonPressed(controllerId, controlId); };

    bool hasFocus() override { return it->hasFocus(); };
    string getDisplay() const override { return it->getDisplay(); };
    int getId() override { return it->getId(); };
    float getX() override { return it->getX(); };
    float getY() override { return it->getY(); };
    float getWidth() override { return it->getWidth(); };
    float getHeight() override { return it->getHeight(); };
    PIXEL_TYPE getColor(int type) override { return it->getColor(type); };
    WGuiBase* getDecorated() { return it; };

    void setFocus(bool bFocus) override { it->setFocus(bFocus); };
    void setDisplay(string s) override { it->setDisplay(s); };
    void setId(int _id) override { it->setId(_id); };
    void setX(float _x) override { it->setX(_x); };
    void setY(float _y) override { it->setY(_y); };
    void setWidth(float _w) override { it->setWidth(_w); };
    void setHeight(float _h) override { it->setHeight(_h); };
    void setHidden(bool bHidden) override { it->setHidden(bHidden); };
    void setVisible(bool bVisisble) override { it->setVisible(bVisisble); };
    bool CheckUserInput(JButton key) override { return it->CheckUserInput(key); };

protected:
    WGuiBase* it;
};

/**
  An item used to represent an award, can also function as a button for more details.
  Visibility and selectability are dependent on whether or not the award has been unlocked.
*/
class WGuiAward : public WGuiItem {
public:
    WGuiAward(int _id, string name, string _text, string _details = "");
    WGuiAward(string _id, string name, string _text, string _details = "");
    ~WGuiAward() override;
    void Render() override;
    bool Selectable() override { return Visible(); };
    bool Visible() override;
    int getId() override { return id; };
    void Underlay() override;
    void Overlay() override;

protected:
    string details;
    int id;
    string textId;
    string text;
};

/**
  When the decorated items are members of a WGuiList, causes them to render in two columns.
*/
class WGuiSplit : public WGuiItem {
public:
    WGuiSplit(WGuiBase* _left, WGuiBase* _right);
    ~WGuiSplit() override;

    bool yieldFocus() override;
    void Reload() override;
    void Overlay() override;
    void Underlay() override;
    void setData() override;
    bool isModal() override;
    void setModal(bool val) override;
    void Render() override;
    void Update(float dt) override;
    void setX(float _x) override;
    void setY(float _y) override;
    void setWidth(float _w) override;
    void setHeight(float _h) override;
    float getHeight() override;
    void ButtonPressed(int controllerId, int controlId) override;
    void confirmChange(bool confirmed) override;

    void Entering(JButton key) override;
    bool Leaving(JButton key) override;
    bool CheckUserInput(JButton key) override;

    bool bRight;
    float percentRight;
    WGuiBase* right;
    WGuiBase* left;
};

/**
  Causes a confirmation dialog to pop up when the decorated item is changed.
*/
class WDecoConfirm : public WGuiDeco {
public:
    WDecoConfirm(JGuiListener* _listener, WGuiBase* it);
    ~WDecoConfirm() override;

    bool isModal() override;
    void setData() override;
    void setModal(bool val) override;
    void Entering(JButton key) override;
    bool Leaving(JButton key) override;
    void Update(float dt) override;
    void Overlay() override;
    void ButtonPressed(int controllerId, int controlId) override;
    bool CheckUserInput(JButton key) override;

    string confirm;
    string cancel;

protected:
    enum {
        OP_UNCONFIRMED,
        OP_CONFIRMING,
        OP_CONFIRMED,
    } mState;

    SimpleMenu* confirmMenu;
    JGuiListener* listener;
    bool bModal;
};

/**
  Decorator for numeric values, transforms them into meaningful strings. Used by options
  that have an enum or similar as their underlying representation. Requires an EnumDefinition
  so it knows what value maps to what string.
*/
class WDecoEnum : public WGuiDeco {
public:
    WDecoEnum(WGuiBase* _it, EnumDefinition* _edef = nullptr);
    void Render() override;
    string lookupVal(int value);

protected:
    EnumDefinition* edef;
};

/**
  Makes the decorated item's visibility contingent on the player cheating.
*/
class WDecoCheat : public WGuiDeco {
public:
    WDecoCheat(WGuiBase* _it);
    bool Visible() override;
    bool Selectable() override;
    void Reload() override;

protected:
    bool bVisible;
};

/**
  Allows the decorated item to send a button notification to a JGuiListener.
*/
class WGuiButton : public WGuiDeco {
public:
    WGuiButton(WGuiBase* _it, int _controller, int _control, JGuiListener* jgl);
    void updateValue() override;
    bool CheckUserInput(JButton key) override;
    bool Selectable() override { return Visible(); };
    PIXEL_TYPE getColor(int type) override;
    virtual int getControlID() { return control; };
    virtual int getControllerID() { return controller; };

protected:
    int control, controller;
    JGuiListener* mListener;
};

/**
  Similar to an HTML heading, this displays text without any other functionality.
*/
class WGuiHeader : public WGuiItem {
public:
    WGuiHeader(string _displayValue) : WGuiItem(std::move(_displayValue)){};
    bool Selectable() override { return false; };
    void Render() override;
};

/**
  Allows the application of a certain color style in the decorated class.
*/
class WDecoStyled : public WGuiDeco {
public:
    WDecoStyled(WGuiItem* _it) : WGuiDeco(_it), mStyle(DS_DEFAULT){};
    PIXEL_TYPE getColor(int type) override;
    void subBack(WGuiBase* item) override;
    enum {
        DS_DEFAULT        = (1 << 0),
        DS_COLOR_BRIGHT   = (1 << 1),
        DS_COLOR_DARK     = (1 << 2),
        DS_STYLE_ALERT    = (1 << 3),
        DS_STYLE_EDGED    = (1 << 4),
        DS_STYLE_BACKLESS = (1 << 5),
    };

    u8 mStyle;
};

/**
  Base class for menu GUIs. Provides useful functionality: basic 1-dimensional item selection
  and the ability to sync the current index of a WSyncable with the menu's selected index
  (to allow the menu to easily iterate through arrays of data). Items are rendered at the
  X/Y position of the individual WGuiItem itself.
*/
class WGuiMenu : public WGuiItem {
public:
    friend class WGuiFilters;
    ~WGuiMenu() override;
    WGuiMenu(JButton next      = JGE_BTN_RIGHT,
             JButton prev      = JGE_BTN_LEFT,
             bool mDPad        = false,
             WSyncable* syncme = nullptr);

    bool yieldFocus() override;
    void Render() override;
    void Reload() override;
    void Update(float dt) override;
    void ButtonPressed(int controllerId, int controlId) override;
    virtual void Add(WGuiBase* item);  // Remember, does not set X & Y of items automatically.
    void confirmChange(bool confirmed) override;
    bool Leaving(JButton key) override;
    void Entering(JButton key) override;
    void subBack(WGuiBase* item) override;
    bool CheckUserInput(JButton key) override;
    WGuiBase* Current();
    virtual int getSelected() { return currentItem; };
    virtual void setSelected(vector<WGuiBase*>::iterator& it) {
        const int c = it - items.begin();
        setSelected(c);
    };
    virtual void setSelected(int newItem);
    virtual bool nextItem();
    virtual bool prevItem();
    bool isModal() override;
    void setModal(bool val) override;

    void setData() override;

protected:
    virtual void syncMove();
    virtual bool isButtonDir(JButton key, int dir);  // For the DPad override.
    JButton buttonNext, buttonPrev;
    bool mDPad;
    int currentItem;
    JButton held;
    WSyncable* sync;
    float duration;
};

/**
  Creates a vertically scrolling menu of items. Items are rendered based on their
  position within the items vector, and disregard the X/Y position of the individual
  WGuiItem itself.
*/
class WGuiList : public WGuiMenu {
public:
    WGuiList(std::string name, WSyncable* syncme = nullptr);

    std::string failMsg;

    void Render() override;
    void confirmChange(bool confirmed) override;
    void ButtonPressed(int controllerId, int controlId) override;
    void setData() override;
    WGuiBase* operator[](int);
    bool CheckUserInput(JButton key) override;

protected:
    bool mFocus;
    int startWindow;
    int endWindow;
};

/**
  Creates a horizontal menu of items, each of which is usually a WGuiMenu derivative
  themselves. Items are rendered based on their position within the items vector, and
  disregard the X/Y position of the individual WGuiItem itself.
*/
class WGuiTabMenu : public WGuiMenu {
public:
    WGuiTabMenu() : WGuiMenu(JGE_BTN_NEXT, JGE_BTN_PREV){};
    void Render() override;
    void Add(WGuiBase* it) override;
    void save();
    bool CheckUserInput(JButton key) override;
};
/**
  A variant of WGuiList that renders as a horizontal row, rather than vertically.
*/
class WGuiListRow : public WGuiList {
public:
    WGuiListRow(std::string n, WSyncable* s = nullptr);
    void Render() override;
};

/**
  The filter building interface.
*/
class WGuiFilters : public WGuiItem {
public:
    friend class WGuiFilterItem;
    WGuiFilters(std::string header, WSrcCards* src);
    ~WGuiFilters() override;
    bool CheckUserInput(JButton key) override;
    std::string getCode();  // For use in filter factory.
    void Update(float dt) override;
    void Render() override;
    void Entering(JButton key) override;
    void addColumn();
    void recolorFilter(int color);
    bool isAvailable(int type);
    bool isAvailableCode(const std::string& code);
    bool Finish(bool emptyset = false);  // Returns true if card set reasonably expected to be changed.
    bool isFinished() const { return bFinished; };
    void ButtonPressed(int controllerId, int controlId) override;
    void buildList();
    void setSrc(WSrcCards* wsc);

protected:
    void clearArgs();
    void addArg(const std::string& display, const std::string& code);
    std::vector<std::pair<std::string, std::string> > tempArgs;  // TODO FIXME this is inefficient
    bool bFinished;
    int recolorTo;
    WSrcCards* source;
    SimpleMenu* subMenu;
    WGuiList* list;
};

/**
  An interface item representing a single component of a filter.
*/
class WGuiFilterItem : public WGuiItem {
public:
    friend class WGuiFilters;
    WGuiFilterItem(WGuiFilters* parent);
    void updateValue() override;
    void ButtonPressed(int controllerId, int controlId) override;
    std::string getCode();
    bool isModal() override;
    enum {
        STATE_UNSET,
        STATE_CHOOSE_TYPE,
        STATE_CHOOSE_VAL,
        STATE_FINISHED,
        STATE_REMOVE,
        STATE_CANCEL,
        BEGIN_FILTERS = 0,
        FILTER_SET    = BEGIN_FILTERS,
        FILTER_ALPHA,
        FILTER_RARITY,
        FILTER_COLOR,
        FILTER_PRODUCE,
        FILTER_TYPE,
        FILTER_SUBTYPE,
        FILTER_BASIC,
        FILTER_CMC,
        FILTER_POWER,
        FILTER_TOUGH,
        END_FILTERS
    };

protected:
    std::string mCode;
    int filterType;
    int filterVal;
    int mState;
    bool mNew;
    WGuiFilters* mParent;
};

/**
  Used by the options menu for keybindings. This WGuiList derivative automatically populates itself
  with OptionKey items representing all the potential interaction bindings.
*/
class WGuiKeyBinder : public WGuiList {
public:
    WGuiKeyBinder(std::string name, GameStateOptions* parent);
    bool isModal() override;
    bool CheckUserInput(JButton) override;
    void setData() override;
    void Update(float) override;
    void Render() override;
    CONFIRM_TYPE needsConfirm() override;
    void ButtonPressed(int controllerId, int controlId) override;
    bool yieldFocus() override;

protected:
    GameStateOptions* parent;
    SimpleMenu* confirmMenu;
    bool modal;
    CONFIRM_TYPE confirmed;
    LocalKeySym confirmingKey;
    JButton confirmingButton;
    std::set<LocalKeySym> confirmedKeys;
    std::set<JButton> confirmedButtons;
    string confirmationString;
};

/**@} This comment used by Doxyyen. */
#endif
