//
//  InteractiveButton.h
//
//  Created by Michael Nguyen on 1/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef INTERACTIVEBUTTON_H
#define INTERACTIVEBUTTON_H

#include <string>
#include <JLBFont.h>
#include <JGui.h>
#include "WResource_Fwd.h"
#include "SimpleButton.h"

using std::string;

#define SCALE_SELECTED 1.2f
#define SCALE_NORMAL 1.0f

const int kDismissButtonId      = 10000;
const int kToggleDeckActionId   = 10001;
const int kSellCardActionId     = 10002;
const int kMenuButtonId         = 10003;
const int kFilterButtonId       = 10004;
const int kNextStatsButtonId    = 10005;
const int kPrevStatsButtonId    = 10006;
const int kCycleCardsButtonId   = 10007;
const int kShowCardListButtonId = 10008;

class InteractiveButton : public SimpleButton {
private:
    JQuadPtr buttonImage;
    JButton mActionKey;

public:
    InteractiveButton(JGuiController* _parent,
                      int id,
                      int fontId,
                      const string& text,
                      float x,
                      float y,
                      JButton actionKey,
                      bool hasFocus      = false,
                      bool autoTranslate = false);

    void Entering() override;
    bool ButtonPressed() override;
    virtual void setImage(const JQuadPtr imagePtr);
    void checkUserClick() override;
    void Render() override;
    std::ostream& toString(std::ostream& out) const override;
};

#endif
