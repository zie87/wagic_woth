#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include <map>
#include <string>

using std::map;
using std::string;

#include <JGE.h>
#include <time.h>
#include "SimplePad.h"

#define GLOBAL_SETTINGS "settings/options.txt"
#define PLAYER_SAVEFILE "data.dat"
#define PLAYER_SETTINGS "options.txt"
#define PLAYER_COLLECTION "collection.dat"
#define PLAYER_TASKS "tasks.dat"
#define SECRET_PROFILE "Maxglee"

#define INVALID_OPTION -1

class WStyle;
class StyleManager;
class Player;
class GameApp;

class Options {
public:
    friend class GameSettings;
    enum {
        // Global settings
        ACTIVE_PROFILE,
        LANG,
        LAST_GLOBAL = LANG,  // This must be the value above, to keep ordering.
                             // Values /must/ match ordering in optionNames, or everything loads wrong.
                             // Profile settings
        ACTIVE_THEME,
        ACTIVE_MODE,
        MUSICVOLUME,
        SFXVOLUME,
        DIFFICULTY,
        CHEATMODE,
        OPTIMIZE_HAND,
        CHEATMODEAIDECK,
        OSD,
        CLOSEDHAND,
        HANDDIRECTION,
        MANADISPLAY,
        REVERSETRIGGERS,
        DISABLECARDS,
        MAX_GRADE,
        ASPHASES,
        FIRSTPLAYER,
        KICKERPAYMENT,
        ECON_DIFFICULTY,
        TRANSITIONS,
        GUI_STYLE,
        INTERRUPT_SECONDS,
        KEY_BINDINGS,
        AIDECKS_UNLOCKED,
        // My interrupts
        INTERRUPTMYSPELLS,
        INTERRUPTMYABILITIES,
        SAVEDETAILEDDECKINFO,
        // Other interrupts
        INTERRUPT_BEFOREBEGIN,
        INTERRUPT_UNTAP,
        INTERRUPT_UPKEEP,
        INTERRUPT_DRAW,
        INTERRUPT_FIRSTMAIN,
        INTERRUPT_BEGINCOMBAT,
        INTERRUPT_ATTACKERS,
        INTERRUPT_BLOCKERS,
        INTERRUPT_DAMAGE,
        INTERRUPT_ENDCOMBAT,
        INTERRUPT_SECONDMAIN,
        INTERRUPT_ENDTURN,
        INTERRUPT_CLEANUP,
        INTERRUPT_AFTEREND,
        BEGIN_AWARDS,  // Options after this use the GameOptionAward struct, which includes a timestamp.
        DIFFICULTY_MODE_UNLOCKED = BEGIN_AWARDS,
        EVILTWIN_MODE_UNLOCKED,
        RANDOMDECK_MODE_UNLOCKED,
        AWARD_COLLECTOR,
        LAST_NAMED,                    // Any option after this does not look up in optionNames.
        SET_UNLOCKS = LAST_NAMED + 1,  // For sets.
    };

    static int optionSet(int setID);
    static int optionInterrupt(int gamePhase);

    static int getID(std::string name);
    static std::string getName(int option);

private:
    static const std::string optionNames[];
};

class GameOption {
public:
    virtual ~GameOption() {}

    int number;
    std::string str;
    // All calls to asColor should include a fallback color for people without a theme.
    PIXEL_TYPE asColor(PIXEL_TYPE fallback = ARGB(255, 255, 255, 255));

    virtual bool isDefault();       // Returns true when number is 0 and std::string is "" or "Default"
    virtual std::string menuStr();  // The std::string we'll use for GameStateOptions.
    virtual bool write(std::ofstream* file, std::string name);
    virtual bool read(std::string input);

    GameOption(int value = 0);
    GameOption(const std::string&);
    GameOption(int, std::string);
};

struct EnumDefinition {
    int findIndex(int value);

    typedef std::pair<int, std::string> assoc;
    std::vector<assoc> values;
};

class GameOptionEnum : public GameOption {
public:
    std::string menuStr() override;
    bool write(std::ofstream* file, std::string name) override;
    bool read(std::string input) override;
    EnumDefinition* def;
};

class GameOptionAward : public GameOption {
public:
    GameOptionAward();
    std::string menuStr() override;
    bool write(std::ofstream* file, std::string name) override;
    bool read(std::string input) override;
    virtual bool giveAward();  // Returns false if already awarded
    virtual bool isViewed();

    virtual void setViewed(bool v = true) { viewed = v; }

private:
    time_t achieved;  // When was it awarded?
    bool viewed;      // Flag it as "New!" or not.
};

class GameOptionKeyBindings : public GameOption {
    bool read(std::string input) override;
    bool write(std::ofstream*, std::string) override;
};

class OptionVolume : public EnumDefinition {
public:
    enum { MUTE = 0, MAX = 100 };

    static EnumDefinition* getInstance() { return &mDef; }

private:
    OptionVolume();
    static OptionVolume mDef;
};

class OptionClosedHand : public EnumDefinition {
public:
    enum { INVISIBLE = 0, VISIBLE = 1 };

