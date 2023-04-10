#ifndef OPTIONITEM_H
#define OPTIONITEM_H
/**
  @file OptionItem.h
  Includes classes and functionality related to the options menu.
*/
#include <JGui.h>
#include <utility>
#include <vector>
#include <string>
#include "GameApp.h"
#include "GameStateOptions.h"
#include "WFilter.h"
#include "WDataSrc.h"
#include "WGui.h"

using std::string;

#define MAX_OPTION_TABS 5
#define MAX_ONSCREEN_OPTIONS 8
#define OPTION_CENTER 4
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/**
  @defgroup WGuiOptions Options Gui
  @ingroup WGui
  @{
*/

/**
  The base class for all option menu items.
*/
class OptionItem : public WGuiItem {
public:
    OptionItem(int _id, string _displayValue);
    ~OptionItem() override{};

    /**
      Returns the index into ::options used to store and retrieve this option.
    */
    int getId() override { return id; }

    /**
      Changes the index into ::options used to store and retrieve this option.
    */
    void setId(int _id) override { id = _id; }

protected:
    int id;
};

/**
  A numeric option item. Can be decorated with WDecoEnum to provide a string representation of the numeric values.
*/
class OptionInteger : public OptionItem {
public:
    int value;          ///< Current value the option is displaying.
    int defValue;       ///< Default value for the option.
    string strDefault;  ///< What to call the default value in the menu.
    int maxValue;       ///< Maximum value of the option.
    int minValue;       ///< Minimum value of the option.
    int increment;      ///< Amount to increment the option by when clicked.

    OptionInteger(int _id, string _displayValue, int _maxValue = 1, int _increment = 1, int _defV = 0,
                  string _sDef = "", int _minValue = 0);

    void Reload() override {
        if (id != INVALID_OPTION) {
            value = options[id].number;
        }
    }

    bool Changed() override { return value != options[id].number; }

    void Render() override;
    void setData() override;
    void updateValue() override {
        value += increment;
        if (value > maxValue) {
            value = minValue;
        }
    }
};

/**
  An option represented as one of a set of strings.
*/
class OptionSelect : public OptionItem {
public:
    size_t value;               ///< Currently selected option, an index into selections.
    vector<string> selections;  ///< Vector containing all possible values.

    virtual void addSelection(string s);
    OptionSelect(int _id, string _displayValue) : OptionItem(_id, std::move(_displayValue)), value(0){};
    void Reload() override { initSelections(); };
    void Render() override;
    bool Selectable() override;
    void Entering(JButton key) override;
    bool Changed() override { return (value != prior_value); }

    void setData() override;
    virtual void initSelections();
    void updateValue() override {
        value++;
        if (value > selections.size() - 1) {
            value = 0;
        }
    };

protected:
    size_t prior_value;  ///< The prior selected value, in case a change is cancelled.
};

/**
  An option representing possible languages. Automatically loads the list of possibilities from the lang/ folder.
*/
class OptionLanguage : public OptionSelect {
public:
    OptionLanguage(string _displayValue);

    void addSelection(string s) override { addSelection(s, s); };
    virtual void addSelection(string s, string show);
    void initSelections() override;
    void confirmChange(bool confirmed) override;
    void Reload() override;
    bool Visible() override;
    bool Selectable() override;
    void setData() override;

protected:
    vector<string> actual_data;  ///< An array containing the actual value we set the option to, rather than the display
                                 ///< value in selections.
};

/**
  An option representing possible theme substyles. Automatically loads the list of possibilities from the current theme.
*/
class OptionThemeStyle : public OptionSelect {
public:
    bool Visible() override;
    void Reload() override;
    void confirmChange(bool confirmed) override;
    OptionThemeStyle(string _displayValue);
};

/**
  An option allowing the user to choose a directory, provided it contains a certain file.
*/
class OptionDirectory : public OptionSelect {
public:
    void Reload() override;
    OptionDirectory(const string& root, int id, string displayValue, const string& type);

protected:
    const string root;  ///< The root directory to search for subdirectories.
    const string type;  ///< The file to check for in a useable subdirectory.
};
/**
  An option allowing the player to choose a theme directory. Requires that the theme directory contains a preview.png.
*/
class OptionTheme : public OptionDirectory {
private:
    static const string DIRTESTER;  ///< A particular file to look for when building the list of possible directories.
public:
    OptionTheme(OptionThemeStyle* style = nullptr);
    JQuadPtr getImage();
    void updateValue() override;
    float getHeight() override;
    void Render() override;
    void confirmChange(bool confirmed) override;
    bool Visible() override;

protected:
    OptionThemeStyle* ts;  ///< The current theme style.
    string author;         ///< The theme author
    bool bChecked;         ///< Whether or not the theme has been checked for metadata
};

/**
  An option allowing the player to choose a profile directory. Requires that the profile directory contains a
  collection.dat.
*/
class OptionProfile : public OptionDirectory {
private:
    static const string DIRTESTER;  ///< A particular file to look for when building the list of possible directories.
public:
    OptionProfile(GameApp* _app, JGuiListener* jgl);
    void addSelection(string s) override;
    bool Selectable() override { return canSelect; };
    bool Changed() override { return (initialValue != value); };
    void Entering(JButton key) override;
    void Reload() override;
    void Render() override;
    void initSelections() override;
    void confirmChange(bool confirmed) override;
    void updateValue() override;
    void populate();

private:
    GameApp* app;
    JGuiListener* listener;
    bool canSelect;
    string preview;
    size_t initialValue;
};

/**
  An option allowing the player to bind a key to a specific interaction.
*/
class OptionKey : public WGuiItem, public KeybGrabber {
public:
    OptionKey(GameStateOptions* g, LocalKeySym, JButton);
    LocalKeySym from;
    JButton to;
    void Render() override;
    void Update(float) override;
    void Overlay() override;
    bool CheckUserInput(JButton key) override;
    void KeyPressed(LocalKeySym key) override;
    bool isModal() override;
    void ButtonPressed(int controllerId, int controlId) override;
    bool Visible() override;
    bool Selectable() override;

protected:
    bool grabbed;
    GameStateOptions* g;
    SimpleMenu* btnMenu;
};

/**@} This comment used by Doxyyen. */
#endif
