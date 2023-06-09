#include "PrecompiledHeader.h"

#include "StoryFlow.h"
#include "MTGDefinitions.h"
#include "WResourceManager.h"
#include "AIPlayer.h"

// TODO remove this dependency
#include "AIPlayerBaka.h"

#include "Rules.h"
#include "Credits.h"
#include "PlayerData.h"
#include "MTGDeck.h"
#include "WFont.h"
#include <JFileSystem.h>

#include <utility>

#define LINE_SPACE 2
#define SPACE_BEFORE_CHOICES 10

float StoryDialog::currentY         = 2;
float StoryDialog::previousY        = 2;
bool StoryReward::rewardSoundPlayed = false;
bool StoryReward::rewardsEnabled    = true;
MTGDeck* StoryReward::collection    = nullptr;

StoryDialogElement::StoryDialogElement(float x, float y, int id) : JGuiObject(id), mX(x), mY(y) {}

StoryText::StoryText(string text, float _mX, float _mY, const string& _align, int _font, int id)
    : StoryDialogElement(_mX, _mY, id)
    , text(std::move(text))
    , align(JGETEXT_LEFT)
    , font(_font) {
    if (_align == "center") {
        align = JGETEXT_CENTER;
        if (mX == 0) {
            mX = SCREEN_WIDTH / 2;
        }
    } else if (_align == "right") {
        align = JGETEXT_RIGHT;
        if (mX == 0) {
            mX = SCREEN_WIDTH - 10;
        }
    }
    if (align == JGETEXT_LEFT && mX <= 0) {
        mX += 10;  // left margin
    }
}
void StoryText::Render() {
    WFont* mFont = WResourceManager::Instance()->GetWFont(font);
    mFont->SetColor(ARGB(200, 255, 255, 255));
    mFont->SetScale(1.0);
    mFont->DrawString(text.c_str(), mX, mY, align);
}

float StoryText::getHeight() {
    WFont* mFont = WResourceManager::Instance()->GetWFont(font);
    return mFont->GetHeight();
}

void StoryText::Update(float dt) {
    // Nothing for now
}

StoryReward::StoryReward(const string& _type,
                         string _value,
                         string text,
                         float _mX,
                         float _mY,
                         const string& _align,
                         int _font,
                         int id)
    : StoryText(std::move(text), _mX, _mY, std::move(_align), _font, id)
    , type(STORY_REWARD_CREDITS) {
    if (_type == "unlockset") {
        type = STORY_REWARD_SET;
    } else if (_type == "card") {
        type = STORY_REWARD_CARD;
    }
    value      = std::move(_value);
    rewardDone = 0;
}

void StoryReward::Render() {
    if (rewardDone <= 0) {
        return;
    }
    StoryText::Render();
}

void StoryReward::Update(float dt) {
    if (rewardDone) {
        return;
    }

    int result = 0;

    switch (type) {
    case STORY_REWARD_CREDITS:
        result = Credits::addCreditBonus(atoi(value.c_str()));
        break;
    case STORY_REWARD_SET: {
        if (!value.empty()) {
            result = Credits::unlockSetByName(value);
        } else {
            result = Credits::unlockRandomSet(true);
        }
        if (!result) {
            break;
        }
        MTGSetInfo* si = setlist.getInfo(result - 1);
        if (si) {
            const string unlockedString = si->getName();
            const size_t pos            = text.find("${SET}");
            if (pos != string::npos) {
                text.replace(pos, pos + 6, unlockedString);
            }
        }
        break;
    }
    case STORY_REWARD_CARD: {
        int cardId    = 0;
        MTGCard* card = nullptr;
        if (!value.empty()) {
            card = MTGCollection()->getCardByName(value);
            if (card) {
                cardId = card->getId();
            }
        } else {
            cardId = MTGCollection()->randomCardId();
            card   = MTGCollection()->getCardById(cardId);
        }

        if (!cardId) {
            break;
        }

        if (!collection) {
            collection = NEW MTGDeck(options.profileFile(PLAYER_COLLECTION).c_str(), MTGCollection());
        }

        result           = Credits::addCardToCollection(cardId, collection);
        const size_t pos = text.find("${CARD}");
        if (pos != string::npos && card) {
            text.replace(pos, pos + 7, card->data->getName());
        }
        break;
    }
    default:
        break;
    }

    if (!result) {
        rewardDone = -1;
        return;
    }

    if (!rewardsEnabled) {
        rewardDone = -1;
        return;
    }

    if (!rewardSoundPlayed && options[Options::SFXVOLUME].number > 0) {
        WResourceManager::Instance()->PlaySample("bonus.wav");
        rewardSoundPlayed = 1;
    }
    rewardDone = 1;
}

