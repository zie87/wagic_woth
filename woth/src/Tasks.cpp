#include "woth/string/algorithms.hpp"

#include "PrecompiledHeader.h"

#include "GameApp.h"
#include "Player.h"
#include "Tasks.h"
#include "AIPlayer.h"

// Todo remove this dependency!
#include "AIPlayerBaka.h"

#include "Translate.h"
#include "MTGDefinitions.h"
#include <JRenderer.h>
#include <math.h>

#include <utility>
#include <algorithm>

#include "utils.h"

static constexpr char ITEM_SEPARATOR = '|';

vector<string> Task::sAIDeckNames;

/*---------------- Task -----------------*/

Task::Task(char _type) : reward(0), accepted(false), expiresIn(1) {
    if (_type == ' ') {
        type = TASK_BASIC;
    } else {
        type = _type;
    }
}

int Task::getReward() {
    if (reward == 0) {
        reward = computeReward();
    }

    return reward;
}

string Task::toString() {
    storeCommonAttribs();
    storeCustomAttribs();
    return ::woth::join(persistentAttribs, ITEM_SEPARATOR);
}

// Store basic attributes to vector, for saving
void Task::storeCommonAttribs() {
    char buff[256];

    persistentAttribs.clear();
    persistentAttribs.emplace_back(1, type);

    sprintf(buff, "%i", expiresIn);
    persistentAttribs.emplace_back(buff);

    sprintf(buff, "%i", accepted ? 1 : 0);
    persistentAttribs.emplace_back(buff);

    sprintf(buff, "%i", opponent);
    persistentAttribs.emplace_back(buff);

    sprintf(buff, "%i", reward);
    persistentAttribs.emplace_back(buff);

    persistentAttribs.push_back(getDesc());

    persistentAttribs.push_back(getOpponentName());
}

int Task::restoreCommonAttribs() {
    if (persistentAttribs.size() < COMMON_ATTRIBS_COUNT) {
#if defined(WIN32) || defined(LINUX)
        DebugTrace("\nTasks.cpp::restoreCommonAttribs: Not enough attributes loaded\n");
#endif

        return -1;
    }
    expiresIn    = atoi(persistentAttribs[1].c_str());
    accepted     = (persistentAttribs[2] == "1");
    opponent     = atoi(persistentAttribs[3].c_str());
    reward       = atoi(persistentAttribs[4].c_str());
    description  = persistentAttribs[5];
    opponentName = persistentAttribs[6];
    return 1;
}

void Task::storeCustomAttribs() {
    // To be extended in child class
}

void Task::restoreCustomAttribs() {
    // To be extended in child class
}

string Task::getOpponentName() {
    if (opponentName.empty()) {
        opponentName = Task::getAIDeckName(opponent);
    }

    return opponentName;
}

string Task::getDesc() { return (description.empty()) ? (description = createDesc()) : description; }

void Task::randomize() {
    opponent     = rand() % getAIDeckCount() + 1;
    opponentName = "";
    setExpiration((rand() % 3) + 1);
    reward = computeReward();
}

bool Task::isExpired() const { return (expiresIn <= 0); }

int Task::getExpiration() const { return expiresIn; }

void Task::setExpiration(int _expiresIn) { expiresIn = _expiresIn; }

void Task::passOneDay() {
    expiresIn--;
    reward = (int)(getReward() * 0.9);  // Todo: degradation and minreward constants
    if (reward < 33) {
        reward = 33;
    }
}

// AI deck buffering code

void Task::LoadAIDeckNames() {
    if (sAIDeckNames.empty()) {
        int found   = 1;
        int nbDecks = 0;
        while (found) {
            found = 0;
            std::ostringstream stream;
            stream << "ai/baka"
                   << "/deck" << nbDecks + 1 << ".txt";
            if (fileExists(stream.str().c_str())) {
                found = 1;
                nbDecks++;
                // TODO: Creating MTGDeck only for getting decks name. Find an easier way.
                auto* mtgd = NEW MTGDeck(stream.str().c_str(), nullptr, 1);
                sAIDeckNames.push_back(mtgd->meta_name);
                delete mtgd;
            }
        }
    }
}

int Task::getAIDeckCount() {
    LoadAIDeckNames();
    return std::min((size_t)options[Options::AIDECKS_UNLOCKED].number, sAIDeckNames.size());
}

