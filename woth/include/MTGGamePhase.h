#ifndef MTGGAMEPHASE_H
#define MTGGAMEPHASE_H

#include "ActionElement.h"
#include "GameObserver.h"

#include <JGui.h>
#include "WFont.h"

class MTGGamePhase : public ActionElement {
protected:
    float animation;
    int currentState;
    WFont* mFont;
    GameObserver* observer;

public:
    MTGGamePhase(GameObserver* g, int id);
    void Update(float dt) override;
    bool CheckUserInput(JButton key) override;
    MTGGamePhase* clone() const override;
    std::ostream& toString(std::ostream& out) const override;
    bool NextGamePhase();
};

#endif
