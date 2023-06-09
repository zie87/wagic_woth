#include "woth/string/algorithms.hpp"

#include "PrecompiledHeader.h"

#include "MTGAbility.h"

#include <utility>
#include "ManaCost.h"
#include "MTGGameZones.h"
#include "AllAbilities.h"
#include "Damage.h"
#include "TargetChooser.h"
#include "CardGui.h"
#include "MTGDeck.h"
#include "Translate.h"
#include "ThisDescriptor.h"
#include "ExtraCost.h"
#include "MTGRules.h"
#include "AbilityParser.h"

// Used for Lord/This parsing
const string kLordKeywords[]    = {"lord(", "foreach(", "aslongas(", "teach(", "all("};
const size_t kLordKeywordsCount = 5;

const string kThisKeywords[]    = {"this(", "thisforeach("};
const size_t kThisKeywordsCount = 2;

// Used for the maxCast/maxPlay ability parsing
const string kMaxCastKeywords[]    = {"maxplay(", "maxcast("};
const int kMaxCastZones[]          = {MTGGameZone::BATTLEFIELD, MTGGameZone::STACK};
const size_t kMaxCastKeywordsCount = 2;

// Used for alternate Costs parsing
const string kAlternateCostKeywords[] = {"kicker", "retrace", "alternative", "buyback", "flashback", "facedown"};
const int kAlternateCostIds[]         = {ManaCost::MANA_PAID_WITH_KICKER,      ManaCost::MANA_PAID_WITH_RETRACE,
                                         ManaCost::MANA_PAID_WITH_ALTERNATIVE, ManaCost::MANA_PAID_WITH_BUYBACK,
                                         ManaCost::MANA_PAID_WITH_FLASHBACK,   ManaCost::MANA_PAID_WITH_MORPH};

// Used for "dynamic ability" parsing
const string kDynamicSourceKeywords[] = {"source", "mytgt", "myself", "myfoe"};
const int kDynamicSourceIds[]         = {AADynamic::DYNAMIC_SOURCE_AMOUNT, AADynamic::DYNAMIC_MYTGT_AMOUNT,
                                         AADynamic::DYNAMIC_MYSELF_AMOUNT, AADynamic::DYNAMIC_MYFOE_AMOUNT};

const string kDynamicTypeKeywords[] = {"power", "toughness", "manacost",       "colors",
                                       "age",   "charge",    "oneonecounters", "thatmuch"};
const int kDynamicTypeIds[]         = {AADynamic::DYNAMIC_ABILITY_TYPE_POWER,
                                       AADynamic::DYNAMIC_ABILITY_TYPE_TOUGHNESS,
                                       AADynamic::DYNAMIC_ABILITY_TYPE_MANACOST,
                                       AADynamic::DYNAMIC_ABILITY_TYPE_COLORS,
                                       AADynamic::DYNAMIC_ABILITY_TYPE_AGE,
                                       AADynamic::DYNAMIC_ABILITY_TYPE_CHARGE,
                                       AADynamic::DYNAMIC_ABILITY_TYPE_ONEONECOUNTERS,
                                       AADynamic::DYNAMIC_ABILITY_TYPE_THATMUCH};

const string kDynamicEffectKeywords[] = {"strike",    "draw",     "lifeloss", "lifegain",      "pumppow",
                                         "pumptough", "pumpboth", "deplete",  "countersoneone"};
const int kDynamicEffectIds[]         = {
    AADynamic::DYNAMIC_ABILITY_EFFECT_STRIKE,        AADynamic::DYNAMIC_ABILITY_EFFECT_DRAW,
    AADynamic::DYNAMIC_ABILITY_EFFECT_LIFELOSS,      AADynamic::DYNAMIC_ABILITY_EFFECT_LIFEGAIN,
    AADynamic::DYNAMIC_ABILITY_EFFECT_PUMPPOWER,     AADynamic::DYNAMIC_ABILITY_EFFECT_PUMPTOUGHNESS,
    AADynamic::DYNAMIC_ABILITY_EFFECT_PUMPBOTH,      AADynamic::DYNAMIC_ABILITY_EFFECT_DEPLETE,
    AADynamic::DYNAMIC_ABILITY_EFFECT_COUNTERSONEONE};

const string kDynamicWhoKeywords[] = {"eachother", "itself",        "targetcontroller", "targetopponent",
                                      "tosrc",     "srccontroller", "srcopponent"};
const int kDynamicWhoIds[]         = {
    AADynamic::DYNAMIC_ABILITY_WHO_EACHOTHER,        AADynamic::DYNAMIC_ABILITY_WHO_ITSELF,
    AADynamic::DYNAMIC_ABILITY_WHO_TARGETCONTROLLER, AADynamic::DYNAMIC_ABILITY_WHO_TARGETOPPONENT,
    AADynamic::DYNAMIC_ABILITY_WHO_TOSOURCE,         AADynamic::DYNAMIC_ABILITY_WHO_SOURCECONTROLLER,
    AADynamic::DYNAMIC_ABILITY_WHO_SOURCEOPPONENT};

int MTGAbility::allowedToCast(MTGCardInstance* card, Player* player) {
    AbilityFactory af(game);
    return af.parseCastRestrictions(card, player, card->getRestrictions());
}

int MTGAbility::allowedToAltCast(MTGCardInstance* card, Player* player) {
    AbilityFactory af(game);
    return af.parseCastRestrictions(card, player, card->getOtherRestrictions());
}

int AbilityFactory::parseCastRestrictions(MTGCardInstance* card, Player* player, const string& restrictions) {
    vector<string> restriction = ::woth::split(restrictions, ',');
    const AbilityFactory af(observer);
    const int cPhase = observer->getCurrentGamePhase();
    for (unsigned int i = 0; i < restriction.size(); i++) {
        const int checkPhaseBased = parseRestriction(restriction[i]);
        switch (checkPhaseBased) {
        case MTGAbility::PLAYER_TURN_ONLY:
            if (player != observer->currentPlayer) {
                return 0;
            }
            break;
        case MTGAbility::OPPONENT_TURN_ONLY:
            if (player == observer->currentPlayer) {
                return 0;
            }
            break;
        case MTGAbility::AS_SORCERY:
            if (player != observer->currentPlayer) {
                return 0;
            }
            if (cPhase != MTG_PHASE_FIRSTMAIN && cPhase != MTG_PHASE_SECONDMAIN) {
                return 0;
            }
            break;
        }
        if (checkPhaseBased >= MTGAbility::MY_BEFORE_BEGIN && checkPhaseBased <= MTGAbility::MY_AFTER_EOT) {
            if (player != observer->currentPlayer) {
                return 0;
            }
            if (cPhase != checkPhaseBased - MTGAbility::MY_BEFORE_BEGIN + MTG_PHASE_BEFORE_BEGIN) {
                return 0;
            }
        }
        if (checkPhaseBased >= MTGAbility::OPPONENT_BEFORE_BEGIN && checkPhaseBased <= MTGAbility::OPPONENT_AFTER_EOT) {
            if (player == observer->currentPlayer) {
                return 0;
            }
            if (cPhase != checkPhaseBased - MTGAbility::OPPONENT_BEFORE_BEGIN + MTG_PHASE_BEFORE_BEGIN) {
                return 0;
            }
        }
        if (checkPhaseBased >= MTGAbility::BEFORE_BEGIN && checkPhaseBased <= MTGAbility::AFTER_EOT) {
            if (cPhase != checkPhaseBased - MTGAbility::BEFORE_BEGIN + MTG_PHASE_BEFORE_BEGIN) {
                return 0;
            }
        }

        const size_t typeRelated = restriction[i].find("type(");
        const size_t seenType    = restriction[i].find("lastturn(");
        size_t seenRelated       = restriction[i].find("lastturn(");
        if (seenRelated == string::npos) {
            seenRelated = restriction[i].find("thisturn(");
        }
        const size_t compRelated = restriction[i].find("compare(");

        size_t check = 0;
        if (typeRelated != string::npos || seenRelated != string::npos || compRelated != string::npos) {
            int firstAmount  = 0;
            int secondAmount = 0;
            int mod          = 0;
            string rtc;
            vector<string> comparasion = ::woth::split(restriction[i], '~');
            if (comparasion.size() != 3) {
                return 0;  // it was incorrectly coded, user should proofread card code.
            }
            const bool less  = comparasion[1].find("lessthan") != string::npos;
            const bool more  = comparasion[1].find("morethan") != string::npos;
            const bool equal = comparasion[1].find("equalto") != string::npos;
            for (unsigned int i = 0; i < comparasion.size(); i++) {
                check = comparasion[i].find("type(");
                if (check == string::npos) {
                    check = comparasion[i].find("lastturn(");
                }
                if (check == string::npos) {
                    check = comparasion[i].find("thisturn(");
                }
                if (check == string::npos) {
                    check = comparasion[i].find("compare(");
                }
                if (check != string::npos) {
                    size_t end             = 0;
                    const size_t foundType = comparasion[i].find("type(");
                    const size_t foundComp = comparasion[i].find("compare(");
                    size_t foundSeen       = comparasion[i].find("lastturn(");
                    if (foundSeen == string::npos) {
                        foundSeen = comparasion[i].find("thisturn(");
                    }
                    if (foundType != string::npos) {
                        end = comparasion[i].find(')', foundType);
                        rtc = comparasion[i].substr(foundType + 5, end - foundType - 5);
                        TargetChooserFactory tcf(observer);
                        TargetChooser* ttc = tcf.createTargetChooser(rtc, card);
                        mod                = atoi(comparasion[i].substr(end + 1).c_str());
                        if (i == 2) {
                            secondAmount = ttc->countValidTargets();
                            secondAmount += mod;
                        } else {
                            firstAmount = ttc->countValidTargets();
                            firstAmount += mod;
                        }

                        SAFE_DELETE(ttc);
                    }
                    if (foundComp != string::npos) {
                        end = comparasion[i].find(')', foundComp);
                        rtc = comparasion[i].substr(foundComp + 8, end - foundComp - 8);
                        mod = atoi(comparasion[i].substr(end + 1).c_str());
                        if (i == 2) {
                            auto* newAmount = NEW WParsedInt(rtc, card);
                            secondAmount    = newAmount->getValue();
                            secondAmount += mod;
                            SAFE_DELETE(newAmount);
                        } else {
                            auto* newAmount = NEW WParsedInt(rtc, card);
                            firstAmount     = newAmount->getValue();
                            firstAmount += mod;
                            SAFE_DELETE(newAmount);
                        }
                    }
                    if (foundSeen != string::npos) {
                        end = comparasion[i].find(')', foundSeen);
                        rtc = comparasion[i].substr(foundSeen + 9, end - foundSeen - 9);
                        mod = atoi(comparasion[i].substr(end + 1).c_str());

                        TargetChooserFactory tcf(observer);
                        TargetChooser* stc = tcf.createTargetChooser(rtc, card);
                        for (int w = 0; w < 2; ++w) {
                            Player* p            = observer->players[w];
                            MTGGameZone* zones[] = {p->game->inPlay,  p->game->graveyard, p->game->hand,
                                                    p->game->library, p->game->stack,     p->game->exile};
                            for (int k = 0; k < 6; k++) {
                                MTGGameZone* z = zones[k];
                                if (stc->targetsZone(z)) {
                                    if (i == 2) {
                                        secondAmount += seenType != string::npos
                                                            ? z->seenLastTurn(rtc, Constants::CAST_ALL)
                                                            : z->seenThisTurn(rtc, Constants::CAST_ALL);

                                    } else {
                                        firstAmount += seenType != string::npos
                                                           ? z->seenLastTurn(rtc, Constants::CAST_ALL)
                                                           : z->seenThisTurn(rtc, Constants::CAST_ALL);
                                    }
                                }
                            }
                        }
                        i == 2 ? secondAmount += mod : firstAmount += mod;
                        SAFE_DELETE(stc);
                    }
                } else if (i == 2) {
                    auto* secondA = NEW WParsedInt(comparasion[2], nullptr, card);
                    secondAmount  = secondA->getValue();
                    SAFE_DELETE(secondA);
                }
            }
            if (firstAmount < secondAmount && !less && !more && !equal) {
                return 0;
            }
            if (equal && firstAmount != secondAmount) {
                return 0;
            }
            if (less && firstAmount >= secondAmount) {
                return 0;
            }
            if (more && firstAmount <= secondAmount) {
                return 0;
            }
        }
        check = restriction[i].find("turn:");
        if (check != string::npos) {
            int Turn           = 0;
            const size_t start = restriction[i].find(':', check);
            const size_t end   = restriction[i].find(' ', check);
            Turn               = atoi(restriction[i].substr(start + 1, end - start - 1).c_str());
            if (observer->turn < Turn - 1) {
                return 0;
            }
        }
        check = restriction[i].find("casted a spell");
        if (check != string::npos) {
            if (player->game->stack->seenThisTurn("*", Constants::CAST_ALL) < 1) {
                return 0;
            }
        }
        check = restriction[i].find("casted(");
        if (check != string::npos) {
            const size_t end = restriction[i].find(')', check);
            const string tc  = restriction[i].substr(check + 7, end - check - 7);
            if (tc.find("|mystack") != string::npos) {
                if (player->game->stack->seenThisTurn(tc, Constants::CAST_ALL) < 1) {
                    return 0;
                }
            } else if (tc.find("|opponentstack") != string::npos) {
                if (player->opponent()->game->stack->seenThisTurn(tc, Constants::CAST_ALL) < 1) {
                    return 0;
                }
            } else if (tc.find("this") != string::npos) {
                int count = 0;
                for (unsigned int k = 0; k < player->game->stack->cardsSeenThisTurn.size(); k++) {
                    MTGCardInstance* stackCard = player->game->stack->cardsSeenThisTurn[k];
                    if (stackCard->next && stackCard->next == card) {
                        count++;
                    }
                    if (stackCard == card) {
                        count++;
                    }
                }
                if (!count) {
                    return 0;
                }
            }
        }
        check = restriction[i].find("morbid");
        if (check != string::npos) {
            bool isMorbid = false;
            for (int cp = 0; cp < 2; cp++) {
                Player* checkCurrent = observer->players[cp];
                MTGGameZone* grave   = checkCurrent->game->graveyard;
                for (unsigned int gy = 0; gy < grave->cardsSeenThisTurn.size(); gy++) {
                    MTGCardInstance* checkCard = grave->cardsSeenThisTurn[gy];
                    if (checkCard->isCreature()) {
                        isMorbid = true;
                        break;
                    }
                }
                if (isMorbid) {
                    break;
                }
            }
            if (!isMorbid) {
                return 0;
            }
        }
        check = restriction[i].find("one of a kind");
        if (check != string::npos) {
            if (player->game->inPlay->hasName(card->name)) {
                return 0;
            }
        }
        check = restriction[i].find("before battle damage");
        if (check != string::npos) {
            if (cPhase > MTG_PHASE_COMBATBLOCKERS) {
                return 0;
            }
        }
        check = restriction[i].find("after battle");
        if (check != string::npos) {
            if (cPhase < MTG_PHASE_COMBATBLOCKERS) {
                return 0;
            }
        }
        check = restriction[i].find("during battle");
        if (check != string::npos) {
            if (cPhase < MTG_PHASE_COMBATBEGIN || cPhase > MTG_PHASE_COMBATEND) {
                return 0;
            }
        }
        check = restriction[i].find("control snow land");
        if (check != string::npos) {
            if (!player->game->inPlay->hasPrimaryType("snow", "land")) {
                return 0;
            }
        }
        check = restriction[i].find("control two or more vampires");
        if (check != string::npos) {
            restriction.emplace_back("type(vampire|mybattlefield)~morethan~1");
        }
        check = restriction[i].find("control less creatures");
        if (check != string::npos) {
            restriction.emplace_back("type(creature|mybattlefield)~lessthan~type(creature|opponentbattlefield)");
        }
    }
    return 1;
}

int AbilityFactory::parseRestriction(const string& s) {
    if (s.find("myturnonly") != string::npos) {
        return ActivatedAbility::PLAYER_TURN_ONLY;
    }
    if (s.find("opponentturnonly") != string::npos) {
        return ActivatedAbility::OPPONENT_TURN_ONLY;
    }
    if (s.find("assorcery") != string::npos) {
        return ActivatedAbility::AS_SORCERY;
    }

    const string types[] = {"my", "opponent", ""};
    const int starts[]   = {ActivatedAbility::MY_BEFORE_BEGIN, ActivatedAbility::OPPONENT_BEFORE_BEGIN,
                            ActivatedAbility::BEFORE_BEGIN};
    for (int j = 0; j < 3; ++j) {
        size_t found = s.find(types[j]);
        if (found != string::npos) {
            for (int i = 0; i < NB_MTG_PHASES; i++) {
                string toFind = types[j];
                toFind.append(Constants::MTGPhaseCodeNames[i]).append("only");
                found = s.find(toFind);
                if (found != string::npos) {
                    return starts[j] + i;
                }
            }
        }
    }

    return ActivatedAbility::NO_RESTRICTION;
}

