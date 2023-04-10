#ifndef JLOGGER_H
#define JLOGGER_H
// logging facility
// #define DOLOG

#include <string>
// The PSP one is to log stuff in JLogger's lastLog, it does not do full log in a text file unless DOLOG is defined
#if defined(DOLOG) || defined(PSP)
#define LOG(x) JLogger::Log(x);
#else
#define LOG(x) {};
#endif

// saving myself the pain of search/replace
#define JLOG(x) LOG(x)

#define LOG_FILE "debug.txt"

class JLogger {
public:
    static void Log(const char* text);
    static void Log(const std::string& text);

    JLogger(const char* text);
    ~JLogger();

    const char* mText;

    static std::string lastLog;
    static int lastTime;
};

#endif
