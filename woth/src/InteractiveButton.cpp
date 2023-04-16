//
//  InteractiveButton.cpp
//  wagic
//
//  Created by Michael Nguyen on 1/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <utility>
#include "PrecompiledHeader.h"

#include "InteractiveButton.h"
#include "Translate.h"
#include "JTypes.h"
#include "WResourceManager.h"
#include "WFont.h"

const int kButtonHeight = 30;

InteractiveButton::InteractiveButton(JGuiController* _parent,
                                     int id,
                                     int fontId,
                                     const string& text,
                                     float x,
                                     float y,
                                     JButton actionKey,
                                     bool hasFocus,
                                     bool autoTranslate)
    : SimpleButton(_parent, id, fontId, std::move(text), x, y, hasFocus, autoTranslate)
    , mActionKey(actionKey) {
    setIsSelectionValid(false);  // by default it's turned off since you can't auto select it.
}

void InteractiveButton::Entering() {}

void InteractiveButton::checkUserClick() {
    int x1 = -1;
    int y1 = -1;
    if (mEngine->GetLeftClickCoordinates(x1, y1)) {
        setIsSelectionValid(false);
        const int buttonImageWidth = static_cast<int>(GetWidth());
        const int x2               = static_cast<int>(getX());
        const int y2               = static_cast<int>(getY() + mYOffset);
        const int buttonHeight     = kButtonHeight;
        if ((x1 >= x2) && (x1 <= (x2 + buttonImageWidth)) && (y1 >= y2) && (y1 < (y2 + buttonHeight))) {
            setIsSelectionValid(true);
        }
    } else {
        setIsSelectionValid(false);
    }
}

bool InteractiveButton::ButtonPressed() {
    checkUserClick();
    if (isSelectionValid()) {
        mEngine->ReadButton();
        mEngine->LeftClickedProcessed();
        mEngine->HoldKey_NoRepeat(mActionKey);
        setIsSelectionValid(false);
        return true;
    }

    return false;
}

void InteractiveButton::Render() {
    if (!isSelectionValid()) {
        return;
    }
    JRenderer* renderer             = JRenderer::GetInstance();
    WFont* mainFont                 = WResourceManager::Instance()->GetWFont(Fonts::MAIN_FONT);
    const string detailedInfoString = _(getText());
    const float stringWidth         = mainFont->GetStringWidth(detailedInfoString.c_str());
    const float pspIconsSize        = 0.5;
    const float mainFontHeight      = mainFont->GetHeight();
    const float boxStartX           = getX() - 5;
    mXOffset                        = 0;
    mYOffset                        = 0;

#ifndef TOUCH_ENABLED
    renderer->FillRoundRect(boxStartX, getY(), stringWidth - 3, mainFontHeight - 9, 5, ARGB(0, 0, 0, 0));
#else
    renderer->FillRoundRect(boxStartX, getY(), stringWidth - 3, mainFontHeight - 5, 5, ARGB(255, 192, 172, 119));
    renderer->DrawRoundRect(boxStartX, getY(), stringWidth - 3, mainFontHeight - 5, 5, ARGB(255, 255, 255, 255));
    mYOffset += 2;
#endif

    const float buttonXOffset = getX() - mXOffset;
    const float buttonYOffset = getY() + mYOffset;
    if (buttonImage != nullptr) {
        renderer->RenderQuad(buttonImage.get(), buttonXOffset - buttonImage.get()->mWidth / 2,
                             buttonYOffset + mainFontHeight / 2, 0, pspIconsSize, pspIconsSize);
    }
    mainFont->SetColor(ARGB(255, 200, 200, 200));
    mainFont->DrawString(detailedInfoString, buttonXOffset, buttonYOffset);
}

void InteractiveButton::setImage(const JQuadPtr imagePtr) {
    buttonImage              = imagePtr;
    const float imageXOffset = getX() - buttonImage.get()->mWidth;

    if (imageXOffset < 0) {
        setX(getX() - imageXOffset / 2 + 5);
    }
}

/* Accessors */

std::ostream& InteractiveButton::toString(std::ostream& out) const {
    return out << "InteractiveButton ::: mHasFocus : " << hasFocus() << " ; parent : " << getParent()
               << " ; mText : " << getText() << " ; mScale : " << getScale() << " ; mTargetScale : " << getTargetScale()
               << " ; mX,mY : " << getX() << "," << getY();
}
