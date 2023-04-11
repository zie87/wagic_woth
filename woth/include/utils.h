#ifndef UTILS_H
#define UTILS_H

#include "woth/string/algorithms.hpp"

#include <JGE.h>

#if defined(PSP)
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspiofilemgr.h>
#include <pspdebug.h>
#include <psputility.h>
#include <pspgu.h>
#include <psprtc.h>

#include <psptypes.h>
#include <malloc.h>

#endif

#include <math.h>
#include <stdio.h>
#include <string>

#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

#include <random>
#include <chrono>

#include "DebugRoutines.h"

namespace wth {

inline void replace_all(std::string& subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

template <class RandomIt>
inline void shuffle(RandomIt first, RandomIt last, unsigned seed) {
    std::shuffle(first, last, std::default_random_engine(seed));
}

template <class RandomIt>
inline void shuffle(RandomIt first, RandomIt last) {
    // TODO: change to jge internal clock!
    const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(first, last, seed);
}

}  // namespace wth

using std::string;

std::string join(std::vector<std::string>& v, const std::string& delim = " ");

// A simple parsing function
// splits string s by finding the first occurence of start, and the first occurence of stop, and returning
// a vector of 3 strings. The first string is everything before the first occurence of start, the second string is
// everything between start and stop the third string is everything after stop. for example, parseBetween ("this is a
// function(foo) call", "function(", ")") will return: ["this is a ", "foo", " call"];
// If an error occurs, returns an empty vector.
// if "stopRequired" is set to false, the function will return a vector of 3 strings even if "stop" is not found in the
// string.
std::vector<std::string>& parseBetween(const std::string& s,
                                       const std::string& start,
                                       const std::string& stop,
                                       bool stopRequired,
                                       std::vector<std::string>& elems);
std::vector<std::string> parseBetween(const std::string& s,
                                      const std::string& start,
                                      const std::string& stop,
                                      bool stopRequired = true);

std::string wordWrap(const std::string& s, float width, int fontId);

// basic hash function
unsigned long hash_djb2(const char* str);

// This class wraps random generation and the pre-loading/saving of randoms
// The idea is to make it instantiable to be able to handle randoms differently per class of group of classes.
// In particular, to be able to control the AI randoms independently of the other game randoms so that we can actually
// test AI
class RandomGenerator {
protected:
    std::list<int> loadedRandomValues;
    std::list<int> usedRandomValues;
    bool log;

public:
    RandomGenerator(bool doLog = false) : log(doLog){};
    void loadRandValues(std::string s);
    std::ostream& saveUsedRandValues(std::ostream& out) const;
    std::ostream& saveLoadedRandValues(std::ostream& out);
    int random();
    template <typename Iter>
    void random_shuffle(Iter first, Iter last) {
        ptrdiff_t i;
        ptrdiff_t n;
        n = (last - first);
        for (i = n - 1; i > 0; --i) {
            using std::swap;
            swap(first[i], first[random() % (i + 1)]);
        }
    };
};

int WRand(bool log = false);

#ifdef LINUX
void dumpStack();
#endif

/* RAM simple check functions header */

// *** DEFINES ***

#if defined(WIN32) || defined(LINUX)
#define RAM_BLOCK (100 * 1024 * 1024)
#else
#define RAM_BLOCK (1024 * 1024)
#endif

// *** FUNCTIONS DECLARATIONS ***

u32 ramAvailableLineareMax(void);
u32 ramAvailable(void);

/*
#ifdef WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
*/

bool fileExists(const char* filename);
bool FileExists(const std::string& filename);
std::string buildFilePath(const std::vector<std::string>& folders, const std::string& filename);
std::string ensureFolder(const std::string& folderName);
/*
template <class T> istream& operator>>(istream& in, T& p)
{
    string s;

    while(std::getline(in, s))
    {
        if(!p.parseLine(s))
        {
            break;
        }
    }

    return in;
}
*/
#endif
