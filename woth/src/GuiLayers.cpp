#include "PrecompiledHeader.h"

#include "GuiLayers.h"
#include "Player.h"
#include "AllAbilities.h"

GuiLayer::GuiLayer(GameObserver* observer)
    : mActionButton(JGE_BTN_OK)
    , observer(observer)
    , hasFocus(false)
    , mCurr(0)
    , modal(0) {}

GuiLayer::~GuiLayer() { resetObjects(); }

void GuiLayer::Add(JGuiObject* object) {
    mObjects.push_back(object);
    const AbilityFactory af(observer);

    auto* a = dynamic_cast<MTGAbility*>(object);

    if (a != nullptr) {
        auto* manaObject = dynamic_cast<AManaProducer*>(af.getCoreAbility(a));
        if (manaObject) {
            manaObjects.push_back(object);
        }
    }
}

int GuiLayer::Remove(JGuiObject* object) {
    const AbilityFactory af(observer);
    auto* a = dynamic_cast<MTGAbility*>(object);

    if (a != nullptr) {
        auto* manaObject = dynamic_cast<AManaProducer*>(af.getCoreAbility(a));
        if (manaObject) {
            for (size_t i = 0; i < manaObjects.size(); i++) {
                if (manaObjects[i] == object) {
                    manaObjects.erase(manaObjects.begin() + i);
                }
            }
        }
    }
    for (size_t i = 0; i < mObjects.size(); i++) {
        if (mObjects[i] == object) {
            delete mObjects[i];
            mObjects.erase(mObjects.begin() + i);
            if (mCurr == (int)(mObjects.size())) {
                mCurr = 0;
            }
            return 1;
        }
    }
    return 0;
}

int GuiLayer::getMaxId() const { return (int)(mObjects.size()); }

void GuiLayer::Render() {
    for (size_t i = 0; i < mObjects.size(); i++) {
        if (mObjects[i] != nullptr) {
            mObjects[i]->Render();
        }
    }
}

void GuiLayer::Update(float dt) {
    for (size_t i = 0; i < mObjects.size(); i++) {
        if (mObjects[i] != nullptr) {
            mObjects[i]->Update(dt);
        }
    }
}

void GuiLayer::resetObjects() {
    for (size_t i = 0; i < mObjects.size(); i++) {
        if (mObjects[i]) {
            // big, ugly hack around CardView / MTGCardInstance ownership problem - these two classes have an
            // interdependency with naked pointers, but the order of destruction can leave a dangling pointer reference
            // inside of a CardView, which attempts to clean up its own pointer reference in an MTGCardInstance when
            // it's destroyed.  Ideally, CardView should only hold onto a weak reference, but that's a bigger overhaul.
            // For now, if we get here, clear out the MTGCardInstance pointer of a CardView before calling delete.
            auto* cardView = dynamic_cast<CardView*>(mObjects[i]);
            if (cardView) {
                cardView->card = nullptr;
            }
            delete mObjects[i];
        }
    }
    mObjects.clear();
    mCurr = 0;
}

int GuiLayer::getIndexOf(JGuiObject* object) {
    for (size_t i = 0; i < mObjects.size(); i++) {
        if (mObjects[i] == object) {
            return i;
        }
    }
    return -1;
}

JGuiObject* GuiLayer::getByIndex(int index) { return mObjects[index]; }
