/* CardPrimitive objects represent the cards database.
 * For MTG we have thousands of those, that stay constantly in Ram
 * on low-end devices such as the PSP, adding stuff to this class can have a very high cost
 * As an example, with 16'000 card primitives (the rough number of cards in MTG), adding a simple 16 bytes attribute
 * costs 250kB (2% of the total available ram on the PSP)
 */
#ifndef CARDPRIMITIVE_H
#define CARDPRIMITIVE_H

#include <string>
#include <vector>
#include <map>
#include <bitset>
#include "config.h"
#include "ManaCost.h"
#include "ObjectAnalytics.h"

const uint8_t kColorBitMask_Artifact = 0x01;
const uint8_t kColorBitMask_Green    = 0x02;
const uint8_t kColorBitMask_Blue     = 0x04;
const uint8_t kColorBitMask_Red      = 0x08;
const uint8_t kColorBitMask_Black    = 0x10;
const uint8_t kColorBitMask_White    = 0x20;
const uint8_t kColorBitMask_Land     = 0x40;

class CastRestrictions {
public:
    std::string restriction;
    std::string otherrestriction;

    CastRestrictions* clone() const { return NEW CastRestrictions(*this); };
};

class CardPrimitive
#ifdef TRACK_OBJECT_USAGE
    : public InstanceCounter<CardPrimitive>
#endif
{

public:
    CardPrimitive() noexcept = default;
    CardPrimitive(CardPrimitive* source);
    virtual ~CardPrimitive();

    void setColor(int _color, int removeAllOthers = 0);
    void setColor(const std::string& _color, int removeAllOthers = 0);
    void removeColor(int color);
    int getColor() const;
    bool hasColor(int inColor) const;
    int countColors() const;

    static uint8_t ConvertColorToBitMask(int inColor);

    int has(int ability);

    void setText(const std::string& value);
    const std::vector<std::string>& getFormattedText();

    void addMagicText(std::string value);
    void addMagicText(std::string value, const std::string& zone);

    void setName(const std::string& value);
    const std::string& getName() const;
    const std::string& getLCName() const;

    void addType(char* type_text);
    void addType(int id);
    void setType(const std::string& type_text);
    void setSubtype(const std::string& value);
    int removeType(std::string value, int removeAll = 0);
    int removeType(int value, int removeAll = 0);
    bool hasSubtype(int _subtype);
    bool hasSubtype(const char* _subtype);
    bool hasSubtype(const std::string& _subtype);
    bool hasType(int _type);
    bool hasType(const char* type);

    void setManaCost(const std::string& value);
    ManaCost* getManaCost();
    bool isCreature();
    bool isLand();
    bool isSpell();

    void setPower(int _power);
    int getPower() const;
    void setToughness(int _toughness);
    int getToughness() const;
    void setRestrictions(std::string _restriction);
    std::string getRestrictions();
    void setOtherRestrictions(std::string _restriction);
    std::string getOtherRestrictions();

private:
    CastRestrictions* restrictions = nullptr;

protected:
    std::string lcname = {};
    ManaCost manaCost  = {};

public:
    std::vector<std::string> formattedText = {};
    std::string text                       = {};
    std::string name                       = {};

    uint8_t colors = 0;
    typedef std::bitset<Constants::NB_BASIC_ABILITIES> BasicAbilitiesSet;
    BasicAbilitiesSet basicAbilities{};

    std::map<std::string, std::string> magicTexts{};
    std::string magicText{};
    int alias = -1;
    std::string spellTargetType{};
    int power         = 0;
    int toughness     = 0;
    int suspendedTime = 0;

    std::vector<int> types = {};
};

#endif
