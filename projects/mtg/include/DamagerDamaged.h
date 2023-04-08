#ifndef DAMAGERDAMAGED_H
#define DAMAGERDAMAGED_H

#include "MTGCardInstance.h"
#include "CardGui.h"

class Player;

struct DamagerDamaged : TransientCardView {
    bool show;
    Player* damageSelecter;
    vector<Damage> damages;
    int damageToDeal;

    void addDamage(int damage, DamagerDamaged* source);
    int removeDamagesTo(DamagerDamaged* target);
    int removeDamagesFrom(DamagerDamaged* source);
    void clearDamage();
    int sumDamages();
    bool hasLethalDamage();
    DamagerDamaged(MTGCardInstance* card, float x, float y, bool show, Player* damageSelecter);
    DamagerDamaged(MTGCardInstance* card, const Pos& ref, bool show, Player* damageSelecter);

    ~DamagerDamaged() override;
    void Render(CombatStep mode);
};

typedef DamagerDamaged DefenserDamaged;
struct AttackerDamaged : DamagerDamaged {
    vector<DefenserDamaged*> blockers;
    AttackerDamaged(MTGCardInstance* card, float x, float y, bool show, Player* damageSelecter);
    AttackerDamaged(MTGCardInstance* card, const Pos& ref, bool show, Player* damageSelecter);
    ~AttackerDamaged() override;
};

#endif
