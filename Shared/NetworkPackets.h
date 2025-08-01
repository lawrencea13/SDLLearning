#pragma once
#include <cstdint>

enum PacketType : uint8_t {
    PACKET_AUTH_TICKET = 0,
    PACKET_INPUT_COMMAND = 1,
    PACKET_SERVER_STATE = 2,
    PACKET_CONNECTED_PLAYER_LIST = 3,
    PACKET_NEW_PLAYER_CONNECTED = 4,
};

#pragma pack(push, 1)
struct ServerStatePacket {
    uint64_t steamID;
    float posX;
    float posY;
    uint32_t inputFrame;
};

struct PlayerInputPacket {
    int8_t moveX;  // -1 = left, 0 = idle, 1 = right
    int8_t moveY;  // -1 = up, 0 = none, 1 = down
    bool attack;
    uint32_t clientInputFrame;
};

#pragma pack(pop)