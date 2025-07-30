#pragma once

#include "GameObject.h"
#include "InputHandling.h"
#include "NetworkManager.h"
#include "NetworkPackets.h"
#include <deque>
#include <chrono>
#include <algorithm>
#include <array>


using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Seconds = std::chrono::duration<double>;

struct BufferedState {
    float x, y;
    double time;    // seconds since app start
	uint32_t inputFrame; 
};

struct PlayerState {
    int x;
    int y;
    int xchange;
    int ychange;
    uint32_t frame;
};

struct InputFrame {
    int moveX;
    int moveY;
    uint32_t frame;
};


class Player : public GameObject {
public:
    Player(int x, int y, int w, int h, std::shared_ptr<SDL_Texture> tex, Game& game);

     // true if this is the local client's version

    void Update() override;
    void Render() override;
	// for local testing, not used in production
    void localTestUpdate();
    void localTestRender();
    //



    
    bool getIsLocalPlayer() const { return isLocalPlayer; }
	bool setIsLocalPlayer(bool local) { isLocalPlayer = local; return isLocalPlayer; }
	bool getIsLocalTesting() const { return localTesting; }


    void ApplyServerState(const ServerStatePacket& packet);

    uint64_t setSteamID(uint64_t id) { steamID = id; return steamID; }
	uint64_t getSteamID() const { return steamID; }

    void storeState(uint32_t frame);


private:
    InputHandler* input = nullptr;
    void sendInput(uint32_t currentFrame); // Still only called if isLocalPlayer
	uint64_t steamID = 0;
    int speed = 300;
    bool isLocalPlayer = false;

    std::deque<PlayerState> stateHistory;
    std::deque<InputFrame> inputHistory;

    static constexpr size_t MAX_HISTORY_SIZE = 128; // later to pull from FPS. Framecount, so it divided by FPS = length(60 history size / 60 fps = 1 second)

    bool localTesting = false; // only for debug, testing rendering vs networking

    static constexpr size_t STATE_BUFFER_SIZE = 32;
    std::array<BufferedState, STATE_BUFFER_SIZE> stateBuf;
    size_t bufHead = 0;   // next write index
    size_t bufCount = 0;  // how many valid entries

    // how far behind real time we render (in seconds)
    static constexpr double INTERP_DELAY = 0.1;
    float drawX = 0.0f, drawY = 0.0f;

};