string Task::getAIDeckName(int id) {
    LoadAIDeckNames();
    return ((unsigned int)id <= sAIDeckNames.size()) ? sAIDeckNames.at(id - 1) : "<Undefined>";
}

// End of AI deck buffering code

// Each child class has to be added to the switch in this function (clumsy..)
Task* Task::createFromStr(const string& params, bool rand) {
    vector<string> exploded;
    Task* result;

    ::woth::split(params, ITEM_SEPARATOR, exploded);

    switch (exploded[0][0]) {
    case TASK_WIN_AGAINST:
        result = NEW TaskWinAgainst();
        break;
    case TASK_SLAUGHTER:
        result = NEW TaskSlaughter();
        break;
    case TASK_DELAY:
        result = NEW TaskDelay();
        break;
    case TASK_IMMORTAL:
        result = NEW TaskImmortal();
        break;
    case TASK_MASSIVE_BURIAL:
        result = NEW TaskMassiveBurial();
        break;
    case TASK_WISDOM:
        result = NEW TaskWisdom();
        break;
    case TASK_PACIFISM:
        result = NEW TaskPacifism();
        break;
    default:
#if defined(WIN32) || defined(LINUX)
        DebugTrace("\nTasks.cpp::createFromStr: Undefined class type\n");
#endif
        result = NEW TaskWinAgainst();
    }

    if (!result) {
#if defined(WIN32) || defined(LINUX)
        DebugTrace("\nTask::createFromStr: Failed to create task\n");
#endif
        return nullptr;
    }

    result->persistentAttribs = exploded;

    if (exploded.size() >= COMMON_ATTRIBS_COUNT) {
        result->restoreCommonAttribs();
        if (exploded.size() > COMMON_ATTRIBS_COUNT) {
            result->restoreCustomAttribs();
        }
    } else if (rand) {
        result->randomize();
    }

    return result;
}

/*---------------- TaskList -----------------*/

TaskList::TaskList(string _fileName) : fileName(std::move(_fileName)) {
    if (fileName.empty()) {
        fileName = options.profileFile(PLAYER_TASKS);
    }
    load(fileName);
    for (int i = 0; i < 9; i++) {
        mBg[i] = nullptr;
    }
    mBgTex = nullptr;  // We only load the background if we use the task screen.
}

int TaskList::save(const string& _fileName) {
    if (!_fileName.empty()) {
        fileName = _fileName;
    }
    if (fileName.empty()) {
#if defined(WIN32) || defined(LINUX)
        DebugTrace("\nTaskList::save: No filename specified\n");
#endif
        return -1;
    }

    std::ofstream file;
    if (JFileSystem::GetInstance()->openForWrite(file, fileName)) {
#if defined(WIN32) || defined(LINUX)
        DebugTrace("\nsaving tasks\n");
#endif

        file << "# Format: <Type>|<Expiration>|<Accepted>|<Opponent>|<Reward>|<Description>[|Additional attributes]\n";

        for (auto it = tasks.begin(); it != tasks.end(); it++) {
            file << (*it)->toString() << "\n";
        }
        file.close();
    }

    return 1;
}

int TaskList::load(const string& _fileName) {
    Task* task;
    if (!_fileName.empty()) {
        fileName = _fileName;
    }
    if (fileName.empty()) {
        DebugTrace("\nTaskList::load: No filename specified\n");
        return -1;
    }

    std::string contents;
    if (!JFileSystem::GetInstance()->readIntoString(fileName, contents)) {
        DebugTrace("\nTaskList::load: Failed to open file\n");
        return -1;
    }

    std::stringstream stream(contents);
    std::string s;
    while (std::getline(stream, s)) {
        if (s.empty()) {
            continue;
        }
        if (s[s.size() - 1] == '\r') {
            s.erase(s.size() - 1);  // Handle DOS files
        }
        if (s[0] == '#') {
            continue;
        }

        task = Task::createFromStr(s);
        if (task) {
            this->addTask(task);
        } else {
            DebugTrace("\nTaskList::load: error creating task\n");
        }
    }

    return 1;
}

void TaskList::addTask(Task* task) { tasks.push_back(task); }

void TaskList::addTask(const string& params, bool rand) { addTask(Task::createFromStr(std::move(params), rand)); }

