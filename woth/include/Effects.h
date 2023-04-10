#ifndef EFFECTS_H
#define EFFECTS_H

#include <JGui.h>

class Effect : public JGuiObject {
    static int id_counter;

public:
    Effect() : JGuiObject(++id_counter){};
};

#endif  // EFFECTS_H
