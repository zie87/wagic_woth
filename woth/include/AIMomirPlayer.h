#ifndef AIMOMIRPLAYER_H
#define AIMOMIRPLAYER_H

#include "AIPlayerBaka.h"

class AIMomirPlayer : public AIPlayerBaka {
public:
    AIMomirPlayer(GameObserver* observer,
                  const string& file,
                  const string& fileSmall,
                  string avatarFile,
                  MTGDeck* deck = nullptr);
    int getEfficiency(OrderedAIAction* action) override;
    int momir();
    int computeActions() override;
    MTGAbility* momirAbility;
    MTGAbility* getMomirAbility();
};

#endif
