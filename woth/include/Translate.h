#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <string>
#include <map>

#if defined _DEBUG
#define DEBUG_TRANSLATE
#endif

class Translator {
protected:
    static Translator* mInstance;
    bool initDone;

    void load(const std::string& filename, std::map<std::string, std::string>* dictionary);

public:
    std::map<std::string, std::string> values;
    std::map<std::string, std::string> tempValues;
    std::map<std::string, std::string> deckValues;
#if defined DEBUG_TRANSLATE
    std::map<std::string, int> missingValues;
    std::map<std::string, int> dontCareValues;
    int checkMisses;
#endif
    std::string translate(std::string toTranslate);
    Translator();
    ~Translator();
    int Add(const std::string& from, std::string to);
    void initCards();
    void initDecks();
    void init();
    static Translator* GetInstance();
    static void EndInstance();
};

std::string _(std::string toTranslate);

extern bool neofont;
#endif
