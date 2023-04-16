//
//  SimpleButton.h
//  - base class for creating buttons/links inside of the game engine.
//
//  Created by Michael Nguyen on 1/21/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef SIMPLEBUTTON_H
#define SIMPLEBUTTON_H

#include <string>
#include <JLBFont.h>
#include <JGui.h>

using std::string;

#define SCALE_SELECTED 1.2f
#define SCALE_NORMAL 1.0f
#define SCALE_SHRINK 0.75f
#define SCALE_SELECTED_LARGE 1.7F
#define SCALE_LARGE_NORMAL 1.5F

class SimpleButton : public JGuiObject {
private:
    float mScale;
    float mTargetScale;
    bool mHasFocus;
    bool mIsValidSelection;
    float mX;
    float mY;

protected:
    float mYOffset;
    JGuiController* parent;
    int mFontId;
    string mText;
    float mXOffset;

public:
    SimpleButton(int id);
    SimpleButton(JGuiController* _parent,
                 int id,
                 int fontId,
                 const string& text,
                 float x,
                 float y,
                 bool hasFocus      = false,
                 bool autoTranslate = false);

    virtual float getScale() const;
    virtual float getTargetScale() const;
    virtual JGuiController* getParent() const;

    virtual int getFontId() const;
    virtual void setFontId(const int& fontId);
    virtual void setX(const float& x) { mX = x; };
    virtual void setY(const float& y) { mY = y; };

    virtual void setIsSelectionValid(bool validSelection);
    virtual void setFocus(bool value);
    virtual void setText(const string& text);

    virtual bool isSelectionValid() const;
    virtual bool hasFocus() const;
    virtual string getText() const;
    float getX() const;
    float getY() const;
    virtual void checkUserClick();

    virtual float GetWidth();
    virtual float GetEnlargedWidth();
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
    };
};

#endif
