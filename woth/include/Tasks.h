#ifndef TASKS_H
#define TASKS_H

#include <vector>

// Task type constant

#define TASK_BASIC 'B'

#define TASK_WIN_AGAINST 'W'
#define TASK_SLAUGHTER 'S'
#define TASK_DELAY 'D'
#define TASK_IMMORTAL 'I'
#define TASK_MASSIVE_BURIAL 'M'
#define TASK_WISDOM 'O'
#define TASK_PACIFISM 'P'
#define TASKS_ALL "WSDIMOP"

#define COMMON_ATTRIBS_COUNT 7

class Task {
protected:
    int reward;  // TODO: Complex rewards. Be consistent with other planned modes with rewards.
    int opponent;
    bool accepted;
    char type;
    int expiresIn;
    string description;
    string opponentName;
    vector<string> persistentAttribs;  // persistentAttributes

    void storeCommonAttribs();
    int restoreCommonAttribs();
    string getOpponentName();
    virtual void storeCustomAttribs();
    virtual void restoreCustomAttribs();
    virtual void randomize();

    virtual int computeReward() = 0;

public:
    virtual ~Task() = default;
    // variable to store and method to obtain names of AI decks
    //!! Todo: This should _really_ be handled elsewhere (dedicated class?)
    static vector<string> sAIDeckNames;
    static void LoadAIDeckNames();
    static int getAIDeckCount();
    static string getAIDeckName(int id);
    // End of AI deck buffering code

    Task(char _type = ' ');

    static Task* createFromStr(const string& params, bool rand = false);
    virtual string toString();
    string getDesc();
    virtual string createDesc()   = 0;
    virtual string getShortDesc() = 0;
    int getExpiration() const;
    int getReward();
    virtual bool isDone(GameObserver* observer, GameApp* _app) = 0;
    bool isExpired() const;
    void setExpiration(int _expiresIn);
    void passOneDay();
};

class TaskList {
protected:
    string fileName;
    float vPos;
    float mElapsed;
    int mState;
    JQuad* mBg[9];
    JTexture* mBgTex;
    float sH, sW;

public:
    vector<Task*> tasks;

    enum {
        TASKS_IN,
        TASKS_ACTIVE,
        TASKS_OUT,
        TASKS_INACTIVE,
    };

    TaskList(string _fileName = "");
    int load(const string& _fileName = "");
    int save(const string& _fileName = "");
    int getState() const { return mState; };
    void addTask(const string& params, bool rand = false);
    void addTask(Task* task);
    void addRandomTask(int diff = 100);
    void removeTask(Task* task);
    void passOneDay();
    void getDoneTasks(GameObserver* observer, GameApp* _app, vector<Task*>* result);
    int getTaskCount() const;

    void Start();
    void End();

    void Update(float dt);
    void Render();
    //!!virtual void ButtonPressed(int controllerId, int controlId);

    ~TaskList();
};

class TaskWinAgainst : public Task {
protected:
    int computeReward() override;

public:
    ~TaskWinAgainst() override = default;
    TaskWinAgainst(int _opponent = 0);
    string createDesc() override;
    string getShortDesc() override;
    bool isDone(GameObserver* observer, GameApp* _app) override;
};

class TaskSlaughter : public TaskWinAgainst {
protected:
    int targetLife;
    int computeReward() override;

public:
    ~TaskSlaughter() override = default;
    TaskSlaughter(int _opponent = 0, int _targetLife = -15);
    string createDesc() override;
    string getShortDesc() override;
    bool isDone(GameObserver* observer, GameApp* _app) override;
    void storeCustomAttribs() override;
    void restoreCustomAttribs() override;
    void randomize() override;
};

class TaskDelay : public TaskWinAgainst {
protected:
    int turn;
    bool afterTurn;
    int computeReward() override;

public:
    ~TaskDelay() override = default;
    TaskDelay(int _opponent = 0, int _turn = 20);
    string createDesc() override;
    string getShortDesc() override;
    bool isDone(GameObserver* observer, GameApp* _app) override;
    void storeCustomAttribs() override;
    void restoreCustomAttribs() override;
    void randomize() override;
};

class TaskImmortal : public Task {
protected:
    int targetLife;
    int level;
    int computeReward() override;

public:
    ~TaskImmortal() override = default;
    TaskImmortal(int _targetLife = 20);

    string createDesc() override;
    string getShortDesc() override;
    bool isDone(GameObserver* observer, GameApp* _app) override;
    void storeCustomAttribs() override;
    void restoreCustomAttribs() override;
    void randomize() override;
};

class TaskMassiveBurial : public Task {
protected:
    int color;
    int bodyCount;
    int computeReward() override;

public:
    ~TaskMassiveBurial() override = default;
    TaskMassiveBurial(int _color = 0, int _bodyCount = 0);

    string createDesc() override;
    string getShortDesc() override;
    bool isDone(GameObserver* observer, GameApp* _app) override;
    void storeCustomAttribs() override;
    void restoreCustomAttribs() override;
    void randomize() override;
};

class TaskWisdom : public Task {
protected:
    int color;
    int cardCount;
    int computeReward() override;

public:
    ~TaskWisdom() override = default;
    TaskWisdom(int _color = 0, int _cardCount = 0);

    string createDesc() override;
    string getShortDesc() override;
    bool isDone(GameObserver* observer, GameApp* _app) override;
    void storeCustomAttribs() override;
    void restoreCustomAttribs() override;
    void randomize() override;
};

class TaskPacifism : public Task {
protected:
    int computeReward() override;
    int lifeSlashCardMin;

public:
    ~TaskPacifism() override = default;
    TaskPacifism(int _lifeSlashCardMin = 0);

    string createDesc() override;
    string getShortDesc() override;
    bool isDone(GameObserver* observer, GameApp* _app) override;
    void storeCustomAttribs() override;
    void restoreCustomAttribs() override;
    void randomize() override;
};

/* ------------ Task template ------------

 class TaskXX : public Task {
 protected:
 virtual int computeReward();
 public:
 TaskXX();

 virtual string createDesc();
 virtual string getShortDesc();
 virtual bool isDone(GameObserver* observer, GameApp * _app);
 virtual void storeCustomAttribs();
 virtual void restoreCustomAttribs();
 virtual void randomize();
 };
 */

#endif
