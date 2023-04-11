#include "woth/string/algorithms.hpp"

#include "PrecompiledHeader.h"

#include "TextScroller.h"
#include "WResourceManager.h"
#include "utils.h"
#include "WFont.h"

TextScroller::TextScroller(int fontId, float x, float y, float width, float scrollSpeed)
    : JGuiObject(0)
    , fontId(fontId)
    , currentId(0)
    , mRandom(0)
    , mScrollSpeed(scrollSpeed)
    , mWidth(width)
    , mX(x)
    , mY(y)
    , start(-width)
    , timer(0) {}

void TextScroller::setRandom(int mode) {
    mRandom = mode;
    if (mRandom && !strings.empty()) {
        currentId = (rand() % strings.size());
        mText     = strings[currentId];
    }
}

void TextScroller::Add(const std::string& text) {
    if (strings.empty()) {
        mText = text;
    }
    strings.push_back(text);
}

void TextScroller::Reset() {
    strings.clear();
    currentId = 0;
}

void TextScroller::Update(float dt) {
    if (strings.empty()) {
        return;
    }

    start += mScrollSpeed * dt;
    WFont* mFont = WResourceManager::Instance()->GetWFont(fontId);
    if (start > mFont->GetStringWidth(mText.c_str())) {
        start = -mWidth;
        if (mRandom) {
            currentId = (rand() % strings.size());
        } else {
            currentId++;
            if (currentId >= strings.size()) {
                currentId = 0;
            }
        }
        mText = strings[currentId];
    }
}

void TextScroller::Render() {
    WFont* mFont = WResourceManager::Instance()->GetWFont(fontId);
    mFont->DrawString(mText.c_str(), mX, mY, JGETEXT_LEFT, start, mWidth);
}

std::ostream& TextScroller::toString(std::ostream& out) const {
    return out << "TextScroller ::: mText : " << mText << " ; tempText : " << tempText << " ; mWidth : " << mWidth
               << " ; mSpeed : " << mScrollSpeed << " ; mX,mY : " << mX << "," << mY << " ; start : " << start
               << " ; timer : " << timer << " ; strings : ?"
               << " ; currentId : " << currentId << " ; mRandom : " << mRandom;
}

VerticalTextScroller::VerticalTextScroller(int fontId, float x, float y, float width, float height, float scrollSpeed,
                                           size_t numItemsShown)
    : TextScroller(fontId, x, y, width, scrollSpeed)
    , mHeight(height)
    , mMarginX(0)
    , mNbItemsShown(numItemsShown)
    , mOriginalY(mY) {
    timer        = 0;
    WFont* mFont = WResourceManager::Instance()->GetWFont(fontId);

    mMarginY = mY - mFont->GetHeight();
    Add("\n");  // initialize the scroller with a blank line
}

void VerticalTextScroller::Add(const std::string& text) {
    strings.push_back(text);
    std::string const wrappedText = wordWrap(text, mWidth, fontId);
    mText.append(wrappedText);
}

/*
        Updates happen everytime the top line disappears from view.
        The top line is then moved to the end of the file and the scrolling resumes where it left off

*/
void VerticalTextScroller::Update(float dt) {
    if (strings.empty()) {
        return;
    }

    const float currentYOffset = mScrollSpeed * dt;

    if (mY <= mMarginY)  // top line has disappeared
    {
        timer = 0;
        // now readjust mText
        const size_t nbLines                 = 1;
        std::vector<std::string> displayText = ::woth::split(mText, '\n');
        std::vector<std::string> newDisplayText;
        for (size_t i = nbLines; i < displayText.size(); ++i) {
            newDisplayText.push_back(displayText[i]);
        }
        for (size_t i = 0; i < nbLines; ++i) {
            newDisplayText.push_back(displayText[i]);
        }

        mText = ::woth::join(newDisplayText, '\n');
        mY    = mOriginalY;
    }
    ++timer;
    mY -= currentYOffset;
}

void VerticalTextScroller::Render() {
    WFont* mFont = WResourceManager::Instance()->GetWFont(fontId);
    mFont->DrawString(mText.c_str(), mX, mY);
}
