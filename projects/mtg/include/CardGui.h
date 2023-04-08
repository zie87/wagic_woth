/* Graphical representation of a Card Instance, used in game */

#ifndef CARDGUI_H
#define CARDGUI_H

#include <hge/hgeparticle.h>
#include <JGui.h>
#include "Pos.h"
#include "PlayGuiObject.h"
#include "WResource_Fwd.h"

class MTGCard;
class MTGCardInstance;
class PlayGuiObject;

namespace DrawMode {
enum { kNormal = 0, kText, kHidden };
const int kNumDrawModes = 3;
}  // namespace DrawMode

struct CardGui : public PlayGuiObject {
protected:
    static std::map<std::string, std::string> counterGraphics;

    /*
    ** Tries to render the Big version of a card picture, backups to text version in case of failure
    */
    static void RenderBig(MTGCard* card, const Pos& pos);

    static void RenderCountersBig(MTGCard* card, const Pos& pos, int drawMode = DrawMode::kNormal);
    static void AlternateRender(MTGCard* card, const Pos& pos);
    static void TinyCropRender(MTGCard* card, const Pos& pos, JQuad* quad);
    static std::string FormattedData(std::string data, const std::string& replace, std::string value);
    static bool FilterCard(MTGCard* card, std::string filter);

public:
    static const float Width;
    static const float Height;
    static const float BigWidth;
    static const float BigHeight;

    PIXEL_TYPE mMask;

    MTGCardInstance* card;
    CardGui(MTGCardInstance* card, float x, float y);
    CardGui(MTGCardInstance* card, const Pos& ref);
    void Render() override;
    void Update(float dt) override;

    void DrawCard(const Pos& inPosition, int inMode = DrawMode::kNormal) const;
    static void DrawCard(MTGCard* inCard, const Pos& inPosition, int inMode = DrawMode::kNormal);

    static JQuadPtr AlternateThumbQuad(MTGCard* card);
    std::ostream& toString(std::ostream&) const override;
};

class CardView : public CardGui {
public:
    typedef enum { nullZone, handZone, playZone } SelectorZone;

    const SelectorZone owner;

    MTGCardInstance* getCard();  // remove this when possible
    CardView(const SelectorZone, MTGCardInstance* card, float x, float y);
    CardView(const SelectorZone, MTGCardInstance* card, const Pos& ref);
    ~CardView() override;

    void Render() override { CardGui::Render(); }

    void Render(JQuad* q) { Pos::Render(q); }

    std::ostream& toString(std::ostream&) const override;

    float GetCenterX();
    float GetCenterY();
};

class TransientCardView : public CardGui {
public:
    TransientCardView(MTGCardInstance* card, float x, float y);
    TransientCardView(MTGCardInstance* card, const Pos& ref);
};

class SimpleCardEffect {
public:
    virtual ~SimpleCardEffect()        = default;
    virtual void doEffect(Pos* card)   = 0;
    virtual void undoEffect(Pos* card) = 0;
};

class SimpleCardEffectRotate : public SimpleCardEffect {
protected:
    float mRotation;

public:
    ~SimpleCardEffectRotate() override = default;
    SimpleCardEffectRotate(float rotation);
    void doEffect(Pos* card) override;
    void undoEffect(Pos* card) override;
};

class SimpleCardEffectMask : public SimpleCardEffect {
protected:
    PIXEL_TYPE mMask;

public:
    ~SimpleCardEffectMask() override = default;
    SimpleCardEffectMask(PIXEL_TYPE mask);
    void doEffect(Pos* card) override;
    void undoEffect(Pos* card) override;
};

#endif