void TaskList::removeTask(Task* task) {
    vector<Task*>::iterator it;

    it = find(tasks.begin(), tasks.end(), task);

    if (it != tasks.end()) {
        SAFE_DELETE(*it);
        tasks.erase(it);
    } else {
        // TODO: task not found handling.
    }
}

void TaskList::Start() {
    vPos     = -SCREEN_HEIGHT;  // Offscreen
    mElapsed = 0;
    mState   = TASKS_IN;
    if (!mBgTex) {
        mBgTex     = WResourceManager::Instance()->RetrieveTexture("taskboard.png", RETRIEVE_LOCK);
        auto unitH = static_cast<float>(mBgTex->mHeight / 4);
        auto unitW = static_cast<float>(mBgTex->mWidth / 4);
        if (unitH == 0 || unitW == 0) {
            return;
        }

        for (int i = 0; i < 9; i++) {
            SAFE_DELETE(mBg[i]);
        }
        if (mBgTex) {
            mBg[0] = NEW JQuad(mBgTex, 0, 0, unitW, unitH);
            mBg[1] = NEW JQuad(mBgTex, unitW, 0, unitW * 2, unitH);
            mBg[2] = NEW JQuad(mBgTex, unitW * 3, 0, unitW, unitH);
            mBg[3] = NEW JQuad(mBgTex, 0, unitH, unitW, unitH * 2);
            mBg[4] = NEW JQuad(mBgTex, unitW, unitH, unitW * 2, unitH * 2);
            mBg[5] = NEW JQuad(mBgTex, unitW * 3, unitH, unitW, unitH * 2);
            mBg[6] = NEW JQuad(mBgTex, 0, unitH * 3, unitW, unitH);
            mBg[7] = NEW JQuad(mBgTex, unitW, unitH * 3, unitW * 2, unitH);
            mBg[8] = NEW JQuad(mBgTex, unitW * 3, unitH * 3, unitW, unitH);
        }

        sH = 64 / unitH;
        sW = 64 / unitW;
    }
}
void TaskList::End() {
    mState   = TASKS_OUT;
    mElapsed = 0;
}

void TaskList::passOneDay() {
    // TODO: "You have failed the task" message to the user when accepted task expires
    for (auto it = tasks.begin(); it != tasks.end();) {
        (*it)->passOneDay();
        if ((*it)->isExpired()) {
            SAFE_DELETE(*it);
            it = tasks.erase(it);
        } else {
            it++;
        }
    }
}

void TaskList::getDoneTasks(GameObserver* observer, GameApp* _app, vector<Task*>* result) {
    result->clear();
    // TODO: Return only accepted tasks
    for (auto it = tasks.begin(); it != tasks.end(); it++) {
        if ((*it)->isDone(observer, _app)) {
            result->push_back(*it);
        }
    }
}

int TaskList::getTaskCount() const { return tasks.size(); }

void TaskList::Update(float dt) {
    mElapsed += dt;

    if (mState == TASKS_IN && vPos < 0) {
        vPos = -SCREEN_HEIGHT + (SCREEN_HEIGHT * mElapsed / 0.75f);  // Todo: more physical drop-in.
        if (vPos >= 0) {
            vPos   = 0;
            mState = TaskList::TASKS_ACTIVE;
        }
    } else if (mState == TASKS_OUT && vPos > -SCREEN_HEIGHT) {
        vPos = -(SCREEN_HEIGHT * mElapsed / 0.75f);
        if (vPos <= -SCREEN_HEIGHT) {
            mState = TASKS_INACTIVE;
        }
    }
}

