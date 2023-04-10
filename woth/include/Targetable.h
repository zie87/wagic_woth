#ifndef TARGETABLE_H
#define TARGETABLE_H

class Targetable {
protected:
    GameObserver* observer;

public:
    virtual ~Targetable() = default;
    Targetable(GameObserver* observer) : observer(observer){};
    virtual const string getDisplayName() const = 0;
    inline GameObserver* getObserver() { return observer; };
    virtual void setObserver(GameObserver* g) { observer = g; };
};

#endif
