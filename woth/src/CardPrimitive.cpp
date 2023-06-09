#include "woth/memory/clone.hpp"

#include "PrecompiledHeader.h"

#include "MTGDefinitions.h"
#include "CardPrimitive.h"

#include <utility>

#include "MTGDeck.h"
#include "Subtypes.h"
#include "Translate.h"

using std::string;

namespace {
/**
** Count the number of set bits in a given integer
*/
int CountSetBits(int n) {
    int count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count;
}
}  // namespace

SUPPORT_OBJECT_ANALYTICS(CardPrimitive)

CardPrimitive::CardPrimitive(CardPrimitive* source) {
    if (!source) {
        return;
    }
    basicAbilities = source->basicAbilities;

    types  = source->types;
    colors = source->colors;
    manaCost.copy(source->getManaCost());
    // reducedCost.copy(source->getReducedManaCost());
    // increasedCost.copy(source->getIncreasedManaCost());
    if (source->getManaCost()->alternative) {
        manaCost.alternative->alternativeName = source->getManaCost()->alternative->alternativeName;
    }

    text          = source->text;
    formattedText = source->formattedText;
    setName(source->name);

    power         = source->power;
    toughness     = source->toughness;
    restrictions  = woth::clone(source->restrictions);
    suspendedTime = source->suspendedTime;

    magicText       = source->magicText;
    magicTexts      = source->magicTexts;
    spellTargetType = source->spellTargetType;
    alias           = source->alias;
}

CardPrimitive::~CardPrimitive() { SAFE_DELETE(restrictions); }

bool CardPrimitive::isCreature() { return hasSubtype(Subtypes::TYPE_CREATURE); }

bool CardPrimitive::isLand() { return hasSubtype(Subtypes::TYPE_LAND); }

bool CardPrimitive::isSpell() { return (!isCreature() && !isLand()); }

void CardPrimitive::setRestrictions(string _restriction) {
    if (!restrictions) {
        restrictions = NEW CastRestrictions();
    }

    restrictions->restriction = std::move(_restriction);
}

string CardPrimitive::getRestrictions() {
    if (!restrictions) {
        return "";
    }

    return restrictions->restriction;
}

void CardPrimitive::setOtherRestrictions(string _restriction) {
    if (!restrictions) {
        restrictions = NEW CastRestrictions();
    }

    restrictions->otherrestriction = std::move(_restriction);
}

string CardPrimitive::getOtherRestrictions() {
    if (!restrictions) {
        return "";
    }

    return restrictions->otherrestriction;
}

void CardPrimitive::setColor(const string& _color, int removeAllOthers) {
    for (size_t i = 0; i < Constants::MTGColorStrings.size(); i++) {
        if (_color == Constants::MTGColorStrings[i]) {
            return setColor(i, removeAllOthers);
        }
    }
    // Keep artifact compare, to Remove this we need change all MTG.txt
    if (_color == "artifact") {
        return setColor(Constants::MTG_COLOR_ARTIFACT, removeAllOthers);
    }
}

void CardPrimitive::setColor(int _color, int removeAllOthers) {
    if (removeAllOthers) {
        colors = 0;
    }

    colors |= ConvertColorToBitMask(_color);
}

void CardPrimitive::removeColor(int _color) {
    const uint8_t mask = ~ConvertColorToBitMask(_color);
    colors &= mask;
}

int CardPrimitive::getColor() const {
    if (colors) {
        for (int i = 1; i < Constants::NB_Colors; i++) {
            if (hasColor(i)) {
                return i;
            }
        }
    }

    return 0;
}

bool CardPrimitive::hasColor(int inColor) const { return (colors & ConvertColorToBitMask(inColor)) > 0; }

int CardPrimitive::countColors() const {
    uint8_t mask =
        kColorBitMask_Green | kColorBitMask_Blue | kColorBitMask_Red | kColorBitMask_Black | kColorBitMask_White;
    mask &= colors;

    return CountSetBits(mask);
}

void CardPrimitive::setManaCost(const string& s) {
    ManaCost::parseManaCost(s, &manaCost);
    for (int i = Constants::MTG_COLOR_GREEN; i <= Constants::MTG_COLOR_WHITE; i++) {
        if (manaCost.hasColor(i)) {
            setColor(i);
        }
    }
}

void CardPrimitive::setType(const string& _type_text) { setSubtype(_type_text); }

void CardPrimitive::addType(char* _type_text) { setSubtype(_type_text); }

void CardPrimitive::setSubtype(const string& value) {
    // find the parent type for this card
    int parentType = 0;
    for (size_t i = 0; i < types.size(); ++i) {
        if (MTGAllCards::isType(types[i])) {
            parentType = types[i];
            break;
        }
    }

    const int id = MTGAllCards::add(value, parentType);
    addType(id);
}

