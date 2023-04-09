#include "woth/json/json.hpp"

/*
[card]
name=Abyssal Specter
abilities=flying
auto=@damaged(controller) from(this):ability$!name(discard) target(*|myhand) reject!$ controller
auto=@damaged(opponent) from(this):ability$!name(discard) target(*|myhand) reject!$ opponent
text=Flying -- Whenever Abyssal Specter deals damage to a player, that player discards a card.
mana={2}{B}{B}
type=Creature
subtype=Specter
power=2
toughness=3
[/card]
*/

static const std::string card_json = R"(
{
    "card_name": "Abyssal Specter",
    "casting_cost": "{2}{B}{B}",
    "supertypes": [],
    "types": ["Creature"],
    "subtypes": [ "Specter" ],
    "power": "2",
    "toughness": "3",
    "text": "Flying -- Whenever Abyssal Specter deals damage to a player, that player discards a card.",
    "abilities": {
        "type": "wagic_script",
        "static": [ "Flying" ],
        "triggered": [
            "auto=@damaged(controller) from(this):ability$!name(discard) target(*|myhand) reject!$ controller",
            "auto=@damaged(opponent) from(this):ability$!name(discard) target(*|myhand) reject!$ opponent"
        ]
    }
}
)";

#include <iostream>

bool parse_card_json(const std::string& json) noexcept {
    woth::json::object card(json);
    if (!card) {
        const char* error_ptr = ::cJSON_GetErrorPtr();
        if (error_ptr != nullptr) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return false;
    }

    std::cout << "[card]\n";
    std::cout << "name=" << card["card_name"].get_string() << '\n';
    std::cout << "abilities=";
    for (const auto ability : card["abilities"]["static"]) {
        std::cout << ability.get_string();
    }
    std::cout << '\n';
    for (const auto ability : card["abilities"]["triggered"]) {
        std::cout << ability.get_string() << '\n';
    }
    std::cout << "text=" << card["text"].get_string() << '\n';
    std::cout << "mana=" << card["casting_cost"].get_string() << '\n';
    std::cout << "type=";
    for (const auto subtype : card["types"]) {
        std::cout << subtype.get_string();
    }
    std::cout << '\n';

    std::cout << "subtype=";
    for (const auto subtype : card["subtypes"]) {
        std::cout << subtype.get_string();
    }
    std::cout << '\n';
    std::cout << "power=" << card["power"].get_string() << '\n';
    std::cout << "toughness=" << card["toughness"].get_string() << '\n';
    std::cout << "[/card]\n";

    return card.is_valid();
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    if (!parse_card_json(card_json)) {
        return -1;
    }
}
