#pragma once

#include <unordered_map>
#include <functional>
#include <cstdint>
#include <enet/enet.h>
#include <iostream>

class PacketDispatcher {
public:
    using PacketHandler = std::function<void(ENetPacket*, ENetPeer*)>;

    void registerHandler(uint8_t packetType, PacketHandler handler) {
        handlers[packetType] = handler;
    }

    void dispatch(uint8_t packetType, ENetPacket* packet, ENetPeer* peer) const {
        auto it = handlers.find(packetType);
        if (it != handlers.end()) {
            it->second(packet, peer);
        }
    }

private:
    std::unordered_map<uint8_t, PacketHandler> handlers;
};
