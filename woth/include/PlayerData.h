#ifndef PLAYERDATA_H
#define PLAYERDATA_H

#include "MTGDeck.h"
#include "Tasks.h"

class PlayerData {
protected:
    void init();

public:
    int credits;
    map<string, string> storySaves;
    MTGDeck* collection;
    TaskList* taskList;
    PlayerData();  // This doesn't init the collection, do not use it to manipulate the player's collection
    PlayerData(MTGAllCards* allcards);
    ~PlayerData();
    int save();
};

#endif
