#ifndef DECKDATAWRAPPER_H
#define DECKDATAWRAPPER_H

#include "MTGDefinitions.h"
#include "MTGCard.h"
#include "CardPrimitive.h"
#include "WDataSrc.h"
#include <map>
#include <string>
using std::map;
using std::string;

class MTGDeck;

class DeckDataWrapper : public WSrcDeck {
public:
    MTGDeck* parent;
    DeckDataWrapper(MTGDeck* deck);
    bool next() override {
        currentPos++;
        return true;
    };
    bool prev() override {
        currentPos--;
        return true;
    };
    void save();
    void save(const string& filepath, bool useExpandedCardNames, string& deckTitle, string& deckDesc);
};

#endif
