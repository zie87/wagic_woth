#ifndef NETWORKPLAYER_H
#define NETWORKPLAYER_H

#include "PrecompiledHeader.h"
#include "Player.h"
#include "JNetwork.h"

extern void RegisterNetworkPlayers();

class ProxyPlayer {
protected:
    Player* mpPlayer;
    JNetwork* mpNetwork;
    static ProxyPlayer* mInstance;

public:
    ProxyPlayer(Player* pxPlayer, JNetwork* pxNetwork);
    static void Serialize(std::istream& in, std::ostream& out);
};

class RemotePlayer : public Player {
protected:
    JNetwork* mpNetwork;
    static RemotePlayer* mInstance;

public:
    RemotePlayer(JNetwork*);
    static void Deserialize(std::istream& in, std::ostream& out);
    bool isLoaded() { return game != NULL; };
};

#endif  // NETWORKPLAYER_H
