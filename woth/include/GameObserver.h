#ifndef GAMEOBSERVER_H
#define GAMEOBSERVER_H

#include "Player.h"
#include "MTGAbility.h"
#include "DuelLayers.h"
#include "MTGCardInstance.h"
#include "PlayGuiObject.h"
#include "TargetChooser.h"
#include "PhaseRing.h"
#include "ReplacementEffects.h"
#include "GuiStatic.h"
#include <queue>
#include <time.h>

class MTGGamePhase;
class MTGAbility;
class MTGCardInstance;
struct CardGui;
class Player;
class TargetChooser;
class Rules;
class TestSuiteGame;
class Trash;
class DeckManager;

class GameObserver final {
private:
    GameType mGameType;
    MTGCardInstance* cardWaitingForTargets;
    std::queue<WEvent*> eventsQueue;
    // used when we're running to log actions
    std::list<std::string> actionsList;
    // used when we're loading to know what to load
    std::list<std::string> loadingList;
    std::list<std::string>::iterator loadingite;
    RandomGenerator randomGenerator;
    WResourceManager* mResourceManager;
    JGE* mJGE;
    DeckManager* mDeckManager;
    Player* gameOver;

    int untap(MTGCardInstance* card);
    bool WaitForExtraPayment(MTGCardInstance* card);
    void cleanup();
    std::string startupGameSerialized;
    bool parseLine(const std::string& s);
    void logAction(const std::string& s);
    bool processActions(bool undo
#ifdef TESTSUITE
                        ,
                        TestSuiteGame* testgame
#endif
    );
    friend std::ostream& operator<<(std::ostream&, const GameObserver&);
    bool mLoading;
    void nextGamePhase();
    void shuffleLibrary(Player* p);
    void createPlayer(const std::string& playerMode
#ifdef TESTSUITE
                      ,
                      TestSuiteGame* testgame
#endif  // TESTSUITE
    );

public:
    int currentPlayerId;
    CombatStep combatStep;
    int turn;
    int forceShuffleLibraries();
    int targetListIsSet(MTGCardInstance* card);
    PhaseRing* phaseRing;
    std::vector<std::list<Phase*> > gameTurn;
    int cancelCurrentAction();
    GamePhase currentGamePhase;
    ExtraCosts* mExtraPayment;
    int oldGamePhase;
    TargetChooser* targetChooser;
    DuelLayers* mLayers;
    ReplacementEffects* replacementEffects;
    std::vector<Player*> players;  // created outside
    time_t startedAt;
    Rules* mRules;
    MTGCardInstance* ExtraRules;
    Trash* mTrash;

    GameType gameType() const { return mGameType; };
    TargetChooser* getCurrentTargetChooser() const;
    void stackObjectClicked(Interruptible* action);

    int cardClickLog(bool log,
                     Player* clickedPlayer,
                     MTGGameZone* zone,
                     MTGCardInstance* backup,
                     size_t index,
                     int toReturn);
    int cardClick(MTGCardInstance* card, MTGAbility* ability);
    int cardClick(MTGCardInstance* card, int abilityType);
    int cardClick(MTGCardInstance* card, Targetable* _object = nullptr, bool log = true);
    GamePhase getCurrentGamePhase() const;
    void setCurrentGamePhase(GamePhase phase) { currentGamePhase = phase; };
    const char* getCurrentGamePhaseName() const;
    const char* getNextGamePhaseName() const;
    void nextCombatStep();
    void userRequestNextGamePhase(bool allowInterrupt = true, bool log = true);
    void cleanupPhase();
    void nextPlayer();

#ifdef TESTSUITE
    void loadTestSuitePlayer(int playerId, TestSuiteGame* testSuite);
#endif  // TESTSUITE
    void loadPlayer(int playerId, PlayerType playerType = PLAYER_TYPE_HUMAN, int decknb = 0, bool premadeDeck = false);
    void loadPlayer(int playerId, Player* player);

    Player* currentPlayer;
    Player* currentActionPlayer;
    Player* isInterrupting;
    Player* opponent();
    Player* nextTurnsPlayer();
    Player* currentlyActing() const;
    GameObserver(WResourceManager* output = nullptr, JGE* input = nullptr);
    ~GameObserver();
    void gameStateBasedEffects();
    void enchantmentStatus();
    void Affinity();
    void addObserver(MTGAbility* observer) const;
    bool removeObserver(ActionElement* observer) const;
    void startGame(GameType, Rules* rules);
    void untapPhase() const;
    MTGCardInstance* isCardWaiting() { return cardWaitingForTargets; }
    int isInPlay(MTGCardInstance* card);
    int isInGrave(MTGCardInstance* card);
    int isInExile(MTGCardInstance* card);
    void Update(float dt);
    void Render();
    void ButtonPressed(PlayGuiObject*);
    int getPlayersNumber() const { return players.size(); };

    int receiveEvent(WEvent* event);
    bool connectRule;

    void logAction(Player* player, const std::string& s = "");
    void logAction(int playerId, const std::string& s = "") { logAction(players[playerId], s); };
    void logAction(MTGCardInstance* card, MTGGameZone* zone, size_t index, int result);
    bool load(const std::string& s,
              bool undo = false
#ifdef TESTSUITE
              ,
              TestSuiteGame* testgame = nullptr
#endif
    );
    bool undo();
    bool isLoading() const { return mLoading; };
    void Mulligan(Player* player = nullptr);
    Player* getPlayer(size_t index) { return players[index]; };
    bool isStarted() const { return (mLayers != nullptr); };
    RandomGenerator* getRandomGenerator() { return &randomGenerator; };
    WResourceManager* getResourceManager();
    CardSelectorBase* getCardSelector() const { return mLayers->mCardSelector; };
    bool operator==(const GameObserver& aGame);
    JGE* getInput() { return mJGE; };
    DeckManager* getDeckManager() { return mDeckManager; };
    void dumpAssert(bool val) const;
    void resetStartupGame();
    void setLoser(Player* aPlayer) { gameOver = aPlayer; };

    bool didWin(Player* aPlayer = nullptr) const {
        if (!gameOver) {
            // nobody won
            return false;
        }
        if (!aPlayer) {
            // somebody won and we don't care who
            return true;
        }
        if (gameOver == aPlayer) {
            // aPlayer lost
            return false;
        }  // aPlayer won
        return true;
    };
};

#endif
