#pragma once
#include <enet/enet.h>
#include <string>
#include <steam/steamtypes.h>
#include <steam/steamclientpublic.h>
#include <steam/isteamgameserver.h>
#include <steam/steam_gameserver.h>
#include <unordered_map>


class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    void shutdown();
    void serviceNetwork();

    void update();

    bool isClientStarted() const { return clientStarted; }


    void generateSteamTicket();
    void sendAuthToServer();

    CSteamID getClientSteamID(ENetPeer* client);

#ifdef DEDICATED_SERVER
    bool startServer(int port = 1234);
    bool initSteamServer();
    bool isSteamServerStarted() const { return steamServerStarted; }
#else
    bool startClient(const std::string& host, int port = 1234);
#endif

private:
#ifdef DEDICATED_SERVER
    ENetHost* server = nullptr;
    bool steamServerStarted = false;
    std::unordered_map<ENetPeer*, CSteamID> clientSteamIDs;
#endif
    ENetPeer* serverPeer = nullptr;
    ENetHost* client = nullptr;
    bool clientStarted = false;

    HAuthTicket authTicket = k_HAuthTicketInvalid;
    uint8 ticketBuffer[1024];
    uint32 ticketSize = 0;
};