void TaskList::Render() {
    JRenderer* r = JRenderer::GetInstance();
    // Setup fonts.
    WFont* f  = WResourceManager::Instance()->GetWFont(Fonts::MAIN_FONT);
    WFont* f2 = WResourceManager::Instance()->GetWFont(Fonts::MAGIC_FONT);
    WFont* f3 = WResourceManager::Instance()->GetWFont(Fonts::MENU_FONT);  // OPTION_FONT
    f2->SetColor(ARGB(255, 205, 237, 240));
    f3->SetColor(ARGB(255, 219, 206, 151));

    // Render background board
    if (mBgTex) {
        r->FillRect(0, vPos, SCREEN_WIDTH, SCREEN_HEIGHT, ARGB(128, 0, 0, 0));
        r->RenderQuad(mBg[0], 0, vPos, 0, sW, sH);                                       // TL
        r->RenderQuad(mBg[2], SCREEN_WIDTH - 64, vPos, 0, sW, sH);                       // TR
        r->RenderQuad(mBg[6], 0, vPos + SCREEN_HEIGHT - 64, 0, sW, sH);                  // BL
        r->RenderQuad(mBg[8], SCREEN_WIDTH - 64, vPos + SCREEN_HEIGHT - 64, 0, sW, sH);  // BR

        // Stretch the sides
        const float stretchV = (144.0f / 128.0f) * sH;
        const float stretchH = (176.0f / 128.0f) * sW;
        r->RenderQuad(mBg[3], 0, vPos + 64, 0, sW, stretchV);                  // L
        r->RenderQuad(mBg[5], SCREEN_WIDTH - 64, vPos + 64, 0, sW, stretchV);  // R
        r->RenderQuad(mBg[1], 64, vPos, 0, stretchH, sH);                      // T1
        r->RenderQuad(mBg[1], 240, vPos, 0, stretchH, sH);                     // T1
        r->RenderQuad(mBg[7], 64, vPos + 208, 0, stretchH, sH);                // B1
        r->RenderQuad(mBg[7], 240, vPos + 208, 0, stretchH, sH);               // B1
        r->RenderQuad(mBg[4], 64, vPos + 64, 0, stretchH, stretchV);           // Center1
        r->RenderQuad(mBg[4], 240, vPos + 64, 0, stretchH, stretchV);          // Center2

        f2->SetColor(ARGB(255, 55, 46, 34));
        f = f2;
    } else {
        r->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ARGB(128, 0, 0, 0));
        r->FillRect(10, 10 + vPos, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10, ARGB(128, 0, 0, 0));
    }

    const float posX = 40;
    float posY       = vPos + 20;
    char buffer[300];
    const string title = _("Task Board");

    f3->DrawString(title.c_str(), static_cast<float>((SCREEN_WIDTH - 20) / 2 - title.length() * 4), posY);
    posY += 30;

    if (tasks.empty()) {
        f->DrawString(_("There are no tasks that need to be done. Come again tomorrow.").c_str(), posX, posY);
        return;
    }

    for (auto it = tasks.begin(); it != tasks.end(); it++) {
        sprintf(buffer, "%s", (*it)->getShortDesc().c_str());
        f2->DrawString(buffer, posX, posY);
        if (mBgTex) {
            f->SetScale(.8f);
        }
        sprintf(buffer, _("Days left: %i").c_str(), (*it)->getExpiration());
        f->DrawString(buffer, SCREEN_WIDTH - 190, posY);
        sprintf(buffer, _("Reward: %i").c_str(), (*it)->getReward());
        f->DrawString(buffer, SCREEN_WIDTH - 100, posY);
        posY += 15;

        sprintf(buffer, "%s", (*it)->getDesc().c_str());
        f->DrawString(buffer, posX + 10, posY);
        posY += 15;
        if (mBgTex) {
            f->SetScale(1);
        }
        // r->DrawLine((SCREEN_WIDTH)/2 - 200, posY, (SCREEN_WIDTH)/2 + 200, posY, ARGB(128, 255, 255, 255));
    }
    f->SetScale(1);
}

void TaskList::addRandomTask(int diff) {
    // TODO: Weighted random (rarity of tasks)
    //       - based on counts of finished tasks?
    //         Winning a task several times may slightly lessen the probability of it being generated
    string s(TASKS_ALL);
    char taskType[2];
    sprintf(taskType, "%c", s[rand() % s.length()]);
    addTask(string(taskType), true);
}

TaskList::~TaskList() {
    for (unsigned int i = 0; i < tasks.size(); i++) {
        SAFE_DELETE(tasks[i]);
    }
    if (mBgTex) {
        WResourceManager::Instance()->Release(mBgTex);
    }
    for (int i = 0; i < 9; i++) {
        SAFE_DELETE(mBg[i]);
    }
}

/*----------------------------------------*/
/*---------------- Tasks -----------------*/
/*----------------------------------------*/

/*----------- TaskWinAgainst -------------*/

