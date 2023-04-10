#ifndef GUISTATIC_H
#define GUISTATIC_H

#include "Player.h"
#include "MTGGameZones.h"
#include "CardDisplay.h"
#include "CardGui.h"
#include "GuiAvatars.h"

class CardView;

struct GuiStatic : public PlayGuiObject {
    GuiAvatars* parent;
    GuiStatic(float desiredHeight, float x, float y, bool hasFocus, GuiAvatars* parent);
    void Entering() override;
    bool Leaving(JButton key) override;
};

struct GuiAvatar : public GuiStatic {
    typedef enum { TOP_LEFT, BOTTOM_RIGHT } Corner;

    static const unsigned Width  = 35;
    static const unsigned Height = 50;

protected:
    int avatarRed;
    int currentLife;
    int currentpoisonCount;
    Corner corner;

public:
    Player* player;
    void Render() override;
    GuiAvatar(float x, float y, bool hasFocus, Player* player, Corner corner, GuiAvatars* parent);
    std::ostream& toString(std::ostream& out) const override;
};

struct GuiGameZone : public GuiStatic {
    static const int Width  = 20;
    static const int Height = 25;
    vector<CardView*> cards;

public:
    MTGGameZone* zone;
    CardDisplay* cd;
    int showCards;
    void Render() override;
    bool CheckUserInput(JButton key) override;
    void Update(float dt) override;
    GuiGameZone(float x, float y, bool hasFocus, MTGGameZone* zone, GuiAvatars* parent);
    ~GuiGameZone() override;
    void ButtonPressed(int controllerId, int controlId) override;
    void toggleDisplay();
    std::ostream& toString(std::ostream& out) const override;
};
// opponenthand
class GuiOpponentHand : public GuiGameZone {
public:
    Player* player;
    GuiOpponentHand(float _x, float _y, bool hasFocus, Player* player, GuiAvatars* Parent);
    int receiveEventPlus(WEvent*);
    int receiveEventMinus(WEvent*);
    std::ostream& toString(std::ostream& out) const override;
};
// end of my addition
class GuiGraveyard : public GuiGameZone {
public:
    Player* player;
    GuiGraveyard(float _x, float _y, bool hasFocus, Player* player, GuiAvatars* parent);
    int receiveEventPlus(WEvent*);
    int receiveEventMinus(WEvent*);
    std::ostream& toString(std::ostream& out) const override;
};

class GuiLibrary : public GuiGameZone {
public:
    Player* player;
    GuiLibrary(float _x, float _y, bool hasFocus, Player* player, GuiAvatars* parent);
    std::ostream& toString(std::ostream& out) const override;
};

#endif  // GUISTATIC_H
