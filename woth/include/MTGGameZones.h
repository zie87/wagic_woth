#ifndef MTGGAMEZONES_H
#define MTGGAMEZONES_H

#include <map>
using std::map;

#include "MTGDeck.h"
#include "MTGCardInstance.h"
#include "PlayRestrictions.h"
#include "TargetChooser.h"

#define MTG_MAX_PLAYER_CARDS 100

class MTGAllCards;
class MTGDeck;
class MTGCardInstance;
class Player;

class MTGGameZone {
protected:
public:
    enum {
        ALL_ZONES = -1,

        MY_GRAVEYARD                = 11,
        OPPONENT_GRAVEYARD          = 12,
        TARGET_OWNER_GRAVEYARD      = 13,
        TARGET_CONTROLLER_GRAVEYARD = 14,
        GRAVEYARD                   = 15,
        OWNER_GRAVEYARD             = 16,
        TARGETED_PLAYER_GRAVEYARD   = 17,

        MY_BATTLEFIELD                = 21,
        OPPONENT_BATTLEFIELD          = 22,
        TARGET_OWNER_BATTLEFIELD      = 23,
        TARGET_CONTROLLER_BATTLEFIELD = 24,
        BATTLEFIELD                   = 25,
        OWNER_BATTLEFIELD             = 26,
        TARGETED_PLAYER_BATTLEFIELD   = 27,

        MY_HAND                = 31,
        OPPONENT_HAND          = 32,
        TARGET_OWNER_HAND      = 33,
        TARGET_CONTROLLER_HAND = 34,
        HAND                   = 35,
        OWNER_HAND             = 36,
        TARGETED_PLAYER_HAND   = 37,

        MY_EXILE                = 41,
        OPPONENT_EXILE          = 42,
        TARGET_OWNER_EXILE      = 43,
        TARGET_CONTROLLER_EXILE = 44,
        EXILE                   = 45,
        OWNER_EXILE             = 46,
        TARGETED_PLAYER_EXILE   = 47,

        MY_LIBRARY                = 51,
        OPPONENT_LIBRARY          = 52,
        TARGET_OWNER_LIBRARY      = 53,
        TARGET_CONTROLLER_LIBRARY = 54,
        LIBRARY                   = 55,
        OWNER_LIBRARY             = 56,
        TARGETED_PLAYER_LIBRARY   = 57,

        MY_STACK                = 61,
        OPPONENT_STACK          = 62,
        TARGET_OWNER_STACK      = 63,
        TARGET_CONTROLLER_STACK = 64,
        STACK                   = 65,
        OWNER_STACK             = 66,
        TARGETED_PLAYER_STACK   = 67,

    };

    Player* owner;
    // Both cards and cardsMap contain the cards of a zone. The vector is used to keep the order, useful for some zones
    // such as the stack, library, etc...
    vector<MTGCardInstance*> cards;
    map<MTGCardInstance*, int> cardsMap;

    // list of cards that have been through this zone in the current turn
    vector<MTGCardInstance*> cardsSeenThisTurn;
    // list of cards that have been through this zone in the last turn
    vector<MTGCardInstance*> cardsSeenLastTurn;
    int nb_cards;
    MTGGameZone();
    virtual ~MTGGameZone();
    void shuffle();
    void addCard(MTGCardInstance* card);
    void debugPrint();
    MTGCardInstance* removeCard(MTGCardInstance* card, int createCopy = 1);
    MTGCardInstance* hasCard(MTGCardInstance* card);
    size_t getIndex(MTGCardInstance* card);
    void cleanupPhase();
    void beforeBeginPhase();

    unsigned int countByType(const char* value);
    unsigned int countByCanTarget(TargetChooser* tc);
    MTGCardInstance* findByName(const string& name);

    // returns true if one of the cards in the zone has the ability
    bool hasAbility(int ability);

    // returns true if one of the cards in the zone has the type
    bool hasType(const char* value);
    bool hasTypeSpecificInt(int value1, int value);
    bool hasSpecificType(const char* value, const char* secondvalue);
    bool hasPrimaryType(const char* value, const char* secondvalue);
    bool hasTypeButNotType(const char* value, const char* secondvalue);
    bool hasName(const string& value);
    bool hasColor(int value);
    bool hasX();