TaskWinAgainst::TaskWinAgainst(int _opponent) : Task(TASK_WIN_AGAINST) { opponent = _opponent; }

int TaskWinAgainst::computeReward() { return 75 + rand() % 75; }

string TaskWinAgainst::createDesc() {
    char buffer[4096];
    switch (rand() % 2) {
    case 0:
        sprintf(buffer, _("You have to defeat %s before it causes too much harm.").c_str(), getOpponentName().c_str());
        break;
    case 1:
        sprintf(buffer, _("Please defeat %s as soon as possible.").c_str(), getOpponentName().c_str());
        break;
    }
    return buffer;
}

string TaskWinAgainst::getShortDesc() {
    char buffer[4096];
    string result;

    sprintf(buffer, _("Defeat %s").c_str(), getOpponentName().c_str());
    result = buffer;

    return result;
}

bool TaskWinAgainst::isDone(GameObserver* observer, GameApp* _app) {
    auto* baka = dynamic_cast<AIPlayerBaka*>(observer->players[1]);
    return ((baka) && (!observer->players[0]->isAI()) && (observer->players[1]->isAI()) &&
            (observer->didWin(observer->players[0]))  // Human player wins
            && (baka->deckId == opponent));
}

/*----------- TaskSlaughter -------------*/

TaskSlaughter::TaskSlaughter(int _opponent, int _targetLife) : TaskWinAgainst(_opponent), targetLife(_targetLife) {
    type = TASK_SLAUGHTER;
}

int TaskSlaughter::computeReward() {
    return 2 * TaskWinAgainst::computeReward() - 9 * targetLife * (targetLife < -50 ? 2 : 1);
}

void TaskSlaughter::randomize() {
    targetLife = -15 - rand() % 10;
    if (!(rand() % 7)) {
        targetLife *= 5;
    }
    Task::randomize();
}

string TaskSlaughter::createDesc() {
    char buffer[4096];
    switch (rand() % 2) {
    case 0:
        sprintf(buffer, _("Defeat %s in a way it won't forget. Bring it to %i life.").c_str(),
                getOpponentName().c_str(), targetLife);
        break;
    case 1:
        sprintf(buffer, _("Slaughter %s! Beat it to %i life or less.").c_str(), getOpponentName().c_str(), targetLife);
        break;
    }
    return buffer;
}

string TaskSlaughter::getShortDesc() {
    char buffer[4096];
    sprintf(buffer, _("Slaughter %s (%i life)").c_str(), getOpponentName().c_str(), targetLife);
    return buffer;
}

bool TaskSlaughter::isDone(GameObserver* observer, GameApp* _app) {
    return TaskWinAgainst::isDone(observer, _app) && (observer->players[1]->life <= targetLife);
}

void TaskSlaughter::storeCustomAttribs() {
    char buff[256];

    sprintf(buff, "%i", targetLife);
    persistentAttribs.emplace_back(buff);
}

void TaskSlaughter::restoreCustomAttribs() { targetLife = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT].c_str()); }

/*----------- TaskDelay -------------*/
// Now serves as both 'Delay' and 'Fast defeat' task.

TaskDelay::TaskDelay(int _opponent, int _turn) : TaskWinAgainst(_opponent), afterTurn(true), turn(_turn) {
    type = TASK_DELAY;
}

int TaskDelay::computeReward() {
    return TaskWinAgainst::computeReward() + (afterTurn ? turn * 33 : (17 - turn) * (17 - turn) * 17);
}

void TaskDelay::randomize() {
    afterTurn = rand() % 2 == 1;
    turn      = afterTurn ? rand() % 15 + 20 : 15 - rand() % 9;
    Task::randomize();
}

string TaskDelay::createDesc() {
    char buffer[4096];
    if (afterTurn) {
        switch (rand() % 2) {
        case 0:
            sprintf(buffer, _("Defeat %s after keeping it occupied for %i turns.").c_str(), getOpponentName().c_str(),
                    turn);
            break;
        case 1:
            sprintf(buffer, _("Defeat %s, but play with it for %i turns first.").c_str(), getOpponentName().c_str(),
                    turn);
            break;
        }
    } else {
        switch (rand() % 2) {
        case 0:
            sprintf(buffer, _("Defeat %s and make sure it doesn't take more than %i turns.").c_str(),
                    getOpponentName().c_str(), turn);
            break;
        case 1:
            sprintf(buffer, _("Defeat %s, in a duel no longer than %i turns.").c_str(), getOpponentName().c_str(),
                    turn);
            break;
        }
    }
    return buffer;
}

