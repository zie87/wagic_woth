#ifndef TEXTSCROLLER_H
#define TEXTSCROLLER_H

class JLBFont;
#include <JGui.h>
#include <string>
#include <vector>

class TextScroller : public JGuiObject {
protected:
    std::string mText;
    std::string tempText;
    int fontId;
    float mWidth;  // width of the text scroller object
    float mScrollSpeed;
    float mX;
    float mY;
    float start;
    int timer;

    std::vector<std::string> strings;
    unsigned int currentId;
    int mRandom;
    int scrollDirection;

public:
    TextScroller(int fontId, float x, float y, float width, float speed = 30);
    void Add(const std::string& text);
    void Reset();
    void setRandom(int mode = 1);
    void Render() override;
    void Update(float dt) override;
    std::ostream& toString(std::ostream& out) const override;
};

class VerticalTextScroller : public TextScroller {
private:
    size_t mNbItemsShown;
    float mHeight;  // maximum height availble for display
    float mMarginX;
    float mMarginY;  // margin used to allow text to scroll off screen without
    // affecting look and feel.  Should be enough
    // for at least one line of text ( mY - line height of current font )
    float mOriginalY;  // mY initially, used to restore scroller to original position after update

public:
    VerticalTextScroller(int fontId,
                         float x,
                         float y,
                         float width,
                         float height,
                         float scrollSpeed    = 30,
                         size_t _minimumItems = 1);
    void Render() override;
    void Update(float dt) override;
    void Add(const std::string& text);
};
#endif