    // How many cards matching a TargetChooser have been put in this zone during the turn
    int seenThisTurn(TargetChooser* tc, int castFilter = Constants::CAST_DONT_CARE, bool lastTurn = false);
    int seenThisTurn(string s, int castFilter = Constants::CAST_DONT_CARE);
    int seenLastTurn(string s, int castFilter = Constants::CAST_DONT_CARE);

    void setOwner(Player* player);
    MTGCardInstance* lastCardDrawn;
    static MTGGameZone* stringToZone(GameObserver* g,
                                     const string& zoneName,
                                     MTGCardInstance* source,
                                     MTGCardInstance* target);
    static int zoneStringToId(const string& zoneName);
    static MTGGameZone* intToZone(GameObserver* g,
                                  int zoneId,
                                  MTGCardInstance* source = nullptr,
                                  MTGCardInstance* target = nullptr);
    static MTGGameZone* intToZone(int zoneId, Player* source, Player* target = nullptr);
    bool needShuffle;
    virtual const char* getName() { return "zone"; };
    virtual std::ostream& toString(std::ostream&) const;
    bool parseLine(const string& s);
};

class MTGLibrary : public MTGGameZone {
public:
    ~MTGLibrary() override = default;
    vector<MTGCardInstance*> placeOnTop;
    std::ostream& toString(std::ostream&) const override;
    const char* getName() override { return "library"; }
};

class MTGGraveyard : public MTGGameZone {
public:
    ~MTGGraveyard() override = default;
    std::ostream& toString(std::ostream&) const override;
    const char* getName() override { return "graveyard"; }
};

class MTGHand : public MTGGameZone {
public:
    ~MTGHand() override = default;
    std::ostream& toString(std::ostream&) const override;
    const char* getName() override { return "hand"; }
};

class MTGRemovedFromGame : public MTGGameZone {
public:
    ~MTGRemovedFromGame() override = default;
    std::ostream& toString(std::ostream&) const override;
    const char* getName() override { return "exile"; }
};

class MTGStack : public MTGGameZone {
public:
    ~MTGStack() override = default;
    std::ostream& toString(std::ostream&) const override;
    const char* getName() override { return "stack"; }
};

class MTGInPlay : public MTGGameZone {
public:
    ~MTGInPlay() override = default;
    void untapAll();
    MTGCardInstance* getNextAttacker(MTGCardInstance* previous);
    std::ostream& toString(std::ostream&) const override;
    const char* getName() override { return "battlefield"; }
};

class MTGPlayerCards {
protected:
    void init();

public:
    Player* owner;
    PlayRestrictions* playRestrictions;
    MTGLibrary* library;
    MTGGraveyard* graveyard;
    MTGHand* hand;
    MTGInPlay* inPlay;
    MTGInPlay* battlefield;  // alias to inPlay

    MTGStack* stack;
    MTGRemovedFromGame* removedFromGame;
    MTGRemovedFromGame* exile;  // alias to removedFromZone
    MTGGameZone* garbage;
    MTGGameZone* garbageLastTurn;
    MTGGameZone* temp;

    MTGPlayerCards();
    MTGPlayerCards(Player*, int* idList, int idListSize);
    MTGPlayerCards(MTGDeck* deck);
    ~MTGPlayerCards();
    void initGame(int shuffle = 1, int draw = 1) const;
    void OptimizedHand(Player* who, int amount = 7, int lands = 3, int creatures = 0, int othercards = 4);
    void setOwner(Player* player);
    void discardRandom(MTGGameZone* from, MTGCardInstance* source) const;
    void drawFromLibrary() const;
    void showHand() const;
    void resetLibrary();
    void initDeck(MTGDeck* deck);
    void beforeBeginPhase();
    MTGCardInstance* putInGraveyard(MTGCardInstance* card) const;
    MTGCardInstance* putInExile(MTGCardInstance* card) const;
    MTGCardInstance* putInLibrary(MTGCardInstance* card) const;
    MTGCardInstance* putInHand(MTGCardInstance* card) const;
    MTGCardInstance* putInZone(MTGCardInstance* card, MTGGameZone* from, MTGGameZone* to) const;
    int isInPlay(MTGCardInstance* card) const;
    int isInGrave(MTGCardInstance* card);
    int isInZone(MTGCardInstance* card, MTGGameZone* zone);
    bool parseLine(const string& s) const;
};

std::ostream& operator<<(std::ostream&, const MTGGameZone&);
std::ostream& operator<<(std::ostream&, const MTGPlayerCards&);

#endif
