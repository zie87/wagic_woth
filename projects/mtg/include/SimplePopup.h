/*
 * SimplePopup.h
 * Created on: Nov 18, 2010
 *
 *      Simple popup dialog box for displaying information only.
 */

#ifndef SIMPLEPOPUP_H_
#define SIMPLEPOPUP_H_

#pragma once
#include <JGui.h>
#include <JTypes.h>
#include <WFont.h>
#include <DeckMetaData.h>

class SimplePopup : public JGuiController {
private:
    float mWidth, mX, mY;
    int mMaxLines;
    int mFontId;
    DeckMetaData* mDeckInformation;
    string mTitle;
    WFont* mTextFont;
    StatsWrapper* mStatsWrapper;
    bool mClosed;
    MTGAllCards* mCollection;

    void drawHorzPole(const string& imageName, bool flipX, bool flipY, float x, float y, float width);
    void drawCorner(const string& imageName, bool flipX, bool flipY, float x, float y);
    void drawVertPole(const string& imageName, bool flipX, bool flipY, float x, float y, float height);

public:
    bool autoTranslate;

    SimplePopup(int id, JGuiListener* listener, const int fontId, const char* _title = "",
                DeckMetaData* deckInfo = nullptr, MTGAllCards* collection = nullptr, float x = 364, float y = 235);
    ~SimplePopup(void) override;
    void drawBoundingBox(float x, float y, float width, float height);
    bool isClosed() const { return mClosed; }
    MTGAllCards* getCollection() { return mCollection; }
    void Render() override;
    void Update(DeckMetaData* deckMetaData);

    string getDetailedInformation(const string& deckFilename);

    void Update(float dt) override;
    void Close();
};

#endif /* SIMPLEPOPUP_H_ */
