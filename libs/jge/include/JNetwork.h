#ifndef JNETWORK_H
#define JNETWORK_H

// Network support for PSP
// #define NETWORK_SUPPORT

#include "JGE.h"
#include <string>
#include <map>

#include <iostream>
#include <sstream>
#include "Threading.h"

class JSocket;
typedef void (*processCmd)(std::istream&, std::ostream&);

class JNetwork {
private:
    int connected_to_ap;
    JSocket* socket;
    jge::mutex sendMutex;
    jge::mutex receiveMutex;
    std::stringstream received;
    std::stringstream toSend;
    static std::map<std::string, processCmd> sCommandMap;

public:
    JNetwork();
    ~JNetwork();
    std::string serverIP;

    int connect(std::string serverIP = "");
    bool isConnected();
    static void ThreadProc(void* param);
#if !defined(WIN32) && !defined(LINUX)
    static int connect_to_apctl(int config);
#endif
    bool sendCommand(std::string command);
    static void registerCommand(const std::string& command, processCmd processCommand, processCmd processResponse);

private:
    jge::thread* mpWorkerThread;
};

#endif
