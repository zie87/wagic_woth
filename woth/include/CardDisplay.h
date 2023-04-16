#ifndef CARDDISPLAY_H
#define CARDDISPLAY_H

#include "PlayGuiObjectController.h"

class TargetChooser;
class MTGGameZone;
class MTGCardInstance;

class CardDisplay : public PlayGuiObjectController {
    int mId;

public:
    int x, y, start_item, nb_displayed_items;
    MTGGameZone* zone;
    TargetChooser* tc;
    JGuiListener* listener;
    CardDisplay(GameObserver* game);
    CardDisplay(int id,
                GameObserver* game,
                int x,
                int y,
                JGuiListener* listener = nullptr,
                TargetChooser* tc      = nullptr,
                int nb_displayed_items = 7);
    void AddCard(MTGCardInstance* _card);
    void rotateLeft();
    void rotateRight();
    bool CheckUserInput(JButton key) override;
    void Update(float dt) override;
    void Render() override;
    void init(MTGGameZone* zone);
    virtual std::ostream& toString(std::ostream& out) const;
};

std::ostream& operator<<(std::ostream& out, const CardDisplay& m);

#endif
