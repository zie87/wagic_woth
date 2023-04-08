#include "PrecompiledHeader.h"

#include "DeckMenu.h"
#include "DeckMenuItem.h"
#include "DeckMetaData.h"
#include "DeckManager.h"
#include "InteractiveButton.h"
#include "JTypes.h"
#include "GameApp.h"
#include "Translate.h"
#include "TextScroller.h"
#include "Tasks.h"

#include <iomanip>
#include <utility>

namespace {
const float kVerticalMargin                  = 16;
const float kHorizontalMargin                = 20;
const float kLineHeight                      = 25;
const float kDescriptionVerticalBoxPadding   = -5;
const float kDescriptionHorizontalBoxPadding = 5;

const float kDefaultFontScale    = 1.0f;
const float kVerticalScrollSpeed = 7.0f;

const int DETAILED_INFO_THRESHOLD = 20;
const int kDetailedInfoButtonId   = 10000;

const PIXEL_TYPE kRedColor = ARGB(0xFF, 0xFF, 0x00, 0x00);
}  // namespace

hgeParticleSystem* DeckMenu::stars = nullptr;

//
//  For the additional info window, maximum characters per line is roughly 30 characters across.
//  TODO:
//    *** Need to make this configurable in a file somewhere to allow for class reuse

DeckMenu::DeckMenu(int id, JGuiListener* listener, int fontId, const string& _title, const int& startIndex,
                   bool showDetailsOverride)
    : JGuiController(JGE::GetInstance(), id, listener)
    , avatarX(232)
    , avatarY(11)
    , backgroundName("DeckMenuBackdrop")
    , descHeight(145)
    , descWidth(195)
    , descX(260 + kDescriptionHorizontalBoxPadding)
    , descY(100 + kDescriptionVerticalBoxPadding)
    , detailedInfoBoxX(400)
    , detailedInfoBoxY(235)
    , mWidth(176)
    , mX(115)
    , mY(50)
    , menuInitialized(false)
    , starsOffsetX(50)
    , statsHeight(50)
    , statsWidth(227)
    , statsX(300)
    , statsY(15)
    , titleWidth(180)
    , titleX(110)
    , titleY(15)
    , fontId(fontId)
    , displayTitle(title)
    , mFont(WResourceManager::Instance()->GetWFont(fontId))
    , mSelectedDeck(nullptr)
    , maxItems(6)
    , selectionT(0)
    , startId(0)
    , timeOpen(0)
    , mShowDetailsScreen(showDetailsOverride)
    , mAlwaysShowDetailsButton(false)
    , mAutoTranslate(true)
    , mClosed(false) {
    // center point in title box

    // width of inner box of title

    const float scrollerWidth  = 200.0f;
    const float scrollerHeight = 28.0f;
    mScroller =
        NEW VerticalTextScroller(Fonts::MAIN_FONT, 14, 235, scrollerWidth, scrollerHeight, kVerticalScrollSpeed);

    mHeight = 2 * kVerticalMargin + (maxItems * kLineHeight);

    // we want to cap the deck titles to 15 characters to avoid overflowing deck names
    title = _(_title);

    if (mFont->GetStringWidth(title.c_str()) > titleWidth) {
        titleFontScale = SCALE_SHRINK;
    } else {
        titleFontScale = SCALE_NORMAL;
    }

    mSelectionTargetY = selectionY = kVerticalMargin;

    if (nullptr == stars) {
        stars = NEW hgeParticleSystem(WResourceManager::Instance()->RetrievePSI(
            "stars.psi", WResourceManager::Instance()->GetQuad("stars").get()));
    }
    stars->FireAt(mX, mY);

    const string detailedInfoString = _("Detailed Info");
    WFont* descriptionFont          = WResourceManager::Instance()->GetWFont(Fonts::MAIN_FONT);

    const float stringWidth = descriptionFont->GetStringWidth(detailedInfoString.c_str());
    const float boxStartX   = detailedInfoBoxX - stringWidth / 2 + 20;

    dismissButton = NEW InteractiveButton(this, kDetailedInfoButtonId, Fonts::MAIN_FONT, detailedInfoString, boxStartX,
                                          detailedInfoBoxY, JGE_BTN_CANCEL);
    JGuiController::Add(dismissButton, true);

    updateScroller();
}

void DeckMenu::RenderDeckManaColors() {
    // display the deck mana colors if known
    // We only want to display the mana symbols on the game screens and not the Deck Editor screens.
    // Need a better way to determine where/when to display the mana symbols.  Perhaps make it a property setting.
    const bool displayDeckMana = backgroundName.find("DeckMenuBackdrop") != string::npos;
    // current set of coordinates puts the mana symbols to the right of the last stat info in the upper right
    // box of the deck selection screen.
    float manaIconX       = 398;
    const float manaIconY = 55;
    if (mSelectedDeck && displayDeckMana) {
        string deckManaColors = mSelectedDeck->getColorIndex();
        if (!deckManaColors.empty() && (deckManaColors.length() == 6)) {
            // due to space constraints don't display icons for colorless mana.
            for (int colorIdx = Constants::MTG_COLOR_GREEN; colorIdx < Constants::MTG_COLOR_LAND; ++colorIdx) {
                if ((deckManaColors.at(colorIdx) == '1') != 0) {
                    JRenderer::GetInstance()->RenderQuad(manaIcons[colorIdx].get(), manaIconX, manaIconY, 0, 0.5f,
                                                         0.5f);
                    manaIconX += 15;
                }
            }
        } else if (!deckManaColors.empty())
            DebugTrace("Error with color index string for " << mSelectedDeck->getName() << ". [" << deckManaColors
                                                            << "].");
    }
}

