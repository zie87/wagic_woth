#ifndef TRANSLATEKEYS_H
#define TRANSLATEKEYS_H

#include <utility>
#include <string>
#include "JGE.h"

typedef std::pair<std::string, JQuad*> KeyRep;
const KeyRep& translateKey(LocalKeySym);
const KeyRep& translateKey(JButton);

#endif
