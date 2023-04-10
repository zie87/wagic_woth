//------------------------------------------------------
// MTGCard Class
//-------------------------------------------------
// TODO Fill BasicAbilities

#include "PrecompiledHeader.h"

#include "MTGDeck.h"
#include "MTGCard.h"
#include "CardPrimitive.h"
#include "Subtypes.h"
#include "Translate.h"

using std::string;

SUPPORT_OBJECT_ANALYTICS(MTGCard)

MTGCard::MTGCard() { init(); }

MTGCard::MTGCard(int set_id) : setId(set_id) { init(); }

MTGCard::MTGCard(MTGCard* source)
    : rarity(source->rarity)
    , data(source->data)
    , mtgid(source->mtgid)
    , setId(source->setId) {}

MTGCard::~MTGCard() {}

int MTGCard::init() {
    setId  = 0;
    mtgid  = 0;
    data   = nullptr;
    rarity = Constants::RARITY_C;
    return 1;
}

void MTGCard::setMTGId(int id) { mtgid = id; }

int MTGCard::getMTGId() const { return mtgid; }
int MTGCard::getId() const { return mtgid; }
char MTGCard::getRarity() const { return rarity; }

void MTGCard::setRarity(char _rarity) { rarity = _rarity; }

string MTGCard::getImageName() const {
    std::stringstream out;
    if (mtgid < 0) {
        // tokens that have negative id have an image name that is the absolute value of their id + letter "t"
        out << -mtgid << "t.jpg";
    } else {
        out << mtgid << ".jpg";
    }
    return out.str();
}

void MTGCard::setPrimitive(CardPrimitive* cp) { data = cp; }
