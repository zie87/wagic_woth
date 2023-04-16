#ifndef PLAYER_H
#define PLAYER_H

#include "JGE.h"
#include "MTGGameZones.h"
#include "Damage.h"
#include "Targetable.h"

class MTGDeck;
class MTGPlayerCards;
class MTGInPlay;
class ManaPool;

class Player : public Damageable {
protected:
    ManaPool* manaPool;
    JTexture* mAvatarTex;
    JQuadPtr mAvatar;
    bool loadAvatar(const std::string& file, std::string resName = "playerAvatar");
    bool premade;

public:
    enum Mode { MODE_TEST_SUITE, MODE_HUMAN, MODE_AI };

    int deckId;
    std::string mAvatarName;
    Mode playMode;
    bool nomaxhandsize;
    MTGPlayerCards* game;
    MTGDeck* mDeck;
    std::string deckFile;
    std::string deckFileSmall;
    std::string deckName;
    std::string phaseRing;
    int offerInterruptOnPhase;
    int skippingTurn;
    int extraTurn;
    std::vector<MTGCardInstance*> curses;
    Player(GameObserver* observer, const std::string& deckFile, std::string deckFileSmall, MTGDeck* deck = nullptr);
    ~Player() override;
    void setObserver(GameObserver* g) override;
    virtual void End();
    virtual int displayStack() { return 1; }
    const std::string getDisplayName() const override;

    int afterDamage() override;

    int gainLife(int value);
    int loseLife(int value);
    int gainOrLoseLife(int value);

    bool isPoisoned() { return (poisonCount > 0); }
    int poisoned() override;
    int damaged();
    int prevented() override;
    void unTapPhase();
    MTGInPlay* inPlay() const;
    ManaPool* getManaPool();
    void takeMulligan() const;

    void cleanupPhase() const;
    virtual int Act(float dt) { return 0; }

    virtual int isAI() { return 0; }

    bool isHuman() const { return (playMode == MODE_HUMAN); }

    Player* opponent();
    int getId();
    JQuadPtr getIcon() override;

    virtual int receiveEvent(WEvent* event) { return 0; }

    virtual void Render() {}

    /**
    ** Returns the path to the stats file of currently selected deck.
    */
    std::string GetCurrentDeckStatsFile() const;
    virtual bool parseLine(const std::string& s);
    friend std::ostream& operator<<(std::ostream&, const Player&);
    bool operator<(Player& aPlayer);
    bool isDead();
};

class HumanPlayer : public Player {
public:
    HumanPlayer(GameObserver* observer,
                const std::string& deckFile,
                std::string deckFileSmall,
                bool premade  = false,
                MTGDeck* deck = nullptr);
    void End() override;
    friend std::ostream& operator<<(std::ostream&, const HumanPlayer&);
};

#endif
