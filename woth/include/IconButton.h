#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <string>
#include <JGui.h>

using std::string;

class IconButtonsController : public JGuiController, public JGuiListener {
public:
    float mX;
    float mY;
    IconButtonsController(JGE* jge, float x, float y);
    void SetColor(PIXEL_TYPE color);
};

class IconButton : public JGuiObject {
private:
    PIXEL_TYPE mColor;
    IconButtonsController* mParent;
    bool mHasFocus;
    int mFontId;
    std::string mText;
    float mScale, mCurrentScale, mTargetScale;
    float mX, mY;
    float mTextRelativeX, mTextRelativeY;
    JTexture* mTex;
    JQuad* mQuad;

public:
    IconButton(int id,
               IconButtonsController* parent,
               const std::string& texture,
               float x,
               float y,
               float scale,
               int fontId,
               std::string text,
               float textRelativeX,
               float textRelativeY,
               bool hasFocus = false);
    IconButton(int id,
               IconButtonsController* parent,
               JQuad* quad,
               float x,
               float y,
               float scale,
               int fontId,
               std::string text,
               float textRelativeX,
               float textRelativeY,
               bool hasFocus = false);
    void init(IconButtonsController* parent,
              JQuad* quad,
              float x,
              float y,
              float scale,
              int fontId,
              std::string text,
              float textRelativeX,
              float textRelativeY,
              bool hasFocus);
    ~IconButton() override;
    std::ostream& toString(std::ostream& out) const override;

    bool hasFocus() const;

    void Render() override;
    void Update(float dt) override;

    void Entering() override;
    bool Leaving(JButton key) override;
    bool ButtonPressed() override;
    void SetColor(PIXEL_TYPE color);
};

#endif