string TaskDelay::getShortDesc() {
    char buffer[4096];
    if (afterTurn) {
        sprintf(buffer, _("Delay %s for %i turns").c_str(), getOpponentName().c_str(), turn);
    } else {
        sprintf(buffer, _("Defeat %s before turn %i").c_str(), getOpponentName().c_str(), turn + 1);
    }
    return buffer;
}

bool TaskDelay::isDone(GameObserver* observer, GameApp* _app) {
    return TaskWinAgainst::isDone(observer, _app) && (afterTurn ? (observer->turn >= turn) : (observer->turn <= turn));
}

void TaskDelay::storeCustomAttribs() {
    char buff[256];

    sprintf(buff, "%i", turn);
    persistentAttribs.emplace_back(buff);
    sprintf(buff, "%i", afterTurn);
    persistentAttribs.emplace_back(buff);
}

void TaskDelay::restoreCustomAttribs() {
    turn = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT].c_str());
    if (persistentAttribs.size() > COMMON_ATTRIBS_COUNT + 1) {
        afterTurn = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT + 1].c_str()) > 0;
    }
}

/* ------------ TaskImmortal ------------ */

TaskImmortal::TaskImmortal(int _targetLife)
    : Task(TASK_IMMORTAL)
    , level((targetLife < 100) ? 0 : ((targetLife < 1000) ? 1 : 2))
    , targetLife(_targetLife) {}

int TaskImmortal::computeReward() { return targetLife * 2 + 150 + rand() % 50; }

string TaskImmortal::createDesc() {
    char buffer[4096];

    sprintf(buffer, _("Defeat any opponent, having at least %i life in the end.").c_str(), targetLife);

    return buffer;
}

string TaskImmortal::getShortDesc() {
    char buffer[4096];

    switch (level) {
    case 0:
        sprintf(buffer, _("Win flawlessly (%i life)").c_str(), targetLife);
        break;
    case 1:
        sprintf(buffer, _("Reach Invulnerability (%i life)").c_str(), targetLife);
        break;
    case 2:
        sprintf(buffer, _("Reach Immortality! (%i life)").c_str(), targetLife);
        break;
    }

    return buffer;
}

bool TaskImmortal::isDone(GameObserver* observer, GameApp* _app) {
    return (!observer->players[0]->isAI()) && (observer->players[1]->isAI()) &&
           (observer->didWin(observer->players[0]))  // Human player wins
           && (observer->players[0]->life >= targetLife);
}

void TaskImmortal::storeCustomAttribs() {
    char buff[256];

    sprintf(buff, "%i", targetLife);
    persistentAttribs.emplace_back(buff);

    sprintf(buff, "%i", level);
    persistentAttribs.emplace_back(buff);
}

void TaskImmortal::restoreCustomAttribs() {
    targetLife = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT].c_str());
    level      = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT + 1].c_str());
}

void TaskImmortal::randomize() {
    level = rand() % 3;
    switch (level) {
    case 0:
        targetLife = 20 + rand() % 10;
        break;
    case 1:
        targetLife = 100 + 5 * (rand() % 5);
        break;
    case 2:
        targetLife = 1000 + 50 * (rand() % 10);
        break;
    }
    Task::randomize();
}
/* ------------ TaskMassiveBurial ------------ */

TaskMassiveBurial::TaskMassiveBurial(int _color, int _bodyCount)
    : Task(TASK_MASSIVE_BURIAL)
    , bodyCount(_bodyCount)
    , color(_color) {
    if ((0 == color) || (0 == bodyCount)) {
        randomize();
    }
}

int TaskMassiveBurial::computeReward() {
    return rand() % 150 + bodyCount * ((Constants::MTG_COLOR_LAND == color) ? 70 : 50);
}

string TaskMassiveBurial::createDesc() {
    char buffer[4096];

    sprintf(buffer, _("Bury %i %s cards to your opponent's graveyard and defeat him.").c_str(), bodyCount,
            Constants::MTGColorStrings[color]);

    return buffer;
}