int AbilityFactory::countCards(TargetChooser* tc, Player* player, int option) {
    int result = 0;
    for (int i = 0; i < 2; i++) {
        if (player && player != observer->players[i]) {
            continue;
        }
        MTGGameZone* zones[] = {observer->players[i]->game->inPlay, observer->players[i]->game->graveyard,
                                observer->players[i]->game->hand};
        for (int k = 0; k < 3; k++) {
            for (int j = zones[k]->nb_cards - 1; j >= 0; j--) {
                MTGCardInstance* current = zones[k]->cards[j];
                if (tc->canTarget(current)) {
                    switch (option) {
                    case COUNT_POWER:
                        result += current->power;
                        break;
                    default:
                        result++;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

Counter* AbilityFactory::parseCounter(const string& s, MTGCardInstance* target, Spell* spell) {
    int nb    = 1;
    int maxNb = 0;
    string name;
    string nbstr    = "1";
    string maxNbstr = "0";
    string spt;

    vector<string> splitCounter            = ::woth::split(s, ',');
    const vector<string> splitCounterCheck = ::woth::split(s, '.');
    if (splitCounter.size() < splitCounterCheck.size()) {
        splitCounter = splitCounterCheck;  // use the one with the most results.
    }
    if (splitCounter.empty()) {
        return nullptr;
    }
    if (!splitCounter.empty())  // counter(1/1)
    {
        spt = splitCounter[0];
    }
    if (splitCounter.size() > 1)  // counter(1/1,1)
    {
        nbstr = splitCounter[1];
    }
    if (splitCounter.size() > 2)  // counter(0/0,1,charge)
    {
        name = splitCounter[2];
    }
    if (splitCounter.size() > 3)  // counter(0/0,1,charge,2)
    {
        maxNbstr = splitCounter[3];
    }
    WParsedInt* wpi;
    if (target) {
        wpi = NEW WParsedInt(nbstr, spell, target);
    } else {
        wpi = NEW WParsedInt(atoi(nbstr.c_str()));
    }
    nb = wpi->getValue();
    delete (wpi);
    WParsedInt* wpinb;
    if (target) {
        wpinb = NEW WParsedInt(maxNbstr, spell, target);
    } else {
        wpinb = NEW WParsedInt(atoi(maxNbstr.c_str()));
    }
    maxNb = wpinb->getValue();
    delete (wpinb);

    int power;
    int toughness;
    if (parsePowerToughness(spt, &power, &toughness)) {
        auto* counter  = NEW Counter(target, name.c_str(), power, toughness);
        counter->nb    = nb;
        counter->maxNb = maxNb;
        return counter;
    }
    return nullptr;
}

int AbilityFactory::parsePowerToughness(const string& s, int* power, int* toughness) {
    vector<string> splitPT            = ::woth::split(s, '/');
    const vector<string> splitPTCheck = ::woth::split(s, '%');
    if (splitPT.size() < 2 && splitPT.size() < splitPTCheck.size()) {
        splitPT = splitPTCheck;
    }
    if (splitPT.empty()) {
        return 0;
    }
    *power     = atoi(splitPT[0].c_str());
    *toughness = atoi(splitPT[1].c_str());
    return 1;
}

TargetChooser* AbilityFactory::parseSimpleTC(const std::string& s,
                                             const std::string& _starter,
                                             MTGCardInstance* card,
                                             bool forceNoTarget) {
    string starter = _starter;
    starter.append("(");

    const size_t found = s.find(starter);
    if (found == string::npos) {
        return nullptr;
    }

    const size_t start = found + starter.size();

    const size_t end = s.find(')', start);
    if (end == string::npos) {
        DebugTrace("malformed syntax " << s);
        return nullptr;
    }

    const string starget = s.substr(start, end - start);
    TargetChooserFactory tcf(observer);
    TargetChooser* tc = tcf.createTargetChooser(starget, card);

    if (tc && forceNoTarget) {
        tc->targetter = nullptr;
    }

    return tc;
}

// evaluate trigger ability
// ie auto=@attacking(mytgt):destroy target(*)
// eval only the text between the @ and the first :
TriggeredAbility* AbilityFactory::parseTrigger(const string& s,
                                               const string& magicText,
                                               int id,
                                               Spell* spell,
                                               MTGCardInstance* card,
                                               Targetable* target) {
    size_t found = string::npos;

    // restrictions on triggers
    const bool once             = (s.find("once") != string::npos);
    const bool sourceUntapped   = (s.find("sourcenottap") != string::npos);
    const bool sourceTap        = (s.find("sourcetap") != string::npos);
    const bool limitOnceATurn   = (s.find("turnlimited") != string::npos);
    const bool isSuspended      = (s.find("suspended") != string::npos);
    const bool opponentPoisoned = (s.find("opponentpoisoned") != string::npos);
    const bool lifelost         = (s.find("foelost(") != string::npos);
    const int lifeamount        = lifelost ? atoi(s.substr(s.find("foelost(") + 8, ')').c_str()) : 0;
    const bool neverRemove      = (s.find("dontremove") != string::npos);

    // Card Changed Zone
    found = s.find("movedto(");
    if (found != string::npos) {
        size_t end     = s.find(')');
        string starget = s.substr(found + 8, end - found - 8);
        TargetChooserFactory tcf(observer);

        TargetChooser* toTc     = nullptr;
        TargetChooser* toTcCard = nullptr;
        end                     = starget.find('|');
        if (end == string::npos) {
            toTcCard = tcf.createTargetChooser("*", card);
            found    = 0;
        } else {
            toTcCard = tcf.createTargetChooser(starget.substr(0, end).append("|*"), card);
            found    = end + 1;
        }
        toTcCard->setAllZones();
        toTcCard->targetter = nullptr;  // avoid protection from
        starget             = starget.substr(found, end - found).insert(0, "*|");
        toTc                = tcf.createTargetChooser(starget, card);
        toTc->targetter     = nullptr;  // avoid protection from

        TargetChooser* fromTc     = nullptr;
        TargetChooser* fromTcCard = nullptr;
        found                     = s.find("from(");
        if (found != string::npos) {
            end = s.find('|', found);
            if (end == string::npos) {
                fromTcCard = tcf.createTargetChooser("*", card);
                found      = found + 5;
            } else {
                fromTcCard = tcf.createTargetChooser(s.substr(found + 5, end - found - 5).append("|*"), card);
                found      = end + 1;
            }
            fromTcCard->setAllZones();
            fromTcCard->targetter = nullptr;  // avoid protection from
            end                   = s.find(')', found);
            starget               = s.substr(found, end - found).insert(0, "*|");
            fromTc                = tcf.createTargetChooser(starget, card);
            fromTc->targetter     = nullptr;  // avoid protection from
        }
        TriggeredAbility* mover = NEW TrCardAddedToZone(observer, id, card, dynamic_cast<TargetZoneChooser*>(toTc),
                                                        toTcCard, dynamic_cast<TargetZoneChooser*>(fromTc), fromTcCard,
                                                        once, sourceUntapped, isSuspended);
        if (neverRemove) {
            mover->forcedAlive  = 1;
            mover->forceDestroy = -1;
        }
        return mover;
    }

    // Card unTapped
    if (TargetChooser* tc = parseSimpleTC(s, "untapped", card)) {
        return NEW TrCardTapped(observer, id, card, tc, false, once);
    }

    // Card Tapped
    if (TargetChooser* tc = parseSimpleTC(s, "tapped", card)) {
        return NEW TrCardTapped(observer, id, card, tc, true, once);
    }

    // Card Tapped for mana
    if (TargetChooser* tc = parseSimpleTC(s, "tappedformana", card)) {
        return NEW TrCardTappedformana(observer, id, card, tc, true, once);
    }

    // CombatTrigger
    // Card card attacked and is blocked
    found = s.find("combat(");
    if (found != string::npos) {
        const size_t end           = s.find(')', found);
        const string combatTrigger = s.substr(found + 7, end - found - 7);
        // find combat traits, only trigger types, the general restrictions are found earlier.
        bool attackingTrigger              = false;
        bool attackedAloneTrigger          = false;
        bool notBlockedTrigger             = false;
        bool attackBlockedTrigger          = false;
        bool blockingTrigger               = false;
        vector<string> combatTriggerVector = ::woth::split(combatTrigger, ',');
        for (unsigned int i = 0; i < combatTriggerVector.size(); i++) {
            if (combatTriggerVector[i] == "attacking") {
                attackingTrigger = true;
            }
            if (combatTriggerVector[i] == "attackedalone") {
                attackedAloneTrigger = true;
            }
            if (combatTriggerVector[i] == "notblocked") {
                notBlockedTrigger = true;
            }
            if (combatTriggerVector[i] == "blocked") {
                attackBlockedTrigger = true;
            }
            if (combatTriggerVector[i] == "blocking") {
                blockingTrigger = true;
            }
        }
        // build triggers TCs
        TargetChooser* tc = parseSimpleTC(s, "source", card);
        if (!tc) {  // a source( is required, from( is optional.
            return nullptr;
        }

        TargetChooser* fromTc = parseSimpleTC(s, "from", card);

        return NEW TrCombatTrigger(observer, id, card, tc, fromTc, once, limitOnceATurn, sourceUntapped,
                                   opponentPoisoned, attackingTrigger, attackedAloneTrigger, notBlockedTrigger,
                                   attackBlockedTrigger, blockingTrigger);
    }

    // Card card is drawn
    if (TargetChooser* tc = parseSimpleTC(s, "drawn", card)) {
        return NEW TrcardDrawn(observer, id, card, tc, once);
    }

    // Card is sacrificed
    if (TargetChooser* tc = parseSimpleTC(s, "sacrificed", card)) {
        return NEW TrCardSacrificed(observer, id, card, tc, once);
    }

    // Card is Discarded
    if (TargetChooser* tc = parseSimpleTC(s, "discarded", card)) {
        return NEW TrCardDiscarded(observer, id, card, tc, once);
    }

    // Card is cycled
    if (TargetChooser* tc = parseSimpleTC(s, "cycled", card)) {
        return NEW TrCardDiscarded(observer, id, card, tc, once, true);
    }

    // Card Damaging non combat
    if (TargetChooser* tc = parseSimpleTC(s, "noncombatdamaged", card)) {
        TargetChooser* fromTc = parseSimpleTC(s, "from", card);
        return NEW TrDamaged(observer, id, card, tc, fromTc, 2, once);
    }

    // Card Damaging combat
    if (TargetChooser* tc = parseSimpleTC(s, "combatdamaged", card)) {
        TargetChooser* fromTc = parseSimpleTC(s, "from", card);
        return NEW TrDamaged(observer, id, card, tc, fromTc, 1, sourceUntapped, limitOnceATurn, once);
    }

    // Card Damaging
    if (TargetChooser* tc = parseSimpleTC(s, "damaged", card)) {
        TargetChooser* fromTc = parseSimpleTC(s, "from", card);
        return NEW TrDamaged(observer, id, card, tc, fromTc, 0, sourceUntapped, limitOnceATurn, once);
    }

    // Lifed
    if (TargetChooser* tc = parseSimpleTC(s, "lifed", card)) {
        TargetChooser* fromTc = parseSimpleTC(s, "from", card);
        return NEW TrLifeGained(observer, id, card, tc, fromTc, 0, sourceUntapped, once);
    }

    // Life Loss
    if (TargetChooser* tc = parseSimpleTC(s, "lifeloss", card)) {
        TargetChooser* fromTc = parseSimpleTC(s, "from", card);
        return NEW TrLifeGained(observer, id, card, tc, fromTc, 1, sourceUntapped, once);
    }

    // Card Damaged and killed by a creature this turn
    if (TargetChooser* tc = parseSimpleTC(s, "vampired", card)) {
        TargetChooser* fromTc = parseSimpleTC(s, "from", card);
        return NEW TrVampired(observer, id, card, tc, fromTc, once);
    }

    // when card becomes the target of a spell or ability
    if (TargetChooser* tc = parseSimpleTC(s, "targeted", card)) {
        TargetChooser* fromTc = parseSimpleTC(s, "from", card);
        return NEW TrTargeted(observer, id, card, tc, fromTc, 0, once);
    }

    if (s.find("counteradded(") != string::npos) {
        vector<string> splitCounter = parseBetween(s, "counteradded(", ")");
        Counter* counter            = parseCounter(splitCounter[1], card, nullptr);
        TargetChooser* tc           = parseSimpleTC(s, "from", card);
        return NEW TrCounter(observer, id, card, counter, tc, 1, once);
    }

    if (s.find("counterremoved(") != string::npos) {
        vector<string> splitCounter = parseBetween(s, "counterremoved(", ")");
        Counter* counter            = parseCounter(splitCounter[1], card, nullptr);
        TargetChooser* tc           = parseSimpleTC(s, "from", card);
        return NEW TrCounter(observer, id, card, counter, tc, 0, once);
    }

    int who = 0;
    if (s.find(" my") != string::npos) {
        who = 1;
    }
    if (s.find(" opponent") != string::npos) {
        who = -1;
    }
    if (s.find(" targetcontroller") != string::npos) {
        who = -2;
    }
    if (s.find(" targetedplayer") != string::npos) {
        who = -3;
    }

    // Next Time...
    found = s.find("next");
    if (found != string::npos) {
        for (int i = 0; i < NB_MTG_PHASES; i++) {
            found = s.find(Constants::MTGPhaseCodeNames[i]);
            if (found != string::npos) {
                return NEW TriggerNextPhase(observer, id, card, target, i, who, sourceUntapped, once);
            }
        }
    }

    // Each Time...
    found = s.find("each");
    if (found != string::npos) {
        for (int i = 0; i < NB_MTG_PHASES; i++) {
            found = s.find(Constants::MTGPhaseCodeNames[i]);
            if (found != string::npos) {
                return NEW TriggerAtPhase(observer, id, card, target, i, who, sourceUntapped, sourceTap, lifelost,
                                          lifeamount, once);
            }
        }
    }

    return nullptr;
}

// When abilities encapsulate each other, gets the deepest one (it is the one likely to have the most relevant
// information)
MTGAbility* AbilityFactory::getCoreAbility(MTGAbility* a) {
    if (auto* fea = dynamic_cast<AForeach*>(a)) {
        return getCoreAbility(fea->ability);
    }

    if (auto* aea = dynamic_cast<AAsLongAs*>(a)) {
        return getCoreAbility(aea->ability);
    }

    if (auto* gta = dynamic_cast<GenericTargetAbility*>(a)) {
        return getCoreAbility(gta->ability);
    }

    if (auto* gaa = dynamic_cast<GenericActivatedAbility*>(a)) {
        return getCoreAbility(gaa->ability);
    }

    if (auto* abi = dynamic_cast<MultiAbility*>(a)) {
        return getCoreAbility(abi->abilities[0]);
    }

    if (auto* na = dynamic_cast<NestedAbility*>(a)) {
        if (na->ability) {
            // only atempt to return a nestedability if it contains a valid ability. example where this causes a bug
            // otherwise. AEquip is considered nested, but contains no ability.
            return getCoreAbility(na->ability);
        }
    }

    if (auto* ma = dynamic_cast<MenuAbility*>(a)) {
        return getCoreAbility(ma->abilities[0]);
    }

    return a;
}

// Parses a string and returns the corresponding MTGAbility object
// Returns NULL if parsing failed
// Beware, Spell CAN be null when the function is called by the AI trying to analyze the effects of a given card
MTGAbility* AbilityFactory::parseMagicLine(string s,
                                           int id,
                                           Spell* spell,
                                           MTGCardInstance* card,
                                           bool activated,
                                           bool forceUEOT,
                                           MTGGameZone* dest) {
    size_t found;
    ::woth::trim(s);
    // TODO This block redundant with calling function
    if (!card && spell) {
        card = spell->source;
    }
    if (!card) {
        return nullptr;
    }
    MTGCardInstance* target = card->target;
    if (!target) {
        target = card;
    }

    // MTG Specific rules
    // adds the bonus credit system
    found = s.find("bonusrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGEventBonus(observer, -1));
        return nullptr;
    }
    // putinplay/cast rule.. this is a parent rule and is required for all cost related rules.
    found = s.find("putinplayrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGPutInPlayRule(observer, -1));
        return nullptr;
    }
    // rule for kicker handling
    found = s.find("kickerrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGKickerRule(observer, -1));
        return nullptr;
    }
    // alternative cost types rule, this is a parent rule and is required for all cost related rules.
    found = s.find("alternativecostrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGAlternativeCostRule(observer, -1));
        return nullptr;
    }
    // alternative cost type buyback
    found = s.find("buybackrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGBuyBackRule(observer, -1));
        return nullptr;
    }
    // alternative cost type flashback
    found = s.find("flashbackrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGFlashBackRule(observer, -1));
        return nullptr;
    }
    // alternative cost type retrace
    found = s.find("retracerule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGRetraceRule(observer, -1));
        return nullptr;
    }
    // alternative cost type suspend
    found = s.find("suspendrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGSuspendRule(observer, -1));
        return nullptr;
    }
    // alternative cost type morph
    found = s.find("morphrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGMorphCostRule(observer, -1));
        return nullptr;
    }
    // this rule handles attacking ability during attacker phase
    found = s.find("attackrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGAttackRule(observer, -1));
        return nullptr;
    }
    // this rule handles blocking ability during blocker phase
    found = s.find("blockrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGBlockRule(observer, -1));
        return nullptr;
    }
    // this rule handles combat related triggers. note, combat related triggered abilities will not work without it.
    found = s.find("combattriggerrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGCombatTriggersRule(observer, -1));
        return nullptr;
    }
    // this handles the legend rule
    found = s.find("legendrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGLegendRule(observer, -1));
        return nullptr;
    }
    // this handles the planeswalker named legend rule which is dramatically different from above.
    found = s.find("planeswalkerrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGPlaneWalkerRule(observer, -1));
        return nullptr;
    }
    found = s.find("planeswalkerdamage");
    if (found != string::npos) {
        observer->addObserver(NEW MTGPlaneswalkerDamage(observer, -1));
        return nullptr;
    }
    found = s.find("planeswalkerattack");
    if (found != string::npos) {
        observer->addObserver(NEW MTGPlaneswalkerAttackRule(observer, -1));
        return nullptr;
    }

    // this handles the clean up of tokens !!MUST BE ADDED BEFORE PERSIST RULE!!
    found = s.find("tokencleanuprule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGTokensCleanup(observer, -1));
        return nullptr;
    }
    // this handles the returning of cards with persist to the battlefield.
    found = s.find("persistrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGPersistRule(observer, -1));
        return nullptr;
    }
    // this handles the vectors of cards which attacked and were attacked and later died during that turn.
    found = s.find("vampirerule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGVampireRule(observer, -1));
        return nullptr;
    }
    // this handles the removel of cards which were unearthed.
    found = s.find("unearthrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGUnearthRule(observer, -1));
        return nullptr;
    }
    // this handles lifelink ability rules.
    found = s.find("lifelinkrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGLifelinkRule(observer, -1));
        return nullptr;
    }
    // this handles death touch ability rule.
    found = s.find("deathtouchrule");
    if (found != string::npos) {
        observer->addObserver(NEW MTGDeathtouchRule(observer, -1));
        return nullptr;
    }

    if (strncmp(s.c_str(), "chooseacolor ", strlen("chooseacolor ")) == 0 ||
        strncmp(s.c_str(), "chooseatype ", strlen("chooseatype ")) == 0) {
        MTGAbility* choose = parseChooseActionAbility(s, card, spell, target, 0, id);
        choose             = NEW GenericActivatedAbility(observer, "", "", id, card, choose, nullptr);
        auto* mainAbility  = NEW MayAbility(observer, id, choose, card, true);
        return mainAbility;
    }

    // need to remove the section inside the transforms ability from the string before parsing
    // TODO: store string values of "&&" so we can remove the classes added just to add support
    // the current parser finds other abilities inside what should be nested abilities, and converts them into
    // actual abilities, this is a limitation.
    string unchangedS;
    unchangedS.append(s);
    found = s.find("transforms((");
    if (found != string::npos && storedString.empty()) {
        const size_t real_end         = s.find("))", found);
        const size_t stypesStartIndex = found + 12;
        storedString.append(s.substr(stypesStartIndex, real_end - stypesStartIndex));
        s.erase(stypesStartIndex, real_end - stypesStartIndex);
    }

    found = s.find("ability$!");
    if (found != string::npos && storedAbilityString.empty()) {
        const size_t real_end = s.find("!$", found);
        const size_t sIndex   = found + 9;
        storedAbilityString.append(s.substr(sIndex, real_end - sIndex));
        s.erase(sIndex, real_end - sIndex);
    } else {
        found = unchangedS.find("ability$!");  // did find it in a changed s, try unchanged.
        if (found != string::npos && storedAbilityString.empty()) {
            const size_t real_end = unchangedS.find("!$", found);
            const size_t sIndex   = found + 9;
            storedAbilityString.append(unchangedS.substr(sIndex, real_end - sIndex));
            unchangedS.erase(sIndex, real_end - sIndex);
        }
    }

    found = s.find("and!(");
    if (found != string::npos && found + 6 != ')' && storedAndAbility.empty()) {
        vector<string> splitAnd = parseBetween(s, "and!(", ")!", false);
        if (!splitAnd.empty()) {
            storedAndAbility.append(splitAnd[1]);
            const size_t real_end = s.find(")!", found);
            const size_t sIndex   = found + 5;
            s.erase(sIndex, real_end - sIndex);
        }
    }

    vector<string> splitTrigger = parseBetween(s, "@", ":");
    if (!splitTrigger.empty()) {
        TriggeredAbility* trigger = parseTrigger(splitTrigger[1], s, id, spell, card, target);
        if (splitTrigger[1].find("restriction{") != string::npos)  // using other/cast restrictions for abilities.
        {
            vector<string> splitRest = parseBetween(s, "restriction{", "}");
            if (!splitRest.empty()) {
                trigger->castRestriction = splitRest[1];
            }
        }
        // Dirty way to remove the trigger text (could get in the way)
        if (trigger) {
            MTGAbility* a = parseMagicLine(splitTrigger[2], id, spell, card, activated);
            if (!a) {
                delete trigger;
                return nullptr;
            }
            return NEW GenericTriggeredAbility(observer, id, card, trigger, a, nullptr, target);
        }
    }

    // This one is not a real ability, it displays a message on the screen. We use this for tutorials
    //  Triggers need to be checked above this one, as events are usuallly what will trigger (...) these messages
    {
        vector<string> splitMsg = parseBetween(s, "tutorial(", ")");
        if (!splitMsg.empty()) {
            const string msg = splitMsg[1];
            return NEW ATutorialMessage(observer, card, msg);
        }

        splitMsg = parseBetween(s, "message(", ")");
        if (!splitMsg.empty()) {
            const string msg = splitMsg[1];
            return NEW ATutorialMessage(observer, card, msg, 0);
        }
    }

    const int restrictions = parseRestriction(s);
    string castRestriction;
    if (s.find("restriction{") != string::npos)  // using other/cast restrictions for abilities.
    {
        vector<string> splitRest = parseBetween(s, "restriction{", "}");
        if (!splitRest.empty()) {
            castRestriction = splitRest[1];
        }
    }
    string newName;
    vector<string> splitName = parseBetween(s, "name(", ")");
    if (!splitName.empty()) {
        newName = splitName[1];
        s       = splitName[0];
        s.append(splitName[2]);
        // we erase the name section from the string to avoid
        // accidently building an mtg ability with the text meant for menuText.
    }

    TargetChooser* tc = nullptr;
    string sWithoutTc = s;
    string tcString;
    // Target Abilities - We also handle the case "notatarget" here, for things such as copy effects
    bool isTarget              = true;
    vector<string> splitTarget = parseBetween(s, "notatarget(", ")");
    if (!splitTarget.empty()) {
        isTarget = false;
    } else {
        splitTarget = parseBetween(s, "target(", ")");
    }

    if (!splitTarget.empty()) {
        TargetChooserFactory tcf(observer);
        tc       = tcf.createTargetChooser(splitTarget[1], card);
        tcString = splitTarget[1];

        if (!isTarget) {
            tc->targetter = nullptr;
        }
        sWithoutTc = splitTarget[0];
        sWithoutTc.append(splitTarget[2]);
    }

    const size_t delimiter     = sWithoutTc.find("}:");
    const size_t firstNonSpace = sWithoutTc.find_first_not_of(' ');
    if (delimiter != string::npos && firstNonSpace != string::npos && sWithoutTc[firstNonSpace] == '{') {
        ManaCost* cost = ManaCost::parseManaCost(sWithoutTc.substr(0, delimiter + 1), nullptr, card);
        int doTap      = 0;  // Tap in the cost ?
        if (cost && cost->extraCosts) {
            for (unsigned int i = 0; i < cost->extraCosts->costs.size(); i++) {
                ExtraCost* tapper = dynamic_cast<TapCost*>(cost->extraCosts->costs[i]);
                if (tapper) {
                    doTap = 1;
                }
            }
        }
        if (doTap || cost) {
            string s1 = sWithoutTc.substr(delimiter + 2);
            // grabbing the sideffect string and amount before parsing abilities.
            // side effect ei:dragond whelp.
            MTGAbility* sideEffect = nullptr;
            string usesBeforeSideEffect;
            const size_t limiteffect_str = s1.find("limit^");
            if (limiteffect_str != string::npos) {
                size_t end                 = s1.rfind('^');
                const string sideEffectStr = s1.substr(limiteffect_str + 6, end - limiteffect_str - 6);
                s1.erase(limiteffect_str, end - limiteffect_str);
                end                  = s1.find('^');
                usesBeforeSideEffect = s1.substr(end + 1);
                s1.erase(end - 1);
                sideEffect = parseMagicLine(sideEffectStr, id, spell, card, 1);
            }

            MTGAbility* a = parseMagicLine(s1, id, spell, card, 1);
            if (!a) {
                DebugTrace("ABILITYFACTORY Error parsing: " << sWithoutTc);
                return nullptr;
            }
            string limit;
            const size_t limit_str = sWithoutTc.find("limit:");
            if (limit_str != string::npos) {
                limit = sWithoutTc.substr(limit_str + 6);
            }
            ////A stupid Special case for ManaProducers, becuase Ai only understands manaabilities that are not nested.
            auto* amp = dynamic_cast<AManaProducer*>(a);
            if (amp) {
                amp->setCost(cost);
                if (cost) {
                    cost->setExtraCostsAction(a, card);
                }
                amp->oneShot               = 0;
                amp->tap                   = doTap;
                amp->limit                 = limit;
                amp->sideEffect            = sideEffect;
                amp->usesBeforeSideEffects = usesBeforeSideEffect;
                amp->restrictions          = restrictions;
                amp->castRestriction       = castRestriction;
                return amp;
            }

            auto* ae = dynamic_cast<AEquip*>(a);
            if (ae) {
                ae->setCost(cost);
                if (!tc) {
                    TargetChooserFactory tcf(observer);
                    tc = tcf.createTargetChooser("creature|mybattlefield", card);
                }
                ae->setActionTC(tc);
                return ae;
            }
            if (tc) {
                tc->belongsToAbility = sWithoutTc;
                return NEW GenericTargetAbility(observer, newName, castRestriction, id, card, tc, a, cost, limit,
                                                sideEffect, usesBeforeSideEffect, restrictions, dest, tcString);
            }
            return NEW GenericActivatedAbility(observer, newName, castRestriction, id, card, a, cost, limit, sideEffect,
                                               usesBeforeSideEffect, restrictions, dest);
        }
        SAFE_DELETE(cost);
    }

    // figure out alternative cost effects
    const string keyword;

    for (size_t i = 0; i < sizeof(kAlternateCostIds) / sizeof(kAlternateCostIds[0]); ++i) {
        if (s.find(kAlternateCostKeywords[i]) == 0) {
            if (!(spell && spell->FullfilledAlternateCost(kAlternateCostIds[i]))) {
                DebugTrace("INFO parseMagicLine: Alternative Cost was not fulfilled for " << s);
                SAFE_DELETE(tc);
                return nullptr;
            }
            return parseMagicLine(s.substr(kAlternateCostKeywords[i].length()), id, spell, card);
        }
    }
    // if/ifnot COND then DO EFFECT.
    const string ifKeywords[] = {"if ", "ifnot "};
    const int checkIf[]       = {1, 2};
    for (size_t i = 0; i < sizeof(checkIf) / sizeof(checkIf[0]); ++i) {
        if (sWithoutTc.find(ifKeywords[i]) == 0) {
            const string cond =
                sWithoutTc.substr(ifKeywords[i].length(), ifKeywords[i].length() + sWithoutTc.find(" then ") - 6);
            const size_t foundElse = s.find(" else ");
            MTGAbility* a2         = nullptr;
            if (foundElse != string::npos) {
                const string s2 = s.substr(foundElse + 6);
                if (!s2.empty()) {
                    s.erase(s.find(" else ") + 1);
                    a2 = parseMagicLine(s2, id, spell, card);
                }
            }
            const string s1 = s;
            MTGAbility* a1  = nullptr;
            if (s1.find(" then ") != string::npos) {
                a1 = parseMagicLine(s1.substr(s1.find(" then " + 1)), id, spell, card);
            }
            if (!a1) {
                return nullptr;
            }
            MTGAbility* a       = NEW IfThenAbility(observer, id, a1, a2, card, (Targetable*)target, checkIf[i], cond);
            a->canBeInterrupted = false;
            a->oneShot          = true;
            if (tc) {
                SAFE_DELETE(tc);
            }
            return a;
        }
    }

    // When...comes into play, you may...
    // When...comes into play, choose one...
    const string mayKeywords[] = {"may ", "choice "};
    const bool mayMust[]       = {false, true};
    ManaCost* mayCost          = nullptr;
    for (size_t i = 0; i < sizeof(mayMust) / sizeof(mayMust[0]); ++i) {
        if (sWithoutTc.find(mayKeywords[i]) == 0) {
            const string s1 = sWithoutTc.substr(mayKeywords[i].length());
            MTGAbility* a1  = nullptr;
            // may pay a cost for this ability
            vector<string> splitMayPay = parseBetween(s1, "pay(", ")", true);
            if (!splitMayPay.empty()) {
                a1      = parseMagicLine(splitMayPay[2], id, spell, card);
                mayCost = ManaCost::parseManaCost(splitMayPay[1], nullptr, card);
            } else {
                a1 = parseMagicLine(s1, id, spell, card);
            }
            if (!a1) {
                return nullptr;
            }

            if (tc) {
                a1 = NEW GenericTargetAbility(observer, newName, castRestriction, id, card, tc, a1);
            } else {
                a1 = NEW GenericActivatedAbility(observer, newName, castRestriction, id, card, a1, nullptr);
            }
            auto* mainAbility = NEW MayAbility(observer, id, a1, card, mayMust[i]);
            if (mayCost) {
                mainAbility->optionalCost = mayCost;
            }
            return mainAbility;
        }
    }
    // Generic "Until end of turn" effect
    if (s.find("ueot ") == 0) {
        const string s1 = s.substr(5);
        MTGAbility* a1  = parseMagicLine(s1, id, spell, card);
        if (!a1) {
            return nullptr;
        }

        return NEW GenericInstantAbility(observer, 1, card, (Damageable*)target, a1);
    }

    // neverending effect
    if (s.find("emblem ") == 0) {
        const string s1 = s.substr(7);
        MTGAbility* a1  = parseMagicLine(s1, id, spell, card);
        if (!a1) {
            return nullptr;
        }

        return NEW GenericAbilityMod(observer, 1, card->controller()->getObserver()->ExtraRules,
                                     card->controller()->getObserver()->ExtraRules, a1);
    }

    // choose a color
    const vector<string> splitChooseAColor = parseBetween(s, "activatechooseacolor ", " activatechooseend");
    if (!splitChooseAColor.empty()) {
        return parseChooseActionAbility(unchangedS, card, spell, target, restrictions, id);
    }

    // choose a type
    const vector<string> splitChooseAType = parseBetween(s, "activatechooseatype ", " activatechooseend");
    if (!splitChooseAType.empty()) {
        return parseChooseActionAbility(unchangedS, card, spell, target, restrictions, id);
    }

    // Upkeep Cost
    found = s.find("upcostmulti");
    if (found != string::npos) {
        return AbilityFactory::parseUpkeepAbility(s, card, spell, restrictions, id);
    }
    // Phase based actions
    found = s.find("phaseactionmulti");
    if (found != string::npos) {
        return parsePhaseActionAbility(s, card, spell, target, restrictions, id);
    }

    int forcedalive = 0;
    // force an ability to ignore destroy while source is still valid.
    // allow the lords to remove the ability instead of gameobserver checks.
    found = s.find("forcedalive");
    if (found != string::npos) {
        forcedalive = 1;
    }
    bool neverRemove = false;
    found            = s.find("dontremove");
    if (found != string::npos) {
        neverRemove = true;
    }
    // rather dirty way to stop thises and lords from conflicting with each other.
    size_t lord = string::npos;
    for (size_t j = 0; j < kLordKeywordsCount; ++j) {
        const size_t found2 = s.find(kLordKeywords[j]);
        if (found2 != string::npos && ((found == string::npos) || found2 < found)) {
            lord = found2;
        }
    }

    // This, ThisForEach;
    found = string::npos;
    int i = -1;
    for (size_t j = 0; j < kThisKeywordsCount; ++j) {
        const size_t found2 = s.find(kThisKeywords[j]);
        if (found2 != string::npos && ((found == string::npos) || found2 < found)) {
            found = found2;
            i     = j;
        }
    }
    if (found != string::npos && found < lord) {
        // why does tc even exist here? This shouldn't happen...
        SAFE_DELETE(tc);  // http://code.google.com/p/wagic/issues/detail?id=424

        const size_t header = kThisKeywords[i].size();
        const size_t end    = s.find(')', found + header);
        string s1;
        if (found == 0 || end != s.size() - 1) {
            s1 = s.substr(end + 1);
        } else {
            s1 = s.substr(0, found);
        }
        if (end != string::npos) {
            const string thisDescriptorString = s.substr(found + header, end - found - header);
            ThisDescriptorFactory tdf;
            ThisDescriptor* td = tdf.createThisDescriptor(observer, thisDescriptorString);

            if (!td) {
                DebugTrace("MTGABILITY: Parsing Error:" << s);
                return nullptr;
            }

            MTGAbility* a = parseMagicLine(s1, id, spell, card, 0, activated);
            if (!a) {
                SAFE_DELETE(td);
                return nullptr;
            }

            MTGAbility* result = nullptr;
            bool oneShot       = false;
            found              = s.find(" oneshot");
            if (found != string::npos || activated || card->hasType(Subtypes::TYPE_SORCERY) ||
                card->hasType(Subtypes::TYPE_INSTANT) || a->oneShot) {
                oneShot = true;
            }
            found = s.find("while ");
            if (found != string::npos) {
                oneShot = false;
            }

            Damageable* _target = nullptr;
            if (spell) {
                _target = spell->getNextDamageableTarget();
            }
            if (!_target) {
                _target = target;
            }

            switch (i) {
            case 0:
                result = NEW AThis(observer, id, card, _target, td, a);
                break;
            case 1:
                result = NEW AThisForEach(observer, id, card, _target, td, a);
                break;
            default:
                result = nullptr;
            }
            if (result) {
                result->oneShot = oneShot;
                a->forcedAlive  = forcedalive;
                if (neverRemove) {
                    result->forceDestroy = -1;
                    result->forcedAlive  = 1;
                    a->forceDestroy      = -1;
                    a->forcedAlive       = 1;
                }
            }
            return result;
        }
        return nullptr;
    }

    // Multiple abilities for ONE cost
    found = s.find("&&");
    if (found != string::npos) {
        SAFE_DELETE(tc);
        vector<string> multiEffects = ::woth::split(s, '&');
        auto* multi                 = NEW MultiAbility(observer, id, card, target, nullptr);
        for (unsigned int i = 0; i < multiEffects.size(); i++) {
            if (!multiEffects[i].empty()) {
                MTGAbility* addAbility = parseMagicLine(multiEffects[i], id, spell, card, activated);
                multi->Add(addAbility);
            }
        }
        multi->oneShot = 1;
        return multi;
    }

    // Lord, foreach, aslongas

    found = string::npos;
    i     = -1;
    for (size_t j = 0; j < kLordKeywordsCount; ++j) {
        const size_t found2 = s.find(kLordKeywords[j]);
        if (found2 != string::npos && ((found == string::npos) || found2 < found)) {
            found = found2;
            i     = j;
        }
    }
    if (found != string::npos) {
        SAFE_DELETE(tc);
        const size_t header = kLordKeywords[i].size();
        const size_t end    = s.find(')', found + header);
        string s1;
        if (found == 0 || end != s.size() - 1) {
            s1 = s.substr(end + 1);
        } else {
            s1 = s.substr(0, found);
        }
        if (end != string::npos) {
            int lordIncludeSelf = 1;
            const size_t other  = s1.find(" other");
            if (other != string::npos) {
                lordIncludeSelf = 0;
                s1.replace(other, 6, "");
            }
            const string lordTargetsString = s.substr(found + header, end - found - header);
            TargetChooserFactory tcf(observer);
            TargetChooser* lordTargets = tcf.createTargetChooser(lordTargetsString, card);

            if (!lordTargets) {
                DebugTrace("MTGABILITY: Parsing Error: " << s);
                return nullptr;
            }

            MTGAbility* a = parseMagicLine(s1, id, spell, card, false,
                                           activated);  // activated lords usually force an end of turn ability
            if (!a) {
                SAFE_DELETE(lordTargets);
                return nullptr;
            }
            MTGAbility* result = nullptr;
            bool oneShot       = false;
            if (activated || i == 4 || a->oneShot) {
                oneShot = true;
            }
            if (card->hasType(Subtypes::TYPE_SORCERY) || card->hasType(Subtypes::TYPE_INSTANT)) {
                oneShot = true;
            }
            found = s.find("while ");
            if (found != string::npos) {
                oneShot = false;
            }
            found = s.find(" oneshot");
            if (found != string::npos) {
                oneShot = true;
            }
            Damageable* _target = nullptr;
            if (spell) {
                _target = spell->getNextDamageableTarget();
            }
            if (!_target) {
                _target = target;
            }

            int mini         = 0;
            int maxi         = 0;
            bool miniFound   = false;
            bool maxiFound   = false;
            bool compareZone = false;

            found = s.find(" >");
            if (found != string::npos) {
                mini      = atoi(s.substr(found + 2, 3).c_str());
                miniFound = true;
            }

            found = s.find(" <");
            if (found != string::npos) {
                maxi      = atoi(s.substr(found + 2, 3).c_str());
                maxiFound = true;
            }

            found = s.find("compare");
            if (found != string::npos) {
                compareZone = true;
            }

            switch (i) {
            case 0:
                result = NEW ALord(observer, id, card, lordTargets, lordIncludeSelf, a);
                break;
            case 1:
                result = NEW AForeach(observer, id, card, _target, lordTargets, lordIncludeSelf, a, mini, maxi);
                break;
            case 2: {
                if (!miniFound && !maxiFound)  // for code without an operator treat as a mini.
                {
                    miniFound = true;
                }
                result = NEW AAsLongAs(observer, id, card, _target, lordTargets, lordIncludeSelf, a, mini, maxi,
                                       miniFound, maxiFound, compareZone);
            } break;
            case 3:
                result = NEW ATeach(observer, id, card, lordTargets, lordIncludeSelf, a);
                break;
            case 4:
                result = NEW ALord(observer, id, card, lordTargets, lordIncludeSelf, a);
                break;
            default:
                result = nullptr;
            }
            if (result) {
                result->oneShot = oneShot;
                a->forcedAlive  = forcedalive;
                if (neverRemove) {
                    result->oneShot      = false;
                    result->forceDestroy = -1;
                    result->forcedAlive  = 1;
                    a->forceDestroy      = -1;
                    a->forcedAlive       = 1;
                }
            }
            return result;
        }
        return nullptr;
    }

    if (!activated && tc) {
        MTGAbility* a = parseMagicLine(sWithoutTc, id, spell, card);
        if (!a) {
            DebugTrace("ABILITYFACTORY Error parsing: " << s);
            return nullptr;
        }
        a = NEW GenericTargetAbility(observer, newName, castRestriction, id, card, tc, a);
        return NEW MayAbility(observer, id, a, card, true);
    }

    SAFE_DELETE(tc);

    // dynamic ability builder
    vector<string> splitDynamic = parseBetween(s, "dynamicability<!", "!>");
    if (!splitDynamic.empty()) {
        const string s1  = splitDynamic[1];
        int type         = 0;
        int effect       = 0;
        int who          = 0;
        int amountsource = 0;

        // source
        for (size_t i = 0; i < sizeof(kDynamicSourceIds) / sizeof(kDynamicSourceIds[0]); ++i) {
            const size_t abilityamountsource = s1.find(kDynamicSourceKeywords[i]);
            if (abilityamountsource != string::npos) {
                amountsource = kDynamicSourceIds[i];
                break;
            }
        }

        // main variable or type
        for (size_t i = 0; i < sizeof(kDynamicTypeIds) / sizeof(kDynamicTypeIds[0]); ++i) {
            const size_t abilitytype = s1.find(kDynamicTypeKeywords[i]);
            if (abilitytype != string::npos) {
                type = kDynamicTypeIds[i];
                break;
            }
        }

        // effect
        for (size_t i = 0; i < sizeof(kDynamicEffectIds) / sizeof(kDynamicEffectIds[0]); ++i) {
            const size_t abilityeffect = s1.find(kDynamicEffectKeywords[i]);
            if (abilityeffect != string::npos) {
                effect = kDynamicEffectIds[i];
                break;
            }
        }

        // target
        for (size_t i = 0; i < sizeof(kDynamicWhoIds) / sizeof(kDynamicWhoIds[0]); ++i) {
            const size_t abilitywho = s1.find(kDynamicWhoKeywords[i]);
            if (abilitywho != string::npos) {
                who = kDynamicWhoIds[i];
                break;
            }
        }

        const string sAbility = splitDynamic[2];
        MTGAbility* stored    = nullptr;
        if (!sAbility.empty()) {
            stored = parseMagicLine(sAbility, id, spell, card);
        }

        MTGAbility* a = NEW AADynamic(observer, id, card, target, type, effect, who, amountsource, stored);
        a->oneShot    = 1;
        return a;
    }

    // Phase based actions
    found = s.find("phaseaction");
    if (found != string::npos) {
        return parsePhaseActionAbility(s, card, spell, target, restrictions, id);
    }

    // flip a coin
    vector<string> splitFlipCoin = parseBetween(s, "flipacoin ", " flipend");
    if (!splitFlipCoin.empty()) {
        const string a1     = splitFlipCoin[1];
        MTGAbility* a       = NEW GenericFlipACoin(observer, id, card, target, a1);
        a->oneShot          = 1;
        a->canBeInterrupted = false;
        return a;
    }

    // Upkeep Cost
    found = s.find("upcost");
    if (found != string::npos) {
        return AbilityFactory::parseUpkeepAbility(s, card, spell, restrictions, id);
    }

    // ninjutsu
    found = s.find("ninjutsu");
    if (found != string::npos) {
        MTGAbility* a = NEW ANinja(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // combat removal
    found = s.find("removefromcombat");
    if (found != string::npos) {
        MTGAbility* a = NEW ACombatRemoval(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // momentary blink
    found = s.find("(blink)");
    if (found != string::npos) {
        const bool ueoteffect      = (s.find("(blink)ueot") != string::npos);
        const bool forsource       = (s.find("(blink)forsrc") != string::npos);
        const bool blinkhand       = (s.find("hand(blink)") != string::npos);
        const size_t returnAbility = s.find("return(");
        const string sAbility      = s.substr(returnAbility + 7);
        MTGAbility* stored         = nullptr;
        if (!sAbility.empty()) {
            stored = parseMagicLine(sAbility, id, spell, card);
        }
        MTGAbility* a = NEW ABlinkGeneric(observer, id, card, target, ueoteffect, forsource, blinkhand, stored);
        a->oneShot    = 1;
        return a;
    }

    // Fizzle (counterspell...)
    found = s.find("fizzle");
    if (found != string::npos) {
        Spell* starget = nullptr;
        if (spell) {
            starget = spell->getNextSpellTarget();
        }
        MTGAbility* a = NEW AAFizzler(observer, id, card, starget);
        a->oneShot    = 1;
        return a;
    }

    // Describes a player target in many abilities
    int who = TargetChooser::UNSET;
    if (s.find(" controller") != string::npos) {
        who = TargetChooser::CONTROLLER;
    }
    if (s.find(" opponent") != string::npos) {
        who = TargetChooser::OPPONENT;
    }
    if (s.find(" targetcontroller") != string::npos) {
        who = TargetChooser::TARGET_CONTROLLER;
    }
    if (s.find(" targetedplayer") != string::npos) {
        who = TargetChooser::TARGETED_PLAYER;
    }
    if (s.find(" owner") != string::npos) {
        who = TargetChooser::OWNER;
    }

    // ability creator the target has to do the ability.
    if (s.find("ability$") != string::npos) {
        if (!storedAbilityString.empty()) {
            auto* abl =
                NEW ATargetedAbilityCreator(observer, id, card, target, nullptr, newName, storedAbilityString, who);
            abl->oneShot = 1;
            storedString.clear();
            return abl;
        }
    }
    // livingweapon (used for token below)
    const bool aLivingWeapon = (s.find("livingweapon") != string::npos);

    // Token creator. Name, type, p/t, abilities
    vector<string> splitToken = parseBetween(s, "token(", ")");
    if (!splitToken.empty()) {
        WParsedInt* multiplier = nullptr;
        const size_t star      = s.find('*');
        string starfound;
        if (star != string::npos) {
            starfound            = s.substr(star + 1);
            const size_t starEnd = starfound.find_first_of(' ');
            starfound            = starfound.substr(0, starEnd);
            multiplier           = NEW WParsedInt(starfound, spell, card);
        }

        const int tokenId = atoi(splitToken[1].c_str());
        if (tokenId) {
            MTGCard* safetycard = MTGCollection()->getCardById(tokenId);
            if (!safetycard) {  // Error, card not foudn in DB
                return NEW ATokenCreator(observer, id, card, target, nullptr, "ID NOT FOUND", "ERROR ID", 0, 0, "", "",
                                         nullptr, 0);
            }

            auto* tok    = NEW ATokenCreator(observer, id, card, target, nullptr, tokenId, starfound, multiplier, who);
            tok->oneShot = 1;
            return tok;
        }

        const string tokenDesc         = splitToken[1];
        vector<string> tokenParameters = ::woth::split(tokenDesc, ',');
        if (tokenParameters.size() < 3) {
            DebugTrace("incorrect Parameters for Token" << tokenDesc);
            return nullptr;
        }
        const string sname  = tokenParameters[0];
        const string stypes = tokenParameters[1];
        const string spt    = tokenParameters[2];

        // reconstructing string abilities from the split version,
        //  then we re-split it again in the token constructor,
        //  this needs to be improved
        string sabilities = (tokenParameters.size() > 3) ? tokenParameters[3] : "";
        for (size_t i = 4; i < tokenParameters.size(); ++i) {
            sabilities.append(",");
            sabilities.append(tokenParameters[i]);
        }
        int value = 0;
        if (spt.find("xx/xx") != string::npos) {
            value = card->X / 2;
        } else if (spt.find("x/x") != string::npos) {
            value = card->X;
        }

        int power;
        int toughness;
        parsePowerToughness(spt, &power, &toughness);

        auto* tok    = NEW ATokenCreator(observer, id, card, target, nullptr, sname, stypes, power + value,
                                         toughness + value, sabilities, starfound, multiplier, who, aLivingWeapon, spt);
        tok->oneShot = 1;
        if (aLivingWeapon) {
            tok->forceDestroy = 1;
        }
        return tok;
    }

    // Equipment
    found = s.find("equip");
    if (found != string::npos) {
        return NEW AEquip(observer, id, card);
    }

    // Equipment (attach)
    found = s.find("attach");
    if (found != string::npos) {
        return NEW AEquip(observer, id, card, nullptr, ActivatedAbility::NO_RESTRICTION);
    }

    // MoveTo Move a card from a zone to another
    vector<string> splitMove = parseBetween(s, "moveto(", ")");
    if (!splitMove.empty()) {
        // hack for http://code.google.com/p/wagic/issues/detail?id=120
        // We assume that auras don't move their own target...
        if (card->hasType(Subtypes::TYPE_AURA)) {
            target = card;
        }

        MTGAbility* a = NEW AAMover(observer, id, card, target, splitMove[1]);
        a->oneShot    = true;
        if (!storedAndAbility.empty()) {
            const string stored = storedAndAbility;
            storedAndAbility.clear();
            (dynamic_cast<AAMover*>(a))->andAbility = parseMagicLine(stored, id, spell, card);
        }
        return a;
    }

    // random mover
    vector<string> splitRandomMove = parseBetween(s, "moverandom(", ")");
    if (!splitRandomMove.empty()) {
        vector<string> splitfrom = parseBetween(splitRandomMove[2], "from(", ")");
        vector<string> splitto   = parseBetween(splitRandomMove[2], "to(", ")");
        if (splitfrom.empty() || splitto.empty()) {
            return nullptr;
        }
        MTGAbility* a = NEW AARandomMover(observer, id, card, target, splitRandomMove[1], splitfrom[1], splitto[1]);
        a->oneShot    = true;
        return a;
    }

    // put a card on bottom of library
    found = s.find("bottomoflibrary");
    if (found != string::npos) {
        MTGAbility* a = NEW AALibraryBottom(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // Copy a target
    found = s.find("copy");
    if (found != string::npos) {
        MTGAbility* a = NEW AACopier(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // imprint
    found = s.find("phaseout");
    if (found != string::npos) {
        MTGAbility* a = NEW AAPhaseOut(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // clone
    found = s.find("clone");
    if (found != string::npos) {
        string with;
        string types;
        vector<string> splitWith = parseBetween(s, "with(", ")");
        if (!splitWith.empty()) {
            with = splitWith[1];
        }
        vector<string> splitTypes = parseBetween(s, "addtype(", ")");
        if (!splitTypes.empty()) {
            types = splitTypes[1];
        }
        MTGAbility* a = NEW AACloner(observer, id, card, target, nullptr, who, with, types);
        a->oneShot    = 1;
        return a;
    }

    // Bury, destroy, sacrifice, reject(discard)
    if (s.find("bury") != string::npos) {
        MTGAbility* a = NEW AABuryCard(observer, id, card, target);
        a->oneShot    = 1;
        if (!storedAndAbility.empty()) {
            const string stored = storedAndAbility;
            storedAndAbility.clear();
            (dynamic_cast<AABuryCard*>(a))->andAbility = parseMagicLine(stored, id, spell, card);
        }
        return a;
    }

    if (s.find("destroy") != string::npos) {
        MTGAbility* a = NEW AADestroyCard(observer, id, card, target);
        a->oneShot    = 1;
        if (!storedAndAbility.empty()) {
            const string stored = storedAndAbility;
            storedAndAbility.clear();
            (dynamic_cast<AADestroyCard*>(a))->andAbility = parseMagicLine(stored, id, spell, card);
        }
        return a;
    }

    if (s.find("sacrifice") != string::npos) {
        MTGAbility* a = NEW AASacrificeCard(observer, id, card, target);
        a->oneShot    = 1;
        if (!storedAndAbility.empty()) {
            const string stored = storedAndAbility;
            storedAndAbility.clear();
            (dynamic_cast<AASacrificeCard*>(a))->andAbility = parseMagicLine(stored, id, spell, card);
        }
        return a;
    }

    if (s.find("reject") != string::npos) {
        MTGAbility* a = NEW AADiscardCard(observer, id, card, target);
        a->oneShot    = 1;
        if (!storedAndAbility.empty()) {
            const string stored = storedAndAbility;
            storedAndAbility.clear();
            (dynamic_cast<AADiscardCard*>(a))->andAbility = parseMagicLine(stored, id, spell, card);
        }
        return a;
    }
    bool oneShot      = false;
    bool forceForever = false;
    found             = s.find("ueot");
    if (found != string::npos) {
        forceUEOT = true;
    }
    found = s.find("oneshot");
    if (found != string::npos) {
        oneShot = true;
    }
    found = s.find("forever");
    if (found != string::npos) {
        forceForever = true;
    }
    // Prevent Damage
    const string preventDamageKeywords[]   = {"preventallcombatdamage", "preventallnoncombatdamage", "preventalldamage",
                                              "fog"};
    const int preventDamageTypes[]         = {0, 2, 1, 0};  // TODO enum ?
    const bool preventDamageForceOneShot[] = {false, false, false, true};

    for (size_t i = 0; i < sizeof(preventDamageTypes) / sizeof(preventDamageTypes[0]); ++i) {
        found = s.find(preventDamageKeywords[i]);
        if (found != string::npos) {
            string to;
            string from;

            vector<string> splitTo = parseBetween(s, "to(", ")");
            if (!splitTo.empty()) {
                to = splitTo[1];
            }

            vector<string> splitFrom = parseBetween(s, "from(", ")");
            if (!splitFrom.empty()) {
                from = splitFrom[1];
            }

            MTGAbility* ab;
            if (forceUEOT || preventDamageForceOneShot[i]) {
                ab = NEW APreventDamageTypesUEOT(observer, id, card, to, from, preventDamageTypes[i]);
            } else {
                ab = NEW APreventDamageTypes(observer, id, card, to, from, preventDamageTypes[i]);
            }

            if (preventDamageForceOneShot[i]) {
                ab->oneShot = 1;
            }

            return ab;
        }
    }

    // Reset damages on cards
    found = s.find("resetdamage");
    if (found != string::npos) {
        MTGAbility* a = NEW AAResetDamage(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // Damage
    vector<string> splitDamage = parseBetween(s, "damage:", " ", false);
    if (!splitDamage.empty()) {
        Targetable* t = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a = NEW AADamager(observer, id, card, t, splitDamage[1], nullptr, who);
        a->oneShot    = 1;
        return a;
    }

    // remove poison
    vector<string> splitPoison = parseBetween(s, "alterpoison:", " ", false);
    if (!splitPoison.empty()) {
        const int poison = atoi(splitPoison[1].c_str());
        Targetable* t    = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a    = NEW AAAlterPoison(observer, id, card, t, poison, nullptr, who);
        a->oneShot       = 1;
        return a;
    }

    // prevent next damage
    vector<string> splitPrevent = parseBetween(s, "prevent:", " ", false);
    if (!splitPrevent.empty()) {
        const int preventing = atoi(splitPrevent[1].c_str());
        Targetable* t        = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a        = NEW AADamagePrevent(observer, id, card, t, preventing, nullptr, who);
        a->oneShot           = 1;
        return a;
    }

    // set hand size
    vector<string> splitSetHand = parseBetween(s, "sethand:", " ", false);
    if (!splitSetHand.empty()) {
        const int hand = atoi(splitSetHand[1].c_str());
        Damageable* t  = spell ? spell->getNextDamageableTarget() : nullptr;
        MTGAbility* a  = NEW AASetHand(observer, id, card, t, hand, nullptr, who);
        a->oneShot     = 1;
        return a;
    }

    // set life total
    vector<string> splitLifeset = parseBetween(s, "lifeset:", " ", false);
    if (!splitLifeset.empty()) {
        auto* life    = NEW WParsedInt(splitLifeset[1], spell, card);
        Damageable* t = spell ? spell->getNextDamageableTarget() : nullptr;
        MTGAbility* a = NEW AALifeSet(observer, id, card, t, life, nullptr, who);
        a->oneShot    = 1;
        return a;
    }

    // gain/lose life
    vector<string> splitLife = parseBetween(s, "life:", " ", false);
    if (!splitLife.empty()) {
        Targetable* t = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a = NEW AALifer(observer, id, card, t, splitLife[1], nullptr, who);
        a->oneShot    = 1;
        return a;
    }

    // Win the game
    found = s.find("wingame");
    if (found != string::npos) {
        Damageable* d = spell ? spell->getNextDamageableTarget() : nullptr;
        MTGAbility* a = NEW AAWinGame(observer, id, card, d, nullptr, who);
        a->oneShot    = 1;
        return a;
    }

    // Draw
    vector<string> splitDraw = parseBetween(s, "draw:", " ", false);
    if (!splitDraw.empty()) {
        Targetable* t = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a = NEW AADrawer(observer, id, card, t, nullptr, splitDraw[1], who);
        a->oneShot    = 1;
        return a;
    }

    // Deplete
    vector<string> splitDeplete = parseBetween(s, "deplete:", " ", false);
    if (!splitDeplete.empty()) {
        Targetable* t = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a = NEW AADepleter(observer, id, card, t, splitDeplete[1], nullptr, who);
        a->oneShot    = 1;
        return a;
    }

    // modify turns
    vector<string> splitModTurn = parseBetween(s, "turns:", " ", false);
    if (!splitModTurn.empty()) {
        Targetable* t = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a = NEW AAModTurn(observer, id, card, t, splitModTurn[1], nullptr, who);
        a->oneShot    = 1;
        return a;
    }

    // Shuffle
    found = s.find("shuffle");
    if (found != string::npos) {
        Targetable* t = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a = NEW AAShuffle(observer, id, card, t, nullptr, who);
        a->oneShot    = 1;
        return a;
    }

    // Remove Mana from ManaPool
    vector<string> splitRemove = parseBetween(s, "removemana(", ")");
    if (!splitRemove.empty()) {
        Targetable* t = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a = NEW AARemoveMana(observer, id, card, t, splitRemove[1], who);
        a->oneShot    = 1;
        return a;
    }

    // Lose subtypes of a given type
    vector<string> splitLoseTypes = parseBetween(s, "losesubtypesof(", ")");
    if (!splitLoseTypes.empty()) {
        const int parentType = MTGAllCards::findType(splitLoseTypes[1]);
        return NEW ALoseSubtypes(observer, id, card, target, parentType);
    }

    // Cast/Play Restrictions
    for (size_t i = 0; i < kMaxCastKeywordsCount; ++i) {
        vector<string> splitCast = parseBetween(s, kMaxCastKeywords[i], ")");
        if (!splitCast.empty()) {
            TargetChooserFactory tcf(observer);
            TargetChooser* castTargets = tcf.createTargetChooser(splitCast[1], card);

            vector<string> splitValue = parseBetween(splitCast[2], "", " ", false);
            if (splitValue.empty()) {
                DebugTrace("MTGABILITY: Error parsing Cast/Play Restriction" << s);
                return nullptr;
            }

            const string valueStr     = splitValue[1];
            const bool modifyExisting = (valueStr.find('+') != string::npos || valueStr.find('-') != string::npos);

            auto* value   = NEW WParsedInt(valueStr, spell, card);
            Targetable* t = spell ? spell->getNextTarget() : nullptr;
            if (card->hasType(Subtypes::TYPE_INSTANT) || card->hasType(Subtypes::TYPE_SORCERY) || forceUEOT) {
                return NEW AInstantCastRestrictionUEOT(observer, id, card, t, castTargets, value, modifyExisting,
                                                       kMaxCastZones[i], who);
            }
            return NEW ACastRestriction(observer, id, card, t, castTargets, value, modifyExisting, kMaxCastZones[i],
                                        who);
        }
    }

    // Discard
    vector<string> splitDiscard = parseBetween(s, "discard:", " ", false);
    if (!splitDiscard.empty()) {
        Targetable* t = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a = NEW AARandomDiscarder(observer, id, card, t, splitDiscard[1], nullptr, who);
        a->oneShot    = 1;
        return a;
    }

    // rampage
    vector<string> splitRampage = parseBetween(s, "rampage(", ")");
    if (!splitRampage.empty()) {
        vector<string> rampageParameters = ::woth::split(splitRampage[1], ',');
        int power;
        int toughness;
        if (!parsePowerToughness(rampageParameters[0], &power, &toughness)) {
            DebugTrace("MTGAbility Parse error in rampage" << s);
            return nullptr;
        }
        const int MaxOpponent = atoi(rampageParameters[1].c_str());
        return NEW ARampageAbility(observer, id, card, power, toughness, MaxOpponent);
    }

    // flanking
    if (s.find("flanker") != string::npos) {
        return NEW AFlankerAbility(observer, id, card);
    }

    // spirit link
    // combat damage spirit link
    if (s.find("spiritlink") != string::npos) {
        bool combatOnly = false;
        if (s.find("combatspiritlink") != string::npos) {
            combatOnly = true;
        }
        return NEW ASpiritLinkAbility(observer, id, card, combatOnly);
    }

    // bushido
    vector<string> splitBushido = parseBetween(s, "bushido(", ")");
    if (!splitBushido.empty()) {
        const string power;
        const string toughness;
        const vector<string> splitPT = ::woth::split(splitBushido[1], '/');
        if (splitPT.empty()) {
            return nullptr;
        }
        return NEW ABushidoAbility(observer, id, card, splitBushido[1]);
    }
    vector<string> splitPhaseAlter = parseBetween(s, "phasealter(", ")");
    if (!splitPhaseAlter.empty()) {
        const string power;
        const string toughness;
        vector<string> splitPhaseAlter2 = ::woth::split(splitPhaseAlter[1], ',');
        if (splitPhaseAlter2.size() < 3) {
            return nullptr;
        }
        string after;
        if (splitPhaseAlter2.size() > 3) {
            vector<string> splitPhaseAlterAfter = parseBetween(splitPhaseAlter2[3], "after<", ">");
            if (!splitPhaseAlterAfter.empty()) {
                after = splitPhaseAlterAfter[1];
            }
        }
        MTGAbility* a1 =
            NEW APhaseAlter(observer, id, card, target, splitPhaseAlter2[0].find("add") != string::npos,
                            splitPhaseAlter2[1], splitPhaseAlter2[2], s.find("nextphase") != string::npos, after);
        a1->canBeInterrupted = false;
        return NEW GenericAbilityMod(observer, 1, card, spell ? spell->getNextDamageableTarget() : (Damageable*)target,
                                     a1);
    }

    // loseAbilities
    if (s.find("loseabilities") != string::npos) {
        return NEW ALoseAbilities(observer, id, card, target);
    }

    // counter
    vector<string> splitCounter = parseBetween(s, "counter(", ")");
    if (!splitCounter.empty()) {
        const string counterString = splitCounter[1];
        Counter* counter           = parseCounter(counterString, target, spell);
        if (!counter) {
            DebugTrace("MTGAbility: can't parse counter:" << s);
            return nullptr;
        }

        MTGAbility* a = NEW AACounter(observer, id, card, target, counterString, counter->name.c_str(), counter->power,
                                      counter->toughness, counter->nb, counter->maxNb);
        delete (counter);
        a->oneShot = 1;
        return a;
    }

    // no counters on target of optional type
    vector<string> splitCounterShroud = parseBetween(s, "countershroud(", ")");
    if (!splitCounterShroud.empty()) {
        const string counterShroudString = splitCounterShroud[1];
        Counter* counter                 = nullptr;
        if (splitCounterShroud[1] == "any") {
            counter = nullptr;
        } else {
            counter = parseCounter(counterShroudString, target, spell);
            if (!counter) {
                DebugTrace("MTGAbility: can't parse counter:" << s);
                return nullptr;
            }
        }
        TargetChooser* csTc = nullptr;
        if (splitCounterShroud[2].size() > 1) {
            TargetChooserFactory af(card->getObserver());
            csTc = af.createTargetChooser(splitCounterShroud[2], card);
        }
        MTGAbility* a = NEW ACounterShroud(observer, id, card, target, csTc, counter);
        return a;
    }
    // use counters to track by counters to track an efect by counter name.
    vector<string> splitCounterTracking = parseBetween(s, "countertrack(", ")");
    if (!splitCounterTracking.empty()) {
        const string splitCounterTrack = splitCounterTracking[1];
        return NEW ACounterTracker(observer, id, card, target, splitCounterTrack);
    }
    // removes all counters of the specifified type.
    vector<string> splitRemoveCounter = parseBetween(s, "removeallcounters(", ")");
    if (!splitRemoveCounter.empty()) {
        const string counterString = splitRemoveCounter[1];
        if (counterString.find("all") != string::npos) {
            MTGAbility* a = NEW AARemoveAllCounter(observer, id, card, target, "All", 0, 0, 1, true);
            a->oneShot    = 1;
            return a;
        }

        Counter* counter = parseCounter(counterString, target, spell);
        if (!counter) {
            DebugTrace("MTGAbility: can't parse counter:" << s);
            return nullptr;
        }

        MTGAbility* a = NEW AARemoveAllCounter(observer, id, card, target, counter->name.c_str(), counter->power,
                                               counter->toughness, counter->nb, false);
        delete (counter);
        a->oneShot = 1;
        return a;
    }

    // Becomes... (animate artifact...: becomes(Creature, manacost/manacost)
    vector<string> splitBecomes = parseBetween(s, "becomes(", ")");
    if (!splitBecomes.empty()) {
        vector<string> becomesParameters = ::woth::split(splitBecomes[1], ',');
        const string stypes              = becomesParameters[0];
        string newPower;
        string newToughness;
        bool ptFound = false;
        if (becomesParameters.size() > 1) {
            vector<string> pt = ::woth::split(becomesParameters[1], '/');
            if (pt.size() > 1) {
                newPower     = pt[0];
                newToughness = pt[1];
                ptFound      = true;
            }
        }
        string sabilities;
        const unsigned int becomesSize = ptFound ? 2 : 1;
        if (becomesParameters.size() > becomesSize) {
            for (unsigned int i = becomesSize; i < becomesParameters.size(); i++) {
                sabilities.append(becomesParameters[i]);
                if (i + 1 < becomesParameters.size()) {
                    sabilities.append(",");
                }
            }
        }
        if (oneShot || forceUEOT || forceForever) {
            return NEW ATransformerInstant(observer, id, card, target, stypes, sabilities, newPower, ptFound,
                                           newToughness, ptFound, vector<string>(), false, forceForever);
        }

        return NEW ATransformer(observer, id, card, target, stypes, sabilities, newPower, ptFound, newToughness,
                                ptFound, vector<string>(), false, forceForever);
    }

    // bloodthirst
    vector<string> splitBloodthirst = parseBetween(s, "bloodthirst:", " ", false);
    if (!splitBloodthirst.empty()) {
        return NEW ABloodThirst(observer, id, card, target, atoi(splitBloodthirst[1].c_str()));
    }

    // Vanishing
    vector<string> splitVanishing = parseBetween(s, "vanishing:", " ", false);
    if (!splitVanishing.empty()) {
        return NEW AVanishing(observer, id, card, nullptr, restrictions, atoi(splitVanishing[1].c_str()), "time");
    }

    // Fading
    vector<string> splitFading = parseBetween(s, "fading:", " ", false);
    if (!splitFading.empty()) {
        return NEW AVanishing(observer, id, card, nullptr, restrictions, atoi(splitFading[1].c_str()), "fade");
    }

    // Alter cost
    if (s.find("altercost(") != string::npos) {
        return getManaReduxAbility(s.substr(s.find("altercost(") + 10), id, spell, card, target);
    }

    // transform....(hivestone,living enchantment)
    // TODO: cleanup this block, it's a rats nest
    found = s.find("transforms((");
    if (found != string::npos) {
        string extraTransforms;
        string transformsParamsString;
        transformsParamsString.append(storedString);  // the string between found and real end is removed at start.

        found = transformsParamsString.find("transforms((");
        if (found != string::npos && extraTransforms.empty()) {
            const size_t real_end         = transformsParamsString.find("))", found);
            const size_t stypesStartIndex = found + 12;
            extraTransforms.append(transformsParamsString.substr(stypesStartIndex, real_end - stypesStartIndex));
            transformsParamsString.erase(stypesStartIndex, real_end - stypesStartIndex);
        }
        vector<string> effectParameters = ::woth::split(transformsParamsString, ',');
        const string stypes             = effectParameters[0];

        const string sabilities = transformsParamsString.substr(stypes.length());
        bool newpowerfound      = false;
        string newpower;
        bool newtoughnessfound = false;
        string newtoughness;
        vector<string> abilities = ::woth::split(sabilities, ',');
        bool newAbilityFound     = false;
        vector<string> newAbilitiesList;
        storedString.erase();
        storedString.append(extraTransforms);
        extraTransforms.erase();
        for (unsigned int i = 0; i < abilities.size(); i++) {
            if (abilities[i].empty()) {
                abilities.erase(abilities.begin() + i);
            }
        }
        for (unsigned int j = 0; j < abilities.size(); j++) {
            vector<string> splitPower = parseBetween(abilities[j], "setpower=", ",", false);
            if (!splitPower.empty()) {
                newpowerfound = true;
                newpower      = splitPower[1];
            }
            vector<string> splitToughness = parseBetween(abilities[j], "settoughness=", ",", false);
            if (!splitToughness.empty()) {
                newtoughnessfound = true;
                newtoughness      = splitToughness[1];
            }
            if (abilities[j].find("newability[") != string::npos) {
                const size_t NewSkill     = abilities[j].find("newability[");
                const size_t NewSkillEnd  = abilities[j].find_last_of(']');
                const string newAbilities = abilities[j].substr(NewSkill + 11, NewSkillEnd - NewSkill - 11);
                newAbilitiesList.push_back(newAbilities);
                newAbilityFound = true;
            }
        }

        if (oneShot || forceUEOT || forceForever) {
            return NEW ATransformerInstant(observer, id, card, target, stypes, sabilities, newpower, newpowerfound,
                                           newtoughness, newtoughnessfound, newAbilitiesList, newAbilityFound,
                                           forceForever);
        }

        return NEW ATransformer(observer, id, card, target, stypes, sabilities, newpower, newpowerfound, newtoughness,
                                newtoughnessfound, newAbilitiesList, newAbilityFound);
    }

    // flip
    vector<string> splitFlipStat = parseBetween(s, "flip(", ")", true);
    if (!splitFlipStat.empty()) {
        string flipStats;
        if (!splitFlipStat[1].empty()) {
            /*vector<string>FlipStats = ::woth::split(splitFlipStat[1],'%');*/
            flipStats = splitFlipStat[1];
        }
        MTGAbility* a = NEW AAFlip(observer, id, card, target, flipStats);
        return a;
    }

    // Change Power/Toughness
    auto* wppt     = NEW WParsedPT(s, spell, card);
    bool nonstatic = false;
    if (wppt->ok) {
        if (s.find("nonstatic") != string::npos) {
            nonstatic = true;
        }
        if (!activated) {
            if (card->hasType(Subtypes::TYPE_INSTANT) || card->hasType(Subtypes::TYPE_SORCERY) || forceUEOT) {
                return NEW PTInstant(observer, id, card, target, wppt, s, nonstatic);
            }
            return NEW APowerToughnessModifier(observer, id, card, target, wppt, s, nonstatic);
        }
        return NEW PTInstant(observer, id, card, target, wppt, s, nonstatic);
    }
    delete wppt;

    // Mana Producer
    found = s.find("add");
    if (found != string::npos) {
        ManaCost* output = ManaCost::parseManaCost(s.substr(found), nullptr, card);
        Targetable* t    = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a    = NEW AManaProducer(observer, id, card, t, output, nullptr, who, s.substr(found));
        a->oneShot       = 1;
        if (!newName.empty()) {
            (dynamic_cast<AManaProducer*>(a))->menutext = newName;
        }
        return a;
    }

    // Protection from...
    vector<string> splitProtection = parseBetween(s, "protection from(", ")");
    if (!splitProtection.empty()) {
        TargetChooserFactory tcf(observer);
        TargetChooser* fromTc = tcf.createTargetChooser(splitProtection[1], card);
        if (!fromTc) {
            return nullptr;
        }
        fromTc->setAllZones();
        if (!activated) {
            if (card->hasType(Subtypes::TYPE_INSTANT) || card->hasType(Subtypes::TYPE_SORCERY) || forceUEOT) {
                MTGAbility* aPF = NEW AProtectionFrom(observer, id, card, target, fromTc, splitProtection[1]);
                return NEW GenericInstantAbility(observer, 1, card, (Damageable*)target, aPF);
            }
            return NEW AProtectionFrom(observer, id, card, target, fromTc, splitProtection[1]);
        }
        return nullptr;  // TODO
    }

    // targetter can not target...
    vector<string> splitCantTarget = parseBetween(s, "cantbetargetof(", ")");
    if (!splitCantTarget.empty()) {
        TargetChooserFactory tcf(observer);
        TargetChooser* fromTc = tcf.createTargetChooser(splitCantTarget[1], card);
        if (!fromTc) {
            return nullptr;
        }
        fromTc->setAllZones();
        if (!activated) {
            if (card->hasType(Subtypes::TYPE_INSTANT) || card->hasType(Subtypes::TYPE_SORCERY) || forceUEOT) {
                return nullptr;  // TODO
            }
            return NEW ACantBeTargetFrom(observer, id, card, target, fromTc);
        }
        return nullptr;  // TODO
    }

    // Can't be blocked by...
    vector<string> splitCantBlock = parseBetween(s, "cantbeblockedby(", ")");
    if (!splitCantBlock.empty()) {
        TargetChooserFactory tcf(observer);
        TargetChooser* fromTc = tcf.createTargetChooser(splitCantBlock[1], card);
        if (!fromTc) {
            return nullptr;
        }
        // default target zone to opponentbattlefield here?
        if (!activated) {
            if (card->hasType(Subtypes::TYPE_INSTANT) || card->hasType(Subtypes::TYPE_SORCERY) || forceUEOT) {
                return nullptr;  // TODO
            }
            return NEW ACantBeBlockedBy(observer, id, card, target, fromTc);
        }
        return nullptr;  // TODO
    }

    // affinity based on targetchooser
    vector<string> splitNewAffinity = parseBetween(s, "affinity(", ")");
    if (!splitNewAffinity.empty()) {
        const string tcString = splitNewAffinity[1];
        string manaString;
        vector<string> splitNewAffinityMana = parseBetween(splitNewAffinity[2], "reduce(", ")");
        if (!splitNewAffinityMana.empty()) {
            manaString = splitNewAffinityMana[1];
        }
        if (manaString.empty()) {
            return nullptr;
        }
        return NEW ANewAffinity(observer, id, card, tcString, manaString);
    }

    // proliferate
    found = s.find("proliferate");
    if (found != string::npos) {
        MTGAbility* a       = NEW AAProliferate(observer, id, card, target);
        a->oneShot          = 1;
        a->canBeInterrupted = false;
        return a;
    }

    // frozen, next untap this does not untap.
    found = s.find("frozen");
    if (found != string::npos) {
        MTGAbility* a = NEW AAFrozen(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // get a new target
    if ((s.find("retarget") != string::npos) || s.find("newtarget") != string::npos) {
        MTGAbility* a = NEW AANewTarget(observer, id, card, target, (s.find("retarget") != string::npos));
        a->oneShot    = 1;
        return a;
    }

    // morph
    found = s.find("morph");
    if (found != string::npos) {
        MTGAbility* a = NEW AAMorph(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // identify what a leveler creature will max out at.
    vector<string> splitMaxlevel = parseBetween(s, "maxlevel:", " ", false);
    if (!splitMaxlevel.empty()) {
        MTGAbility* a = NEW AAWhatsMax(observer, id, card, card, nullptr, atoi(splitMaxlevel[1].c_str()));
        a->oneShot    = 1;
        return a;
    }

    // switch targest power with toughness
    found = s.find("swap");
    if (found != string::npos) {
        MTGAbility* a = NEW ASwapPTUEOT(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }
    // exchange life with target; if creature then toughness is life.
    found = s.find("exchangelife");
    if (found != string::npos) {
        Targetable* t = spell ? spell->getNextTarget() : nullptr;
        MTGAbility* a = NEW AAExchangeLife(observer, id, card, t, nullptr, who);
        a->oneShot    = 1;
        return a;
    }

    // Regeneration
    found = s.find("regenerate");
    if (found != string::npos) {
        MTGAbility* a = NEW AStandardRegenerate(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // Gain/loose simple Ability
    for (int j = 0; j < Constants::NB_BASIC_ABILITIES; j++) {
        found = s.find(Constants::MTGBasicAbilities[j]);
        if (found == 0 || found == 1) {
            int modifier = 0;
            if (s.find("absorb") || s.find("flanking")) {
                modifier += 1;
            } else {
                modifier = 1;
            }

            if (found > 0 && s[found - 1] == '-') {
                modifier = 0;
            } else if (found > 0 && s[found - 1] == '-' && (s.find("absorb") || s.find("flanking"))) {
                modifier -= 1;
            }

            if (!activated) {
                if (card->hasType(Subtypes::TYPE_INSTANT) || card->hasType(Subtypes::TYPE_SORCERY) || forceUEOT) {
                    return NEW AInstantBasicAbilityModifierUntilEOT(observer, id, card, target, j, modifier);
                }
                return NEW ABasicAbilityModifier(observer, id, card, target, j, modifier);
            }
            return NEW ABasicAbilityAuraModifierUntilEOT(observer, id, card, target, nullptr, j, modifier);
        }
    }

    // Untapper (Ley Druid...)
    found = s.find("untap");
    if (found != string::npos) {
        MTGAbility* a = NEW AAUntapper(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // Tapper (icy manipulator)
    found = s.find("tap");
    if (found != string::npos) {
        MTGAbility* a = NEW AATapper(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // adds the rule to destroy children if parent dies
    found = s.find("connectrule");
    if (found != string::npos) {
        observer->addObserver(NEW ParentChildRule(observer, -1));
        observer->connectRule = true;
        return nullptr;
    }
    // standard block
    found = s.find("block");
    if (found != string::npos) {
        MTGAbility* a = NEW AABlock(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    // create an association between cards.
    found = s.find("connect");
    if (found != string::npos) {
        MTGAbility* a = NEW AAConnect(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    found = s.find("steal");
    if (found != string::npos) {
        MTGAbility* a = NEW AInstantControlSteal(observer, id, card, target);
        a->oneShot    = 1;
        return a;
    }

    DebugTrace(" no matching ability found. " << s);
    return nullptr;
}

MTGAbility* AbilityFactory::parseUpkeepAbility(const string& s,
                                               MTGCardInstance* card,
                                               Spell* spell,
                                               int restrictions,
                                               int id) {
    bool Cumulative         = false;
    const size_t cumulative = s.find("cumulativeupcost");
    if (cumulative != string::npos) {
        Cumulative = true;
    }
    const size_t start     = s.find('[');
    const size_t end       = s.find(']', start);
    string s1              = s.substr(start + 1, end - start - 1);
    const size_t seperator = s1.find(';');
    int phase              = MTG_PHASE_UPKEEP;
    int once               = 0;
    if (seperator != string::npos) {
        for (int i = 0; i < NB_MTG_PHASES; i++) {
            if (s1.find("next") != string::npos) {
                once = 1;
            }

            const string phaseStr = Constants::MTGPhaseCodeNames[i];
            if (s1.find(phaseStr) != string::npos) {
                phase = PhaseRing::phaseStrToInt(phaseStr);
                break;
            }
        }
        s1 = s1.substr(0, seperator);
    }
    ManaCost* cost = ManaCost::parseManaCost(s1);

    if (!cost) {
        DebugTrace("MTGABILITY: Parsing Error: " << s);
        return nullptr;
    }

    const string sAbility = s.substr(end + 1);
    MTGAbility* a         = parseMagicLine(sAbility, id, spell, card);

    if (!a) {
        DebugTrace("MTGABILITY: Parsing Error: " << s);
        delete (cost);
        return nullptr;
    }
    return NEW AUpkeep(observer, id, card, a, cost, restrictions, phase, once, Cumulative);
    ;
}

MTGAbility* AbilityFactory::parsePhaseActionAbility(const string& s,
                                                    MTGCardInstance* card,
                                                    Spell* spell,
                                                    MTGCardInstance* target,
                                                    int restrictions,
                                                    int id) {
    vector<string> splitActions = parseBetween(s, "[", "]");
    if (splitActions.empty()) {
        DebugTrace("MTGABILITY:Parsing Error " << s);
        return nullptr;
    }
    const string s1 = splitActions[1];
    int phase       = MTG_PHASE_UPKEEP;
    for (int i = 0; i < NB_MTG_PHASES; i++) {
        const string phaseStr = Constants::MTGPhaseCodeNames[i];
        if (s1.find(phaseStr) != string::npos) {
            phase = PhaseRing::phaseStrToInt(phaseStr);
            break;
        }
    }

    const bool opponentturn = (s1.find("my") == string::npos);
    const bool myturn       = (s1.find("opponent") == string::npos);
    const bool sourceinPlay = (s1.find("sourceinplay") != string::npos);
    const bool next         = (s1.find("next") ==
                       string::npos);  // Why is this one the opposite of the two others? That's completely inconsistent
    const bool once         = (s1.find("once") != string::npos);

    MTGCardInstance* _target = nullptr;
    if (spell) {
        _target = spell->getNextCardTarget();
    }
    if (!_target) {
        _target = target;
    }
    return NEW APhaseActionGeneric(observer, id, card, _target, ::woth::trim(splitActions[2]), restrictions, phase,
                                   sourceinPlay, next, myturn, opponentturn, once);
}

MTGAbility* AbilityFactory::parseChooseActionAbility(const string& s,
                                                     MTGCardInstance* card,
                                                     Spell* spell,
                                                     MTGCardInstance* target,
                                                     int restrictions,
                                                     int id) {
    vector<string> splitChooseAColor2 = parseBetween(s, "activatechooseacolor ", " activatechooseend");
    if (!splitChooseAColor2.empty()) {
        const string a1     = splitChooseAColor2[1];
        MTGAbility* a       = NEW GenericChooseTypeColor(observer, id, card, target, a1, true);
        a->oneShot          = 1;
        a->canBeInterrupted = false;
        return a;
    }
    // choose a type
    vector<string> splitChooseAType2 = parseBetween(s, "activatechooseatype ", " activatechooseend");
    if (!splitChooseAType2.empty()) {
        const string a1 = splitChooseAType2[1];
        MTGAbility* a =
            NEW GenericChooseTypeColor(observer, id, card, target, a1, false, s.find("nonwall") != string::npos);
        a->oneShot          = 1;
        a->canBeInterrupted = false;
        return a;
    }
    // choose a color
    vector<string> splitChooseAColor = parseBetween(s, "chooseacolor ", " chooseend");
    if (!splitChooseAColor.empty()) {
        const string a1     = splitChooseAColor[1];
        MTGAbility* a       = NEW GenericChooseTypeColor(observer, id, card, target, a1, true);
        a->oneShot          = 1;
        a->canBeInterrupted = false;
        return a;
    }
    // choose a type
    vector<string> splitChooseAType = parseBetween(s, "chooseatype ", " chooseend");
    if (!splitChooseAType.empty()) {
        const string a1 = splitChooseAType[1];
        MTGAbility* a =
            NEW GenericChooseTypeColor(observer, id, card, target, a1, false, s.find("nonwall") != string::npos);
        a->oneShot          = 1;
        a->canBeInterrupted = false;
        return a;
    }
    return nullptr;
}

// Tells the AI if the ability should target itself or an ennemy
int AbilityFactory::abilityEfficiency(MTGAbility* a, Player* p, int mode, TargetChooser* tc, Targetable* target) {
    if (!a) {
        return BAKA_EFFECT_DONTKNOW;
    }

    if (auto* abi = dynamic_cast<GenericTargetAbility*>(a)) {
        if (mode == MODE_PUTINTOPLAY) {
            return BAKA_EFFECT_GOOD;
        }
        return abilityEfficiency(abi->ability, p, mode, abi->getActionTc());
    }
    if (auto* abi = dynamic_cast<GenericActivatedAbility*>(a)) {
        if (mode == MODE_PUTINTOPLAY) {
            return BAKA_EFFECT_GOOD;
        }
        return abilityEfficiency(abi->ability, p, mode, tc);
    }
    if (auto* abi = dynamic_cast<MultiAbility*>(a)) {
        return abilityEfficiency(abi->abilities[0], p, mode, tc);
    }
    if (auto* abi = dynamic_cast<MayAbility*>(a)) {
        return abilityEfficiency(abi->ability, p, mode, tc);
    }
    if (auto* abi = dynamic_cast<ALord*>(a)) {
        const int myCards    = countCards(abi->getActionTc(), p);
        const int theirCards = countCards(abi->getActionTc(), p->opponent());
        int efficiency       = abilityEfficiency(abi->ability, p, mode, tc);
        if (efficiency == BAKA_EFFECT_GOOD) {
            myCards < theirCards ? efficiency = BAKA_EFFECT_BAD : efficiency = BAKA_EFFECT_GOOD;
        } else if (efficiency == BAKA_EFFECT_BAD) {
            myCards >= theirCards ? efficiency = BAKA_EFFECT_BAD : efficiency = BAKA_EFFECT_GOOD;
        }
        return efficiency;
        /*this method below leads to too many undesired effects, basically it doesn't work how the original coder
        thought it would. leaving it for reference to avoid it reaccuring during a refactor. if ( ((myCards <=
        theirCards) && efficiency == BAKA_EFFECT_GOOD) || ((myCards >= theirCards) && efficiency == BAKA_EFFECT_BAD)   )
        return efficiency;
        return -efficiency; */
    }
    if (auto* abi = dynamic_cast<AAsLongAs*>(a)) {
        return abilityEfficiency(abi->ability, p, mode, tc);
    }
    if (auto* abi = dynamic_cast<AForeach*>(a)) {
        return abilityEfficiency(abi->ability, p, mode, tc);
    }
    if (auto* abi = dynamic_cast<ATeach*>(a)) {
        return abilityEfficiency(abi->ability, p, mode, tc);
    }
    if (auto* atac = dynamic_cast<ATargetedAbilityCreator*>(a)) {
        Player* targetedPlyr;
        switch (atac->who) {
        case TargetChooser::CONTROLLER:
            targetedPlyr = atac->source->controller();
            break;
        case TargetChooser::OPPONENT:
            targetedPlyr = atac->source->controller()->opponent();
            break;
        case TargetChooser::TARGET_CONTROLLER:
            if (dynamic_cast<MTGCardInstance*>(target)) {
                targetedPlyr = (dynamic_cast<MTGCardInstance*>(atac->target))->controller();
                break;
            }
        case TargetChooser::TARGETED_PLAYER: {
            targetedPlyr = atac->source->playerTarget ? atac->source->playerTarget : p;
            break;
        }
        default:
            targetedPlyr = atac->source->controller()->opponent();
            break;
        }
        int result = 0;
        if (targetedPlyr) {
            auto* testDummy = NEW MTGCardInstance();
            testDummy->setObserver(targetedPlyr->getObserver());
            testDummy->owner              = targetedPlyr;
            testDummy->storedSourceCard   = atac->source;
            vector<string> magictextlines = ::woth::split(atac->sabilities, '_');
            if (!magictextlines.empty()) {
                for (unsigned int i = 0; i < magictextlines.size(); i++) {
                    MTGAbility* ata = parseMagicLine(magictextlines[i], -1, nullptr, testDummy);
                    if (ata) {
                        result += abilityEfficiency(getCoreAbility(ata), targetedPlyr, mode);
                        SAFE_DELETE(ata);
                    }
                }
            }
            SAFE_DELETE(testDummy);
        }
        return result;
    }
    if (dynamic_cast<AAFizzler*>(a)) {
        return BAKA_EFFECT_BAD;
    }
    if (dynamic_cast<AADamagePrevent*>(a)) {
        return BAKA_EFFECT_GOOD;
    }
    if (dynamic_cast<AACloner*>(a)) {
        return BAKA_EFFECT_GOOD;
    }
    if (dynamic_cast<ASwapPTUEOT*>(a)) {
        return BAKA_EFFECT_BAD;
    }
    if (dynamic_cast<AAUntapper*>(a)) {
        return BAKA_EFFECT_GOOD;
    }
    if (dynamic_cast<AATapper*>(a)) {
        return BAKA_EFFECT_BAD;
    }
    if (dynamic_cast<AManaProducer*>(a)) {
        return BAKA_EFFECT_GOOD;
    }

    // Equipment that gets immediately attached. Todo: check the abilities associated with Equip, to make sure they're
    // good (for now it seems to be the majority of the cases)?
    if (dynamic_cast<AEquip*>(a)) {
        return BAKA_EFFECT_GOOD;
    }

    // For now, ACounterTracker is only used for Creatures that "belong" to one of our domains, need to target one of
    // our own lands, so we return a "positive" value
    if (dynamic_cast<ACounterTracker*>(a)) {
        return BAKA_EFFECT_GOOD;
    }

    if (auto* ac = dynamic_cast<AACounter*>(a)) {
        const bool negative_effect = ac->power < 0 || ac->toughness < 0;
        if ((ac->nb > 0 && negative_effect) || (ac->nb < 0 && !negative_effect)) {
            return BAKA_EFFECT_BAD;
        }
        return BAKA_EFFECT_GOOD;
    }

    if (dynamic_cast<ATokenCreator*>(a)) {
        return BAKA_EFFECT_GOOD;
    }

    if (auto* aam = dynamic_cast<AAMover*>(a)) {
        MTGGameZone* z = aam->destinationZone(target);
        if (tc && tc->targetsZone(p->game->library)) {
            if (z == p->game->hand || z == p->game->inPlay) {
                return BAKA_EFFECT_GOOD;
            }
        }
        return BAKA_EFFECT_BAD;  // TODO
    }

    if (dynamic_cast<AACopier*>(a)) {
        return BAKA_EFFECT_GOOD;
    }
    if (dynamic_cast<AABuryCard*>(a)) {
        return BAKA_EFFECT_BAD;
    }
    if (dynamic_cast<AADestroyCard*>(a)) {
        return BAKA_EFFECT_BAD;
    }
    if (dynamic_cast<AStandardRegenerate*>(a)) {
        return BAKA_EFFECT_GOOD;
    }
    if (auto* abi = dynamic_cast<AALifer*>(a)) {
        return abi->getLife() > 0 ? BAKA_EFFECT_GOOD : BAKA_EFFECT_BAD;
    }
    if (auto* abi = dynamic_cast<AAAlterPoison*>(a)) {
        return abi->poison > 0 ? BAKA_EFFECT_GOOD : BAKA_EFFECT_BAD;
    }
    if (dynamic_cast<AADepleter*>(a)) {
        return BAKA_EFFECT_BAD;
    }
    if (dynamic_cast<AADrawer*>(a)) {
        return BAKA_EFFECT_GOOD;
    }
    if (dynamic_cast<AARandomDiscarder*>(a)) {
        return BAKA_EFFECT_BAD;
    }
    if (dynamic_cast<ARampageAbility*>(a)) {
        return BAKA_EFFECT_GOOD;
    }
    if (dynamic_cast<ABushidoAbility*>(a)) {
        return BAKA_EFFECT_GOOD;
    }
    if (auto* abi = dynamic_cast<PTInstant*>(a)) {
        return (abi->wppt->power.getValue() >= 0 && abi->wppt->toughness.getValue() >= 0) ? BAKA_EFFECT_GOOD
                                                                                          : BAKA_EFFECT_BAD;
    }
    if (auto* abi = dynamic_cast<APowerToughnessModifier*>(a)) {
        return (abi->wppt->power.getValue() >= 0 && abi->wppt->toughness.getValue() >= 0) ? BAKA_EFFECT_GOOD
                                                                                          : BAKA_EFFECT_BAD;
    }
    if (auto* abi = dynamic_cast<PTInstant*>(a)) {
        return abilityEfficiency(abi->ability, p, mode, tc);
    }

    if (dynamic_cast<ACantBeBlockedBy*>(a)) {
        return BAKA_EFFECT_GOOD;
    }
    if (dynamic_cast<AProtectionFrom*>(a)) {
        return BAKA_EFFECT_GOOD;
    }

    map<int, bool> badAbilities;
    badAbilities[(int)Constants::CANTATTACK]     = true;
    badAbilities[(int)Constants::CANTBLOCK]      = true;
    badAbilities[(int)Constants::CLOUD]          = true;
    badAbilities[(int)Constants::DEFENDER]       = true;
    badAbilities[(int)Constants::DOESNOTUNTAP]   = true;
    badAbilities[(int)Constants::MUSTATTACK]     = true;
    badAbilities[(int)Constants::CANTREGEN]      = true;
    badAbilities[(int)Constants::NOACTIVATED]    = true;
    badAbilities[(int)Constants::NOACTIVATEDTAP] = true;
    badAbilities[(int)Constants::NOMANA]         = true;
    badAbilities[(int)Constants::ONLYMANA]       = true;
    badAbilities[(int)Constants::EXILEDEATH]     = true;
    badAbilities[(int)Constants::WEAK]           = true;

    if (auto* abi = dynamic_cast<AInstantBasicAbilityModifierUntilEOT*>(a)) {
        const int result = badAbilities[abi->ability] ? BAKA_EFFECT_BAD : BAKA_EFFECT_GOOD;
        return (abi->value > 0) ? result : -result;
    }
    if (auto* abi = dynamic_cast<ABasicAbilityModifier*>(a)) {
        const int result = (badAbilities[abi->ability]) ? BAKA_EFFECT_BAD : BAKA_EFFECT_GOOD;
        return (abi->modifier > 0) ? result : -result;
    }
    if (auto* abi = dynamic_cast<ABasicAbilityAuraModifierUntilEOT*>(a)) {
        return abilityEfficiency(abi->ability, p, mode);
    }
    if (dynamic_cast<AManaProducer*>(a)) {
        return BAKA_EFFECT_GOOD;
    }
    return BAKA_EFFECT_DONTKNOW;
}

// Returns the "X" cost that was paid for a spell
int AbilityFactory::computeX(Spell* spell, MTGCardInstance* card) {
    if (spell) {
        return spell->computeX(card);
    }
    if (card) {
        return card->X;
    }
    return 0;
}

int AbilityFactory::getAbilities(vector<MTGAbility*>* v,
                                 Spell* spell,
                                 MTGCardInstance* card,
                                 int id,
                                 MTGGameZone* dest) {
    if (!card && spell) {
        card = spell->source;
    }
    if (!card) {
        return 0;
    }

    string magicText;
    if (dest) {
        card->graveEffects = false;
        card->exileEffects = false;

        for (int i = 0; i < 2; ++i) {
            MTGPlayerCards* zones = observer->players[i]->game;
            if (dest == zones->hand) {
                magicText = card->magicTexts["hand"];
                break;
            }
            if (dest == zones->graveyard) {
                magicText          = card->magicTexts["graveyard"];
                card->graveEffects = true;
                break;
            }
            if (dest == zones->stack) {
                magicText = card->magicTexts["stack"];
                break;
            }
            if (dest == zones->exile) {
                magicText          = card->magicTexts["exile"];
                card->exileEffects = true;
                break;
            }
            if (dest == zones->library) {
                magicText = card->magicTexts["library"];
                break;
            }
            // Other zones needed ?
        }
    } else {
        if (card->previous && card->previous->morphed && !card->turningOver) {
            magicText       = card->magicTexts["facedown"];
            card->power     = 2;
            card->life      = 2;
            card->toughness = 2;
            card->setColor(0, 1);
            card->name = "Morph";
            card->types.clear();
            const string cre = "Creature";
            card->setType(cre.c_str());
            card->basicAbilities.reset();
            card->getManaCost()->resetCosts();
        } else if (card && !card->morphed && card->turningOver) {
            card->power += card->origpower - 2;
            card->life += card->origtoughness - 2;
            card->toughness += card->origtoughness - 2;
            card->setColor(0, 1);
            card->name   = card->model->data->name;
            card->types  = card->model->data->types;
            card->colors = card->model->data->colors;

            card->basicAbilities |= card->model->data->basicAbilities;

            ManaCost* copyCost = card->model->data->getManaCost();
            card->getManaCost()->copy(copyCost);
            magicText            = card->model->data->magicText;
            const string faceupC = card->magicTexts["faceup"];
            magicText.append("\n");
            magicText.append(faceupC);

        } else if (card && card->hasType(Subtypes::TYPE_EQUIPMENT) && card->target) {
            magicText              = card->model->data->magicText;
            const string equipText = card->magicTexts["skill"];
            magicText.append("\n");
            magicText.append(equipText);
        } else {
            magicText = card->magicText;
        }
    }
    if (card->alias && magicText.empty() && !dest) {
        MTGCard* c = MTGCollection()->getCardById(card->alias);
        if (!c) {
            return 0;
        }
        magicText = c->data->magicText;
    }
    string line;
    const int size = magicText.size();
    if (size == 0) {
        return 0;
    }
    size_t found;
    int result = id;

    magicText = AutoLineMacro::Process(magicText);

    while (!magicText.empty()) {
        found = magicText.find('\n');
        if (found != string::npos) {
            line      = magicText.substr(0, found);
            magicText = magicText.substr(found + 1);
        } else {
            line      = magicText;
            magicText = "";
        }
        MTGAbility* a = parseMagicLine(line, result, spell, card, false, false, dest);
        if (a) {
            v->push_back(a);
            result++;
        } else {
            DebugTrace("ABILITYFACTORY ERROR: Parser returned NULL");
        }
    }
    return result;
}

// Some basic functionalities that can be added automatically in the text file
/*
 * Several objects are computed from the text string, and have a direct influence on what action we should take
 * (direct impact on the game such as draw a card immediately, or create a New GameObserver and add it to the
 * Abilities,etc..) These objects are:
 *   - trigger (if there is an "@" in the string, this is a triggered ability)
 *   - target (if there ie a "target(" in the string, then this is a TargetAbility)
 *   - doTap (a dirty way to know if tapping is included in the cost...
 */
int AbilityFactory::magicText(int id,
                              Spell* spell,
                              MTGCardInstance* card,
                              int mode,
                              TargetChooser* tc,
                              MTGGameZone* dest) {
    int dryMode = 0;
    if (!spell && !dest) {
        dryMode = 1;
    }

    vector<MTGAbility*> v;
    int result = getAbilities(&v, spell, card, id, dest);

    for (size_t i = 0; i < v.size(); ++i) {
        MTGAbility* a = v[i];
        if (!a) {
            DebugTrace("ABILITYFACTORY ERROR: Parser returned NULL");
            continue;
        }

        if (dryMode) {
            result = abilityEfficiency(a, card->controller(), mode, tc);
            for (size_t i = 0; i < v.size(); ++i) {
                SAFE_DELETE(v[i]);
            }
            return result;
        }

        if (!a->oneShot) {
            // Anything involving Mana Producing abilities cannot be interrupted
            MTGAbility* core = getCoreAbility(a);
            if (dynamic_cast<AManaProducer*>(core)) {
                a->canBeInterrupted = false;
            }
        }

        const bool moreThanOneTarget = spell && spell->tc && spell->tc->getNbTargets() > 1;

        if (moreThanOneTarget) {
            a->target = spell->getNextTarget();
        }

        if (a->target && moreThanOneTarget) {
            auto* aMay = dynamic_cast<MayAbility*>(a);
            while (a->target) {
                if (a->oneShot) {
                    a->resolve();
                } else {
                    if (!aMay || (aMay && a->target == spell->tc->getNextTarget(nullptr))) {
                        MTGAbility* mClone = a->clone();
                        mClone->addToGame();
                    }
                }
                a->target = spell->getNextTarget(a->target);
            }
            SAFE_DELETE(a);
        } else {
            if (a->oneShot) {
                a->resolve();
                delete (a);
            } else {
                a->addToGame();
                auto* dontAdd = dynamic_cast<MayAbility*>(a);
                if (!dontAdd) {
                    if (a->source) {
                        a->source->cardsAbilities.push_back(a);
                    } else if (spell && spell->source) {
                        spell->source->cardsAbilities.push_back(a);
                    }
                }
                // keep track of abilities being added to the game on each card it belongs to, this ignores p/t bonuses
                // given from other cards, or ability bonuses, making it generally easier to strip a card of it's
                // abilities.
            }
        }
    }

    return result;
}

void AbilityFactory::addAbilities(int _id, Spell* spell) {
    MTGCardInstance* card = spell->source;

    if (spell->getNbTargets() == 1) {
        card->target = spell->getNextCardTarget();
        if (card->target && (!spell->tc->canTarget(card->target) || card->target->isPhased)) {
            MTGPlayerCards* zones = card->controller()->game;
            zones->putInZone(card, spell->from, card->owner->game->graveyard);
            return;  // fizzle
        }
        card->playerTarget = spell->getNextPlayerTarget();
    }
    _id = magicText(_id, spell);

    MTGPlayerCards* zones = card->controller()->game;

    const int id = card->alias;
    switch (id) {
    case 1092:  // Aladdin's lamp
    {
        auto* ability = NEW AAladdinsLamp(observer, _id, card);
        observer->addObserver(ability);
        break;
    }
    case 1095:  // Armageddon clock
    {
        auto* ability = NEW AArmageddonClock(observer, _id, card);
        observer->addObserver(ability);
        break;
    }

    case 1191:  // Blue Elemental Blast
    {
        if (card->target) {
            card->target->controller()->game->putInGraveyard(card->target);
        } else {
            Spell* starget = spell->getNextSpellTarget();
            observer->mLayers->stackLayer()->Fizzle(starget);
        }
        break;
    }
    case 1282:  // Chaoslace
    {
        if (card->target) {
            card->target->setColor(Constants::MTG_COLOR_RED, 1);
        } else {
            Spell* starget = spell->getNextSpellTarget();
            starget->source->setColor(Constants::MTG_COLOR_RED, 1);
        }
        break;
    }
    case 1335:  // Circle of protection : black
    {
        observer->addObserver(NEW ACircleOfProtection(observer, _id, card, Constants::MTG_COLOR_BLACK));
        break;
    }
    case 1336:  // Circle of protection : blue
    {
        observer->addObserver(NEW ACircleOfProtection(observer, _id, card, Constants::MTG_COLOR_BLUE));
        break;
    }
    case 1337:  // Circle of protection : green
    {
        observer->addObserver(NEW ACircleOfProtection(observer, _id, card, Constants::MTG_COLOR_GREEN));
        break;
    }
    case 1338:  // Circle of protection : red
    {
        observer->addObserver(NEW ACircleOfProtection(observer, _id, card, Constants::MTG_COLOR_RED));
        break;
    }
    case 1339:  // Circle of protection : white
    {
        observer->addObserver(NEW ACircleOfProtection(observer, _id, card, Constants::MTG_COLOR_WHITE));
        break;
    }
    case 1102:  // Conservator
    {
        observer->addObserver(NEW AConservator(observer, _id, card));
        break;
    }

    case 1103:  // Crystal Rod
    {
        std::vector<int8_t> cost;
        cost.push_back(Constants::MTG_COLOR_ARTIFACT);
        cost.push_back(1);
        auto* ability = NEW ASpellCastLife(observer, _id, card, Constants::MTG_COLOR_BLUE, NEW ManaCost(cost, 1), 1);
        observer->addObserver(ability);
        break;
    }
    case 1152:  // Deathlace
    {
        if (card->target) {
            card->target->setColor(Constants::MTG_COLOR_BLACK, 1);
        } else {
            Spell* starget = spell->getNextSpellTarget();
            starget->source->setColor(Constants::MTG_COLOR_BLACK, 1);
        }
        break;
    }

    case 1345:  // Farmstead
    {
        observer->addObserver(NEW AFarmstead(observer, _id, card, card->target));
        break;
    }
    case 1291:  // Fireball
    {
        const int x = computeX(spell, card);
        observer->addObserver(NEW AFireball(observer, _id, card, spell, x));
        break;
    }
    case 1113:  // Iron Star
    {
        std::vector<int8_t> cost;
        cost.push_back(Constants::MTG_COLOR_ARTIFACT);
        cost.push_back(1);
        auto* ability = NEW ASpellCastLife(observer, _id, card, Constants::MTG_COLOR_RED, NEW ManaCost(cost, 1), 1);
        observer->addObserver(ability);
        break;
    }
    case 1351:  // Island Sanctuary
    {
        observer->addObserver(NEW AIslandSanctuary(observer, _id, card));
        break;
    }
    case 1114:  // Ivory cup
    {
        std::vector<int8_t> cost;
        cost.push_back(Constants::MTG_COLOR_ARTIFACT);
        cost.push_back(1);
        auto* ability = NEW ASpellCastLife(observer, _id, card, Constants::MTG_COLOR_WHITE, NEW ManaCost(cost, 1), 1);
        observer->addObserver(ability);
        break;
    }
    case 1117:  // Jandors Ring
    {
        observer->addObserver(NEW AJandorsRing(observer, _id, card));
        break;
    }
    case 1254:  // Kudzu
    {
        observer->addObserver(NEW AKudzu(observer, id, card, card->target));
        break;
    }
    case 1257:  // Lifelace
    {
        if (card->target) {
            card->target->setColor(Constants::MTG_COLOR_GREEN, 1);
        } else {
            Spell* starget = spell->getNextSpellTarget();
            starget->source->setColor(Constants::MTG_COLOR_GREEN, 1);
        }
        break;
    }
    case 1124:  // Mana Vault (the rest is softcoded!)
    {
        observer->addObserver(NEW ARegularLifeModifierAura(observer, _id + 2, card, card, MTG_PHASE_DRAW, -1, 1));
        break;
    }
    case 1215:  // Power Leak
    {
        observer->addObserver(NEW APowerLeak(observer, _id, card, card->target));
        break;
    }
    case 1358:  // Purelace
    {
        if (card->target) {
            card->target->setColor(Constants::MTG_COLOR_WHITE, 1);
        } else {
            Spell* starget = spell->getNextSpellTarget();
            starget->source->setColor(Constants::MTG_COLOR_WHITE, 1);
        }
        break;
    }
    case 1312:  // Red Elemental Blast
    {
        if (card->target) {
            card->target->controller()->game->putInGraveyard(card->target);
        } else {
            Spell* starget = spell->getNextSpellTarget();
            observer->mLayers->stackLayer()->Fizzle(starget);
        }
        break;
    }

    case 1139:  // The Rack
    {
        observer->addObserver(NEW ALifeZoneLink(observer, _id, card, MTG_PHASE_UPKEEP, -3));
        break;
    }

    case 1140:  // Throne of Bone
    {
        std::vector<int8_t> cost;
        cost.push_back(Constants::MTG_COLOR_ARTIFACT);
        cost.push_back(1);
        auto* ability = NEW ASpellCastLife(observer, _id, card, Constants::MTG_COLOR_BLACK, NEW ManaCost(cost, 1), 1);
        observer->addObserver(ability);
        break;
    }

    case 1142:  // Wooden Sphere
    {
        std::vector<int8_t> cost;
        cost.push_back(Constants::MTG_COLOR_ARTIFACT);
        cost.push_back(1);
        auto* ability = NEW ASpellCastLife(observer, _id, card, Constants::MTG_COLOR_GREEN, NEW ManaCost(cost, 1), 1);
        observer->addObserver(ability);
        break;
    }
    case 1143:  // Animate Dead
    {
        auto* a = NEW AAnimateDead(observer, _id, card, card->target);
        observer->addObserver(a);
        card->target = (dynamic_cast<MTGCardInstance*>(a->target));
        break;
    }
    case 1156:  // Drain Life
    {
        Damageable* target = spell->getNextDamageableTarget();
        int x              = spell->cost->getConvertedCost() -
                2;  // TODO Fix that !!! + X should be only black mana, that needs to be checked !
        observer->mLayers->stackLayer()->addDamage(card, target, x);
        if (target->life < x) {
            x = target->life;
        }
        observer->currentlyActing()->gainLife(x);
        break;
    }
    case 1159:  // Erg Raiders
    {
        auto* ability = NEW AErgRaiders(observer, _id, card);
        observer->addObserver(ability);
        break;
    }
    case 1202:  // Hurkyl's Recall
    {
        Player* player = spell->getNextPlayerTarget();
        if (player) {
            for (int i = 0; i < 2; i++) {
                MTGInPlay* inplay = observer->players[i]->game->inPlay;
                for (int j = inplay->nb_cards - 1; j >= 0; j--) {
                    MTGCardInstance* card = inplay->cards[j];
                    if (card->owner == player && card->hasType(Subtypes::TYPE_ARTIFACT)) {
                        player->game->putInZone(card, inplay, player->game->hand);
                    }
                }
            }
        }
        break;
    }
    case 1209:  // Mana Short
    {
        Player* player = spell->getNextPlayerTarget();
        if (player) {
            MTGInPlay* inplay = player->game->inPlay;
            for (int i = 0; i < inplay->nb_cards; i++) {
                MTGCardInstance* current = inplay->cards[i];
                if (current->hasType(Subtypes::TYPE_LAND)) {
                    current->tap();
                }
            }
            player->getManaPool()->Empty();
        }
        break;
    }
    case 1167:  // Mind Twist
    {
        const int xCost = computeX(spell, card);
        for (int i = 0; i < xCost; i++) {
            observer->opponent()->game->discardRandom(observer->opponent()->game->hand, card);
        }
        break;
    }
    case 1176:  // Sacrifice
    {
        auto* ability = NEW ASacrifice(observer, _id, card, card->target);
        observer->addObserver(ability);
        break;
    }
    case 1194:  // Control Magic
    {
        observer->addObserver(NEW AControlStealAura(observer, _id, card, card->target));
        break;
    }
    case 1231:  // Volcanic Eruption
    {
        int x                   = computeX(spell, card);
        int _x                  = x;
        MTGCardInstance* target = spell->getNextCardTarget();
        while (target && _x) {
            target->destroy();
            _x--;
            target = spell->getNextCardTarget(target);
        }
        x -= _x;
        for (int i = 0; i < 2; i++) {
            observer->mLayers->stackLayer()->addDamage(card, observer->players[i], x);
            for (int j = 0; j < observer->players[i]->game->inPlay->nb_cards; j++) {
                MTGCardInstance* current = observer->players[i]->game->inPlay->cards[j];
                if (current->isCreature()) {
                    observer->mLayers->stackLayer()->addDamage(card, current, x);
                }
            }
        }
        break;
    }
    case 1288:  // EarthBind
    {
        observer->addObserver(NEW AEarthbind(observer, _id, card, card->target));
        break;
    }
    case 1344:  // Eye for an Eye
    {
        Damage* damage = spell->getNextDamageTarget();
        if (damage) {
            observer->mLayers->stackLayer()->addDamage(card, damage->source->controller(), damage->damage);
        }
        break;
    }
    case 1243:  // Fastbond
    {
        observer->addObserver(NEW AFastbond(observer, _id, card));
        break;
    }
    case 1227:  // Toughtlace
    {
        if (card->target) {
            card->target->setColor(Constants::MTG_COLOR_BLUE, 1);
        } else {
            Spell* starget = spell->getNextSpellTarget();
            starget->source->setColor(Constants::MTG_COLOR_BLUE, 1);
        }
        break;
    }

    case 2732:  // Kjeldoran Frostbeast
    {
        observer->addObserver(NEW AKjeldoranFrostbeast(observer, _id, card));
        break;
    }

        // --- addon Mirage ---

    case 3410:  // Seed of Innocence
    {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < observer->players[i]->game->inPlay->nb_cards; j++) {
                MTGCardInstance* current = observer->players[i]->game->inPlay->cards[j];
                if (current->hasType("Artifact")) {
                    observer->players[i]->game->putInGraveyard(current);
                    current->controller()->gainLife(current->getManaCost()->getConvertedCost());
                }
            }
        }
        break;
    }

        //-- addon 10E---

    case 129767:  // Threaten
    {
        observer->addObserver(NEW AInstantControlSteal(observer, _id, card, card->target));
        break;
    }

    case 129774:  // Traumatize
    {
        int nbcards;
        Player* player      = spell->getNextPlayerTarget();
        MTGLibrary* library = player->game->library;
        nbcards             = (library->nb_cards) / 2;
        for (int i = 0; i < nbcards; i++) {
            if (library->nb_cards) {
                player->game->putInZone(library->cards[library->nb_cards - 1], library, player->game->graveyard);
            }
        }
        break;
    }

    case 135215:  // Sylvan Basilisk
    {
        observer->addObserver(NEW ABasilik(observer, _id, card));
        break;
    }
    case 130553:  // Beacon of Immortality
    {
        Player* player = spell->getNextPlayerTarget();
        if (player->life < (INT_MAX / 4)) {
            player->life += player->life;
        }
        zones->putInZone(card, spell->from, zones->library);
        zones->library->shuffle();
        break;
    }
    case 135262:  // Beacon of Destruction & unrest
    {
        zones->putInZone(card, spell->from, zones->library);
        zones->library->shuffle();
        break;
    }
    case 129750:  // Sudden Impact
    {
        Damageable* target = spell->getNextDamageableTarget();
        Player* p          = spell->getNextPlayerTarget();
        MTGHand* hand      = p->game->hand;
        const int damage   = hand->nb_cards;
        observer->mLayers->stackLayer()->addDamage(card, target, damage);
        break;
    }
    case 130369:  // Soulblast
    {
        int damage         = 0;
        Damageable* target = spell->getNextDamageableTarget();
        for (int j = card->controller()->game->inPlay->nb_cards - 1; j >= 0; --j) {
            MTGCardInstance* current = card->controller()->game->inPlay->cards[j];
            if (current->hasType(Subtypes::TYPE_CREATURE)) {
                card->controller()->game->putInGraveyard(current);
                damage += current->power;
            }
        }
        observer->mLayers->stackLayer()->addDamage(card, target, damage);
        break;
    }

    case 129698:  // Reminisce
    {
        int nbcards;
        Player* player          = spell->getNextPlayerTarget();
        MTGLibrary* library     = player->game->library;
        MTGGraveyard* graveyard = player->game->graveyard;
        nbcards                 = (graveyard->nb_cards);
        for (int i = 0; i < nbcards; i++) {
            if (graveyard->nb_cards) {
                player->game->putInZone(graveyard->cards[graveyard->nb_cards - 1], graveyard, library);
            }
        }
        library->shuffle();
        break;
    }

        // --- addon Ravnica---

    case 89114:  // Psychic Drain
    {
        Player* player      = spell->getNextPlayerTarget();
        MTGLibrary* library = player->game->library;
        const int x         = computeX(spell, card);
        for (int i = 0; i < x; i++) {
            if (library->nb_cards) {
                player->game->putInZone(library->cards[library->nb_cards - 1], library, player->game->graveyard);
            }
        }
        observer->currentlyActing()->gainLife(x);
        break;
    }

    default:
        break;
    }

    /* We want to get rid of these basicAbility things.
     * basicAbilities themselves are alright, but creating New object depending on them is dangerous
     * The main reason is that classes that add an ability to a card do NOT create these objects, and therefore do NOT
     * Work.
     * For example, setting EXALTED for a creature is not enough right now...
     * It shouldn't be necessary to add an object. State based abilities could do the trick
     */

    if (card->basicAbilities[(int)Constants::EXALTED]) {
        observer->addObserver(NEW AExalted(observer, _id, card));
    }

    if (card->basicAbilities[(int)Constants::FLANKING]) {
        observer->addObserver(NEW AFlankerAbility(observer, _id, card));
    }

    const int HomeAbilities[] = {(int)Constants::FORESTHOME, (int)Constants::ISLANDHOME, (int)Constants::MOUNTAINHOME,
                                 (int)Constants::SWAMPHOME, (int)Constants::PLAINSHOME};
    const char* HomeLands[]   = {"forest", "island", "mountain", "swamp", "plains"};

    for (unsigned int i = 0; i < sizeof(HomeAbilities) / sizeof(HomeAbilities[0]); ++i) {
        if (card->basicAbilities[HomeAbilities[i]]) {
            observer->addObserver(NEW AStrongLandLinkCreature(observer, _id, card, HomeLands[i]));
        }
    }

    if (card->previous && card->previous->previous && card->previous->previous->suspended) {
        card->basicAbilities[(int)Constants::HASTE] = 1;
    }

    if (card->hasType(Subtypes::TYPE_INSTANT) || card->hasType(Subtypes::TYPE_SORCERY)) {
        MTGPlayerCards* zones = card->controller()->game;
        if (card->alternateCostPaid[ManaCost::MANA_PAID_WITH_BUYBACK] > 0) {
            zones->putInZone(card, zones->stack, zones->hand);
        } else if (card->alternateCostPaid[ManaCost::MANA_PAID_WITH_FLASHBACK] > 0) {
            zones->putInZone(card, zones->stack, zones->exile);
        } else {
            zones->putInZone(card, zones->stack, zones->graveyard);
        }
    }
}

// mehods used in parseMagicLine()

// ManaRedux -> manaredux(colorless,+2)
//           -> manaredux(green,-2)
MTGAbility* AbilityFactory::getManaReduxAbility(const string& s,
                                                int id,
                                                Spell* spell,
                                                MTGCardInstance* card,
                                                MTGCardInstance* target) {
    int color             = -1;
    const string manaCost = s.substr(s.find(',') + 1);

    const string ColorStrings[] = {Constants::kManaColorless, Constants::kManaGreen, Constants::kManaBlue,
                                   Constants::kManaRed,       Constants::kManaBlack, Constants::kManaWhite};

    for (unsigned int i = 0; i < sizeof(ColorStrings) / sizeof(ColorStrings[0]); ++i) {
        if (s.find(ColorStrings[i]) != string::npos) {
            color = i;
            break;
        }
    }
    if (color == -1) {
        DebugTrace("An error has happened in creating a Mana Redux Ability! " << s);
        return nullptr;
    }
    // figure out the mana cost
    const int amount = atoi(manaCost.c_str());
    return NEW AAlterCost(observer, id, card, target, amount, color);
}

MTGAbility::MTGAbility(const MTGAbility& a) : ActionElement(a) {
    // Todo get rid of menuText, it is only used as a placeholder in getMenuText, for something that could be a string
    for (int i = 0; i < 50; ++i) {
        menuText[i] = a.menuText[i];
    }

    game = a.game;

    oneShot          = a.oneShot;
    forceDestroy     = a.forceDestroy;
    forcedAlive      = a.forcedAlive;
    canBeInterrupted = a.canBeInterrupted;

    // costs get copied, and will be deleted in the destructor
    mCost = a.mCost ? NEW ManaCost(*a.mCost) : nullptr;

    // alternative costs are not deleted in the destructor...who deletes them???
    alternative = a.alternative;  // ? NEW ManaCost(a.alternative) : NULL;
    BuyBack     = a.BuyBack;      //? NEW ManaCost(a.BuyBack) : NULL;
    FlashBack   = a.FlashBack;    // ? NEW ManaCost(a.FlashBack) : NULL;
    Retrace     = a.Retrace;      // ? NEW ManaCost(a.Retrace) : NULL;
    morph       = a.morph;        //? NEW ManaCost(a.morph) : NULL;
    suspend     = a.suspend;      // ? NEW ManaCost(a.suspend) : NULL;

    // Those two are pointers, but we don't delete them in the destructor, no need to copy them
    target = a.target;
    source = a.source;

    aType          = a.aType;
    naType         = a.naType;
    abilitygranted = a.abilitygranted;
};

MTGAbility::MTGAbility(GameObserver* observer, int id, MTGCardInstance* card)
    : ActionElement(id)
    , aType(MTGAbility::UNKNOWN)
    , canBeInterrupted(true)
    , forceDestroy(0)
    , game(observer)
    , mCost(nullptr)
    , oneShot(0)
    , source(card)
    , target(card) {}

MTGAbility::MTGAbility(GameObserver* observer, int id, MTGCardInstance* _source, Targetable* _target)
    : ActionElement(id)
    , aType(MTGAbility::UNKNOWN)
    , canBeInterrupted(true)
    , forceDestroy(0)
    , game(observer)
    , mCost(nullptr)
    , oneShot(0)
    , source(_source)
    , target(_target) {}

void MTGAbility::setCost(ManaCost* cost, bool forceDelete) {
    if (mCost) {
        DebugTrace("WARNING: Mtgability.cpp, attempt to set cost when previous cost is not null");
        if (forceDelete) {
            delete (mCost);
        }
    }
    mCost = cost;
}

int MTGAbility::stillInUse(MTGCardInstance* card) {
    if (card == source || card == target) {
        return 1;
    }
    return 0;
}

MTGAbility::~MTGAbility() { SAFE_DELETE(mCost); }

int MTGAbility::addToGame() {
    game->addObserver(this);
    return 1;
}

int MTGAbility::removeFromGame() {
    game->removeObserver(this);
    return 1;
}

// returns 1 if this ability needs to be removed from the list of active abilities
int MTGAbility::testDestroy() {
    if (game->mLayers->stackLayer()->has(this)) {
        return 0;
    }
    if (waitingForAnswer) {
        return 0;
    }
    if (forceDestroy == 1) {
        return 1;
    }
    if (forceDestroy == -1) {
        return 0;
    }
    if (source->graveEffects && game->isInGrave(source)) {
        return 0;
    }
    if (source->exileEffects && game->isInExile(source)) {
        return 0;
    }
    if (this->forcedAlive == 1) {
        return 0;
    }
    if (!game->isInPlay(source)) {
        return 1;
    }
    if (target && !dynamic_cast<Player*>(target) && !game->isInPlay(dynamic_cast<MTGCardInstance*>(target))) {
        return 1;
    }
    return 0;
}

int MTGAbility::fireAbility() {
    if (canBeInterrupted) {
        game->mLayers->stackLayer()->addAbility(this);
    } else {
        resolve();
    }
    return 1;
}

std::ostream& MTGAbility::toString(std::ostream& out) const {
    return out << "MTGAbility ::: menuText : " << menuText << " ; game : " << game
               << " ; forceDestroy : " << forceDestroy << " ; mCost : " << mCost << " ; target : " << target
               << " ; aType : " << aType << " ; source : " << source;
}

Player* MTGAbility::getPlayerFromTarget(Targetable* target) {
    if (!target) {
        return nullptr;
    }

    if (auto* cTarget = dynamic_cast<MTGCardInstance*>(target)) {
        return cTarget->controller();
    }

    if (auto* cPlayer = dynamic_cast<Player*>(target)) {
        return cPlayer;
    }

    return (dynamic_cast<Interruptible*>(target))->source->controller();
}

Player* MTGAbility::getPlayerFromDamageable(Damageable* target) {
    if (!target) {
        return nullptr;
    }

    if (target->type_as_damageable == DAMAGEABLE_MTGCARDINSTANCE) {
        return (dynamic_cast<MTGCardInstance*>(target))->controller();
    }

    return dynamic_cast<Player*>(target);
}

//

NestedAbility::NestedAbility(MTGAbility* _ability) : ability(_ability) {}

//

ActivatedAbility::ActivatedAbility(GameObserver* observer,
                                   int id,
                                   MTGCardInstance* card,
                                   ManaCost* _cost,
                                   int restrictions,
                                   string limit,
                                   MTGAbility* sideEffect,
                                   string usesBeforeSideEffects,
                                   string castRestriction)
    : MTGAbility(observer, id, card)
    , abilityCost(nullptr)
    , restrictions(restrictions)
    , counters(0)
    , needsTapping(0)
    , limit(std::move(limit))
    , sideEffect(sideEffect)
    , sa(nullptr)
    , usesBeforeSideEffects(std::move(usesBeforeSideEffects))
    , castRestriction(std::move(castRestriction)) {
    setCost(_cost);
}

int ActivatedAbility::isReactingToClick(MTGCardInstance* card, ManaCost* mana) {
    if (card->isPhased) {
        return 0;
    }
    Player* player   = game->currentlyActing();
    const int cPhase = game->getCurrentGamePhase();
    switch (restrictions) {
    case PLAYER_TURN_ONLY:
        if (player != game->currentPlayer) {
            return 0;
        }
        break;
    case OPPONENT_TURN_ONLY:
        if (player == game->currentPlayer) {
            return 0;
        }
        break;
    case AS_SORCERY:
        if (player != game->currentPlayer) {
            return 0;
        }
        if (cPhase != MTG_PHASE_FIRSTMAIN && cPhase != MTG_PHASE_SECONDMAIN) {
            return 0;
        }
        break;
    }
    if (restrictions >= MY_BEFORE_BEGIN && restrictions <= MY_AFTER_EOT) {
        if (player != game->currentPlayer) {
            return 0;
        }
        if (cPhase != restrictions - MY_BEFORE_BEGIN + MTG_PHASE_BEFORE_BEGIN) {
            return 0;
        }
    }

    if (restrictions >= OPPONENT_BEFORE_BEGIN && restrictions <= OPPONENT_AFTER_EOT) {
        if (player == game->currentPlayer) {
            return 0;
        }
        if (cPhase != restrictions - OPPONENT_BEFORE_BEGIN + MTG_PHASE_BEFORE_BEGIN) {
            return 0;
        }
    }

    if (restrictions >= BEFORE_BEGIN && restrictions <= AFTER_EOT) {
        if (cPhase != restrictions - BEFORE_BEGIN + MTG_PHASE_BEFORE_BEGIN) {
            return 0;
        }
    }
    limitPerTurn = 0;
    if (!limit.empty()) {
        auto* value  = NEW WParsedInt(limit, nullptr, source);
        limitPerTurn = value->getValue();
        delete value;
        // only run this check if we have a valid limit string.
        // limits on uses are based on when the ability is used, not when it is resolved
        // incrementing of counters directly after the fireability()
        // as ability limits are supposed to count the use regaurdless of the ability actually
        // resolving. this check was previously located in genericactivated, and incrementing was handled in the
        // resolve.
        if (limitPerTurn && counters >= limitPerTurn) {
            return 0;
        }
    }
    if (!castRestriction.empty()) {
        AbilityFactory af(game);
        const int checkCond = af.parseCastRestrictions(card, card->controller(), castRestriction);
        if (!checkCond) {
            return 0;
        }
    }
    if (card == source && source->controller() == player &&
        (!needsTapping || (!source->isTapped() && !source->hasSummoningSickness()))) {
        ManaCost* cost = getCost();
        if (!cost) {
            return 1;
        }
        if (card->hasType(Subtypes::TYPE_PLANESWALKER)) {
            for (unsigned int k = 0; k < card->cardsAbilities.size(); ++k) {
                auto* check = dynamic_cast<ActivatedAbility*>(card->cardsAbilities[k]);
                if (check && check->counters) {
                    return 0;
                }
            }
            if (player != game->currentPlayer) {
                return 0;
            }
            if (cPhase != MTG_PHASE_FIRSTMAIN && cPhase != MTG_PHASE_SECONDMAIN) {
                return 0;
            }
        }
        if (source->has(Constants::NOACTIVATED)) {
            return 0;
        }
        const AbilityFactory af(game);
        MTGAbility* fmp = nullptr;
        fmp             = af.getCoreAbility(this);
        auto* amp       = dynamic_cast<AManaProducer*>(this);
        auto* femp      = dynamic_cast<AManaProducer*>(fmp);
        if (source->has(Constants::NOMANA) && (amp || femp)) {
            return 0;
        }
        if (source->has(Constants::ONLYMANA) && !(amp || femp)) {
            return 0;
        }
        cost->setExtraCostsAction(this, card);
        if (source->has(Constants::NOACTIVATEDTAP) && cost->extraCosts) {
            for (unsigned int i = 0; i < cost->extraCosts->costs.size(); ++i) {
                ExtraCost* eCost = cost->getExtraCost(i);
                if (dynamic_cast<TapCost*>(eCost)) {
                    return 0;
                }
            }
        }

        if (!mana) {
            mana = player->getManaPool();
        }
        if (!mana->canAfford(cost)) {
            return 0;
        }
        if (!cost->canPayExtra()) {
            return 0;
        }
        return 1;
    }
    return 0;
}

int ActivatedAbility::reactToClick(MTGCardInstance* card) {
    if (!isReactingToClick(card)) {
        return 0;
    }
    Player* player = game->currentlyActing();
    ManaCost* cost = getCost();
    if (cost) {
        if (!cost->isExtraPaymentSet()) {
            game->mExtraPayment = cost->extraCosts;
            return 0;
        }
        auto* previousManaPool = utils::detail::copy_or_new(player->getManaPool());
        game->currentlyActing()->getManaPool()->pay(cost);
        cost->doPayExtra();
        SAFE_DELETE(abilityCost);
        abilityCost = previousManaPool->Diff(player->getManaPool());
        delete previousManaPool;
    }
    return ActivatedAbility::activateAbility();
}

int ActivatedAbility::reactToTargetClick(Targetable* object) {
    if (!isReactingToTargetClick(object)) {
        return 0;
    }
    Player* player = game->currentlyActing();
    ManaCost* cost = getCost();
    if (cost) {
        if (auto* cObject = dynamic_cast<MTGCardInstance*>(object)) {
            cost->setExtraCostsAction(this, cObject);
        }
        if (!cost->isExtraPaymentSet()) {
            game->mExtraPayment = cost->extraCosts;
            return 0;
        }
        auto* previousManaPool = utils::detail::copy_or_new(player->getManaPool());
        game->currentlyActing()->getManaPool()->pay(cost);
        cost->doPayExtra();
        SAFE_DELETE(abilityCost);
        abilityCost = previousManaPool->Diff(player->getManaPool());
        delete previousManaPool;
    }
    return ActivatedAbility::activateAbility();
}

int ActivatedAbility::activateAbility() {
    MTGAbility* fmp       = nullptr;
    bool wasTappedForMana = false;
    // taking foreach manaproducers off the stack and sending tapped for mana events.
    const AbilityFactory af(game);
    fmp            = af.getCoreAbility(this);
    auto* amp      = dynamic_cast<AManaProducer*>(this);
    auto* femp     = dynamic_cast<AManaProducer*>(fmp);
    ManaCost* cost = getCost();
    if ((amp || femp) && cost && cost->extraCosts) {
        for (unsigned int i = 0; i < cost->extraCosts->costs.size(); i++) {
            ExtraCost* tapper = dynamic_cast<TapCost*>(cost->extraCosts->costs[i]);
            if (tapper) {
                needsTapping = 1;
            }
            wasTappedForMana = true;
        }
    } else if (amp || femp) {
        if (amp) {
            needsTapping = amp->tap;
        } else {
            needsTapping = femp->tap;
        }
    }
    if (needsTapping && (source->isInPlay(game) || wasTappedForMana)) {
        if (amp || femp) {
            WEvent* e = NEW WEventCardTappedForMana(source, 0, 1);
            game->receiveEvent(e);
        }
    }
    if (amp || femp) {
        counters++;
        if (sideEffect && !usesBeforeSideEffects.empty()) {
            activateSideEffect();
        }
        this->resolve();
        return 1;
    }
    counters++;
    if (sideEffect && !usesBeforeSideEffects.empty()) {
        activateSideEffect();
    }
    fireAbility();
    return 1;
}

void ActivatedAbility::activateSideEffect() {
    auto* use = NEW WParsedInt(usesBeforeSideEffects, nullptr, source);
    uses      = use->getValue();
    delete use;
    if (counters == uses) {
        sa         = sideEffect->clone();
        sa->target = this->target;
        sa->source = this->source;
        if (sa->oneShot) {
            sa->fireAbility();
        } else {
            auto* wrapper = NEW GenericInstantAbility(game, 1, source, dynamic_cast<Damageable*>(this->target), sa);
            wrapper->addToGame();
        }
    }
}

ActivatedAbility::~ActivatedAbility() {
    SAFE_DELETE(abilityCost);
    SAFE_DELETE(sideEffect);
    SAFE_DELETE(sa);
}

std::ostream& ActivatedAbility::toString(std::ostream& out) const {
    out << "ActivatedAbility ::: restrictions : " << restrictions << " ; needsTapping : " << needsTapping << " (";
    return MTGAbility::toString(out) << ")";
}

TargetAbility::TargetAbility(GameObserver* observer,
                             int id,
                             MTGCardInstance* card,
                             TargetChooser* _tc,
                             ManaCost* _cost,
                             int _playerturnonly,
                             string castRestriction)
    : ActivatedAbility(observer, id, card, _cost, _playerturnonly, "", nullptr, "", std::move(castRestriction))
    , NestedAbility(nullptr)  // Todo fix this mess, why do we have to pass "", NULL, "" here before cast restrictions?
{
    tc = _tc;
}

TargetAbility::TargetAbility(GameObserver* observer,
                             int id,
                             MTGCardInstance* card,
                             ManaCost* _cost,
                             int _playerturnonly,
                             string castRestriction)
    : ActivatedAbility(observer, id, card, _cost, _playerturnonly, "", nullptr, "", std::move(castRestriction))
    , NestedAbility(nullptr)  // Todo fix this mess, why do we have to pass "", NULL, "" here before cast restrictions?
{
    tc = nullptr;
}

int TargetAbility::reactToTargetClick(Targetable* object) {
    if (auto* cObject = dynamic_cast<MTGCardInstance*>(object)) {
        return reactToClick(cObject);
    }

    if (waitingForAnswer) {
        if (tc->toggleTarget(object) == TARGET_OK_FULL) {
            waitingForAnswer = 0;
            game->mLayers->actionLayer()->setCurrentWaitingAction(nullptr);
            return ActivatedAbility::reactToClick(source);
        }
        return 1;
    }
    return 0;
}

int TargetAbility::reactToClick(MTGCardInstance* card) {
    if (!waitingForAnswer) {
        if (isReactingToClick(card)) {
            ManaCost* cost = getCost();
            if (cost && !cost->isExtraPaymentSet()) {
                game->mExtraPayment = cost->extraCosts;
                return 0;
            }

            waitingForAnswer = 1;
            game->mLayers->actionLayer()->setCurrentWaitingAction(this);
            tc->initTargets();
            return 1;
        }
    } else {
        if (card == source &&
            ((tc->targetsReadyCheck() == TARGET_OK && !tc->targetMin) || tc->targetsReadyCheck() == TARGET_OK_FULL)) {
            waitingForAnswer = 0;
            game->mLayers->actionLayer()->setCurrentWaitingAction(nullptr);
            return ActivatedAbility::reactToClick(source);
        }
        if (tc->toggleTarget(card) == TARGET_OK_FULL) {
            const int result = ActivatedAbility::reactToClick(source);
            if (result) {
                waitingForAnswer = 0;
                game->mLayers->actionLayer()->setCurrentWaitingAction(nullptr);
                if (tc->targetter) {
                    WEvent* e = NEW WEventTarget(card, source);
                    game->receiveEvent(e);
                }
            }
            return result;
        }
        return 1;
    }
    return 0;
}

void TargetAbility::Render() {
    // TODO ?
}

int TargetAbility::resolve() {
    Targetable* t = tc->getNextTarget();
    if (t && ability) {
        if (abilityCost) {
            source->X      = 0;
            ManaCost* diff = abilityCost->Diff(getCost());
            source->X      = diff->hasX();
            delete (diff);
        }

        ability->target = t;
        // do nothing if the target controller responded by phasing out the target.
        if (dynamic_cast<MTGCardInstance*>(t) && (dynamic_cast<MTGCardInstance*>(t))->isPhased) {
            return 0;
        }
        auto* targetedPlyr = dynamic_cast<Player*>(t);
        if (targetedPlyr) {
            source->playerTarget = targetedPlyr;
        }
        if (ability->oneShot) {
            while (t) {
                ability->resolve();
                t               = tc->getNextTarget(t);
                ability->target = t;
            }
            tc->initTargets();
            return 1;
        }
        while (t) {
            MTGAbility* a = ability->clone();
            a->addToGame();
            t               = tc->getNextTarget(t);
            ability->target = t;
        }
        tc->initTargets();
        return 1;
    }
    return 0;
}

const char* TargetAbility::getMenuText() {
    if (ability) {
        return ability->getMenuText();
    }
    return ActivatedAbility::getMenuText();
}

TargetAbility::~TargetAbility() { SAFE_DELETE(ability); }

std::ostream& TargetAbility::toString(std::ostream& out) const {
    out << "TargetAbility ::: (";
    return ActivatedAbility::toString(out) << ")";
}

//

TriggeredAbility::TriggeredAbility(GameObserver* observer, int id, MTGCardInstance* card, Targetable* _target)
    : MTGAbility(observer, id, card, _target) {}

TriggeredAbility::TriggeredAbility(GameObserver* observer, int id, MTGCardInstance* card)
    : MTGAbility(observer, id, card) {}

int TriggeredAbility::receiveEvent(WEvent* e) {
    if (triggerOnEvent(e)) {
        if (dynamic_cast<WEventTarget*>(e)) {
            //@targetted trigger as per mtg rules is a state based trigger
            // that resolves instantly before the event that targetted it.
            resolve();
            return 1;
        }
        fireAbility();
        return 1;
    }
    return 0;
}

void TriggeredAbility::Update(float dt) {
    if (trigger()) {
        fireAbility();
    }
}

std::ostream& TriggeredAbility::toString(std::ostream& out) const {
    out << "TriggeredAbility ::: (";
    return MTGAbility::toString(out) << ")";
}

// Trigger
Trigger::Trigger(GameObserver* observer, int id, MTGCardInstance* source, bool once, TargetChooser* _tc)
    : TriggeredAbility(observer, id, source)
    , mOnce(once)
    , mActiveTrigger(true) {
    tc = _tc;
}

int Trigger::triggerOnEvent(WEvent* event) {
    if (!mActiveTrigger) {
        return 0;
    }

    // Abilities don't work if the card is phased
    if (source->isPhased) {
        return 0;
    }

    if (!triggerOnEventImpl(event)) {
        return 0;
    }

    if (mOnce && mActiveTrigger) {
        mActiveTrigger = false;
    }

    if (!castRestriction.empty()) {
        if (!source) {
            return 1;  // can't check these restrictions without a source aka:in a rule.txt
        }
        AbilityFactory af(game);
        const int checkCond = af.parseCastRestrictions(source, source->controller(), castRestriction);

        if (!checkCond) {
            return 0;
        }
    }
    return 1;
}

//
InstantAbility::InstantAbility(GameObserver* observer, int _id, MTGCardInstance* source)
    : MTGAbility(observer, _id, source)
    , init(0) {}

void InstantAbility::Update(float dt) {
    if (!init) {
        init = resolve();
    }
}

InstantAbility::InstantAbility(GameObserver* observer, int _id, MTGCardInstance* source, Targetable* _target)
    : MTGAbility(observer, _id, source, _target)
    , init(0) {}

// Instant abilities last generally until the end of the turn
int InstantAbility::testDestroy() {
    const GamePhase newPhase = game->getCurrentGamePhase();
    if (newPhase != currentPhase && newPhase == MTG_PHASE_AFTER_EOT) {
        return 1;
    }
    currentPhase = newPhase;
    return 0;
}

std::ostream& InstantAbility::toString(std::ostream& out) const {
    out << "InstantAbility ::: init : " << init << " (";
    return MTGAbility::toString(out) << ")";
}

bool ListMaintainerAbility::canTarget(MTGGameZone* zone) {
    if (tc) {
        return tc->targetsZone(zone);
    }
    for (int i = 0; i < 2; i++) {
        Player* p = game->players[i];
        if (zone == p->game->inPlay) {
            return true;
        }
    }
    return false;
}

void ListMaintainerAbility::updateTargets() {
    // remove invalid ones
    map<MTGCardInstance*, bool> temp;
    for (auto it = cards.begin(); it != cards.end(); ++it) {
        MTGCardInstance* card = (*it).first;
        if (!canBeInList(card) || card->mPropertiesChangedSinceLastUpdate) {
            temp[card] = true;
        }
    }

    for (auto it = temp.begin(); it != temp.end(); ++it) {
        MTGCardInstance* card = (*it).first;
        cards.erase(card);
        removed(card);
    }
    temp.clear();
    // add New valid ones
    for (int i = 0; i < 2; i++) {
        Player* p            = game->players[i];
        MTGGameZone* zones[] = {p->game->inPlay, p->game->graveyard, p->game->hand, p->game->library};
        for (int k = 0; k < 4; k++) {
            MTGGameZone* zone = zones[k];
            if (canTarget(zone)) {
                for (int j = 0; j < zone->nb_cards; j++) {
                    MTGCardInstance* card = zone->cards[j];
                    if (canBeInList(card)) {
                        if (cards.find(card) == cards.end()) {
                            temp[card] = true;
                        }
                    }
                }
            }
        }
    }

    for (auto it = temp.begin(); it != temp.end(); ++it) {
        MTGCardInstance* card = (*it).first;
        cards[card]           = true;
        added(card);
    }

    temp.clear();

    for (int i = 0; i < 2; ++i) {
        Player* p = game->players[i];
        if (!players[p] && canBeInList(p)) {
            players[p] = true;
            added(p);
        } else if (players[p] && !canBeInList(p)) {
            players[p] = false;
            removed(p);
        }
    }
}

void ListMaintainerAbility::checkTargets() {
    // remove invalid ones
    map<MTGCardInstance*, bool> tempCheck;
    for (auto it = checkCards.begin(); it != checkCards.end(); ++it) {
        MTGCardInstance* card = (*it).first;
        if (!canBeInList(card) || card->mPropertiesChangedSinceLastUpdate) {
            tempCheck[card] = true;
        }
    }

    for (auto it = tempCheck.begin(); it != tempCheck.end(); ++it) {
        MTGCardInstance* card = (*it).first;
        checkCards.erase(card);
    }

    tempCheck.clear();

    // add New valid ones
    for (int i = 0; i < 2; i++) {
        Player* p            = game->players[i];
        MTGGameZone* zones[] = {p->game->inPlay, p->game->graveyard, p->game->hand, p->game->library};
        for (int k = 0; k < 4; k++) {
            MTGGameZone* zone = zones[k];
            if (canTarget(zone)) {
                for (int j = 0; j < zone->nb_cards; j++) {
                    MTGCardInstance* card = zone->cards[j];
                    if (canBeInList(card)) {
                        if (checkCards.find(card) == checkCards.end()) {
                            tempCheck[card] = true;
                        }
                    }
                }
            }
        }
    }
    for (auto it = tempCheck.begin(); it != tempCheck.end(); ++it) {
        MTGCardInstance* card = (*it).first;
        checkCards[card]      = true;
    }
}

void ListMaintainerAbility::Update(float dt) { updateTargets(); }

// Destroy the spell -> remove all targets
int ListMaintainerAbility::destroy() {
    auto it = cards.begin();

    while (it != cards.end()) {
        MTGCardInstance* card = (*it).first;
        cards.erase(card);
        removed(card);
        it = cards.begin();
    }
    return 1;
}

std::ostream& ListMaintainerAbility::toString(std::ostream& out) const {
    out << "ListMaintainerAbility ::: (";
    return MTGAbility::toString(out) << ")";
}

TriggerAtPhase::TriggerAtPhase(GameObserver* observer,
                               int id,
                               MTGCardInstance* source,
                               Targetable* target,
                               int _phaseId,
                               int who,
                               bool sourceUntapped,
                               bool sourceTap,
                               bool lifelost,
                               int lifeamount,
                               bool once)
    : TriggeredAbility(observer, id, source, target)
    , phaseId(_phaseId)
    , who(who)
    , sourceUntapped(sourceUntapped)
    , sourceTap(sourceTap)
    , lifelost(lifelost)
    , lifeamount(lifeamount)
    , once(once)
    , activeTrigger(true) {
    if (game) {
        newPhase     = game->getCurrentGamePhase();
        currentPhase = newPhase;
    }
}

int TriggerAtPhase::trigger() {
    if (!activeTrigger) {
        return 0;
    }
    if (source->isPhased) {
        return 0;
    }
    if (lifelost) {
        const int lifeloss = source->controller()->opponent()->lifeLostThisTurn;
        if (lifeloss < lifeamount) {
            return 0;
        }
    }
    if (sourceUntapped && source->isTapped() == 1) {
        return 0;
    }
    if (sourceTap && !source->isTapped()) {
        return 0;
    }
    if (testDestroy()) {
        return 0;  // http://code.google.com/p/wagic/issues/detail?id=426
    }
    int result = 0;
    if (currentPhase != newPhase && newPhase == phaseId) {
        result = 0;
        switch (who) {
        case 1:
            if (game->currentPlayer == source->controller()) {
                result = 1;
            }
            break;
        case -1:
            if (game->currentPlayer != source->controller()) {
                result = 1;
            }
            break;
        case -2:
            if (source->target) {
                if (game->currentPlayer == source->target->controller()) {
                    result = 1;
                }
            } else {
                if (game->currentPlayer == source->controller()) {
                    result = 1;
                }
            }
            break;
        case -3:
            if (source->playerTarget) {
                if (game->currentPlayer == source->playerTarget) {
                    result = 1;
                }
            }
            break;
        default:
            result = 1;
            break;
        }
        if (!castRestriction.empty()) {
            if (!source) {
                result = 1;  // can't check these restrictions without a source aka:in a rule.txt
            }
            AbilityFactory af(game);
            const int checkCond = af.parseCastRestrictions(source, source->controller(), castRestriction);
            if (!checkCond) {
                result = 0;
            }
        }
    }
    if (once && activeTrigger) {
        activeTrigger = false;
    }

    return result;
}

TriggerAtPhase* TriggerAtPhase::clone() const { return NEW TriggerAtPhase(*this); }

TriggerNextPhase::TriggerNextPhase(GameObserver* observer,
                                   int id,
                                   MTGCardInstance* source,
                                   Targetable* target,
                                   int _phaseId,
                                   int who,
                                   bool sourceUntapped,
                                   bool sourceTap,
                                   bool once)
    : TriggerAtPhase(observer, id, source, target, _phaseId, who, sourceUntapped, sourceTap, once)
    , activeTrigger(true)
    , destroyActivated(0) {}

int TriggerNextPhase::testDestroy() {
    // dirty hack because of http://code.google.com/p/wagic/issues/detail?id=426
    if (newPhase <= phaseId && !destroyActivated) {
        destroyActivated = 1;
    }
    if (destroyActivated > 1 || (newPhase > phaseId && destroyActivated)) {
        destroyActivated++;
        return 1;
    }
    return 0;
}

TriggerNextPhase* TriggerNextPhase::clone() const { return NEW TriggerNextPhase(*this); }

GenericTriggeredAbility::GenericTriggeredAbility(GameObserver* observer,
                                                 int id,
                                                 MTGCardInstance* _source,
                                                 TriggeredAbility* _t,
                                                 MTGAbility* a,
                                                 MTGAbility* dc,
                                                 Targetable* _target)
    : TriggeredAbility(observer, id, _source, _target)
    , NestedAbility(a) {
    if (!target) {
        target = source;
    }
    t                = _t;
    destroyCondition = dc;

    t->source       = source;
    t->target       = target;
    ability->source = source;
    ability->target = target;
    if (destroyCondition) {
        destroyCondition->source = source;
        destroyCondition->target = target;
    }
}

int GenericTriggeredAbility::trigger() { return t->trigger(); }

int GenericTriggeredAbility::triggerOnEvent(WEvent* e) {
    if (t->triggerOnEvent(e)) {
        targets.push(getTriggerTarget(e, ability));
        return 1;
    }
    return 0;
}

Targetable* GenericTriggeredAbility::getTriggerTarget(WEvent* e, MTGAbility* a) {
    auto* ttc = dynamic_cast<TriggerTargetChooser*>(a->getActionTc());
    if (ttc) {
        return e->getTarget(ttc->triggerTarget);
    }

    auto* na = dynamic_cast<NestedAbility*>(a);
    if (na) {
        return getTriggerTarget(e, na->ability);
    }

    auto* ma = dynamic_cast<MultiAbility*>(a);
    if (ma) {
        for (size_t i = 0; i < ma->abilities.size(); i++) {
            return getTriggerTarget(e, ma->abilities[i]);
        }
    }

    return nullptr;
}

void GenericTriggeredAbility::setTriggerTargets(Targetable* ta, MTGAbility* a) {
    auto* ttc = dynamic_cast<TriggerTargetChooser*>(a->getActionTc());
    if (ttc) {
        a->target   = ta;
        ttc->target = ta;
    }

    auto* na = dynamic_cast<NestedAbility*>(a);
    if (na) {
        setTriggerTargets(ta, na->ability);
    }

    auto* ma = dynamic_cast<MultiAbility*>(a);
    if (ma) {
        for (size_t i = 0; i < ma->abilities.size(); i++) {
            setTriggerTargets(ta, ma->abilities[i]);
        }
    }
}

void GenericTriggeredAbility::Update(float dt) {
    const GamePhase newPhase = game->getCurrentGamePhase();
    t->newPhase              = newPhase;
    TriggeredAbility::Update(dt);
    t->currentPhase = newPhase;
}

int GenericTriggeredAbility::resolve() {
    if (source->isPhased) {
        return 0;
    }
    if (!targets.empty()) {
        setTriggerTargets(targets.front(), ability);
        targets.pop();
    }
    if (ability->oneShot) {
        return ability->resolve();
    }
    MTGAbility* clone = ability->clone();
    clone->addToGame();
    return 1;
}

int GenericTriggeredAbility::testDestroy() {
    if (!TriggeredAbility::testDestroy()) {
        return 0;
    }
    if (destroyCondition) {
        return (destroyCondition->testDestroy());
    }
    return t->testDestroy();
}

GenericTriggeredAbility::~GenericTriggeredAbility() {
    delete t;
    delete ability;
    SAFE_DELETE(destroyCondition);
}

const char* GenericTriggeredAbility::getMenuText() { return ability->getMenuText(); }

GenericTriggeredAbility* GenericTriggeredAbility::clone() const {
    auto* a             = NEW GenericTriggeredAbility(*this);
    a->t                = t->clone();
    a->ability          = ability->clone();
    a->destroyCondition = destroyCondition->clone();
    return a;
}

/*Mana Producers (lands)
 //These have a reactToClick function, and therefore two manaProducers on the same card conflict with each other
 //That means the player has to choose one. although that is perfect for cards such as birds of paradise or badlands,
 other solutions need to be provided for abilities that add mana (ex: mana flare)
 */

AManaProducer::AManaProducer(GameObserver* observer,
                             int id,
                             MTGCardInstance* card,
                             Targetable* t,
                             ManaCost* _output,
                             ManaCost* _cost,
                             int who,
                             string producing)
    : ActivatedAbilityTP(observer, id, card, t, _cost, who)
    , Producing(std::move(producing))
    ,

    output(_output) {
    aType = MTGAbility::MANA_PRODUCER;
    setCost(_cost);
}

int AManaProducer::isReactingToClick(MTGCardInstance* _card, ManaCost* mana) {
    int result = 0;
    if (!mana) {
        mana = game->currentlyActing()->getManaPool();
    }
    if (_card == source && (!tap || !source->isTapped()) && game->currentlyActing()->game->inPlay->hasCard(source) &&
        (source->hasType(Subtypes::TYPE_LAND) || !tap || !source->hasSummoningSickness()) && !source->isPhased) {
        ManaCost* cost = getCost();
        if (!cost || (mana->canAfford(cost) &&
                      (!cost->extraCosts || cost->extraCosts->canPay()))) /*counter cost bypass react to click*/
        {
            result = 1;
        }
    }
    return result;
}

int AManaProducer::resolve() {
    Targetable* _target = getTarget();
    Player* player      = getPlayerFromTarget(_target);
    if (!player) {
        return 0;
    }

    player->getManaPool()->add(output, source);
    return 1;
}

int AManaProducer::reactToClick(MTGCardInstance* _card) {
    if (!isReactingToClick(_card)) {
        return 0;
    }
    if (!ActivatedAbility::isReactingToClick(_card)) {
        return 0;
    }

    ManaCost* cost = getCost();
    if (cost) {
        cost->setExtraCostsAction(this, _card);
        if (!cost->isExtraPaymentSet()) {
            game->mExtraPayment = cost->extraCosts;
            return 0;
        }
        game->currentlyActing()->getManaPool()->pay(cost);
        cost->doPayExtra();
    }

    if (options[Options::SFXVOLUME].number > 0) {
        WResourceManager::Instance()->PlaySample("mana.wav");
    }
    if (!Producing.empty()) {
        if (Producing.find("manapool") != string::npos) {  // unique card doubling cube.
            output->copy(source->controller()->getManaPool());
        }
    }
    return ActivatedAbility::activateAbility();
}

const char* AManaProducer::getMenuText() {
    if (!menutext.empty()) {
        return menutext.c_str();
    }
    menutext = _("Add ");
    char buffer[128];
    int alreadyHasOne = 0;
    for (int i = 0; i < 6; i++) {
        const int value = output->getCost(i);
        if (value) {
            if (alreadyHasOne) {
                menutext.append(",");
            }
            sprintf(buffer, "%i ", value);
            menutext.append(buffer);
            if (i >= Constants::MTG_COLOR_GREEN && i <= Constants::MTG_COLOR_WHITE) {
                menutext.append(_(Constants::MTGColorStrings[i]));
            }
            alreadyHasOne = 1;
        }
    }
    menutext.append(_(" mana"));
    return menutext.c_str();
}

AManaProducer::~AManaProducer() { SAFE_DELETE(output); }

AManaProducer* AManaProducer::clone() const {
    auto* a   = NEW AManaProducer(*this);
    a->output = NEW ManaCost();
    a->output->copy(output);
    return a;
}

AbilityTP::AbilityTP(GameObserver* observer, int id, MTGCardInstance* card, Targetable* _target, int who)
    : MTGAbility(observer, id, card)
    , who(who) {
    if (_target) {
        target = _target;
    }
}

Targetable* AbilityTP::getTarget() {
    switch (who) {
    case TargetChooser::TARGET_CONTROLLER:
        return getPlayerFromTarget(target);
    case TargetChooser::CONTROLLER:
        return source->controller();
    case TargetChooser::OPPONENT:
        return source->controller()->opponent();
    case TargetChooser::OWNER:
        return source->owner;
    case TargetChooser::TARGETED_PLAYER:
        return source->playerTarget;
    default:
        return target;
    }
    return nullptr;
}

ActivatedAbilityTP::ActivatedAbilityTP(GameObserver* observer,
                                       int id,
                                       MTGCardInstance* card,
                                       Targetable* _target,
                                       ManaCost* cost,
                                       int who)
    : ActivatedAbility(observer, id, card, cost, 0)
    , who(who) {
    if (_target) {
        target = _target;
    }
}

Targetable* ActivatedAbilityTP::getTarget() {
    switch (who) {
    case TargetChooser::TARGET_CONTROLLER:
        return getPlayerFromTarget(target);
    case TargetChooser::CONTROLLER:
        return source->controller();
    case TargetChooser::OPPONENT:
        return source->controller()->opponent();
    case TargetChooser::OWNER:
        return source->owner;
    case TargetChooser::TARGETED_PLAYER:
        return source->playerTarget;
    default:
        return target;
    }
    return nullptr;
}

InstantAbilityTP::InstantAbilityTP(GameObserver* observer, int id, MTGCardInstance* card, Targetable* _target, int who)
    : InstantAbility(observer, id, card)
    , who(who) {
    if (_target) {
        target = _target;
    }
}

// This is the same as Targetable * ActivatedAbilityTP::getTarget(), anyway to move them together?
Targetable* InstantAbilityTP::getTarget() {
    switch (who) {
    case TargetChooser::TARGET_CONTROLLER:
        return getPlayerFromTarget(target);
    case TargetChooser::CONTROLLER:
        return source->controller();
    case TargetChooser::OPPONENT:
        return source->controller()->opponent();
    case TargetChooser::OWNER:
        return source->owner;
    default:
        return target;
    }
    return nullptr;
}
