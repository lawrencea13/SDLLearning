#include "NetworkManager.h"
#include <iostream>
#include <steam/steam_api.h>
#include <vector>
#include "Logger.h"

#define LOG(...) Logger::Log(__VA_ARGS__)

NetworkManager::NetworkManager() {}

NetworkManager::~NetworkManager() {
    shutdown();
}

void NetworkManager::shutdown() {

    if (client) {
        enet_host_destroy(client);
        client = nullptr;
    }
}

void NetworkManager::serviceNetwork() {

    if (!client) return;
    ENetEvent event;

    while (enet_host_service(client, &event, 0) > 0) {
        
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout << "Connected to server, service network event called.\n";

            break;
        case ENET_EVENT_TYPE_RECEIVE:
            if (event.packet && event.packet->dataLength > 0) {
                uint8_t packetType = event.packet->data[0];
                dispatcher.dispatch(packetType, event.packet, event.peer);
            }


            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "Disconnected from server.\n";
            break;
        }
    }

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

    if (!client || authTicket == k_HAuthTicketInvalid || ticketSize == 0)
        return;

    CSteamID steamID = SteamUser()->GetSteamID();
    uint64 steamID64 = steamID.ConvertToUint64();

    size_t payloadSize = sizeof(uint64) + ticketSize;
    uint8_t* payload = new uint8_t[payloadSize];

    memcpy(payload, &steamID64, sizeof(uint64));
    memcpy(payload + sizeof(uint64), ticketBuffer, ticketSize);

    // Let the existing sendPacket() function prepend the type byte
    sendPacket(PACKET_AUTH_TICKET, payload, payloadSize);

    delete[] payload;
}

void NetworkManager::sendPacket(PacketType type, const void* data, size_t size, uint8_t channel, uint32_t flags)
{
    if (!serverPeer) return;

    ENetPacket* packet = enet_packet_create(nullptr, size + 1, flags);
    packet->data[0] = static_cast<uint8_t>(type);
    std::memcpy(packet->data + 1, data, size);
    enet_peer_send(serverPeer, channel, packet);
}

uint64_t NetworkManager::getLocalSteamID() const {
    return localSteamID;
}



void NetworkManager::registerServerEventHandlers() {
    // should just be player states being updated as game runs
    dispatcher.registerHandler(PACKET_SERVER_STATE, [this](ENetPacket* packet, ENetPeer* peer) {
        if (packet->dataLength < sizeof(ServerStatePacket) || (packet->dataLength % sizeof(ServerStatePacket)) != 0) {
            std::cerr << "Invalid server state packet size\n";
            return;
        }

		// We may receive multiple ServerStatePackets in one ENetPacket
        size_t count = packet->dataLength / sizeof(ServerStatePacket);
        for (size_t i = 0; i < count; ++i) {
            ServerStatePacket state{};
            std::memcpy(&state, packet->data + i * sizeof(ServerStatePacket), sizeof(ServerStatePacket));

            LOG("[CLIENT] NetworkManager received state: pos=(%f,%f) frame=%u", state.posX, state.posY, state.inputFrame);

            if (serverStateCallback)
                serverStateCallback(state);
        }

        });

	// this is sent when a client first connects, and contains all the current players
    dispatcher.registerHandler(PACKET_CONNECTED_PLAYER_LIST, [this](ENetPacket* packet, ENetPeer* peer) {
        if (packet->dataLength < sizeof(ServerStatePacket)) {
            std::cerr << "Invalid ServerStatePacket size\n";
            return;
        }

        size_t count = packet->dataLength / sizeof(ServerStatePacket);
        for (size_t i = 0; i < count; ++i) {
            ServerStatePacket state{};
            std::memcpy(&state, packet->data + i * sizeof(ServerStatePacket), sizeof(ServerStatePacket));

            if (onPlayerStateReceived)
                onPlayerStateReceived(state);
        }
        });

    // this is sent when another player connects after us
    dispatcher.registerHandler(PACKET_NEW_PLAYER_CONNECTED, [this](ENetPacket* packet, ENetPeer* peer) {
        if (packet->dataLength < sizeof(ServerStatePacket)) {
            std::cerr << "Invalid ServerStatePacket size in NEW_PLAYER_CONNECTED\n";
            return;
        }

        ServerStatePacket state{};
        std::memcpy(&state, packet->data, sizeof(ServerStatePacket));

        if (otherPlayerConnected)
            otherPlayerConnected(state);
        });

    dispatcher.registerHandler(PACKET_PLAYER_DISCONNECTED, [this](ENetPacket* packet, ENetPeer* peer) {
        if (packet->dataLength < sizeof(uint64_t)) {
            std::cerr << "Invalid player disconnected packet size\n";
            return;
        }
        uint64_t steamID;
        std::memcpy(&steamID, packet->data, sizeof(uint64_t));
        if(onOtherPlayerDisconnect)
            onOtherPlayerDisconnect(steamID);
		});
}


void NetworkManager::addClientConnection(uint64_t steamID, ENetPeer* peer) {

}


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
        // note that if the client connects but doesn't send these, they will simply remain connected to the server without being authenticated
        // note this should be patched or the client shouldn't be able to proceed until authentication is requested
        // e.g. connect to login server, authorize, then proceed, meaning these methods may need to be moved to a button
        generateSteamTicket();
        sendAuthToServer();
        localSteamID = SteamUser()->GetSteamID().ConvertToUint64();
        registerServerEventHandlers();
        return true;
    }
    else {
        std::cerr << "Connection to server failed." << std::endl;
        enet_peer_reset(serverPeer);
        return false;
    }
}


