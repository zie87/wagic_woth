#ifndef MENUITEM_H
#define MENUITEM_H

#include "WFont.h"
#include <JGui.h>
#include <hge/hgeparticle.h>
#include <string>

#define SCALE_SELECTED 1.2f
#define SCALE_NORMAL 1.0f

class hgeParticleSystem;

class MenuItem : public JGuiObject {
protected:
    bool mHasFocus;
    WFont* mFont;
    std::string mText;
    float mX;
    float mY;
    int updatedSinceLastRender;
    float lastDt;

    float mScale;
    float mTargetScale;
    JQuad* onQuad;
    JQuad* offQuad;
    hgeParticleSystem* mParticleSys;

public:
    MenuItem(int id,
             WFont* font,
             std::string text,
             float x,
             float y,
             JQuad* _off,
             JQuad* _on,
             const char* particle,
             JQuad* particleQuad,
             bool hasFocus = false);
    ~MenuItem() override;
    void Render() override;
    void Update(float dt) override;

    void Entering() override;
    bool Leaving(JButton key) override;
    bool ButtonPressed() override;
    bool getTopLeft(float& top, float& left) override {
        top  = mY;
        left = mX;
        return true;
    };

    std::ostream& toString(std::ostream& out) const override;
};

class OtherMenuItem : public MenuItem {
private:
    JButton mKey;
    float mTimeIndex;

public:
    OtherMenuItem(int id,
                  WFont* font,
                  std::string text,
                  float x,
                  float y,
                  JQuad* _off,
                  JQuad* _on,
                  JButton _key,
                  bool hasFocus = false);
    ~OtherMenuItem() override;
    void Render() override;
    void Update(float dt) override;
};

#endif
