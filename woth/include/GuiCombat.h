#ifndef GUICOMBAT_H
#define GUICOMBAT_H

#include <vector>
#include "WEvent.h"
#include "CardGui.h"
#include "MTGCardInstance.h"
#include "DamagerDamaged.h"

class GuiCombat : public GuiLayer {
protected:
    DamagerDamaged* active;
    AttackerDamaged* activeAtk;
    static JTexture* ok_tex;
    Pos ok, enemy_avatar;
    DamagerDamaged* current;
    enum { BLK, ATK, OK, NONE } cursor_pos;

    CombatStep step;
    void validateDamage();
    void addOne(DefenserDamaged* blocker, CombatStep);
    void removeOne(DefenserDamaged* blocker, CombatStep);
    void remaskBlkViews(AttackerDamaged* before, AttackerDamaged* after);
    void shiftLeft();
    void shiftRight(DamagerDamaged* oldActive);
    bool didClickOnButton(const Pos& buttonPosition, int& x, int& y);
    int resolve();

public:
    vector<AttackerDamaged*> attackers;
    void autoaffectDamage(AttackerDamaged* attacker, CombatStep);

    GuiCombat(GameObserver* go);
    ~GuiCombat() override;
    void Update(float dt) override;
    void Render() override;
    bool clickOK();
    bool CheckUserInput(JButton key) override;
    int receiveEventPlus(WEvent* e) override;
    int receiveEventMinus(WEvent* e) override;
    typedef vector<AttackerDamaged*>::iterator inner_iterator;
};

#endif  // GUICOMBAT_H
