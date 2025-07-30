#include "Game.h"
#include <chrono>
#include <thread>

Game* game = nullptr;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

int main(int argc, char *argv[]) {

    game = new Game();

    game->init();

    const int TICK_RATE = 60;
    const float TICK_INTERVAL = 1.0f / TICK_RATE;

    TimePoint lastTick = Clock::now();

    while (game->running()) {
        TimePoint now = Clock::now();
        std::chrono::duration<float> frameTime = now - lastTick;
        lastTick = now;

        game->deltaTime = frameTime.count();
        game->accumulator += frameTime.count();

        game->tick(TICK_INTERVAL);
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Prevent CPU pegging
    }


    game->clean();
    return 0;

}