#include "PrecompiledHeader.h"

#include "DeckManager.h"
#include "DeckStats.h"

#include <utility>
#include "Player.h"
#include "GameObserver.h"
#include "MTGDeck.h"
#include "ManaCostHybrid.h"

DeckStats* DeckStats::mInstance = nullptr;

DeckStat::DeckStat(int nbgames, int victories, std::string manaColorIndex)
    : nbgames(nbgames)
    , victories(victories)
    , manaColorIndex(std::move(manaColorIndex)) {}

int DeckStat::percentVictories() const {
    if (nbgames == 0) {
        return 50;
    }
    return (100 * victories / nbgames);
}

DeckStats* DeckStats::GetInstance() {
    if (!mInstance) {
        mInstance = NEW DeckStats();
    }
    return mInstance;
}

DeckStats::~DeckStats() {
    std::map<std::string, std::map<std::string, DeckStat*> >::iterator it;
    for (it = masterDeckStats.begin(); it != masterDeckStats.end(); ++it) {
        std::string const key                     = it->first;
        std::map<std::string, DeckStat*> innerMap = masterDeckStats[key];
        std::map<std::string, DeckStat*>::iterator it2;
        for (it2 = innerMap.begin(); it2 != innerMap.end(); it2++) {
            SAFE_DELETE(it2->second);
        }
        innerMap.clear();
    }
    masterDeckStats.clear();
}

DeckStat* DeckStats::getDeckStat(const std::string& opponentsFile) {
    std::map<std::string, DeckStat*> stats = masterDeckStats[currentDeck];
    auto it                                = stats.find(opponentsFile);
    if (it == stats.end()) {
        return nullptr;
    }
    return it->second;
}

int DeckStats::nbGames() {
    int nbgames                            = 0;
    std::map<std::string, DeckStat*> stats = masterDeckStats[currentDeck];
    std::map<std::string, DeckStat*>::iterator it;
    for (it = stats.begin(); it != stats.end(); it++) {
        DeckStat* d = it->second;
        nbgames += d->nbgames;
    }
    return nbgames;
}

int DeckStats::percentVictories(const std::string& opponentsFile) {
    std::map<std::string, DeckStat*> stats = masterDeckStats[currentDeck];
    auto it                                = stats.find(opponentsFile);
    if (it == stats.end()) {
        return 50;
    }
    return (it->second->percentVictories());
}

int DeckStats::percentVictories() {
    int victories                          = 0;
    int nbgames                            = 0;
    std::map<std::string, DeckStat*> stats = masterDeckStats[currentDeck];
    std::map<std::string, DeckStat*>::iterator it;
    for (it = stats.begin(); it != stats.end(); it++) {
        DeckStat* d = it->second;
        nbgames += d->nbgames;
        victories += d->victories;
    }
    if (nbgames) {
        return (victories * 100) / nbgames;
    }
    return 50;
}

void DeckStats::load(const std::string& filename) {
    currentDeck = filename;
    if (masterDeckStats.find(filename) != masterDeckStats.end()) {
        return;
    }

    std::string contents;
    if (JFileSystem::GetInstance()->readIntoString(filename, contents)) {
        std::stringstream stream(contents);
        std::string s;
        // get the associated player deck file:
        const int deckId = atoi(filename.substr(filename.find("_deck") + 5, filename.find(".txt")).c_str());
        char buffer[512];
        sprintf(buffer, "deck%i.txt", deckId);
        const string playerDeckFilePath = options.profileFile(buffer);
        DeckMetaData* playerDeckMetaData =
            DeckManager::GetInstance()->getDeckMetaDataByFilename(playerDeckFilePath, false);
        if (!playerDeckMetaData) {
            DebugTrace("DeckStats.cpp:CONSISTENCY ERROR: DeckStats are set, but no deck meta data");
            return;
        }
        // check if this player deck has already been profiled for manacolors
        char next = stream.peek();
        string manaColorIndex;
        if (next == 'M') {
            std::getline(stream, s);
            manaColorIndex = s.substr(s.find(':') + 1);
            playerDeckMetaData->setColorIndex(manaColorIndex);
        }

        while (std::getline(stream, s)) {
            const string deckfile = s;
            std::getline(stream, s);
            const int games = atoi(s.c_str());
            std::getline(stream, s);
            const int victories = atoi(s.c_str());
            next                = stream.peek();

            if (next == 'M') {
                std::getline(stream, s);
                manaColorIndex = s.substr(s.find(':') + 1);
            }
            if (masterDeckStats[filename].find(deckfile) != masterDeckStats[filename].end()) {
                SAFE_DELETE(masterDeckStats[filename][deckfile]);
            }
            auto* newDeckStat                     = NEW DeckStat(games, victories, manaColorIndex);
            (masterDeckStats[filename])[deckfile] = newDeckStat;
        }
    }
}

