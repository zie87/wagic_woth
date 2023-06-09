#ifndef TOKEN_H
#define TOKEN_H

#include "MTGCardInstance.h"

class Token : public MTGCardInstance {
    MTGCardInstance* tokenSource;

public:
    Token(string _name, MTGCardInstance* source, int _power = 0, int _toughness = 0);
    Token(int id);
    Token(const Token& source);
    MTGCardInstance* clone() override;
};

#endif
