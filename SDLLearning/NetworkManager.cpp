#include "NetworkManager.h"
#include <iostream>
#include <steam/steam_api.h>
#include <vector>
#include "NetworkPackets.h"


NetworkManager::NetworkManager() {}

NetworkManager::~NetworkManager() {
    shutdown();
}

void NetworkManager::shutdown() {
#ifdef DEDICATED_SERVER
    if (server) {
        enet_host_destroy(server);
        server = nullptr;
    }
#else
    if (client) {
        enet_host_destroy(client);
        client = nullptr;
    }
#endif
}

void NetworkManager::serviceNetwork() {
#ifdef DEDICATED_SERVER
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
            std::cout << "Received a packetboi\n";
            switch (packetType) {
            case PACKET_AUTH_TICKET: {
                if (event.packet->dataLength < 9) break;

                const uint8_t* packetData = event.packet->data;
                uint64_t steamID64;
                memcpy(&steamID64, packetData + 1, sizeof(uint64));

                CSteamID steamID(steamID64);

                const uint8_t* ticketData = packetData + 1 + sizeof(uint64);
                int ticketLen = event.packet->dataLength - 1 - sizeof(uint64);

                EBeginAuthSessionResult result = SteamGameServer()->BeginAuthSession(
                    ticketData,
                    ticketLen,
                    steamID
                );

                if (result == k_EBeginAuthSessionResultOK) {
                    std::cout << "Steam auth successful for SteamID: " << steamID64 << "\n";
                    clientSteamIDs[event.peer] = steamID;
                }
                else {
                    std::cout << "Steam auth failed: " << static_cast<int>(result) << "\n";
                    enet_peer_disconnect(event.peer, 0);
                }
                break;
            }

            default:
                std::cout << "Unknown packet type: " << static_cast<int>(packetType) << "\n";
                break;
            }
            enet_packet_destroy(event.packet);
            break;
        }

        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "A client disconnected.\n";
            clientSteamIDs.erase(event.peer);
            break;
        }
    }
#else
    if (!client) return;
    ENetEvent event;

    while (enet_host_service(client, &event, 0) > 0) {
        std::cout << "Client network event occurred\n";
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout << "Connected to server.\n";

            break;
        case ENET_EVENT_TYPE_RECEIVE:
            std::cout << "Received data from server.\n";
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "Disconnected from server.\n";
            break;
        }
    }
#endif
}

void NetworkManager::update()
{
}

void NetworkManager::generateSteamTicket()
{
    if (!SteamUser()) {
        std::cerr << "SteamUser not available. Steam might not be initialized.\n";
        return;
    }

    if (authTicket != k_HAuthTicketInvalid) {
        SteamUser()->CancelAuthTicket(authTicket);
        authTicket = k_HAuthTicketInvalid;
    }

    authTicket = SteamUser()->GetAuthSessionTicket(
        ticketBuffer,
        sizeof(ticketBuffer),
        &ticketSize,
        nullptr
    );

    if (authTicket == k_HAuthTicketInvalid || ticketSize == 0) {
        std::cerr << "Failed to generate Steam auth ticket.\n";
        return;
    }


}

void NetworkManager::sendAuthToServer()
{
#ifndef DEDICATED_SERVER
    if (!client || authTicket == k_HAuthTicketInvalid || ticketSize == 0)
        return;
#endif
    CSteamID steamID = SteamUser()->GetSteamID();
    uint64 steamID64 = steamID.ConvertToUint64();

    // packet format: [PACKET_TYPE][steamID][ticketData...]
    size_t packetSize = 1 + sizeof(uint64) + ticketSize;
    uint8_t* data = new uint8_t[packetSize];

    data[0] = PACKET_AUTH_TICKET;
    memcpy(data + 1, &steamID64, sizeof(uint64));
    memcpy(data + 1 + sizeof(uint64), ticketBuffer, ticketSize);
    ENetPacket* packet = enet_packet_create(data, packetSize, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(serverPeer, 0, packet);
    enet_host_flush(client);
}

#ifdef DEDICATED_SERVER
CSteamID NetworkManager::getClientSteamID(ENetPeer* client)
{
    auto it = clientSteamIDs.find(client);
    if (it != clientSteamIDs.end()) {
        CSteamID steamID = it->second;
        return steamID;
    }
    else {
        std::cerr << "Client not in auth list, THIS INDICATES THAT THIS IS SERIOUSLY BROKEN.";
    }
    return CSteamID();
}
#endif

#ifdef DEDICATED_SERVER
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
    clientStarted = true;
    return true;
}

bool NetworkManager::initSteamServer()
{
    const uint32_t IP = 0;
    const uint16_t steamPort = 8766;  // Port for Steam P2P traffic
    const uint16_t gamePort = 1234;   // Game port for your ENet server
    const uint16_t queryPort = 27015; // Server browser port

    const AppId_t gameAppID = 480;

    bool success = SteamGameServer_Init(
        IP,
        gamePort,
        queryPort,
        eServerModeAuthentication,
        "1.0.0.0"  // Your game version string
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
#else
bool NetworkManager::startClient(const std::string& address, int port) {
    ENetAddress enetAddress;
    enet_address_set_host(&enetAddress, address.c_str());
    enetAddress.port = port;

    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!client) {
        std::cerr << "Failed to create ENet client." << std::endl;
        return false;
    }

    serverPeer = enet_host_connect(client, &enetAddress, 2, 0);
    if (!serverPeer) {
        std::cerr << "Failed to connect to server." << std::endl;
        return false;
    }

    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Successfully connected to server!" << std::endl;
        clientStarted = true;
        generateSteamTicket();
        sendAuthToServer();
        return true;
    }
    else {
        std::cerr << "Connection to server failed." << std::endl;
        enet_peer_reset(serverPeer);
        return false;
    }
}
#endif