std::ostream& StoryText::toString(std::ostream& out) const { return out << "StoryText ::: text : " << text; }

StoryImage::StoryImage(string img, float mX, float mY) : StoryDialogElement(mX, mY), img(std::move(img)) {}
void StoryImage::Render() {
    const JQuadPtr quad = WResourceManager::Instance()->RetrieveTempQuad(img);
    if (quad) {
        float x = mX;
        if (mX == -1) {
            x = SCREEN_WIDTH / 2;
            quad->SetHotSpot(quad->mWidth / 2, 0);
        }
        JRenderer::GetInstance()->RenderQuad(quad.get(), x, mY);
    }
}

float StoryImage::getHeight() {
    const JQuadPtr quad = WResourceManager::Instance()->RetrieveQuad(img);
    if (quad) {
        return quad->mHeight;
    }
    return 0;
}

void StoryImage::Update(float dt) {
    // Nothing for now
}

std::ostream& StoryImage::toString(std::ostream& out) const { return out << "StoryImage ::: img : " << img; }

StoryPage::StoryPage(StoryFlow* mParent) : mParent(mParent) {}

void StoryChoice::Render() {
    WFont* mFont = WResourceManager::Instance()->GetWFont(font);
    mFont->SetColor(ARGB(200, 255, 255, 255));
    if (mHasFocus) {
        mFont->SetColor(ARGB(255, 255, 255, 0));
    }
    mFont->SetScale(mScale);
    mFont->DrawString(text.c_str(), mX, mY, align);
}

float StoryChoice::getHeight() {
    WFont* mFont = WResourceManager::Instance()->GetWFont(font);
    return mFont->GetHeight() * mScale;
}

void StoryChoice::Update(float dt) {
    if (mScale < mTargetScale) {
        mScale += 8.0f * dt;
        if (mScale > mTargetScale) {
            mScale = mTargetScale;
        }
    } else if (mScale > mTargetScale) {
        mScale -= 8.0f * dt;
        if (mScale < mTargetScale) {
            mScale = mTargetScale;
        }
    }
}

void StoryChoice::Entering() {
    mHasFocus    = true;
    mTargetScale = 1.2f;
}

bool StoryChoice::Leaving(JButton key) {
    mHasFocus    = false;
    mTargetScale = 1.0f;
    return true;
}

bool StoryChoice::ButtonPressed() { return true; }

bool StoryChoice::hasFocus() const { return mHasFocus; }

std::ostream& StoryChoice::toString(std::ostream& out) const {
    return out << "StoryChoice ::: mHasFocus : " << mHasFocus;
}

StoryChoice::StoryChoice(string pageId,
                         string text,
                         int JGOid,
                         float mX,
                         float mY,
                         const string& _align,
                         int _font,
                         bool hasFocus)
    : StoryText(std::move(text), mX, mY, std::move(_align), _font, JGOid)
    , pageId(std::move(pageId))
    , mHasFocus(hasFocus)
    , mScale(1.0f)
    , mTargetScale(1.0f) {
    if (hasFocus) {
        mTargetScale = 1.2f;
    }
}

// Actually loads a duel
void StoryDuel::init() {
    game = new GameObserver(WResourceManager::Instance(), JGE::GetInstance());

    char folder[255];
    char deckFile[255];
    char deckFileSmall[255];
    sprintf(folder, CAMPAIGNS_FOLDER "%s/%s", mParent->folder.c_str(), pageId.c_str());

    sprintf(deckFile, "%s/deck.txt", folder);
    sprintf(deckFileSmall, "campaign_%s", mParent->folder.c_str());
    game->loadPlayer(0, NEW HumanPlayer(game, deckFile, deckFileSmall, true));

    sprintf(deckFile, "%s/opponent_deck.txt", folder);
    sprintf(deckFileSmall, "campaign_ennemy_%s_%s", mParent->folder.c_str(), pageId.c_str());
    game->loadPlayer(1, NEW AIPlayerBaka(game, deckFile, deckFileSmall, "baka.jpg"));

    string rulesFile = folder;
    rulesFile.append("/rules.txt");
    rules = NEW Rules(bg);
    rules->load(rulesFile);

    rules->gamemode = GAME_TYPE_STORY;
    game->startGame(GAME_TYPE_STORY, rules);
}

