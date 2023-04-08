#ifndef DECKEDITORMENU_H
#define DECKEDITORMENU_H

#pragma once
#include "DeckMenu.h"
#include "DeckDataWrapper.h"
#include "DeckStats.h"

class DeckEditorMenu : public DeckMenu {
protected:
    string deckTitle;

private:
    void drawDeckStatistics();

    DeckDataWrapper* selectedDeck;
    StatsWrapper* stw;

public:
    DeckEditorMenu(int id, JGuiListener* listener = nullptr, int fontId = 1, const char* _title = "",
                   DeckDataWrapper* selectedDeck = nullptr, StatsWrapper* stats = nullptr);
    void Render() override;
    ~DeckEditorMenu() override;
};

#endif