void DeckStats::save(const std::string& filename) {
    std::ofstream file;
    if (JFileSystem::GetInstance()->openForWrite(file, filename)) {
        char writer[512];
        std::map<std::string, DeckStat*> stats = masterDeckStats[currentDeck];
        std::map<std::string, DeckStat*>::iterator it;
        std::string manaColorIndex;
        const int deckId = atoi(filename.substr(filename.find("_deck") + 5, filename.find(".txt")).c_str());
        char buffer[512];
        sprintf(buffer, "deck%i.txt", deckId);
        std::string const playerDeckFilePath = options.profileFile(buffer);
        DeckManager* deckManager             = DeckManager::GetInstance();
        DeckMetaData* playerDeckMeta         = deckManager->getDeckMetaDataByFilename(playerDeckFilePath, false);
        if (playerDeckMeta && playerDeckMeta->getColorIndex().empty()) {
            StatsWrapper* stw = deckManager->getExtendedDeckStats(playerDeckMeta, MTGAllCards::getInstance(), false);
            manaColorIndex    = stw->getManaColorIndex();
            playerDeckMeta->setColorIndex(manaColorIndex);
        }
        file << "MANA:" << manaColorIndex << std::endl;
        for (it = stats.begin(); it != stats.end(); it++) {
            sprintf(writer, "%s\n", it->first.c_str());
            file << writer;
            sprintf(writer, "%i\n", it->second->nbgames);
            file << writer;
            sprintf(writer, "%i\n", it->second->victories);
            file << writer;
            file << "MANA:" << it->second->manaColorIndex << std::endl;
        }
        file.close();
        if (playerDeckMeta) {
            playerDeckMeta->Invalidate();
        }
    }
}

void DeckStats::saveStats(Player* player, Player* opponent, GameObserver* game) {
    int victory = 1;
    if (!game->didWin()) {
        if (player->life == opponent->life) {
            return;
        }
        if (player->life < opponent->life) {
            victory = 0;
        }
    } else if (!game->didWin(player)) {
        victory = 0;
    }
    load(currentDeck);
    std::map<std::string, DeckStat*>* stats = &masterDeckStats[currentDeck];
    auto it                                 = stats->find(opponent->deckFileSmall);
    std::string manaColorIndex;
    DeckManager* deckManager = DeckManager::GetInstance();
    DeckMetaData* aiDeckMeta = deckManager->getDeckMetaDataByFilename(opponent->deckFile, true);
    StatsWrapper* stw        = deckManager->getExtendedDeckStats(aiDeckMeta, MTGAllCards::getInstance(), true);
    manaColorIndex           = stw->getManaColorIndex();
    if (it == stats->end()) {
        stats->insert(make_pair(opponent->deckFileSmall, NEW DeckStat(1, victory, manaColorIndex)));
    } else {
        it->second->victories += victory;
        it->second->nbgames += 1;
        if (it->second->manaColorIndex.empty()) {
            it->second->manaColorIndex = manaColorIndex;
        }
    }
    save(currentDeck);

    DeckMetaData* playerMeta = DeckManager::GetInstance()->getDeckMetaDataByFilename(player->deckFile, false);

    // metadata caches its internal data (number of games, victories, etc)
    // tell it to refresh when stats are updated
    if (playerMeta) {
        playerMeta->Invalidate();
    }

    DeckMetaData* aiMeta = DeckManager::GetInstance()->getDeckMetaDataByFilename(opponent->deckFile, true);
    if (aiMeta) {
        aiMeta->Invalidate();
    }
}

void DeckStats::EndInstance() { SAFE_DELETE(mInstance); }

