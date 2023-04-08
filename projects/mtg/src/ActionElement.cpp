#include "PrecompiledHeader.h"

#include "ActionElement.h"
#include "MTGCardInstance.h"
#include "Targetable.h"
#include "TargetChooser.h"

ActionElement::ActionElement(int id)
    : JGuiObject(id)
    , activeState(INACTIVE)
    , currentPhase(MTG_PHASE_INVALID)
    , modal(0)
    , newPhase(MTG_PHASE_INVALID)
    , tc(nullptr)
    , waitingForAnswer(0) {}

ActionElement::ActionElement(const ActionElement& a)
    : JGuiObject(a)
    , activeState(a.activeState)
    , currentPhase(a.currentPhase)
    , modal(a.modal)
    , newPhase(a.newPhase)
    , tc(a.tc ? a.tc->clone() : nullptr)
    , waitingForAnswer(a.waitingForAnswer) {}

ActionElement::~ActionElement() { SAFE_DELETE(tc); }

int ActionElement::getActivity() const { return activeState; }

int ActionElement::isReactingToTargetClick(Targetable* object) {
    if (auto* cObject = dynamic_cast<MTGCardInstance*>(object)) {
        return isReactingToClick(cObject);
    }
    return 0;
}

int ActionElement::reactToTargetClick(Targetable* object) {
    if (auto* cObject = dynamic_cast<MTGCardInstance*>(object)) {
        return reactToClick(cObject);
    }
    return 0;
}
