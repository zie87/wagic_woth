#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "JTypes.h"

#include "CardSelector.h"
#include "DuelLayers.h"

#include <map>
#include <vector>

// private class only used by Navigator, see implementation file
class CardZone;

class Navigator : public CardSelectorBase {
public:
    Navigator(GameObserver* observer, DuelLayers* inDuelLayers);
    ~Navigator() override;

    // Inherited functions from GuiLayer
    bool CheckUserInput(JButton inKey) override;
    bool CheckUserInput(int x, int y);
    void Update(float dt) override;
    void Render() override;

    // Limitor operations
    void PushLimitor() override;
    void PopLimitor() override;
    void Limit(LimitorFunctor<PlayGuiObject>* inLimitor, CardView::SelectorZone inZone) override;

    void Add(PlayGuiObject*) override;
    void Remove(PlayGuiObject*) override;
    void Push() override {}
    void Pop() override {}

protected:
    PlayGuiObject* GetCurrentCard();

    /**
     ** Helper function that translates a card type into an internal zone ID (used as the index for the card zone map)
     */
    int CardToCardZone(PlayGuiObject* card);

    void HandleKeyStroke(JButton inKey);

private:
    std::map<int, CardZone*> mCardZones;
    CardZone* mCurrentZone;
    Pos mDrawPosition;

    DuelLayers* mDuelLayers;

    bool mLimitorEnabled;
    std::stack<CardZone*> mCurrentZoneStack;
};

#endif  // NAVIGATOR_H
