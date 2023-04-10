#ifndef GUICARDSCONTROLLER_H
#define GUICARDSCONTROLLER_H

#include "PlayGuiObjectController.h"

class GuiCardsController : public PlayGuiObjectController {
public:
    GuiCardsController(GameObserver* observer) : PlayGuiObjectController(observer){};
};

#endif
