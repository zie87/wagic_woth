#ifndef WDATASRC_H
#define WDATASRC_H

#include "WResource_Fwd.h"

class WCardFilter;
struct WCardSort;
struct WDistort;
class PriceList;
class MTGCard;
class MTGDeck;
class MTGAllCards;
class JQuad;

class WSyncable {
public:
    WSyncable(int i = 0) : hooked(nullptr), currentPos(0) {}

    virtual ~WSyncable() {}

    // Local
    virtual bool Hook(WSyncable* s);
    virtual int getOffset() { return currentPos; }

    virtual bool setOffset(int i) {
        currentPos = i;
        return true;
    }

    // Recursive
    virtual int getPos();
    virtual bool next();
    virtual bool prev();

protected:
    WSyncable* hooked;  // Simple link list
    int currentPos;
};

class WDataSource : public WSyncable {
public:
    WDataSource() {}

    virtual JQuadPtr getImage(int offset = 0) { return JQuadPtr(); }

    virtual JQuadPtr getThumb(int offset = 0) { return JQuadPtr(); }

    virtual MTGCard* getCard(int offset = 0, bool ignore = false) { return nullptr; }

    virtual MTGDeck* getDeck(int offset = 0) { return nullptr; }

    virtual WDistort* getDistort(int offset = 0) { return nullptr; }

    virtual bool thisCard(int mtgid) { return false; }

    virtual int getControlID() { return -1; }

    // TODO FIXME: Need a "not a valid button" define.
    virtual void Update(float dt) { mLastInput += dt; }

    virtual void Touch() { mLastInput = 0; };
    virtual float getElapsed() { return mLastInput; }

    virtual void setElapsed(float f) { mLastInput = f; }

protected:
    float mLastInput;
};

class WSrcImage : public WDataSource {
public:
    JQuadPtr getImage(int offset = 0) override;
    WSrcImage(std::string s);

protected:
    std::string filename;
};

class WSrcCards : public WDataSource {
protected:
    std::vector<MTGCard*> cards;
    std::vector<size_t> validated;
    WCardFilter* filtersRoot;
    float mDelay;

public:
    WSrcCards(float delay = 0.2);
    ~WSrcCards() override;

    JQuadPtr getImage(int offset = 0) override;
    JQuadPtr getThumb(int offset = 0) override;
    MTGCard* getCard(int offset = 0, bool ignore = false) override;

    virtual int Size(bool all = false);  // Returns the number of cards, or the number of cards that match the filter.

    virtual void Shuffle();
    bool thisCard(int mtgid) override;
    bool next() override;
    bool prev() override;

    virtual void Sort(int method);
    bool setOffset(int pos) override;
    virtual bool isEmptySet(WCardFilter* f);
    virtual void addFilter(WCardFilter* f);
    virtual void clearFilters();
    virtual WCardFilter* unhookFilters();
    virtual bool matchesFilters(MTGCard* c);
    virtual void validate();
    virtual void bakeFilters();  // Discards all invalidated cards.
    virtual float filterFee();

    virtual void updateCounts(){};
    virtual void clearCounts(){};
    virtual void addCount(MTGCard* c, int qty = 1){};

    // Loads into us. Calls validate()
    virtual int loadMatches(MTGAllCards* ac);                   // loadMatches adds the cards from something
    virtual int loadMatches(MTGDeck* deck);                     // into this, if it matches our filter
    virtual int loadMatches(WSrcCards* src, bool all = false);  // If all==true, ignore filters on src.

    // We put it into something else
    virtual int addRandomCards(MTGDeck* i, int howmany = 1);
    virtual int addToDeck(MTGDeck* i, int num = -1);  // Returns num that didn't add
    virtual WCardFilter* getFiltersRoot() { return filtersRoot; }

    enum {
        MAX_CYCLES = 4,  // How many cycles to search, for addToDeck
        SORT_COLLECTOR,
        SORT_ALPHA,
        SORT_RARITY
    };
};