// StatsWrapper

float noLandsProbInTurn[Constants::STATS_FOR_TURNS]     = {0.0f};
float noCreaturesProbInTurn[Constants::STATS_FOR_TURNS] = {0.0f};

int countCardsPerCostAndColor[Constants::STATS_MAX_MANA_COST + 1][Constants::MTG_NB_COLORS + 1]     = {{0, 0}};
int countCreaturesPerCostAndColor[Constants::STATS_MAX_MANA_COST + 1][Constants::MTG_NB_COLORS + 1] = {{0, 0}};
int countSpellsPerCostAndColor[Constants::STATS_MAX_MANA_COST + 1][Constants::MTG_NB_COLORS + 1]    = {{0, 0}};

int countCardsPerCost[Constants::STATS_MAX_MANA_COST + 1]       = {0};
int countCreaturesPerCost[Constants::STATS_MAX_MANA_COST + 1]   = {0};
int countSpellsPerCost[Constants::STATS_MAX_MANA_COST + 1]      = {0};
int countLandsPerColor[Constants::MTG_NB_COLORS + 1]            = {0};
int countBasicLandsPerColor[Constants::MTG_NB_COLORS + 1]       = {0};
int countNonLandProducersPerColor[Constants::MTG_NB_COLORS + 1] = {0};
int totalCostPerColor[Constants::MTG_NB_COLORS + 1]             = {0};

void StatsWrapper::initValues() {
    // initilize all member values to 0
    // Stats parameters and status
    mDeckId = currentPage = pageCount = 0;
    needUpdate                        = true;

    // Actual stats
    percentVictories = 0;
    gamesPlayed = cardCount = countLands = totalPrice = totalManaCost = 0;
    totalCreatureCost = totalSpellCost = countManaProducers = 0;
    avgManaCost = avgCreatureCost = avgSpellCost = 0.0f;

    countCreatures = countSpells = countInstants = countEnchantments = countSorceries = countArtifacts = 0;
}

StatsWrapper::StatsWrapper(int deckId) : mDeckId(deckId) {
    char buffer[512];
    sprintf(buffer, "stats/player_deck%i.txt", deckId);
    std::string const deckstats = options.profileFile(buffer);
    initStatistics(deckstats);
}

StatsWrapper::StatsWrapper(const string& deckstats) { initStatistics(std::move(deckstats)); }

void StatsWrapper::initStatistics(const string& deckstats) {
    // initialize member variables to make sure they have valid values
    initValues();

    // Load deck statistics
    DeckStats* stats = DeckStats::GetInstance();
    aiDeckNames.clear();
    aiDeckStats.clear();

    if (FileExists(deckstats)) {
        stats->load(deckstats);
        percentVictories = stats->percentVictories();
        gamesPlayed      = stats->nbGames();

        // Detailed deck statistics against AI
        int nbDecks = 0;
        char buffer[512];
        char smallDeckName[512];
        std::ostringstream oss;
        oss << "deck" << (nbDecks + 1);
        std::string const bakaDir      = "ai/baka/";
        std::string const deckFilename = oss.str();
        sprintf(buffer, "%s/%s.txt", bakaDir.c_str(), deckFilename.c_str());
        if (fileExists(buffer)) {
            nbDecks++;

            sprintf(smallDeckName, "%s_deck%i", "ai_baka", nbDecks);
            DeckStat* deckStat = stats->getDeckStat(string(smallDeckName));

            if ((deckStat != nullptr) && (deckStat->nbgames > 0)) {
                aiDeckNames.push_back(deckFilename);
                aiDeckStats.push_back(deckStat);
            }
        }
    } else {
        gamesPlayed      = 0;
        percentVictories = 0;
    }
}

string StatsWrapper::getManaColorIndex() {
    std::ostringstream oss;
    for (int i = Constants::MTG_COLOR_ARTIFACT; i < Constants::MTG_COLOR_LAND; ++i) {
        if (totalCostPerColor[i] != 0) {
            oss << "1";
        } else {
            oss << "0";
        }
    }
    return oss.str();
}

