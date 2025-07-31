#pragma once
#include <enet/enet.h>
#include <string>
#include <steam/steamtypes.h>
#include <steam/steamclientpublic.h>
#include <steam/isteamgameserver.h>
#include <steam/steam_gameserver.h>
#include <unordered_map>
#include "PacketDispatcher.h"
#include "NetworkPackets.h"


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
    void sendPacket(PacketType type, const void* data, size_t size, uint8_t channel = 0, uint32_t flags = 1);

    void registerServerEventHandlers();
    void addClientConnection(uint64_t steamID, ENetPeer* peer);

    CSteamID getClientSteamID(ENetPeer* client);
    uint64_t getLocalSteamID() const;

    PacketDispatcher& getDispatcher() { return dispatcher; }
    ENetHost* getClient() const { return client; }
	
    std::function<void(const ServerStatePacket&)> serverStateCallback;
    std::function<void(const ServerStatePacket&)> onPlayerStateReceived;
    std::function<void(const ServerStatePacket&)> otherPlayerConnected;

    bool startClient(const std::string& host, int port = 1234);


private:
    ENetPeer* serverPeer = nullptr;
    ENetHost* client = nullptr;
    bool clientStarted = false;

    uint64_t localSteamID;

    HAuthTicket authTicket = k_HAuthTicketInvalid;
    uint8 ticketBuffer[1024];
    uint32 ticketSize = 0;
    PacketDispatcher dispatcher;
};