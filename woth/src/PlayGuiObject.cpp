#include "PrecompiledHeader.h"

#include "PlayGuiObject.h"

PlayGuiObject::PlayGuiObject(float desiredHeight, float x, float y, int inID, bool hasFocus)
    : JGuiObject(inID)
    , Pos(x, y, 1.0, 0.0, 255)
    , defaultHeight(desiredHeight)
    , mHasFocus(hasFocus)
    , mHeight(desiredHeight)
    , type(0)
    , wave(0) {}
PlayGuiObject::PlayGuiObject(float desiredHeight, const Pos& ref, int inID, bool hasFocus)
    : JGuiObject(inID)
    , Pos(ref)
    , defaultHeight(desiredHeight)
    , mHasFocus(hasFocus)
    , mHeight(desiredHeight)
    , type(0)
    , wave(0) {}

void PlayGuiObject::Update(float dt) {
    if (mHasFocus && mHeight < defaultHeight * 1.2f) {
        mHeight += defaultHeight * 0.8f * dt;
        //      fprintf(stderr, "increasing size to %f - %d", mHeight, GetId() );

        if (mHeight > defaultHeight * 1.2f) {
            mHeight = defaultHeight * 1.2f;
        }
    } else if (!mHasFocus && mHeight > defaultHeight) {
        mHeight -= defaultHeight * 0.8f * dt;
        if (mHeight < defaultHeight) {
            mHeight = defaultHeight;
        }
    }
    wave = (wave + 2 * (int)(100 * dt)) % 255;
    for (auto it = effects.begin(); it != effects.end(); ++it) {
        (*it)->Update(dt);
    }
    Pos::Update(dt);
}

void PlayGuiObject::Render() {
    for (auto it = effects.begin(); it != effects.end(); ++it) {
        (*it)->Render();
    }
}