class WSrcDeckViewer : public WSrcCards {
public:
    WSrcDeckViewer(WSrcCards* _active, WSrcCards* _inactive);
    ~WSrcDeckViewer() override;
    void swapSrc();

    // Wrapped functions
    JQuadPtr getImage(int offset = 0) override { return active->getImage(offset); }

    JQuadPtr getThumb(int offset = 0) override { return active->getThumb(offset); }

    MTGCard* getCard(int offset = 0, bool ignore = false) override { return active->getCard(offset, ignore); }

    int Size(bool all = false) override { return active->Size(); }

    WCardFilter* getFiltersRoot() override { return active->getFiltersRoot(); }

    void Shuffle() override { active->Shuffle(); }

    bool thisCard(int mtgid) override { return active->thisCard(mtgid); }

    bool next() override { return active->next(); }

    bool prev() override { return active->prev(); }

    void Sort(int method) override { active->Sort(method); }

    bool setOffset(int pos) override { return active->setOffset(pos); }

    bool isEmptySet(WCardFilter* f) override { return active->isEmptySet(f); }

    void addFilter(WCardFilter* f) override { active->addFilter(f); }

    void clearFilters() override { active->clearFilters(); }

    WCardFilter* unhookFilters() override { return active->unhookFilters(); }

    bool matchesFilters(MTGCard* c) override { return active->matchesFilters(c); }

    void validate() override { active->validate(); }

    void bakeFilters() override { active->bakeFilters(); }
    // Discards all invalidated cards.
    float filterFee() override { return active->filterFee(); }

    void updateCounts() override { active->updateCounts(); }

    void clearCounts() override { active->clearCounts(); }

    void addCount(MTGCard* c, int qty = 1) override { active->addCount(c, qty); }

    int loadMatches(MTGAllCards* ac) override { return active->loadMatches(ac); }

    int loadMatches(MTGDeck* deck) override { return active->loadMatches(deck); }

    int loadMatches(WSrcCards* src, bool all = false) override { return loadMatches(src, all); }

    int addRandomCards(MTGDeck* i, int howmany = 1) override { return active->addRandomCards(i, howmany); }

    int addToDeck(MTGDeck* i, int num = -1) override { return active->addToDeck(i, num); }

protected:
    WSrcCards* active;
    WSrcCards* inactive;
};

class WSrcUnlockedCards : public WSrcCards {  // Only unlocked cards.
public:
    WSrcUnlockedCards(float mDelay = 0.2);
};

class WSrcDeck : public WSrcCards {
public:
    WSrcDeck(float delay = 0.2) : WSrcCards(delay) { clearCounts(); };
    int loadMatches(MTGDeck* deck) override;
    virtual int Add(MTGCard* c, int quantity = 1);
    virtual int Remove(MTGCard* c, int quantity = 1, bool erase = false);
    void Rebuild(MTGDeck* d);
    int count(MTGCard* c);
    int countByName(MTGCard* card, bool editions = false);
    int totalPrice();
    enum {
        // 0 to MTG_NB_COLORS are colors. See MTG_COLOR_ in Constants::.
        UNFILTERED_COPIES = Constants::MTG_NB_COLORS,
        UNFILTERED_UNIQUE,
        UNFILTERED_MIN_COPIES,  // For 'unlock all' cheat, awards screen
        UNFILTERED_MAX_COPIES,  // future use in format restriction, awards screen
        FILTERED_COPIES,
        FILTERED_UNIQUE,
        MAX_COUNTS
    };
    void clearCounts() override;
    void updateCounts() override;
    void addCount(MTGCard* c, int qty = 1) override;
    int getCount(int count = UNFILTERED_COPIES);

protected:
    std::map<int, int> copies;  // Maps MTGID to card counts.
    int counts[MAX_COUNTS];
};

struct WCSortCollector {
    bool operator()(const MTGCard* l, const MTGCard* r);
};

struct WCSortAlpha {
    bool operator()(const MTGCard* l, const MTGCard* r);
};

struct WCSortRarity {
    int rareToInt(char r);
    bool operator()(const MTGCard* l, const MTGCard* r);
};

#endif
