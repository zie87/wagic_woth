#ifndef CARDEFFECT_H
#define CARDEFFECT_H

#include <JGE.h>
#include "Effects.h"

struct CardGui;

class CardEffect : public Effect {
public:
    CardEffect(CardGui* target);
    ~CardEffect() override;

private:
    CardGui* target;

public:
    void Render() override;
};

#endif  // CARDEFFECT_H