void DeckMenu::RenderBackground() {
    std::ostringstream bgFilename;
    bgFilename << backgroundName << ".png";

    static bool loadBackground = true;
    if (loadBackground) {
        // Erwan 2010/07/11 the TEXTURE_SUB_5551 below is useless, JGE doesn't support it for png. I'm letting it here
        // to avoid causing a bug, but it should be removed
        const JQuadPtr background = WResourceManager::Instance()->RetrieveTempQuad(bgFilename.str(), TEXTURE_SUB_5551);
        if (background.get()) {
            const float scaleX = SCREEN_WIDTH_F / background.get()->mWidth;
            const float scaleY = SCREEN_HEIGHT_F / background.get()->mHeight;
            JRenderer::GetInstance()->RenderQuad(background.get(), 0, 0, 0, scaleX, scaleY);
        } else {
            loadBackground = false;
        }
    }
}

DeckMetaData* DeckMenu::getSelectedDeck() {
    if (mSelectedDeck) {
        return mSelectedDeck;
    }

    return nullptr;
}

void DeckMenu::enableDisplayDetailsOverride() { mAlwaysShowDetailsButton = true; }

bool DeckMenu::showDetailsScreen() {
    DeckMetaData* currentMenuItem = getSelectedDeck();
    if (currentMenuItem) {
        if (mAlwaysShowDetailsButton) {
            return true;
        }
        if (mShowDetailsScreen && currentMenuItem->getVictories() > DETAILED_INFO_THRESHOLD) {
            return true;
        }
    }

    return false;
}

void DeckMenu::initMenuItems() {
    float sY = mY + kVerticalMargin;
    for (int i = startId; i < mCount; ++i) {
        const float y         = mY + kVerticalMargin + i * kLineHeight;
        auto* currentMenuItem = dynamic_cast<DeckMenuItem*>(mObjects[i]);
        currentMenuItem->Relocate(mX, y);
        if (currentMenuItem->hasFocus()) {
            sY = y;
        }
    }
    mSelectionTargetY = selectionY = sY;

#ifndef TOUCH_ENABLED
    // Grab a texture in VRAM.
    pspIconsTexture = WResourceManager::Instance()->RetrieveTexture("iconspsp.png", RETRIEVE_MANAGE);

    char buf[512];
    for (int i = 0; i < 8; i++) {
        sprintf(buf, "iconspsp%d", i);
        pspIcons[i] = WResourceManager::Instance()->RetrieveQuad("iconspsp.png", (float)i * 32, 0, 32, 32, buf);
        pspIcons[i]->SetHotSpot(16, 16);
    }
    dismissButton->setImage(pspIcons[5]);
#endif
}

void DeckMenu::selectRandomDeck(bool isAi) {
    DeckManager* deckManager        = DeckManager::GetInstance();
    vector<DeckMetaData*>* deckList = isAi ? deckManager->getAIDeckOrderList() : deckManager->getPlayerDeckOrderList();
    const int random                = (WRand() * 1000) % deckList->size();
    selectDeck(random, isAi);
}

void DeckMenu::selectDeck(int deckId, bool isAi) {
    DeckManager* deckManager        = DeckManager::GetInstance();
    vector<DeckMetaData*>* deckList = isAi ? deckManager->getAIDeckOrderList() : deckManager->getPlayerDeckOrderList();
    mSelectedDeck                   = deckList->at(deckId);
}

