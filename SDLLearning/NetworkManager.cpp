#include "NetworkManager.h"
#include <iostream>
#include <steam/steam_api.h>
#include <vector>



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
            dispatcher.dispatch(packetType, event.packet, event.peer);

            enet_packet_destroy(event.packet);
            break;
        }

        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "A client disconnected.\n";
            //connections.erase(getClientSteamID(event.peer).ConvertToUint64());
            clientSteamIDs.erase(event.peer);
            break;
        }
    }
#else
    if (!client) return;
    ENetEvent event;

    while (enet_host_service(client, &event, 0) > 0) {
        uint8_t packetType = event.packet->data[0];
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout << "Connected to server, service network event called.\n";

            break;
        case ENET_EVENT_TYPE_RECEIVE:
			std::cout << "Received packet from server.\n";
            dispatcher.dispatch(packetType, event.packet, event.peer);


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

    if (!client || authTicket == k_HAuthTicketInvalid || ticketSize == 0)
        return;

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

void NetworkManager::sendPacket(PacketType type, const void* data, size_t size, uint8_t channel)
{
    if (!serverPeer) return;

    ENetPacket* packet = enet_packet_create(nullptr, size + 1, ENET_PACKET_FLAG_RELIABLE);
    packet->data[0] = static_cast<uint8_t>(type);
    std::memcpy(packet->data + 1, data, size);
    enet_peer_send(serverPeer, channel, packet);
}

uint64_t NetworkManager::getLocalSteamID() const {
    return localSteamID;
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
#endif

void NetworkManager::registerServerEventHandlers() {
#ifdef DEDICATED_SERVER
    dispatcher.registerHandler(PACKET_AUTH_TICKET,
        [this](ENetPacket* packet, ENetPeer* peer) {
            this->handleAuthTicketPacket(packet, peer);
        });


    dispatcher.registerHandler(
        PACKET_INPUT_COMMAND,
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

#else
    dispatcher.registerHandler(PACKET_SERVER_STATE, [this](ENetPacket* packet, ENetPeer* peer) {
        if (packet->dataLength < sizeof(ServerStatePacket)) {
            std::cerr << "Invalid server state packet size\n";
            return;
        }

        const ServerStatePacket* state = reinterpret_cast<const ServerStatePacket*>(packet->data + 1);

		//std::cout << "New Player X: " << state->posX << "\n New Player Y: " << state->posY << "\n";

        if (serverStateCallback) {
            serverStateCallback(*state);
        }

        });

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

#endif
}


void NetworkManager::addClientConnection(uint64_t steamID, ENetPeer* peer) {

}


#ifdef DEDICATED_SERVER
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
    if (packet->dataLength < 2) return; // useless packet

        const uint8_t* packetData = packet->data;
        uint64_t steamID64;
        memcpy(&steamID64, packetData + 1, sizeof(uint64));

        CSteamID steamID(steamID64);

        const uint8_t* ticketData = packetData + 1 + sizeof(uint64);
        int ticketLen = packet->dataLength - 1 - sizeof(uint64);

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


void NetworkManager::sendPacketToPeer(PacketType type, const void* data, size_t size, ENetPeer* peer, uint8_t channel)
{
    ENetPacket* packet = enet_packet_create(nullptr, size + 1, ENET_PACKET_FLAG_RELIABLE);
    packet->data[0] = static_cast<uint8_t>(type);
    std::memcpy(packet->data + 1, data, size);
    enet_peer_send(peer, channel, packet);
}


void NetworkManager::broadcastToAllExcept(PacketType type, const void* data, size_t size, ENetPeer* exclude, uint8_t channel)
{
    for (const auto& [peer, steamID] : clientSteamIDs)
    {
        if (peer == exclude)
            continue;

        sendPacketToPeer(type, data, size, peer, channel);
    }
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

void NetworkManager::setServerStateCallback(std::function<void(const ServerStatePacket&)> callback) {
    serverStateCallback = std::move(callback);
}

#endif
