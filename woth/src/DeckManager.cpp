#include "PrecompiledHeader.h"

#include "DeckManager.h"
#include "Player.h"
#include <JRenderer.h>

void DeckManager::updateMetaDataList(std::vector<DeckMetaData*>* refList, bool isAI) {
    if (refList) {
        std::vector<DeckMetaData*>* inputList = isAI ? &aiDeckOrderList : &playerDeckOrderList;
        inputList->clear();
        inputList->assign(refList->begin(), refList->end());
    }
}

std::vector<DeckMetaData*>* DeckManager::getPlayerDeckOrderList() { return &playerDeckOrderList; }

std::vector<DeckMetaData*>* DeckManager::getAIDeckOrderList() { return &aiDeckOrderList; }

/*
** Predicate helper for getDeckMetadataByID()
*/
struct DeckIDMatch {
    DeckIDMatch(int id) : mID(id) {}

    bool operator()(DeckMetaData* inPtr) const { return inPtr->getDeckId() == mID; }

    int mID;
};

DeckMetaData* DeckManager::getDeckMetaDataById(int deckId, bool isAI) {
    DeckMetaData* deck                   = nullptr;
    std::vector<DeckMetaData*>& deckList = isAI ? aiDeckOrderList : playerDeckOrderList;

    auto pos = find_if(deckList.begin(), deckList.end(), DeckIDMatch(deckId));
    if (pos != deckList.end()) {
        deck = *pos;
    } else {
        std::ostringstream deckFilename;
        std::string filepath;
        if (isAI) {
            filepath = options.profileFile("ai/baka/");
        } else {
            filepath = options.profileFile("");
        }

        deckFilename << filepath << "/deck" << deckId << ".txt";
        AddMetaData(deckFilename.str(), isAI);
        deck = deckList.back();
    }
    return deck;
}

/*
** Predicate helper for getDeckMetadataByFilename()
*/
struct DeckFilenameMatch {
    DeckFilenameMatch(const std::string& filename) : mFilename(filename) {}

    bool operator()(DeckMetaData* inPtr) const { return inPtr->getFilename() == mFilename; }

    std::string mFilename;
};

DeckMetaData* DeckManager::getDeckMetaDataByFilename(const std::string& filename, bool isAI) {
    DeckMetaData* deck                   = nullptr;
    std::vector<DeckMetaData*>& deckList = isAI ? aiDeckOrderList : playerDeckOrderList;

    auto pos = std::find_if(deckList.begin(), deckList.end(), DeckFilenameMatch(filename));
    if (pos != deckList.end()) {
        deck = *pos;
    } else {
        if (FileExists(filename)) {
            AddMetaData(filename, isAI);
            deck = deckList.back();
        }
    }
    return deck;
}

void DeckManager::AddMetaData(const string& filename, bool isAI) {
    if (isAI) {
        aiDeckOrderList.push_back(NEW DeckMetaData(filename, isAI));
        aiDeckStatsMap.insert(std::make_pair(filename.c_str(), new StatsWrapper(aiDeckOrderList.back()->getDeckId())));
    } else {
        playerDeckOrderList.push_back(NEW DeckMetaData(filename, isAI));
        playerDeckStatsMap.insert(
            std::make_pair(filename.c_str(), new StatsWrapper(playerDeckOrderList.back()->getDeckId())));
    }
}