StoryDuel::StoryDuel(TiXmlElement* root, StoryFlow* mParent)
    : StoryPage(mParent)
    , game(nullptr)
    , pageId(root->Attribute("id"))
    , rules(nullptr) {
    for (TiXmlNode* node = root->FirstChild(); node; node = node->NextSibling()) {
        TiXmlElement* element = node->ToElement();
        if (element) {
            const char* textC = element->GetText();
            if (strcmp(element->Value(), "onwin") == 0) {
                onWin = textC;
            } else if (strcmp(element->Value(), "onlose") == 0) {
                onLose = textC;
            } else if (strcmp(element->Value(), "bg") == 0) {
                const string text = textC;
                bg                = string("campaigns/").append(mParent->folder).append("/").append(text);
                if (!fileExists(bg.c_str())) {
                    bg = text;
                }
            } else {
                StoryPage::loadElement(element);  // Father
            }
        }
    }
}

StoryDuel::~StoryDuel() {
    SAFE_DELETE(rules);
    SAFE_DELETE(game);
}

void StoryDuel::Update(float dt) {
    if (!game) {
        init();
    }
    game->Update(dt);
    if (game->didWin()) {
        if (game->didWin(game->players[0])) {
            mParent->gotoPage(onWin);
        } else {
            mParent->gotoPage(onLose);
        }
        SAFE_DELETE(game);
    }
}

void StoryDuel::Render() {
    if (!game) {
        return;
    }
    game->Render();
}

string StoryPage::safeAttribute(TiXmlElement* element, const string& attribute) {
    string s;
    if (element->Attribute(attribute.c_str())) {
        s = element->Attribute(attribute.c_str());
    }
    return s;
}

int StoryPage::loadElement(TiXmlElement* element) {
    if (!element) {
        return 0;
    }
    const char* textC = element->GetText();
    const string text = textC;
    if (strcmp(element->Value(), "music") == 0) {
        musicFile = string("campaigns/").append(mParent->folder).append("/").append(text);
        if (!fileExists(musicFile.c_str())) {
            musicFile = text;
        }
        return 1;
    }
    return 0;
}

StoryDialog::StoryDialog(TiXmlElement* root, StoryFlow* mParent)
    : StoryPage(mParent)
    , JGuiListener()
    , JGuiController(JGE::GetInstance(), 1, nullptr) {
    currentY = 0;

    for (TiXmlNode* node = root->FirstChild(); node; node = node->NextSibling()) {
        TiXmlElement* element = node->ToElement();
        if (element) {
            const string sX = safeAttribute(element, "x");
            auto x          = static_cast<float>(atof(sX.c_str()));
            if (x > 0 && x < 1) {
                x = SCREEN_WIDTH_F * x;
            }
            const string sY = safeAttribute(element, "y");
            auto y          = static_cast<float>(atof(sY.c_str()));
            if (y > 0 && y < 1) {
                y = SCREEN_HEIGHT_F * y;
            }
            string align       = safeAttribute(element, "align");
            const char* textC  = element->GetText();
            const string text  = textC;
            const string sFont = safeAttribute(element, "font");
            const int font     = atoi(sFont.c_str());

            if (strcmp(element->Value(), "text") == 0) {
                graphics.push_back(NEW StoryText(text, x, y, align, font));
            } else if (strcmp(element->Value(), "title") == 0) {
                graphics.push_back(NEW StoryText(text, x, y, "center", Fonts::MENU_FONT));
            } else if (strcmp(element->Value(), "img") == 0) {
                // special case to force center
                if (sX.empty()) {
                    x = -1;
                }
                const string img = string("campaigns/").append(mParent->folder).append("/").append(text);
                graphics.push_back(NEW StoryImage(img, x, y));
            } else if (strcmp(element->Value(), "answer") == 0) {
                const string id = element->Attribute("goto");
                if (align.empty()) {
                    align = "center";
                }
                const int i = mObjects.size();
                auto* sc    = NEW StoryChoice(id, text, i, x, y, align, font, (i == 0));
                graphics.push_back(sc);
                Add(sc);
            } else if (strcmp(element->Value(), "reward") == 0) {
                const string type  = safeAttribute(element, "type");
                const string value = safeAttribute(element, "value");
                graphics.push_back(NEW StoryReward(type, value, text, x, y, align, font));
            } else {
                StoryPage::loadElement(element);  // Father
            }
        }
    }
    this->mListener = this;
}

void StoryDialog::Update(float dt) {
    for (size_t i = 0; i < graphics.size(); ++i) {
        graphics[i]->Update(dt);
    }

    if (StoryReward::collection) {
        StoryReward::collection->save();
        SAFE_DELETE(StoryReward::collection);
    }

    const JButton key = mEngine->ReadButton();
    CheckUserInput(key);
}

