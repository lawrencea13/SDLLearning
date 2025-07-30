#pragma once

#include <unordered_map>
#include <memory>
#include <optional>
#include <functional>
#include <cstdint>

#include "Player.h"
#include "PacketDispatcher.h"
#include "NetworkPackets.h"

class PlayerManager {
public:
    using PlayerID = uint64_t;

    PlayerManager() {};


    void RegisterPacketHandlers(PacketDispatcher& dispatcher) {
        dispatcher.registerHandler(PACKET_SERVER_STATE, [this](ENetPacket* packet, ENetPeer* peer) {
            if (!packet || packet->dataLength < sizeof(ServerStatePacket))
                return;

            ServerStatePacket serverState;
            std::memcpy(&serverState, packet->data, sizeof(ServerStatePacket));

            HandleServerState(serverState);
            });
    }

    std::shared_ptr<Player> CreatePlayer(uint64_t steamID, int x, int y, int w, int h, std::shared_ptr<SDL_Texture> tex, Game& game) {
        if (players.find(steamID) != players.end()) {
            // TODO: log a warning if a player with this SteamID already exists
			// if a player exists and we're calling create player, it means something went wrong
			std::cout << "Warning: Attempted to create a player with SteamID: " << steamID << ", but one already exists. Returning existing player." << std::endl;
            return players[steamID];
        }

        auto player = std::make_shared<Player>(x, y, w, h, tex, game);
        players[steamID] = player;
        return player;
    }

    
    void AddPlayer(PlayerID id, std::shared_ptr<Player> player) {
        players[id] = std::move(player);
    }

    void RemovePlayer(PlayerID id) {
        players.erase(id);
    }

    std::shared_ptr<Player> GetPlayer(PlayerID id) const {
        auto it = players.find(id);
        if (it != players.end()) return it->second;
        return nullptr;
    }

    const std::unordered_map<PlayerID, std::shared_ptr<Player>>& GetAllPlayers() const {
        return players;
    }


private:
    std::unordered_map<PlayerID, std::shared_ptr<Player>> players;

    void HandleServerState(const ServerStatePacket& packet) {
        auto player = GetPlayer(packet.steamID);
        if (!player) return;

		player->ApplyServerState(packet);
    }
};