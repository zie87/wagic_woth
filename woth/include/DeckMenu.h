/*
 A class for menus with a fixed layout
 */
#ifndef DECKMENU_H
#define DECKMENU_H

#include <string>
#include "WFont.h"
#include "hge/hgeparticle.h"
#include "DeckMetaData.h"
#include "TextScroller.h"
#include "InteractiveButton.h"

class DeckMenu : public JGuiController {
private:
    InteractiveButton* dismissButton;

protected:
    float mHeight, mWidth, mX, mY;
    float titleX, titleY, titleWidth;
    float descX, descY, descHeight, descWidth;
    float statsX, statsY, statsHeight, statsWidth;
    float avatarX, avatarY;
    float detailedInfoBoxX, detailedInfoBoxY;
    float starsOffsetX;

    bool menuInitialized;
    string backgroundName;

    int fontId;
    string title;
    string displayTitle;
    WFont* mFont;
    float titleFontScale;

    int maxItems, startId;

    float selectionT, selectionY;
    float timeOpen;

    static hgeParticleSystem* stars;

    void initMenuItems();
    string getDescription();
    string getMetaInformation();
    DeckMetaData* mSelectedDeck;
    bool mShowDetailsScreen;
    bool mAlwaysShowDetailsButton;
    bool mClosed;

public:
    VerticalTextScroller* mScroller;
    bool mAutoTranslate;
    float mSelectionTargetY;

    int getSelectedDeckId() const { return mSelectedDeck->getDeckId(); }

    void selectDeck(int deckId, bool isAi);
    void selectRandomDeck(bool isAi);

    // used for detailed info button
    JQuadPtr pspIcons[8];
    JTexture* pspIconsTexture;

    DeckMenu(int id,
             JGuiListener* listener,
             int fontId,
             const string& _title         = "",
             const int& startIndex        = 0,
             bool alwaysShowDetailsButton = false);
    ~DeckMenu() override;

    DeckMetaData* getSelectedDeck();
    void enableDisplayDetailsOverride();
    bool showDetailsScreen();

    virtual bool isClosed() const { return mClosed; }

    void Render() override;
    void Update(float dt) override;
    virtual void Add(int id,
                     const char* Text,
                     string desc                = "",
                     bool forceFocus            = false,
                     DeckMetaData* deckMetaData = nullptr);
    virtual void Close();
    void updateScroller() const;
    void RenderBackground();
    void RenderDeckManaColors();

    static void destroy();
};

#endif
