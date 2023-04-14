#ifndef WOTH_PLAYER_BASE_PLAYER_HPP
#define WOTH_PLAYER_BASE_PLAYER_HPP

namespace woth {

class base_player {
public:
    // LoseLife
    // GainLife
    // SetLifeTotal
    // GetLifeTotal
    // AddPoisonCounters

    // GetTeam
    // GetNextPlayerInTeam
    // GetNextPlayer
    // GetOpponent

    // LoseGame
    // WinGame
    // OutOfTheGame

    // RevealHand
    // Hand_Count
    // Hand_GetNth
    // Hand_GetRandom

    // DiscardHand
    // DiscardRandomCard
    // DrawCards
    // DrawCard
    // MillCards

    // ShuffleLibrary
    // Library_Count
    // Library_GetBottom
    // Library_GetNth
    // Library_GetTop

    // Graveyard_Count
    // Graveyard_GetNth

    // Planeswalk
    // PlaneDeck_Count
    // PlaneDeck_GetBottom
    // PlaneDeck_GetNth
    // PlaneDeck_GetTop

    // PayResourceCost
    // CanPayResourceCost
    // PayManaCost
    // CanPayManaCost
    // GetTotalMana

    // CastSpellUsingResourceCost
    // CanCastSpellUsingResourceCost
    // CastSpellForFree
    // CanCastSpellForFree

    // SetAttackController
    // SetBlockController
    // AddPlayScore
    // GetAlwaysUseOptionalAbilitiesSetting
    // GetFlipResult
    // FlipCoin
    // SetCustomQueryInstructionCardPtr
    // SetCustomQueryInstructionValue
    // MarkForFilter
    // AddBadge
    // DisplayMessage
    // ChooseNewTargets
    // Sacrifice
    // PlayerDataChest
    // CopySpell
    // GetUniqueID
    // CountBasicLandTypes
    // OpponentHasLandOfColour
    // HasLandOfColour
    // GetPredominantColour
    // GetCurrentCharacteristics
    // IsSorceryTime
    // GetGlobalIndex
    // IsHuman
    // IsAI
    // MyTurn
    // PreventNextDamage
    // PreventDamage
    // Protection
    // MoveLocalZone
    // ChooseColour
    // GetNumericalChoiceResult
    // GetMultipleChoiceResult
    // AskNumericalChoiceQuestion
    // AddNumericalChoiceAnswer
    // BeginNewNumericalChoice
    // AskMultipleChoiceQuestion
    // AddMultipleChoiceAnswer
    // BeginNewMultipleChoice
    // SetItemPrompt
    // SetItemCount
    // ChooseItemsFromDC
    // ChooseItemFromDC
    // ChooseItems
    // ChooseItem
    // LookAtPlayersHand

private:
};

}  // namespace woth

#endif  // WOTH_PLAYER_BASE_PLAYER_HPP
