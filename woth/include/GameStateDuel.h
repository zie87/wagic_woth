#ifndef GAMESTATEDUEL_H
#define GAMESTATEDUEL_H

#include "GameState.h"
#include "SimpleMenu.h"
#include "SimplePopup.h"
#include "DeckMenu.h"
#include "MTGDeck.h"
#include "GameObserver.h"
#ifdef AI_CHANGE_TESTING
#include "Threading.h"
#endif  // AI_CHANGE_TESTING

#define CHOOSE_OPPONENT 7

#ifdef TESTSUITE
class TestSuite;
#endif
class Credits;
#ifdef NETWORK_SUPPORT
class JNetwork;
#endif

class GameStateDuel : public GameState, public JGuiListener {
private:
#ifdef TESTSUITE
    TestSuite* testSuite;
#endif

    Credits* credits;
    int mGamePhase;
    Player* mCurrentPlayer;
    GameObserver* game;
    DeckMenu* deckmenu;
    DeckMenu* opponentMenu;
    SimpleMenu* menu;
    SimplePopup* popupScreen;  // used for informational screens, modal
    static int selectedPlayerDeckId;
    static int selectedAIDeckId;

    bool premadeDeck;
    int OpponentsDeckid;
    string musictrack;

    bool MusicExist(const string& FileName);
    void ConstructOpponentMenu();  // loads the opponentMenu if it doesn't exist
    void initScroller();
    void setGamePhase(int newGamePhase);

public:
    GameStateDuel(GameApp* parent);
    ~GameStateDuel() override;
#ifdef TESTSUITE
    void loadTestSuitePlayers();
#endif

#ifdef AI_CHANGE_TESTING
    int startTime;
    int totalTestGames;
    int testPlayer2Victories;
    int totalAIDecks;
    static jge::mutex mMutex;
    vector<jge::thread> mWorkerThread;
    static void ThreadProc(void* inParam);
    void handleResults(GameObserver* aGame) {
        mMutex.lock();
        totalTestGames++;
        if (aGame->didWin(aGame->players[1])) testPlayer2Victories++;
        mMutex.unlock();
    };
#endif

    void ButtonPressed(int ControllerId, int ControlId) override;
    void Start() override;
    void End() override;
    void Update(float dt) override;
    void Render() override;
    void initRand(unsigned seed = 0);

    void OnScroll(int inXVelocity, int inYVelocity) override;

    enum ENUM_DUEL_STATE_MENU_ITEM {
        MENUITEM_CANCEL        = kCancelMenuID,
        MENUITEM_NEW_DECK      = -10,
        MENUITEM_RANDOM_PLAYER = kRandomPlayerMenuID,
        MENUITEM_RANDOM_AI     = kRandomAIPlayerMenuID,
        MENUITEM_MAIN_MENU     = -13,
        MENUITEM_EVIL_TWIN     = kEvilTwinMenuID,
        MENUITEM_MULLIGAN      = -15,
        MENUITEM_UNDO          = -16,
#ifdef TESTSUITE
        MENUITEM_LOAD = -17,
#endif
#ifdef NETWORK_SUPPORT
        MENUITEM_REMOTE_CLIENT = -18,
        MENUITEM_REMOTE_SERVER = -19,
#endif
        MENUITEM_MORE_INFO = kInfoMenuID
    };
};

#endif
