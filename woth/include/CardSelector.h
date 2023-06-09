#ifndef CARDSELECTOR_H
#define CARDSELECTOR_H

#include <vector>
#include <stack>
#include "CardGui.h"
#include "GuiLayers.h"
#include "Pos.h"

using std::vector;

class PlayGuiObject;
class DuelLayers;

// The X lib annoyingly defines True to be 1, leading to
// hard to understand syntax errors. Not using it, so it's
// safe to undefine it.
#ifdef True
#undef True
#endif

template <typename T>
struct LimitorFunctor {
    virtual ~LimitorFunctor() = default;
    virtual bool select(T*)   = 0;
    virtual bool greyout(T*)  = 0;
    typedef T Target;
};

class CardSelectorBase : public GuiLayer {
public:
    CardSelectorBase(GameObserver* observer, int inDrawMode = DrawMode::kNormal)
        : GuiLayer(observer)
        , mDrawMode(inDrawMode)

              {};
    virtual void Add(PlayGuiObject*)                                                            = 0;
    virtual void Remove(PlayGuiObject*)                                                         = 0;
    bool CheckUserInput(JButton key) override                                                   = 0;
    virtual void PushLimitor()                                                                  = 0;
    virtual void PopLimitor()                                                                   = 0;
    virtual void Limit(LimitorFunctor<PlayGuiObject>* inLimitor, CardView::SelectorZone inZone) = 0;
    virtual void Push()                                                                         = 0;
    virtual void Pop()                                                                          = 0;
    virtual int GetDrawMode() { return mDrawMode; }

protected:
    int mDrawMode;
};

class CardSelector : public CardSelectorBase {
public:
    struct SelectorMemory {
        PlayGuiObject* object;
        float x, y;
        SelectorMemory(PlayGuiObject* object);
        SelectorMemory();
    };

protected:
    std::vector<PlayGuiObject*> cards;
    PlayGuiObject* active;
    DuelLayers* duel;
    LimitorFunctor<PlayGuiObject>* limitor;
    Pos bigpos;
    std::map<const CardView::SelectorZone, SelectorMemory> lasts;
    std::stack<std::pair<LimitorFunctor<PlayGuiObject>*, CardView::SelectorZone> > limitorStack;
    std::stack<SelectorMemory> memoryStack;

    PlayGuiObject* fetchMemory(SelectorMemory&);

public:
    CardSelector(GameObserver* observer, DuelLayers*);
    void Add(PlayGuiObject*) override;
    void Remove(PlayGuiObject*) override;
    bool CheckUserInput(JButton key) override;
    void Update(float dt) override;
    void Render() override;
    void Push() override;
    void Pop() override;

    void Limit(LimitorFunctor<PlayGuiObject>* limitor, CardView::SelectorZone) override;
    void PushLimitor() override;
    void PopLimitor() override;

    typedef PlayGuiObject Target;
};

typedef LimitorFunctor<CardSelector::Target> Limitor;

struct Exp {
    static inline bool test(CardSelector::Target*, CardSelector::Target*);
};

#endif
