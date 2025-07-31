#pragma once

#include "GameObject.h"
#include "NetworkManager.h"
#include "NetworkPackets.h"
#include <deque>

struct PlayerState {
    int x;
    int y;
    int xchange;
    int ychange;
    uint32_t frame;
};


class Player : public GameObject {
public:
    Player(int x, int y, int w, int h, Game& game);

     // true if this is the local client's version

    void Update() override;
	
    void ApplyInput(const PlayerInputPacket& input);

    uint64_t setSteamID(uint64_t id) { steamID = id; return steamID; }
	uint64_t getSteamID() const { return steamID; }
    uint32_t getLastReceivedFrame() const { return s_lastReceivedFrame; }


private:
	uint64_t steamID = 0;
    uint32_t s_lastReceivedFrame = 0;
    int speed = 300;

    float s_xchange = 0;
    float s_ychange = 0;
};
