#ifndef DECKMENUITEM_H
#define DECKMENUITEM_H

#include <string>
#include <JLBFont.h>
#include <JGui.h>
#include "DeckMenu.h"

using std::string;

class DeckMenuItem : public JGuiObject {
private:
    bool mHasFocus;
    bool mScrollEnabled;
    bool mDisplayInitialized;
    bool mIsValidSelection;

    DeckMenu* parent;
    int fontId;
    std::string mText;
    float mX;
    float mY;
    float mTitleResetWidth;
    std::string mDescription;
    static float mYOffset;
    float mScrollerOffset;
    DeckMetaData* mMetaData;
    std::string mImageFilename;
    void checkUserClick();

public:
    DeckMenuItem(DeckMenu* _parent, int id, int fontId, string text, float x, float y, bool hasFocus = false,
                 bool autoTranslate = false, DeckMetaData* meta = nullptr);
    ~DeckMenuItem() override;

    virtual void Relocate(float x, float y);
    virtual void RenderWithOffset(float yOffset);
    void Render() override;
    void Update(float dt) override;
    void Entering() override;
    bool Leaving(JButton key) override;
    bool ButtonPressed() override;
    std::ostream& toString(std::ostream& out) const override;

    bool getTopLeft(float& top, float& left) override {
        top  = mY + mYOffset;
        left = mX;
        return true;
    }

    // Accessors

    std::string getImageFilename() const { return mImageFilename; };
    float getY() const { return mY; };
    float getX() const { return mX; };
    std::string getDescription() const { return mDescription; };
    std::string getText() const { return mText; };
    bool hasFocus() const { return mHasFocus; };
    bool hasMetaData() const { return mMetaData == nullptr ? false : true; };

    float getWidth() const;
    std::string getDeckName() const;

    std::string getDeckStatsSummary() const {
        if (mMetaData) {
            return mMetaData->getStatsSummary();
        }
        return "";
    }

    DeckMetaData* getMetaData() const { return mMetaData; }

    // Setters
    void setDescription(const std::string& description) { mDescription = description; };

    ;
};

#endif