void DeckMenu::Render() {
    JRenderer* renderer = JRenderer::GetInstance();
    float height        = mHeight;

    if (!menuInitialized) {
        initMenuItems();
        stars->Fire();
        timeOpen        = 0;
        menuInitialized = true;
    }

    if (timeOpen < 1) {
        height *= timeOpen > 0 ? timeOpen : -timeOpen;
    }

    for (int i = startId; i < startId + maxItems; i++) {
        if (i > mCount - 1) {
            break;
        }
        auto* currentMenuItem = dynamic_cast<DeckMenuItem*>(mObjects[i]);
        if (currentMenuItem->getY() - kLineHeight * startId < mY + height - kLineHeight + 7) {
            // only load stats for visible items in the list
            DeckMetaData* metaData = currentMenuItem->getMetaData();
            if (metaData && !metaData->mStatsLoaded) {
                metaData->LoadStats();
            }

            if (currentMenuItem->hasFocus()) {
                mSelectedDeck          = metaData;
                WFont* descriptionFont = WResourceManager::Instance()->GetWFont(Fonts::MAIN_FONT);

                // display the "more info" button if special condition is met
                if (showDetailsScreen()) {
                    dismissButton->setIsSelectionValid(true);
                    dismissButton->Render();
                } else {
                    dismissButton->setIsSelectionValid(false);
                }
                // display the avatar image
                const string currentAvatarImageName = currentMenuItem->getImageFilename();
                if (!currentAvatarImageName.empty()) {
                    const JQuadPtr quad =
                        WResourceManager::Instance()->RetrieveTempQuad(currentAvatarImageName, TEXTURE_SUB_AVATAR);
                    if (quad.get()) {
                        if (currentMenuItem->getText() == "Evil Twin") {
                            JQuad* evil = quad.get();
                            evil->SetHFlip(true);
                            renderer->RenderQuad(quad.get(), avatarX, avatarY);
                            evil = nullptr;
                        } else {
                            renderer->RenderQuad(quad.get(), avatarX, avatarY);
                        }
                    }
                }

                // fill in the description part of the screen
                const string text = wordWrap(_(currentMenuItem->getDescription()), descWidth, descriptionFont->mFontID);
                descriptionFont->DrawString(text.c_str(), descX, descY);

                // fill in the statistical portion
                if (currentMenuItem->hasMetaData()) {
                    std::ostringstream oss;
                    oss << _("Deck: ") << currentMenuItem->getDeckName() << std::endl;
                    oss << currentMenuItem->getDeckStatsSummary();
                    descriptionFont->DrawString(oss.str(), statsX, statsY);
                }

                // change the font color of the current menu item
                mFont->SetColor(ARGB(255, 255, 255, 255));
            } else {  // reset the font color to be slightly muted
                mFont->SetColor(ARGB(150, 255, 255, 255));
            }
            currentMenuItem->RenderWithOffset(-kLineHeight * startId);
        }
    }

    if (!title.empty()) {
        mFont->SetColor(ARGB(255, 255, 255, 255));
        mFont->DrawString(title.c_str(), titleX, titleY, JGETEXT_CENTER);
    }

    mScroller->Render();
    RenderBackground();
    RenderDeckManaColors();

    renderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE);
    stars->Render();
    renderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
}

void DeckMenu::Update(float dt) {
    JGuiController::Update(dt);

    if (mCurr > startId + maxItems - 1) {
        startId = mCurr - maxItems + 1;
    } else if (mCurr < startId) {
        startId = mCurr;
    }

    stars->Update(dt);
    selectionT += 3 * dt;
    selectionY += (mSelectionTargetY - selectionY) * 8 * dt;

    const float starsX = starsOffsetX + ((mWidth - 2 * kHorizontalMargin) * (1 + cos(selectionT)) / 2);
    const float starsY = selectionY + 5 * cos(selectionT * 2.35f) + kLineHeight / 2 - kLineHeight * startId;
    stars->MoveTo(starsX, starsY);

    //

    if (timeOpen < 0) {
        timeOpen += dt * 10;
        if (timeOpen >= 0) {
            timeOpen = 0;
            mClosed  = true;
            stars->FireAt(mX, mY);
        }
    } else {
        mClosed = false;
        timeOpen += dt * 10;
    }
    if (mScroller) {
        mScroller->Update(dt);
    }
}

void DeckMenu::Add(int id, const char* text, string desc, bool forceFocus, DeckMetaData* deckMetaData) {
    auto* menuItem = NEW DeckMenuItem(this, id, fontId, text, 0, mY + kVerticalMargin + mCount * kLineHeight,
                                      (mCount == 0), mAutoTranslate, deckMetaData);
    Translator* t  = Translator::GetInstance();
    auto it        = t->deckValues.find(text);
    string deckDescription;

    if (it != t->deckValues.end()) {  // translate decks desc
        deckDescription = it->second;
    } else {
        deckDescription = deckMetaData ? deckMetaData->getDescription() : std::move(desc);
    }

    menuItem->setDescription(deckDescription);

    JGuiController::Add(menuItem);
    if (mCount <= maxItems) {
        mHeight += kLineHeight;
    }

    if (forceFocus) {
        mObjects[mCurr]->Leaving(JGE_BTN_DOWN);
        mCurr = mCount - 1;
        menuItem->Entering();
    }
}

void DeckMenu::updateScroller() const {
    // add all the items from the Tasks db.
    TaskList taskList;
    mScroller->Reset();

    for (auto it = taskList.tasks.begin(); it != taskList.tasks.end(); it++) {
        std::ostringstream taskDescription;
        taskDescription << "Credits: " << std::setw(4) << (*it)->getReward() << " / "
                        << "Days Left: " << (*it)->getExpiration() << std::endl
                        << (*it)->getDesc() << std::endl
                        << std::endl;
        mScroller->Add(taskDescription.str());
    }
}

void DeckMenu::Close() {
    timeOpen = -1.0;
    stars->Stop(true);
}

void DeckMenu::destroy() { SAFE_DELETE(DeckMenu::stars); }

DeckMenu::~DeckMenu() {
    WResourceManager::Instance()->Release(pspIconsTexture);
    SAFE_DELETE(mScroller);
    dismissButton = nullptr;
}