void DeckManager::DeleteMetaData(const string& filename, bool isAI) {
    std::map<std::string, StatsWrapper*>::iterator it;
    std::vector<DeckMetaData*>::iterator metaDataIter;

    if (isAI) {
        it = aiDeckStatsMap.find(filename);
        if (it != aiDeckStatsMap.end()) {
            SAFE_DELETE(it->second);
            aiDeckStatsMap.erase(it);
        }

        for (metaDataIter = mInstance->aiDeckOrderList.begin(); metaDataIter != mInstance->aiDeckOrderList.end();
             ++metaDataIter) {
            if ((*metaDataIter)->getFilename() == filename) {
                SAFE_DELETE(*metaDataIter);
                aiDeckOrderList.erase(metaDataIter);
                break;
            }
        }
    } else {
        it = playerDeckStatsMap.find(filename);
        if (it != playerDeckStatsMap.end()) {
            SAFE_DELETE(it->second);
            playerDeckStatsMap.erase(it);
        }

        for (metaDataIter = mInstance->playerDeckOrderList.begin();
             metaDataIter != mInstance->playerDeckOrderList.end(); ++metaDataIter) {
            if ((*metaDataIter)->getFilename() == filename) {
                SAFE_DELETE(*metaDataIter);
                playerDeckOrderList.erase(metaDataIter);
                break;
            }
        }
    }
}

StatsWrapper* DeckManager::getExtendedStatsForDeckId(int deckId, MTGAllCards* collection, bool isAI) {
    DeckMetaData* selectedDeck = getDeckMetaDataById(deckId, isAI);
    if (selectedDeck == nullptr) {
        std::ostringstream deckName;
        deckName << options.profileFile() << "/deck" << deckId << ".txt";
        std::map<std::string, StatsWrapper*>* statsMap = isAI ? &aiDeckStatsMap : &playerDeckStatsMap;
        auto* stats                                    = NEW StatsWrapper(deckId);
        statsMap->insert(make_pair(deckName.str(), stats));
        return stats;
    }
    return getExtendedDeckStats(selectedDeck, collection, isAI);
}

StatsWrapper* DeckManager::getExtendedDeckStats(DeckMetaData* selectedDeck, MTGAllCards* collection, bool isAI) {
    StatsWrapper* stats = nullptr;

    const string deckName = selectedDeck ? selectedDeck->getFilename() : "";
    const int deckId      = selectedDeck ? selectedDeck->getDeckId() : 0;

    map<string, StatsWrapper*>* statsMap = isAI ? &aiDeckStatsMap : &playerDeckStatsMap;
    if (statsMap->find(deckName) == statsMap->end()) {
        stats = NEW StatsWrapper(deckId);
        stats->updateStats(deckName, collection);
        statsMap->insert(make_pair(deckName, stats));
    } else {
        stats = statsMap->find(deckName)->second;
        if (stats->needUpdate) {
            stats->updateStats(deckName, collection);
        }
    }
    return stats;
}

DeckManager* DeckManager::mInstance = nullptr;

void DeckManager::EndInstance() { SAFE_DELETE(mInstance); }

DeckManager* DeckManager::GetInstance() {
    if (!mInstance) {
        mInstance = NEW DeckManager();
    }

    return mInstance;
}

//  p1 is assumed to be the player you want stats for
//  p2 is the opponent
int DeckManager::getDifficultyRating(Player* statsPlayer, Player* player) {
    if (!player->deckFile.empty()) {
        DeckMetaData* meta = getDeckMetaDataByFilename(player->deckFile, (player->isAI() == 1));
        return meta->getDifficulty();
    }
    return EASY;
}

DeckManager::~DeckManager() {
    map<string, StatsWrapper*>::iterator it;
    vector<DeckMetaData*>::iterator metaDataIter;

    for (it = aiDeckStatsMap.begin(); it != aiDeckStatsMap.end(); it++) {
        SAFE_DELETE(it->second);
    }
    for (it = playerDeckStatsMap.begin(); it != playerDeckStatsMap.end(); it++) {
        SAFE_DELETE(it->second);
    }

    for (metaDataIter = aiDeckOrderList.begin(); metaDataIter != aiDeckOrderList.end(); ++metaDataIter) {
        SAFE_DELETE(*metaDataIter);
    }

    for (metaDataIter = playerDeckOrderList.begin(); metaDataIter != playerDeckOrderList.end(); ++metaDataIter) {
        SAFE_DELETE(*metaDataIter);
    }

    aiDeckOrderList.clear();
    playerDeckOrderList.clear();
    aiDeckStatsMap.clear();
    playerDeckStatsMap.clear();
}