void CardPrimitive::addType(int id) { types.push_back(id); }

// TODO Definitely move some of these functions to CardInstance. There is no reason to remove a type from an
// CardPrimitive since they represent the Database Removes a type from the types of a given card If removeAll is true,
// removes all occurences of this type, otherwise only removes the first occurence
int CardPrimitive::removeType(string value, int removeAll) {
    const int id = MTGAllCards::findType(std::move(value));
    return removeType(id, removeAll);
}

int CardPrimitive::removeType(int id, int removeAll) {
    int result = 0;
    for (int i = types.size() - 1; i >= 0; i--) {
        if (types[i] == id) {
            types.erase(types.begin() + i);
            result++;
            if (!removeAll) {
                return result;
            }
        }
    }
    return result;
}

void CardPrimitive::setText(const string& value) { text = value; }

/* This alters the card structure, but this is intentional for performance and
 *  space purpose: The only time we get the card text is to render it
 *  on the screen, in a formatted way.
 * Formatting the string every frame is not efficient, especially since we always display it the same way
 * Formatting all strings at startup is inefficient too.
 * Instead, we format when requested, but only once, and cache the result.
 * To avoid memory to blow up, in exchange of the cached result, we erase the original string
 */
const vector<string>& CardPrimitive::getFormattedText() {
    if (text.empty()) {
        return formattedText;
    }

    std::string::size_type found = text.find_first_of("{}");
    while (found != string::npos) {
        text[found] = '/';
        found       = text.find_first_of("{}", found + 1);
    }
    WFont* mFont = WResourceManager::Instance()->GetWFont(Fonts::MAGIC_FONT);
    mFont->FormatText(text, formattedText);

    text = "";

    return formattedText;
};

void CardPrimitive::addMagicText(string value) {
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    if (!magicText.empty()) {
        magicText.append("\n");
    }
    magicText.append(value);
}

void CardPrimitive::addMagicText(string value, const string& key) {
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    if (!magicTexts[key].empty()) {
        magicTexts[key].append("\n");
    }
    magicTexts[key].append(value);
}

void CardPrimitive::setName(const string& value) {
    name   = value;
    lcname = value;
    std::transform(lcname.begin(), lcname.end(), lcname.begin(), ::tolower);
}

const string& CardPrimitive::getName() const { return name; }

const string& CardPrimitive::getLCName() const { return lcname; }

ManaCost* CardPrimitive::getManaCost() { return &manaCost; }

bool CardPrimitive::hasType(int _type) {
    for (size_t i = 0; i < types.size(); i++) {
        if (types[i] == _type) {
            return true;
        }
    }
    return false;
}

bool CardPrimitive::hasSubtype(int _subtype) { return hasType(_subtype); }

bool CardPrimitive::hasType(const char* _type) {
    const int id = MTGAllCards::findType(_type);
    return hasType(id);
}

bool CardPrimitive::hasSubtype(const char* _subtype) {
    const int id = MTGAllCards::findType(_subtype);
    return hasType(id);
}

bool CardPrimitive::hasSubtype(const string& _subtype) {
    const int id = MTGAllCards::findType(_subtype);
    return hasType(id);
}

int CardPrimitive::has(int basicAbility) { return basicAbilities[basicAbility]; }

//---------------------------------------------
// Creature specific
//---------------------------------------------
void CardPrimitive::setPower(int _power) { power = _power; }

int CardPrimitive::getPower() const { return power; }

void CardPrimitive::setToughness(int _toughness) { toughness = _toughness; }

int CardPrimitive::getToughness() const { return toughness; }

uint8_t CardPrimitive::ConvertColorToBitMask(int inColor) {
    uint8_t value = 0;
    switch (inColor) {
    case Constants::MTG_COLOR_ARTIFACT:
        value = kColorBitMask_Artifact;
        break;

    case Constants::MTG_COLOR_GREEN:
        value = kColorBitMask_Green;
        break;

    case Constants::MTG_COLOR_BLUE:
        value = kColorBitMask_Blue;
        break;

    case Constants::MTG_COLOR_RED:
        value = kColorBitMask_Red;
        break;

    case Constants::MTG_COLOR_BLACK:
        value = kColorBitMask_Black;
        break;

    case Constants::MTG_COLOR_WHITE:
        value = kColorBitMask_White;
        break;

    case Constants::MTG_COLOR_LAND:
        value = kColorBitMask_Land;
        break;

    default:
        break;
    }

    return value;
}
