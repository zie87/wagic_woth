#include "PrecompiledHeader.h"

#include "AIStats.h"
#include "GameObserver.h"
#include "Player.h"
#include "MTGCardInstance.h"
#include "WEvent.h"
#include "AllAbilities.h"
// TODO:better comments this is too cryptic to work on by anyone but original coder.
bool compare_aistats(AIStat* first, AIStat* second) {
    auto damage1 = static_cast<float>(first->value / first->occurences);
    auto damage2 = static_cast<float>(second->value / second->occurences);
    return (damage1 > damage2);
}

AIStats::AIStats(Player* _player, char* _filename) : filename(_filename), player(_player) { load(_filename); }

AIStats::~AIStats() {
    list<AIStat*>::iterator it;
    for (it = stats.begin(); it != stats.end(); it++) {
        AIStat* stat = *it;
        delete stat;
    }
}

void AIStats::updateStatsCard(MTGCardInstance* cardInstance, Damage* damage, float multiplier) {
    MTGCard* card = cardInstance->model;
    if (!card) {
        return;  // card can be null because some special cardInstances (such as ExtraRules) don't have a "model"
    }

    AIStat* stat = find(card);
    if (!stat) {
        stat = NEW AIStat(card->getMTGId(), 0, 1, 0);
        stats.push_back(stat);
    }
    if (damage->target == player) {
        stat->value += static_cast<int>(multiplier * STATS_PLAYER_MULTIPLIER * damage->damage);
    } else if (damage->target->type_as_damageable == DAMAGEABLE_MTGCARDINSTANCE) {
        auto* target = dynamic_cast<MTGCardInstance*>(damage->target);
        if (target->controller() == player && !target->isInPlay(player->getObserver())) {
            // One of my creatures got lethal damage...
            stat->value += static_cast<int>(multiplier * STATS_CREATURE_MULTIPLIER * damage->damage);
        }
    }
}

int AIStats::receiveEvent(WEvent* event) {
    auto* e = dynamic_cast<WEventDamage*>(event);
    if (!e) {
        return 0;  // we take only Damage events into accountright now
    }
    Damage* damage            = e->damage;
    MTGGameZone* opponentZone = player->opponent()->game->inPlay;

    MTGCardInstance* card = damage->source;
    updateStatsCard(card, damage);

    // Auras on damage source can be the cause
    for (int i = 0; i < opponentZone->nb_cards; ++i) {
        MTGCardInstance* aura = opponentZone->cards[i];
        if (aura->target == card) {
            updateStatsCard(aura, damage, STATS_AURA_MULTIPLIER);
        }
    }

    GameObserver* g = player->getObserver();
    // Lords
    map<MTGCardInstance*, int> lords;
    for (size_t i = 1; i < g->mLayers->actionLayer()->mObjects.size(); i++) {  // 0 is not a mtgability...hackish
        MTGAbility* a = (dynamic_cast<MTGAbility*>(g->mLayers->actionLayer()->mObjects[i]));
        if (auto* al = dynamic_cast<ALord*>(a)) {
            if (al->cards.find(card) != al->cards.end() && opponentZone->hasCard(al->source)) {
                lords[al->source] = 1;
            }
        }
    }
    if (const size_t nb = lords.size()) {
        for (auto it = lords.begin(); it != lords.end(); ++it) {
            updateStatsCard(it->first, damage, STATS_LORD_MULTIPLIER / nb);
        }
    }

    stats.sort(compare_aistats);  // this could be slow, if it is, let's run it only at the end of the turn
    return 1;
}
// the following tells ai if a creature should be blocked or targeted
bool AIStats::isInTop(MTGCardInstance* card, unsigned int max, bool tooSmallCountsForTrue) {
    // return true;
    // uncomment the above return to make Ai always multiblock your creatures.
    if (stats.size() < max) {
        return tooSmallCountsForTrue;
    }
    unsigned int n  = 0;
    MTGCard* source = card->model;
    const int id    = source->getMTGId();
    list<AIStat*>::iterator it;
    for (it = stats.begin(); it != stats.end(); it++) {
        if (n >= max) {
            return false;
        }
        AIStat* stat = *it;
        if (stat->source == id) {
            if ((stat->value + card->DangerRanking()) >= 3) {
                return true;
            }
            return false;
        }
        n++;
    }
    return false;
}

AIStat* AIStats::find(MTGCard* source) {
    const int id = source->getMTGId();
    list<AIStat*>::iterator it;
    for (it = stats.begin(); it != stats.end(); it++) {
        AIStat* stat = *it;
        if (stat->source == id) {
            return stat;
        }
    }
    return nullptr;
}

void AIStats::load(char* filename) {
    std::string contents;
    if (JFileSystem::GetInstance()->readIntoString(filename, contents)) {
        std::stringstream stream(contents);
        std::string s;
        while (std::getline(stream, s)) {
            const int cardid = atoi(s.c_str());
            std::getline(stream, s);
            const int value = atoi(s.c_str());
            std::getline(stream, s);
            const bool direct = atoi(s.c_str()) > 0;
            auto* stat        = NEW AIStat(cardid, value, 1, direct);
            stats.push_back(stat);
        }
    } else {
        DebugTrace("FATAL: AIStats.cpp:load : can't load" << filename);
    }
}
void AIStats::save() {
    std::ofstream file;
    if (JFileSystem::GetInstance()->openForWrite(file, filename)) {
        char writer[128];
        list<AIStat*>::iterator it;
        for (it = stats.begin(); it != stats.end(); it++) {
            AIStat* stat = *it;
            if (stat->value > 0) {
                sprintf(writer, "%i\n%i\n%i\n", stat->source, stat->value / 2, stat->direct);
                file << writer;
            }
        }
        file.close();
    }
}

void AIStats::Render() {
    GameObserver* g = player->getObserver();
    float x0        = 10;
    if (player == g->players[1]) {
        x0 = 280;
    }
    JRenderer::GetInstance()->FillRoundRect(x0, 10, 200, 180, 5, ARGB(50, 0, 0, 0));

    WFont* f = g->getResourceManager()->GetWFont(Fonts::MAIN_FONT);
    int i    = 0;
    char buffer[512];
    list<AIStat*>::iterator it;
    for (it = stats.begin(); it != stats.end(); ++it) {
        if (i > 10) {
            break;
        }
        AIStat* stat = *it;
        if (stat->value > 0) {
            MTGCard* card = MTGCollection()->getCardById(stat->source);
            if (card) {
                sprintf(buffer, "%s %i", card->data->getName().c_str(), stat->value);
                f->DrawString(buffer, x0 + 5, 10 + 16 * (float)i);
                i++;
            }
        }
    }
}
