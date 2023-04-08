#include "NetworkPlayer.h"
#include <sstream>

RemotePlayer* RemotePlayer::mInstance = NULL;
ProxyPlayer* ProxyPlayer::mInstance   = NULL;

void RegisterNetworkPlayers() {
    //  JNetwork::registerCommand("GetPlayer", ProxyPlayer::Serialize, RemotePlayer::Deserialize);
}

RemotePlayer::RemotePlayer(JNetwork* pxNetwork) : Player("remote", "", NULL), mpNetwork(pxNetwork) {
    mInstance = this;
    mpNetwork->sendCommand("GetPlayer");
}

void RemotePlayer::Deserialize(std::istream& in, std::ostream& out) {
    //    istringstream ss(mInstance->mpNetwork->receiveString());
    in >> *mInstance;
}

ProxyPlayer::ProxyPlayer(Player* pxPlayer, JNetwork* pxNetwork) : mpPlayer(pxPlayer), mpNetwork(pxNetwork) {
    mInstance = this;
    JNetwork::registerCommand("GetPlayer", ProxyPlayer::Serialize, RemotePlayer::Deserialize);

    //  ostringstream ss;
    //  ss << "Player : " << *mpPlayer;
    //  mpNetwork->send(ss.str());
}

void ProxyPlayer::Serialize(std::istream& in, std::ostream& out) { out << *(mInstance->mpPlayer); }