void StoryDialog::RenderElement(StoryDialogElement* elmt) {
    if (!elmt->mY) {
        elmt->mY = currentY;
    }
    if (elmt->mY == -1) {
        elmt->mY = previousY;
    }
    elmt->Render();
    previousY = currentY;
    currentY  = elmt->mY + elmt->getHeight() + LINE_SPACE;
}

void StoryDialog::Render() {
    currentY  = 2;
    previousY = currentY;
    for (size_t i = 0; i < graphics.size(); ++i) {
        auto* elmt = (StoryDialogElement*)(graphics[i]);
        if (mCount && elmt == mObjects[0]) {
            currentY += SPACE_BEFORE_CHOICES;
        }
        RenderElement(elmt);
    }
}

void StoryDialog::ButtonPressed(int controllerid, int controlid) {
    if (controlid == kInfoMenuID) {
        return;
    }
    if (controlid == kCancelMenuID) {
        return;
    }

    mParent->gotoPage((dynamic_cast<StoryChoice*>(mObjects[controlid]))->pageId);
}

StoryDialog::~StoryDialog() {
    mCount = 0;  // avoid parent deleting
    for (size_t i = 0; i < graphics.size(); ++i) {
        delete (graphics[i]);
    }
}

StoryFlow::StoryFlow(const string& folder) : folder(folder) {
    string path = "campaigns/";
    path.append(folder).append("/story.xml");
    parse(path);
}

StoryPage* StoryFlow::loadPage(TiXmlElement* element) {
    TiXmlNode* typeNode = element->FirstChild("type");
    if (!typeNode) {
        return nullptr;
    }
    StoryPage* result = nullptr;
    const char* type  = typeNode->ToElement()->GetText();
    if (strcmp(type, "duel") == 0) {
        result = NEW StoryDuel(element, this);
    } else {
        result = NEW StoryDialog(element, this);
    }
    return result;
}

//
bool StoryFlow::_gotoPage(const string& id) {
    StoryReward::rewardSoundPlayed = false;
    if (pages.find(id) == pages.end()) {
        return false;
    }
    currentPageId = id;
    if (!pages[currentPageId]->musicFile.empty()) {
        GameApp::playMusic(pages[currentPageId]->musicFile);
    }
    return true;
}

bool StoryFlow::gotoPage(const string& id) {
    StoryReward::rewardsEnabled = true;
    return _gotoPage(std::move(id));
}

bool StoryFlow::loadPageId(const string& id) {
    StoryReward::rewardsEnabled = false;
    return _gotoPage(std::move(id));
}

bool StoryFlow::parse(const string& path) {
    JFileSystem* fileSystem = JFileSystem::GetInstance();
    if (!fileSystem) {
        return false;
    }

    if (!fileSystem->OpenFile(path)) {
        return false;
    }

    const int size  = fileSystem->GetFileSize();
    char* xmlBuffer = NEW char[size];
    fileSystem->ReadFile(xmlBuffer, size);

    TiXmlDocument doc;
    doc.Parse(xmlBuffer);

    fileSystem->CloseFile();
    delete[] xmlBuffer;

    for (TiXmlNode* node = doc.FirstChild(); node; node = node->NextSibling()) {
        TiXmlElement* element = node->ToElement();
        if (element != nullptr) {
            if (strcmp(element->Value(), "page") == 0) {
                const string id = element->Attribute("id");

                DebugTrace("parsing " << id << "...");

                StoryPage* sp = loadPage(element);
                pages[id]     = sp;
                if (currentPageId.empty()) {
                    gotoPage(id);
                }
            } else {
                // Error
            }
        }
    }

    // autoLoad
    auto* pd = NEW PlayerData();
    auto it  = pd->storySaves.find(folder);
    if (it != pd->storySaves.end()) {
        if (it->second != "End") {
            loadPageId(it->second);
        }
    }
    SAFE_DELETE(pd);

    return true;
}

void StoryFlow::Update(float dt) { pages[currentPageId]->Update(dt); }

void StoryFlow::Render() { pages[currentPageId]->Render(); }

StoryFlow::~StoryFlow() {
    for (auto i = pages.begin(); i != pages.end(); ++i) {
        SAFE_DELETE(i->second);
    }
    pages.clear();

    // autoSave progress
    auto* pd               = NEW PlayerData();
    pd->storySaves[folder] = currentPageId;
    pd->save();
    SAFE_DELETE(pd);
}
