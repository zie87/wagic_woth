#include "PrecompiledHeader.h"

#include "utils.h"
#include "MTGDefinitions.h"
#include "Subtypes.h"
#include "WResourceManager.h"
#include "WFont.h"
#include <sys/stat.h>

#include <utility>

#ifdef PSP
#include "pspsdk.h"
#endif

namespace wagic {
#ifdef TRACK_FILE_USAGE_STATS
std::map<std::string, int> ifstream::sFileMap;
#endif
}  // namespace wagic

using std::queue;
using std::vector;

int RandomGenerator::random() {
    int result;
    if (loadedRandomValues.empty() || !log) {
        result = rand();
    } else {
        result = loadedRandomValues.front();
        loadedRandomValues.pop_front();
    }
    if (log) {
        usedRandomValues.push_back(result);
    }
    return result;
}

std::ostream& RandomGenerator::saveUsedRandValues(std::ostream& out) const {
    std::list<int>::const_iterator ite;
    for (ite = usedRandomValues.begin(); ite != usedRandomValues.end(); ite++) {
        out << *ite << ",";
    }

    return out;
}

std::ostream& RandomGenerator::saveLoadedRandValues(std::ostream& out) {
    std::list<int>::iterator ite;
    for (ite = loadedRandomValues.begin(); ite != loadedRandomValues.end(); ite++) {
        out << *ite << ",";
    }

    return out;
}

void RandomGenerator::loadRandValues(std::string s) {
    loadedRandomValues.clear();
    usedRandomValues.clear();

    while (!s.empty()) {
        unsigned int value;
        const size_t limiter = s.find(',');
        if (limiter != string::npos) {
            value = atoi(s.substr(0, limiter).c_str());
            s     = s.substr(limiter + 1);
        } else {
            value = atoi(s.c_str());
            s     = "";
        }
        if (value) {
            loadedRandomValues.push_back(value);
        }
    }
}

int WRand(bool log) { return rand(); }

bool fileExists(const char* filename) { return JFileSystem::GetInstance()->FileExists(filename); }

bool FileExists(const string& filename) { return JFileSystem::GetInstance()->FileExists(filename); }

/*
#ifdef LINUX

#include <execinfo.h>
void dumpStack()
{
void* buffer[50];
int s = backtrace(buffer, 50);
char** tab = backtrace_symbols(buffer, s);
for (int i = 1; i < s; ++i) printf("%s\n", tab[i]);
printf("\n");
free(tab);
}

#endif
*/

/* RAM simple check functions source */

// *** FUNCTIONS ***

u32 ramAvailableLineareMax(void) {
    u32 size;
    u32 sizeblock;
    u8* ram;

    // Init variables
    size      = 0;
    sizeblock = RAM_BLOCK;

#ifdef PSP
    int disableInterrupts = pspSdkDisableInterrupts();
#endif

    // Check loop
    while (sizeblock) {
        // Increment size
        size += sizeblock;

        // Allocate ram
        ram = (u8*)malloc(size);

        // Check allocate
        if (!(ram)) {
            // Restore old size
            size -= sizeblock;

            // Size block / 2
            sizeblock >>= 1;
        } else {
            free(ram);
        }
    }

#ifdef PSP
    pspSdkEnableInterrupts(disableInterrupts);
#endif

    return size;
}

u32 ramAvailable(void) {
    u8** ram;
    u8** temp;
    u32 size;
    u32 count;
    u32 x;

    // Init variables
    ram   = nullptr;
    size  = 0;
    count = 0;

#ifdef PSP
    int disableInterrupts = pspSdkDisableInterrupts();
#endif

    // Check loop
    for (;;) {
        // Check size entries
        if (!(count % 10)) {
            // Allocate more entries if needed
            temp = (u8**)realloc(ram, sizeof(u8*) * (count + 10));
            if (!(temp)) {
                break;
            }

            // Update entries and size (size contains also size of entries)
            ram = temp;
            size += (sizeof(u8*) * 10);
        }

        // Find max lineare size available
        x = ramAvailableLineareMax();
        if (!(x)) {
            break;
        }

        // Allocate ram
        ram[count] = (u8*)malloc(x);
        if (!(ram[count])) {
            break;
        }

        // Update variables
        size += x;
        count++;
    }

    // Free ram
    if (ram) {
        for (x = 0; x < count; x++) {
            free(ram[x]);
        }
        free(ram);
    }

#ifdef PSP
    pspSdkEnableInterrupts(disableInterrupts);
#endif
    return size;
}

std::vector<std::string>& parseBetween(const std::string& s,
                                       const string& start,
                                       const string& stop,
                                       bool stopRequired,
                                       std::vector<std::string>& elems) {
    const size_t found = s.find(start);
    if (found == string::npos) {
        return elems;
    }

    const size_t offset = found + start.size();
    const size_t end    = s.find(stop, offset);
    if (end == string::npos && stopRequired) {
        return elems;
    }

    elems.push_back(s.substr(0, found));
    if (end != string::npos) {
        elems.push_back(s.substr(offset, end - offset));
        elems.push_back(s.substr(end + 1));
    } else {
        elems.push_back(s.substr(offset));
        elems.emplace_back("");
    }

    return elems;
}

std::vector<std::string> parseBetween(const std::string& s,
                                      const string& start,
                                      const string& stop,
                                      bool stopRequired) {
    std::vector<std::string> elems;
    return parseBetween(s, std::move(start), std::move(stop), stopRequired, elems);
}

// This is a customized word wrap based on pixel width.  It tries it's best
// to wrap strings using spaces as delimiters.
// Not sure how this translates into non-english fonts.
std::string wordWrap(const std::string& sentence, float width, int fontId) {
    WFont* mFont          = WResourceManager::Instance()->GetWFont(fontId);
    const float lineWidth = mFont->GetStringWidth(sentence.c_str());
    string retVal         = sentence;
    if (lineWidth < width) {
        return sentence;
    }

    int numLines = 1;
    int breakIdx = 0;
    for (size_t idx = 0; idx < sentence.length(); idx++) {
        if (sentence[idx] == ' ') {
            const string currentSentence = sentence.substr(breakIdx, idx - breakIdx);
            const float stringLength     = mFont->GetStringWidth(currentSentence.c_str());
            if (stringLength >= width) {
                if (stringLength > width) {
                    while (sentence[idx - 1] != ' ') {
                        idx--;
                    }
                }
                retVal[idx - 1] = '\n';
                breakIdx        = idx;
                numLines++;
            }
        } else if (sentence[idx] == '\n') {
            const string currentSentence = sentence.substr(breakIdx, idx - breakIdx);
            const float stringLength     = mFont->GetStringWidth(currentSentence.c_str());
            if (stringLength >= width) {
                if (stringLength > width) {
                    while (sentence[idx - 1] != ' ') {
                        idx--;
                    }
                    retVal[idx - 1] = '\n';
                }
                numLines++;
            }
            breakIdx = idx;
            numLines++;
        }
    }

    return retVal;
}

unsigned long hash_djb2(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

std::string buildFilePath(const vector<string>& folders, const string& filename) {
    string result;
    for (size_t i = 0; i < folders.size(); ++i) {
        result.append(folders[i]);
        if (result[result.length() - 1] != '/') {
            result.append("/");
        }
    }

    result.append(filename);
    return result;
}
std::string ensureFolder(const std::string& folderName) {
    if (folderName.empty()) {
        return "";
    }

    string result = folderName;
    if (result[result.length() - 1] != '/') {
        result.append("/");
    }
    return result;
}
