#ifndef COUNTERS_H
#define COUNTERS_H
#include <string>

using std::string;
class MTGCardInstance;

/* One family of counters. Ex : +1/+1 */
class Counter {
public:
    std::string name;
    int nb;
    int maxNb;
    int power, toughness;
    MTGCardInstance* target;
    Counter(MTGCardInstance* _target, int _power, int _toughness);
    Counter(MTGCardInstance* _target, const char* _name, int _power = 0, int _toughness = 0);
    int init(MTGCardInstance* _target, const char* _name, int _power, int _toughness);
    bool sameAs(const char* _name, int _power, int _toughness) const;
    bool cancels(int _power, int _toughness) const;
    int cancelCounter(int power, int toughness);
    int added() const;
    int removed() const;
};

/* Various families of counters attached to an instance of a card */
class Counters {
public:
    int mCount;
    std::vector<Counter*> counters;
    MTGCardInstance* target;
    Counters(MTGCardInstance* _target);
    ~Counters();
    int addCounter(const char* _name, int _power = 0, int _toughness = 0);
    int addCounter(int _power, int _toughness);
    int removeCounter(const char* _name, int _power = 0, int _toughness = 0);
    int removeCounter(int _power, int _toughness);
    Counter* hasCounter(const char* _name, int _power = 0, int _toughness = 0);
    Counter* hasCounter(int _power, int _toughness);
    Counter* getNext(Counter* previous = nullptr);
    int init();
};

#endif
