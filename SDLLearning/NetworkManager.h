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
    void sendPacket(PacketType type, const void* data, size_t size, uint8_t channel = 0);

    void registerServerEventHandlers();
    void addClientConnection(uint64_t steamID, ENetPeer* peer);

    CSteamID getClientSteamID(ENetPeer* client);
    uint64_t getLocalSteamID() const;

    PacketDispatcher& getDispatcher() { return dispatcher; }

#ifdef DEDICATED_SERVER
    std::function<void(uint64_t steamID, ENetPeer* peer)> onPlayerAuthenticated;
    std::function<void(uint64_t steamID, const PlayerInputPacket&)> inputCallback;
#else
    // names aren't great here..1 is for initial connect, other is for updates after the fact.
    std::function<void(const ServerStatePacket&)> serverStateCallback;
    std::function<void(const ServerStatePacket&)> onPlayerStateReceived;
#endif

#ifdef DEDICATED_SERVER
    bool startServer(int port = 1234);
    bool initSteamServer();
    bool isSteamServerStarted() const { return steamServerStarted; }
    
    void sendPacketToPeer(PacketType type, const void* data, size_t size, ENetPeer* peer, uint8_t channel);

    
    void broadcastToAllExcept(PacketType type, const void* data, size_t size, ENetPeer* exclude, uint8_t channel);
#else
    bool startClient(const std::string& host, int port = 1234);
    void setServerStateCallback(std::function<void(const ServerStatePacket&)> callback);
#endif

private:
#ifdef DEDICATED_SERVER
    ENetHost* server = nullptr;
    bool steamServerStarted = false;
    std::unordered_map<ENetPeer*, CSteamID> clientSteamIDs;

    void handleAuthTicketPacket(ENetPacket* packet, ENetPeer* peer);
    
#endif
    ENetPeer* serverPeer = nullptr;
    ENetHost* client = nullptr;
    bool clientStarted = false;

    uint64_t localSteamID;

    HAuthTicket authTicket = k_HAuthTicketInvalid;
    uint8 ticketBuffer[1024];
    uint32 ticketSize = 0;
    PacketDispatcher dispatcher;
};