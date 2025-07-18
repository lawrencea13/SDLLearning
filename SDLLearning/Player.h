#pragma once

#include "GameObject.h"
#include "InputHandling.h"
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
    Player(int x, int y, int w, int h, std::shared_ptr<SDL_Texture> tex, Game& game);

     // true if this is the local client's version

    void Update() override;
    void Render() override;
    
    bool getIsLocalPlayer() const { return isLocalPlayer; }
	bool setIsLocalPlayer(bool local) { isLocalPlayer = local; return isLocalPlayer; }

#ifdef DEDICATED_SERVER
	
    void ApplyInput(const PlayerInputPacket& input);
#else
    void ApplyServerState(const ServerStatePacket& packet);
#endif
    uint64_t setSteamID(uint64_t id) { steamID = id; return steamID; }
	uint64_t getSteamID() const { return steamID; }

    void storeState(uint32_t frame);
    PlayerState getStateAtFrame(uint32_t frame) const;


private:
    InputHandler* input = nullptr;
    void sendInput(uint32_t currentFrame); // Still only called if isLocalPlayer
	uint64_t steamID = 0;
    bool isLocalPlayer = false;

    std::deque<PlayerState> stateHistory;
    static constexpr size_t MAX_HISTORY_SIZE = 128; // later to pull from FPS. Framecount, so it divided by FPS = length(60 history size / 60 fps = 1 second)


};