string TaskMassiveBurial::getShortDesc() {
    char buffer[4096];
    switch (color) {
    case Constants::MTG_COLOR_GREEN:
        sprintf(buffer, _("Tame the nature (%i)").c_str(), bodyCount);
        break;
    case Constants::MTG_COLOR_BLUE:
        sprintf(buffer, _("Evaporation (%i)").c_str(), bodyCount);
        break;
    case Constants::MTG_COLOR_RED:
        sprintf(buffer, _("Bring the order (%i)").c_str(), bodyCount);
        break;
    case Constants::MTG_COLOR_BLACK:
        sprintf(buffer, _("Exorcism (%i)").c_str(), bodyCount);
        break;
    case Constants::MTG_COLOR_WHITE:
        sprintf(buffer, _("Dusk (%i)").c_str(), bodyCount);
        break;
    case Constants::MTG_COLOR_LAND:
        sprintf(buffer, _("Selective disaster (%i)").c_str(), bodyCount);
        break;
    }
    return buffer;
}

bool TaskMassiveBurial::isDone(GameObserver* observer, GameApp* _app) {
    int countColor                 = 0;
    vector<MTGCardInstance*> cards = observer->players[1]->game->graveyard->cards;

    for (auto it = cards.begin(); it != cards.end(); it++) {
        if ((*it)->hasColor(color)) {
            countColor++;
        }
    }

    return (countColor >= bodyCount);
}

void TaskMassiveBurial::storeCustomAttribs() {
    char buff[256];
    sprintf(buff, "%i", color);
    persistentAttribs.emplace_back(buff);

    sprintf(buff, "%i", bodyCount);
    persistentAttribs.emplace_back(buff);
}

void TaskMassiveBurial::restoreCustomAttribs() {
    color     = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT].c_str());
    bodyCount = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT + 1].c_str());
}

void TaskMassiveBurial::randomize() {
    color     = rand() % (Constants::NB_Colors - 1) + 1;
    bodyCount = 5 + ((Constants::MTG_COLOR_LAND == color) ? rand() % 10 : rand() % 20);
    Task::randomize();
}

/* ------------ TaskWisdom ------------ */

TaskWisdom::TaskWisdom(int _color, int _cardCount) : Task(TASK_WISDOM), cardCount(_cardCount), color(_color) {
    if ((0 == color) || (0 == cardCount)) {
        randomize();
    }
}

int TaskWisdom::computeReward() { return (rand() % 150) + (cardCount * 50) + (cardCount > 7 ? 200 : 0); }

string TaskWisdom::createDesc() {
    char buffer[4096];

    sprintf(buffer, _("Win a game with at least %i %s cards in your hand.").c_str(), cardCount,
            Constants::MTGColorStrings[color]);

    return buffer;
}

string TaskWisdom::getShortDesc() {
    char buffer[4096];
    switch (color) {
    case Constants::MTG_COLOR_GREEN:
        sprintf(buffer, _("Animal herder (%i)").c_str(), cardCount);
        break;
    case Constants::MTG_COLOR_BLUE:
        sprintf(buffer, _("Accumulated knowledge (%i)").c_str(), cardCount);
        break;
    case Constants::MTG_COLOR_RED:
        sprintf(buffer, _("Retained anger (%i)").c_str(), cardCount);
        break;
    case Constants::MTG_COLOR_BLACK:
        sprintf(buffer, _("Necropotence (%i)").c_str(), cardCount);
        break;
    case Constants::MTG_COLOR_WHITE:
        sprintf(buffer, _("Dawn of crusade (%i)").c_str(), cardCount);
        break;
    case Constants::MTG_COLOR_LAND:
        sprintf(buffer, _("Mana reserves (%i)").c_str(), cardCount);
        break;
    }
    return buffer;
}

bool TaskWisdom::isDone(GameObserver* observer, GameApp* _app) {
    int countColor                 = 0;
    vector<MTGCardInstance*> cards = observer->players[0]->game->hand->cards;

    for (auto it = cards.begin(); it != cards.end(); it++) {
        if ((*it)->hasColor(color)) {
            countColor++;
        }
    }

    return (!observer->players[0]->isAI()) && (observer->players[1]->isAI()) &&
           (observer->didWin(observer->players[0]))  // Human player wins
           && (countColor >= cardCount);
}