void StatsWrapper::updateStats(const string& filename, MTGAllCards* collection) {
    if (FileExists(filename)) {
        auto* mtgd            = NEW MTGDeck(filename.c_str(), collection);
        auto* deckDataWrapper = NEW DeckDataWrapper(mtgd);
        updateStats(deckDataWrapper);
        SAFE_DELETE(mtgd);
        SAFE_DELETE(deckDataWrapper);
    }
}

void StatsWrapper::updateStats(DeckDataWrapper* myDeck) {
    if (!this->needUpdate || !myDeck) {
        return;
    }

    this->needUpdate = false;
    this->cardCount  = myDeck->getCount(WSrcDeck::UNFILTERED_COPIES);
    this->countLands = myDeck->getCount(Constants::MTG_COLOR_LAND);
    this->totalPrice = myDeck->totalPrice();

    this->countManaProducers = 0;
    // Mana cost
    int currentCount;
    int convertedCost;
    ManaCost* currentCost;
    this->totalManaCost     = 0;
    this->totalCreatureCost = 0;
    this->totalSpellCost    = 0;
    MTGCard* current        = nullptr;

    // Clearing arrays
    for (int i = 0; i <= Constants::STATS_MAX_MANA_COST; i++) {
        this->countCardsPerCost[i]     = 0;
        this->countCreaturesPerCost[i] = 0;
        this->countSpellsPerCost[i]    = 0;
    }

    for (int i = 0; i <= Constants::NB_Colors; i++) {
        this->totalCostPerColor[i]             = 0;
        this->countLandsPerColor[i]            = 0;
        this->countBasicLandsPerColor[i]       = 0;
        this->countNonLandProducersPerColor[i] = 0;
    }

    for (int i = 0; i <= Constants::STATS_MAX_MANA_COST; i++) {
        for (int k = 0; k <= Constants::NB_Colors; k++) {
            this->countCardsPerCostAndColor[i][k]     = 0;
            this->countCreaturesPerCostAndColor[i][k] = 0;
            this->countSpellsPerCostAndColor[i][k]    = 0;
        }
    }

    for (int ic = 0; ic < myDeck->Size(true); ic++) {
        current       = myDeck->getCard(ic, true);
        currentCost   = current->data->getManaCost();
        convertedCost = currentCost->getConvertedCost();
        currentCount  = myDeck->count(current);

        // Add to the cards per cost counters
        this->totalManaCost += convertedCost * currentCount;
        if (convertedCost > Constants::STATS_MAX_MANA_COST) {
            convertedCost = Constants::STATS_MAX_MANA_COST;
        }
        this->countCardsPerCost[convertedCost] += currentCount;
        if (current->data->isCreature()) {
            this->countCreaturesPerCost[convertedCost] += currentCount;
            this->totalCreatureCost += convertedCost * currentCount;
        } else if (current->data->isSpell()) {
            this->countSpellsPerCost[convertedCost] += currentCount;
            this->totalSpellCost += convertedCost * currentCount;
        }

        // Lets look for mana producing abilities

        // http://code.google.com/p/wagic/issues/detail?id=650
        // Basic lands are not producing their mana through regular abilities anymore,
        // but through a rule that is outside of the primitives. This block is a hack to address this
        const int colors[]   = {Constants::MTG_COLOR_GREEN, Constants::MTG_COLOR_BLUE, Constants::MTG_COLOR_RED,
                                Constants::MTG_COLOR_BLACK, Constants::MTG_COLOR_WHITE};
        const string lands[] = {"forest", "island", "mountain", "swamp", "plains"};
        for (unsigned int i = 0; i < sizeof(colors) / sizeof(colors[0]); ++i) {
            const int colorId = colors[i];
            const string type = lands[i];
            if (current->data->hasType(type.c_str())) {
                if (current->data->hasType("Basic")) {
                    this->countBasicLandsPerColor[colorId] += currentCount;
                } else {
                    this->countLandsPerColor[colorId] += currentCount;
                }
            }
        }

        vector<string> abilitiesVector;
        const string thisstring = current->data->magicText;
        abilitiesVector         = ::woth::split(thisstring, '\n');

        for (int v = 0; v < (int)abilitiesVector.size(); v++) {
            string s       = abilitiesVector[v];
            const size_t t = s.find("add");
            if (t != string::npos) {
                s            = s.substr(t + 3);
                ManaCost* mc = ManaCost::parseManaCost(s);
                for (int j = 0; j < Constants::NB_Colors; j++) {
                    if (mc->hasColor(j)) {
                        if (current->data->isLand()) {
                            if (current->data->hasType("Basic")) {
                                this->countBasicLandsPerColor[j] += currentCount;
                            } else {
                                this->countLandsPerColor[j] += currentCount;
                            }
                        } else {
                            this->countNonLandProducersPerColor[j] += currentCount;
                        }
                    }
                }
                SAFE_DELETE(mc);
            }
        }

        // Add to the per color counters
        //  a. regular costs
        for (int j = 0; j < Constants::NB_Colors; j++) {
            this->totalCostPerColor[j] += currentCost->getCost(j) * currentCount;
            if (current->data->hasColor(j)) {
                // Add to the per cost and color counter
                this->countCardsPerCostAndColor[convertedCost][j] += currentCount;
                if (current->data->isCreature()) {
                    this->countCreaturesPerCostAndColor[convertedCost][j] += currentCount;
                } else if (current->data->isSpell()) {
                    this->countSpellsPerCostAndColor[convertedCost][j] += currentCount;
                }
            }
        }

        //  b. Hybrid costs
        ManaCostHybrid* hybridCost;
        int i;
        i = 0;

        while ((hybridCost = currentCost->getHybridCost(i++)) != nullptr) {
            this->totalCostPerColor[hybridCost->color1] += hybridCost->value1 * currentCount;
            this->totalCostPerColor[hybridCost->color2] += hybridCost->value2 * currentCount;
        }
    }

    this->totalColoredSymbols = 0;
    for (int j = 1; j < Constants::NB_Colors; j++) {
        this->totalColoredSymbols += this->totalCostPerColor[j];
    }

    this->countCardsPerCost[0] -= this->countLands;

    // Counts by type
    this->countCreatures    = countCardsByType("Creature", myDeck);
    this->countInstants     = countCardsByType("Instant", myDeck);
    this->countEnchantments = countCardsByType("Enchantment", myDeck);
    this->countSorceries    = countCardsByType("Sorcery", myDeck);
    this->countSpells       = this->countInstants + this->countEnchantments + this->countSorceries;
    // this->countArtifacts = countCardsByType("Artifact", myDeck);

    // Average mana costs
    this->avgManaCost     = ((this->cardCount - this->countLands) <= 0)
                                ? 0
                                : (float)this->totalManaCost / (this->cardCount - this->countLands);
    this->avgCreatureCost = (this->countCreatures <= 0) ? 0 : (float)this->totalCreatureCost / this->countCreatures;
    this->avgSpellCost    = (this->countSpells <= 0) ? 0 : (float)this->totalSpellCost / this->countSpells;

    // Probabilities
    // TODO: this could be optimized by reusing results
    for (int i = 0; i < Constants::STATS_FOR_TURNS; i++) {
        this->noLandsProbInTurn[i]     = noLuck(this->cardCount, this->countLands, 7 + i) * 100;
        this->noCreaturesProbInTurn[i] = noLuck(this->cardCount, this->countCreatures, 7 + i) * 100;
    }
}

// This should probably be cached in DeckDataWrapper
// or at least be calculated for all common types in one go
int StatsWrapper::countCardsByType(const char* _type, DeckDataWrapper* myDeck) {
    int result = 0;
    for (int i = 0; i < myDeck->Size(true); i++) {
        MTGCard* current = myDeck->getCard(i, true);
        if (current->data->hasType(_type)) {
            result += myDeck->count(current);
        }
    }
    return result;
}

// n cards total, a of them are of desired type (A), x drawn
// returns probability of no A's
float StatsWrapper::noLuck(int n, int a, int x) {
    if ((a >= n) || (a == 0)) {
        return 1;
    }
    if ((n == 0) || (x == 0) || (x > n) || (n - a < x)) {
        return 0;
    }

    a            = n - a;
    float result = 1;

    for (int i = 0; i < x; i++) {
        result *= (float)(a - i) / (n - i);
    }
    return result;
}

StatsWrapper::~StatsWrapper() {
    aiDeckNames.clear();
    aiDeckStats.clear();
}
