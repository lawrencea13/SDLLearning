#pragma once
#include <enet/enet.h>
#include <string>
#include <steam/steamtypes.h>
#include <steam/steamclientpublic.h>
#include <steam/isteamgameserver.h>
#include <steam/steam_gameserver.h>
#include <unordered_map>
#include "PacketDispatcher.h"
#include "Shared/NetworkPackets.h"


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


    PacketDispatcher& getDispatcher() { return dispatcher; }

    std::function<void(uint64_t steamID, ENetPeer* peer)> onPlayerAuthenticated;
    std::function<void(uint64_t steamID, const PlayerInputPacket&)> inputCallback;
    std::function<void(uint64_t steamID)> disconnectCallback;

    bool startServer(int port = 1234);
    bool initSteamServer();
    bool isSteamServerStarted() const { return steamServerStarted; }

	ENetHost* getServer() const { return server; }
    
    void sendPacketToPeer(PacketType type, const void* data, size_t size, ENetPeer* peer, uint8_t channel, uint32_t flags = 1);

    
    void broadcastToAllExcept(PacketType type, const void* data, size_t size, ENetPeer* exclude, uint8_t channel, uint32_t flags = 1);

    void broadcastToAll(PacketType type, const void* data, size_t size, uint8_t channel, uint32_t flags = 1);


private:
    ENetHost* server = nullptr;
    bool steamServerStarted = false;
    std::unordered_map<ENetPeer*, CSteamID> clientSteamIDs;

    void handleAuthTicketPacket(ENetPacket* packet, ENetPeer* peer);
	int handlePeerDisconnect(ENetPeer* peer);
    
    ENetPeer* serverPeer = nullptr;
    ENetHost* client = nullptr;
    bool clientStarted = false;

    HAuthTicket authTicket = k_HAuthTicketInvalid;
    uint8 ticketBuffer[1024];
    uint32 ticketSize = 0;
    PacketDispatcher dispatcher;
};