void TaskWisdom::storeCustomAttribs() {
    char buff[256];
    sprintf(buff, "%i", color);
    persistentAttribs.emplace_back(buff);

    sprintf(buff, "%i", cardCount);
    persistentAttribs.emplace_back(buff);
}

void TaskWisdom::restoreCustomAttribs() {
    color     = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT].c_str());
    cardCount = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT + 1].c_str());
}

void TaskWisdom::randomize() {
    color     = rand() % (Constants::NB_Colors - 1) + 1;
    cardCount = 2 + ((Constants::MTG_COLOR_LAND == color) ? rand() % 5 : rand() % 7);
    Task::randomize();
}

/* ------------ TaskPacifism ------------ */

TaskPacifism::TaskPacifism(int _lifeSlashCardMin) : Task(TASK_PACIFISM), lifeSlashCardMin(_lifeSlashCardMin) {
    if (lifeSlashCardMin == 0) {
        randomize();
    }
}

int TaskPacifism::computeReward() {
    return 200 + (rand() % 50) + (lifeSlashCardMin * 5) + (lifeSlashCardMin > 20 ? 200 : 0);
}

string TaskPacifism::createDesc() {
    char buffer[4096];

    switch (rand() % 2) {
    case 0:
        sprintf(buffer, _("Let your opponent live with at least %i life and cards in library, but defeat him.").c_str(),
                lifeSlashCardMin);
        break;
    case 1:
        sprintf(buffer, _("Win a game with your opponent having at least %i life and cards in library.").c_str(),
                lifeSlashCardMin);
        break;
    }
    return buffer;
}

string TaskPacifism::getShortDesc() {
    char buffer[4096];
    sprintf(buffer, _("Pacifism (%i)").c_str(), lifeSlashCardMin);
    return buffer;
}

bool TaskPacifism::isDone(GameObserver* observer, GameApp* _app) {
    return (!observer->players[0]->isAI()) && (observer->players[1]->isAI()) &&
           (observer->didWin(observer->players[0]))  // Human player wins
           && (observer->players[1]->life >= lifeSlashCardMin) &&
           ((int)observer->players[1]->game->library->cards.size() >= lifeSlashCardMin);
}

void TaskPacifism::storeCustomAttribs() {
    char buff[256];

    sprintf(buff, "%i", lifeSlashCardMin);
    persistentAttribs.emplace_back(buff);
}

void TaskPacifism::restoreCustomAttribs() { lifeSlashCardMin = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT].c_str()); }

void TaskPacifism::randomize() {
    lifeSlashCardMin = rand() % 25 + 1;
    Task::randomize();
}

/* ------------ Task template ------------

 TaskXX::TaskXX() : Task(TASK_XX) {
 // TODO: Implement
 }

 int TaskXX::computeReward() {
 // TODO: Implement
 return 100;
 }

 string TaskXX::createDesc() {
 // TODO: Implement
 char buffer[4096];

 switch (rand()%2) {
 case 0:
 sprintf(buffer, _("%s").c_str(), getOpponentName().c_str());
 break;
 case 1:
 sprintf(buffer, _("%s").c_str(), getOpponentName().c_str());
 break;
 }
 return buffer;
 }

 string TaskXX::getShortDesc(){
 // TODO: Implement
 char buffer[4096];
 sprintf(buffer, _("%s").c_str(), getOpponentName().c_str());
 return buffer;
 }

 bool TaskXX::isDone(GameObserver* observer, GameApp * _app) {
 // TODO: Implement
 return (!observer->players[0]->isAI()) && (observer->players[1]->isAI()) && (observer->gameOver != _p1) // Human player
 wins
 }

 void TaskXX::storeCustomAttribs() {
 // TODO: Implement
 char buff[256];
 persistentAttribs.push_back(VarXX);

 sprintf(buff, "%i", VarXY);
 persistentAttribs.push_back(buff);
 }

 void TaskXX::restoreCustomAttribs() {
 // TODO: Implement
 VarXX = persistentAttribs[COMMON_ATTRIBS_COUNT];
 VarXY = atoi(persistentAttribs[COMMON_ATTRIBS_COUNT+1].c_str());
 }

 void TaskXX::randomize() {
 // TODO: Implement
 VarXX = rand()%10 + 1;
 Task::randomize();
 }

 */
