#include "NetworkManager.h"
#include <iostream>
#include <steam/steam_api.h>
#include <vector>



NetworkManager::NetworkManager() {}

NetworkManager::~NetworkManager() {
    shutdown();
}

void NetworkManager::shutdown() {
    if (server) {
        enet_host_destroy(server);
        server = nullptr;
    }

}

void NetworkManager::serviceNetwork() {
    if (!server) return;
    ENetEvent event;
    
    while (enet_host_service(server, &event, 10) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout << "A client connected.\n";
            enet_host_flush(server);
            break;

        case ENET_EVENT_TYPE_RECEIVE: {
            if (event.packet->dataLength < 1) {
                enet_packet_destroy(event.packet);
                break;
            }

            uint8_t packetType = event.packet->data[0];
            dispatcher.dispatch(packetType, event.packet, event.peer);

            enet_packet_destroy(event.packet);
            break;
        }

        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "A client disconnected.\n";
            
			// for now, not checking return value, just assume it was successful
            handlePeerDisconnect(event.peer);
            
            break;
        }
    }
}

void NetworkManager::update()
{
}





void NetworkManager::sendPacket(PacketType type, const void* data, size_t size, uint8_t channel)
{
    if (!serverPeer) return;

    ENetPacket* packet = enet_packet_create(nullptr, size + 1, ENET_PACKET_FLAG_RELIABLE);
    packet->data[0] = static_cast<uint8_t>(type);
    std::memcpy(packet->data + 1, data, size);
    enet_peer_send(serverPeer, channel, packet);
}


CSteamID NetworkManager::getClientSteamID(ENetPeer* client)
{
    auto it = clientSteamIDs.find(client);
    if (it != clientSteamIDs.end()) {
        CSteamID steamID = it->second;
        return steamID;
    }
    else {
        std::cerr << "Client not in auth list, THIS INDICATES THAT THIS IS SERIOUSLY BROKEN.";
        enet_peer_disconnect(client, 0);
        
    }
    return CSteamID();
}

bool NetworkManager::startServer(int port) {
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    server = enet_host_create(&address, 32, 2, 0, 0);
    if (!server) {
        std::cerr << "Failed to start server.\n";
        return false;
    }

    std::cout << "Server started on port " << port << "\n";

    registerServerEventHandlers();

    clientStarted = true;
    return true;
}


void NetworkManager::registerServerEventHandlers() {

    dispatcher.registerHandler(PACKET_AUTH_TICKET,
        [this](ENetPacket* packet, ENetPeer* peer) {
            this->handleAuthTicketPacket(packet, peer);
        });

	// called when receiving a packet from a client with their input
    dispatcher.registerHandler(PACKET_INPUT_COMMAND,
        [this](ENetPacket* packet, ENetPeer* peer) {
            if (!packet || packet->dataLength < sizeof(PlayerInputPacket))
                return;

            PlayerInputPacket input{};
            std::memcpy(&input, packet->data, sizeof(input));
            auto it = clientSteamIDs.find(peer);
            if (it == clientSteamIDs.end()) {
                // not authed, fuck off goober
				enet_peer_disconnect(peer, 0);
                return;
            }
            uint64_t steamID = it->second.ConvertToUint64();

            if (inputCallback) {
                inputCallback(steamID, input);
            }
        }
    );

}


void NetworkManager::addClientConnection(uint64_t steamID, ENetPeer* peer) {

}


bool NetworkManager::initSteamServer()
{
    const uint32_t IP = 0;
    const uint16_t steamPort = 8766;  // Port for Steam P2P traffic
    const uint16_t gamePort = 1234;   // Game port for ENet server
    const uint16_t queryPort = 27015; // Server browser port

    const AppId_t gameAppID = 480;

    bool success = SteamGameServer_Init(
        IP,
        gamePort,
        queryPort,
        eServerModeAuthentication,
        "1.0.0.0"
    );

    if (!success) {
        std::cerr << "Failed to initialize Steam Game Server\n";
        steamServerStarted = false;
        return false;
    }

    // Set basic server info
    SteamGameServer()->SetProduct("MinxMO");
    SteamGameServer()->SetGameDescription("Testing description BRO");
    SteamGameServer()->LogOnAnonymous(); // Or LogOn() if using a server Steam account

    std::cout << "Steam Game Server initialized.\n";
    steamServerStarted = true;
    return true;
}

void NetworkManager::handleAuthTicketPacket(ENetPacket* packet, ENetPeer* peer) {
    if (packet->dataLength < sizeof(uint64) + 1) return;

        const uint8_t* packetData = packet->data;

        uint64_t steamID64;
        memcpy(&steamID64, packetData, sizeof(uint64));

        CSteamID steamID(steamID64);

        const uint8_t* ticketData = packetData + sizeof(uint64);
        int ticketLen = packet->dataLength - sizeof(uint64);

        EBeginAuthSessionResult result = SteamGameServer()->BeginAuthSession(
            ticketData,
            ticketLen,
            steamID
        );

        if (result == k_EBeginAuthSessionResultOK) {
            std::cout << "Steam auth successful for SteamID: " << steamID64 << "\n";
            // unfortunately easier to leave in
            clientSteamIDs[peer] = steamID;
            //Connection conn;
            //conn.steamID = steamID.ConvertToUint64();
            //conn.peer = peer;
            //connections[conn.steamID] = conn;
            ENetPacket* packet = enet_packet_create("Hello Client!", strlen("Hello Client!") + 1, ENET_PACKET_FLAG_RELIABLE);
            if (onPlayerAuthenticated) {
                onPlayerAuthenticated(steamID.ConvertToUint64(), peer);
            }
            enet_peer_send(peer, 0, packet);
        }
        else {
            std::cout << "Steam auth failed: " << static_cast<int>(result) << "\n";
            enet_peer_disconnect(peer, 0);
        }
}

int NetworkManager::handlePeerDisconnect(ENetPeer* peer)
{
    auto it = clientSteamIDs.find(peer);
    if (it == clientSteamIDs.end()) {
        return 1;
    }
    
    CSteamID sid = it->second;

    if(disconnectCallback) {
        disconnectCallback(sid.ConvertToUint64());
	}

    SteamGameServer()->EndAuthSession(sid);
    clientSteamIDs.erase(it);

	


    return 0;
}


void NetworkManager::sendPacketToPeer(PacketType type, const void* data, size_t size, ENetPeer* peer, uint8_t channel, uint32_t flags)
{
    ENetPacket* packet = enet_packet_create(nullptr, size + 1, flags);
    packet->data[0] = static_cast<uint8_t>(type);
    std::memcpy(packet->data + 1, data, size);
    enet_peer_send(peer, channel, packet);
}


void NetworkManager::broadcastToAllExcept(PacketType type, const void* data, size_t size, ENetPeer* exclude, uint8_t channel, uint32_t flags)
{
    for (const auto& [peer, steamID] : clientSteamIDs)
    {
        if (peer == exclude)
            continue;

        sendPacketToPeer(type, data, size, peer, channel);
    }
}

void NetworkManager::broadcastToAll(PacketType type, const void* data, size_t size, uint8_t channel, uint32_t flags)
{
    for (const auto& [peer, steamID] : clientSteamIDs)
    {
        sendPacketToPeer(type, data, size, peer, channel);
    }
}


