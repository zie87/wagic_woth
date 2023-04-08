#include "PrecompiledHeader.h"

#include "AbilityParser.h"
#include "utils.h"

using std::string;
using std::vector;

vector<AutoLineMacro*> AutoLineMacro::gAutoLineMacros;
map<string, bool> AutoLineMacro::gAutoLineMacrosIndex;

AutoLineMacro::AutoLineMacro(const string& s) { parse(s); }

void AutoLineMacro::parse(const string& stringMacro) {
    string s = stringMacro;
    // we convert to lower, because the counterpart (auto strings) is converted to lower at parse time
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    const size_t firstSpace = s.find(' ');
    if (firstSpace == string::npos) {
        DebugTrace("FATAL:error parsing macro : " << s);
        return;
    }

    const size_t firstParenthesis = s.find('(');

    if (firstParenthesis != string::npos && firstParenthesis < firstSpace) {
        // The string has params
        mName                                = s.substr(0, firstParenthesis);
        const size_t firstClosingParenthesis = s.find(')');
        const string params = s.substr(firstParenthesis + 1, firstClosingParenthesis - (firstParenthesis + 1));
        mParams             = split(params, ',');
        mResult             = s.substr(firstClosingParenthesis + 2);
    } else {
        // no params
        mName   = s.substr(0, firstSpace);
        mResult = s.substr(firstSpace + 1);
    }

    wth::replace_all(mResult, "\\n", "\n");
}

string AutoLineMacro::process(const string& s) {
    string temp = s;
    if (mParams.empty()) {
        // no params, simple macro
        wth::replace_all(temp, mName, mResult);
        return temp;
    }

    // params, complex macro
    const string toFind = mName + "(";
    string result;
    size_t occurence = temp.find(toFind);
    if (occurence == string::npos) {
        return s;
    }

    while (occurence != string::npos) {
        result.append(temp.substr(0, occurence));
        const size_t closingParenthesis = temp.find(')');
        const size_t paramsStart        = occurence + toFind.length();
        const string params             = temp.substr(paramsStart, closingParenthesis - paramsStart);

        vector<string> vParams = split(params, ',');
        if (vParams.size() != mParams.size()) {
            return s;
        }
        string tempResult = mResult;
        for (size_t i = 0; i < vParams.size(); ++i) {
            wth::replace_all(tempResult, mParams[i], vParams[i]);
        }
        result.append(tempResult);

        temp      = temp.substr(closingParenthesis + 1);
        occurence = temp.find(toFind);
    }
    result.append(temp);
    return result;
}

bool AutoLineMacro::AddMacro(const string& s) {
    auto* alm = NEW AutoLineMacro(s);
    if (gAutoLineMacrosIndex[alm->mName]) {
        DebugTrace("WARNING, Macro already exists: " << alm->mName);
        delete alm;
        return false;
    }

    gAutoLineMacrosIndex[alm->mName] = true;
    gAutoLineMacros.push_back(alm);
    return true;
}

void AutoLineMacro::Destroy() {
    for (size_t i = 0; i < gAutoLineMacros.size(); ++i) {
        SAFE_DELETE(gAutoLineMacros[i]);
    }
}

string AutoLineMacro::Process(const string& s) {
    string result = s;
    for (size_t i = 0; i < gAutoLineMacros.size(); ++i) {
        result = gAutoLineMacros[i]->process(result);
    }
    return result;
}
