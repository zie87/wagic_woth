/*
 *  Wagic, The Homebrew ?! is licensed under the BSD license
 *  See LICENSE in the Folder's root
 *  http://wololo.net/wagic/
 */

#ifndef ACTIONLAYER_H
#define ACTIONLAYER_H

#include "GuiLayers.h"
#include "ActionElement.h"
#include "SimpleMenu.h"
#include "MTGAbility.h"

#include <set>

class GuiLayer;
class Targetable;
class WEvent;

class ActionLayer : public GuiLayer, public JGuiListener {
public:
    vector<ActionElement*> garbage;
    Targetable* menuObject;
    SimpleMenu* abilitiesMenu;
    SimpleMenu* abilitiesTriggered;
    MTGCardInstance* currentActionCard;
    int stuffHappened;
    void Render() override;
    void Update(float dt) override;
    bool CheckUserInput(JButton key) override;
    ActionLayer(GameObserver* observer);
    ~ActionLayer() override;
    int cancelCurrentAction();
    ActionElement* isWaitingForAnswer();
    int isReactingToTargetClick(Targetable* card);
    int receiveEventPlus(WEvent* event) override;
    int reactToTargetClick(Targetable* card);
    int isReactingToClick(MTGCardInstance* card);
    bool getMenuIdFromCardAbility(MTGCardInstance* card, MTGAbility* ability, int& menuId);
    int reactToClick(MTGCardInstance* card);
    int reactToClick(ActionElement* ability, MTGCardInstance* card);
    int reactToTargetClick(ActionElement* ability, Targetable* card);
    int stillInUse(MTGCardInstance* card);
    void setMenuObject(Targetable* object, bool must = false);
    void setCustomMenuObject(Targetable* object, bool must = false,
                             vector<MTGAbility*> abilities = vector<MTGAbility*>());
    void ButtonPressed(int controllerid, int controlid) override;
    void ButtonPressedOnMultipleChoice(int choice = -1);
    void doReactTo(int menuIndex);
    TargetChooser* getCurrentTargetChooser();
    void setCurrentWaitingAction(ActionElement* ae);
    MTGAbility* getAbility(int type);
    int checkCantCancel() const { return cantCancel; };

    // Removes from game but does not move the element to garbage. The caller must take care of deleting the element.
    int removeFromGame(ActionElement* e);

    bool moveToGarbage(ActionElement* e);

    void cleanGarbage();

protected:
    ActionElement* currentWaitingAction;
    int cantCancel;
    std::set<ActionElement*> mReactions;
};

#endif
