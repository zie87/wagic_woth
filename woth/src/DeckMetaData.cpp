#include "woth/string/algorithms.hpp"

#include "PrecompiledHeader.h"

#include "DeckMetaData.h"
#include "DeckStats.h"
#include "MTGDeck.h"
#include "utils.h"
#include "Translate.h"

// Possible improvements:
// Merge this with DeckStats
// Have this class handle all the Meta Data rather than relying on MTGDeck. Then MTGDeck would have a MetaData object...

DeckMetaData::DeckMetaData(const string& filename, bool isAI)
    : mFilename(filename)
    , mGamesPlayed(0)
    , mVictories(0)
    , mPercentVictories(0)
    , mDifficulty(0)
    , mDeckLoaded(false)
    , mStatsLoaded(false)
    , mIsAI(isAI) {
    // TODO, figure out how we can defer this to later - currently,
    // there's a catch 22, as we sort the deck list alphabetically, so we need to open the deck file
    // to get its name.  This means that for the opponent list, we crack open 106 files just to read the deck name
    //, which is the bulk of the remaining 4 second delay we see the first time we try to pick an opponent on the first
    // match
    LoadDeck();
}

void DeckMetaData::LoadDeck() {
    if (!mDeckLoaded) {
        MTGDeck deck(mFilename.c_str(), nullptr, 1);
        mName        = ::woth::trim(deck.meta_name);
        mDescription = ::woth::trim(deck.meta_desc);
        mDeckId      = atoi((mFilename.substr(mFilename.find("deck") + 4, mFilename.find(".txt"))).c_str());

        std::vector<std::string> requirements = ::woth::split(deck.meta_unlockRequirements, ',');
        for (size_t i = 0; i < requirements.size(); ++i) {
            mUnlockRequirements.push_back(Options::getID(requirements[i]));
        }

        mDeckLoaded = true;
        if (!mIsAI) {
            mAvatarFilename = "avatar.jpg";
        } else {
            std::ostringstream avatarFilename;
            avatarFilename << "avatar" << getAvatarId() << ".jpg";
            mAvatarFilename = avatarFilename.str();
        }
    }
}

void DeckMetaData::LoadStats() {
    if (!mStatsLoaded) {
        DeckStats* stats = DeckStats::GetInstance();
        if (mIsAI) {
            mPercentVictories = 0;
            mVictories        = 0;
            mGamesPlayed      = 0;
            mColorIndex       = "";
            mDifficulty       = 0;

            stats->load(mPlayerDeck);
            DeckStat* opponentDeckStats = stats->getDeckStat(mStatsFilename);
            if (opponentDeckStats) {
                mPercentVictories = opponentDeckStats->percentVictories();
                mVictories        = opponentDeckStats->victories;
                mGamesPlayed      = opponentDeckStats->nbgames;
                mColorIndex       = opponentDeckStats->manaColorIndex;

                if (mPercentVictories < 34) {
                    mDifficulty = HARD;
                } else if (mPercentVictories < 55) {
                    mDifficulty = NORMAL;
                } else {
                    mDifficulty = EASY;
                }
                mStatsLoaded = true;
            }
        } else {
            if (FileExists(mStatsFilename)) {
                stats->load(mStatsFilename);
                mGamesPlayed      = stats->nbGames();
                mPercentVictories = stats->percentVictories();
                mVictories        = static_cast<int>(mGamesPlayed * (mPercentVictories / 100.0f));
                mStatsLoaded      = true;
            }
        }
    }
}

// since we only have 100 stock avatar images, we need to recycle the images for deck numbers > 99
int DeckMetaData::getAvatarId() const { return mDeckId % 100; }

// Accessors

string DeckMetaData::getFilename() { return mFilename; }

string DeckMetaData::getName() { return mName; }

int DeckMetaData::getDeckId() const { return mDeckId; }

std::vector<int> DeckMetaData::getUnlockRequirements() { return mUnlockRequirements; }

std::string DeckMetaData::getAvatarFilename() { return mAvatarFilename; }

std::string DeckMetaData::getColorIndex() { return mColorIndex; }

int DeckMetaData::getGamesPlayed() const { return mGamesPlayed; }

int DeckMetaData::getVictories() const { return mVictories; }

int DeckMetaData::getVictoryPercentage() const { return mPercentVictories; }

int DeckMetaData::getDifficulty() const { return mDifficulty; }

std::string DeckMetaData::getDifficultyString() const {
    std::string difficultyString = "Normal";
    switch (mDifficulty) {
    case HARD:
        difficultyString = "Hard";
        break;
    case EASY:
        difficultyString = "Easy";
        break;
    }

    return difficultyString;
}

std::string DeckMetaData::getDescription() { return mDescription; }

std::string DeckMetaData::getStatsSummary() {
    LoadStats();

    std::ostringstream statsSummary;
    statsSummary << _("Difficulty: ") << _(getDifficultyString()) << std::endl
                 << _("Victory %: ") << getVictoryPercentage() << std::endl
                 << _("Games Played: ") << getGamesPlayed() << std::endl;

    return statsSummary.str();
}

void DeckMetaData::setColorIndex(const std::string& colorIndex) { mColorIndex = colorIndex; }

void DeckMetaData::setDeckName(const std::string& newDeckTitle) { mName = newDeckTitle; }

void DeckMetaData::Invalidate() { mStatsLoaded = false; }
