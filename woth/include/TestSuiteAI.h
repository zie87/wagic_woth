#ifndef TESTSUITEAI_H
#define TESTSUITEAI_H

#ifdef TESTSUITE

#define MAX_TESTSUITE_ACTIONS 100
#define MAX_TESTUITE_CARDS 100

#include "Threading.h"
#include "AIPlayerBaka.h"

#include <stdint.h>

class TestSuiteActions {
public:
    int nbitems;
    string actions[MAX_TESTSUITE_ACTIONS];
    void add(string action);
    TestSuiteActions();
    void cleanup();
};

class TestSuiteGame;
class TestSuite;
class TestSuiteAI;
class TestSuiteState {
public:
    GamePhase phase;
    void parsePlayerState(int playerId, const string& s);
    TestSuiteState();
    ~TestSuiteState();

    vector<TestSuiteAI*> players;
    void cleanup(TestSuiteGame* tsGame);
};

class TestSuiteGame {
    friend class TestSuiteAI;
    friend class TestSuite;

protected:
    string filename;
    int summoningSickness;
    bool forceAbility;
    GameType gameType;
    unsigned int seed;
    int aiMaxCalls;
    TestSuiteState endState;
    TestSuiteState initState;
    TestSuiteActions actions;
    float timerLimit;
    bool isOK;
    int currentAction;
    GameObserver* observer;

    static jge::mutex mMutex;
    virtual void handleResults(bool wasAI, int error);
    TestSuite* testsuite;
    bool load();

public:
    virtual ~TestSuiteGame();
    TestSuiteGame(TestSuite* testsuite);
    TestSuiteGame(TestSuite* testsuite, string _filename);
    void ResetManapools();
    void initGame();
    void assertGame();
    MTGPlayerCards* buildDeck(Player* player, int playerId);
    GameType getGameType() { return gameType; };
    string getNextAction();
    Interruptible* getActionByMTGId(int mtgid);
    static int Log(const char* text);
    void setObserver(GameObserver* anObserver) { observer = anObserver; };
};

class TestSuite : public TestSuiteGame {
private:
    int currentfile;
    int nbfiles;
    string files[1024];

    void cleanup();
    vector<jge::thread*> mWorkerThread;
    Rules* mRules;
    bool mProcessing;

public:
    int startTime, endTime;
    unsigned int seed;
    int nbFailed, nbTests, nbAIFailed, nbAITests;
    TestSuite(const char* filename);
    ~TestSuite() override;
    void initGame(GameObserver* g);
    void pregameTests();
    int loadNext();
    string getNextFile() {
        jge::mutex::scoped_lock const lock(mMutex);
        if (currentfile >= nbfiles) {
            return "";
        }
        currentfile++;
        return files[currentfile - 1];
    };
    static void ThreadProc(void* inParam);
    void setRules(Rules* rules) { mRules = rules; };
    void handleResults(bool wasAI, int error) override;

    size_t run();
};

class TestSuiteAI : public AIPlayerBaka {
private:
    MTGCardInstance* getCard(string action);
    float timer;
    TestSuiteGame* suite;

public:
    TestSuiteAI(TestSuiteGame* tsGame, int playerId);
    int Act(float dt) override;
    int displayStack() override;
    bool summoningSickness() { return (suite->summoningSickness == 1); }
};

#endif
#endif