    static EnumDefinition* getInstance() { return &mDef; }

private:
    OptionClosedHand();
    static OptionClosedHand mDef;
};

class OptionHandDirection : public EnumDefinition {
public:
    enum { VERTICAL = 0, HORIZONTAL = 1 };

    static EnumDefinition* getInstance() { return &mDef; }

private:
    OptionHandDirection();
    static OptionHandDirection mDef;
};

class OptionManaDisplay : public EnumDefinition {
public:
    enum { DYNAMIC = 0, STATIC = 1, NOSTARSDYNAMIC = 2, BOTH = 3 };

    static EnumDefinition* getInstance() { return &mDef; }

private:
    OptionManaDisplay();
    static OptionManaDisplay mDef;
};

class OptionMaxGrade : public EnumDefinition {
public:
    static EnumDefinition* getInstance() { return &mDef; }

private:
    OptionMaxGrade();
    static OptionMaxGrade mDef;
};

class OptionASkipPhase : public EnumDefinition {
public:
    static EnumDefinition* getInstance() { return &mDef; }

private:
    OptionASkipPhase();
    static OptionASkipPhase mDef;
};

class OptionWhosFirst : public EnumDefinition {
public:
    enum { WHO_P = 0, WHO_O = 1, WHO_R = 2 };

    static EnumDefinition* getInstance() { return &mDef; }

private:
    OptionWhosFirst();
    static OptionWhosFirst mDef;
};

class OptionKicker : public EnumDefinition {
public:
    enum {
        KICKER_ALWAYS = 0,
        KICKER_CHOICE = 1,
    };

    static EnumDefinition* getInstance() { return &mDef; }

private:
    OptionKicker();
    static OptionKicker mDef;
};

class OptionEconDifficulty : public EnumDefinition {
public:
    static EnumDefinition* getInstance() { return &mDef; }

private:
    OptionEconDifficulty();
    static OptionEconDifficulty mDef;
};

class OptionDifficulty : public EnumDefinition {
public:
    enum { NORMAL = 0, HARD = 1, HARDER = 2, EVIL = 3 };

    static EnumDefinition* getInstance() { return &mDef; }

private:
    OptionDifficulty();
    static OptionDifficulty mDef;
};

class GameOptions {
public:
    std::string mFilename;
    int save();
    int load();

    GameOption* get(int);
    GameOption* get(const std::string& optionName);
    GameOption& operator[](int);
    GameOption& operator[](const std::string&);
    GameOptions(std::string filename);
    ~GameOptions();

private:
    std::vector<GameOption*> values;
    std::map<std::string, GameOption*> unknownMap;
    GameOption* factorNewGameOption(const std::string& optionName, const std::string& value = "");
};

class GameSettings {
public:
    friend class GameApp;

    GameSettings();
    ~GameSettings();
    int save();

    SimplePad* keypadStart(std::string input,
                           std::string* _dest = nullptr,
                           bool _cancel       = true,
                           bool _numpad       = false,
                           float _x           = SCREEN_WIDTH_F / 2,
                           float _y           = SCREEN_HEIGHT_F / 2);
    std::string keypadFinish();
    void keypadShutdown();
    void keypadTitle(std::string set);

    bool keypadActive() {
        if (keypad) {
            {
                return keypad->isActive();
            }
        }

        return false;
    }

    void keypadUpdateText(unsigned char key) {
        if (keypad) {
            switch (key) {
            case 1:  // save the current text
                keypad->pressKey(key);
                break;
            case 10:  // cancel the edit
                keypad->CancelEdit();
                break;
            case 32:
                keypad->pressKey(KPD_SPACE);
                break;
            case 127:
                keypad->pressKey(KPD_DEL);
                break;
            default:
                keypad->pressKey(key);
                break;
            }
        }
    }

    void keypadUpdate(float dt) {
        if (keypad) {
            {
                keypad->Update(dt);
            }
        }
    }

    void keypadRender() {
        if (keypad) {
            {
                keypad->Render();
            }
        }
    }

    bool newAward() const;

    // These return a filepath accurate to the current mode/profile/theme, and can
    // optionally fallback to a file within a certain directory.
    // The sanity=false option returns the adjusted path even if the file doesn't exist.
    std::string profileFile(const std::string& filename = "",
                            const std::string& fallback = "",
                            bool sanity                 = false) const;

    void reloadProfile();  // Reloads profile using current options[ACTIVE_PROFILE]
    void checkProfile();   // Confirms that a profile is loaded and contains a collection.
    void createUsersFirstDeck(int setId);

    GameOption* get(int) const;
    GameOption& operator[](int) const;
    GameOption& operator[](const std::string&) const;

    GameOptions* profileOptions;
    GameOptions* globalOptions;

    static GameOption invalid_option;

    WStyle* getStyle();
    StyleManager* getStyleMan();
    void automaticStyle(Player* p1, Player* p2);

private:
    GameApp* theGame;
    SimplePad* keypad;

    StyleManager* styleMan;
    void createProfileFolders() const;
};

extern GameSettings options;

#endif
