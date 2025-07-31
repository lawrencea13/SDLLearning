#include "Game.h"
#include <chrono>
#include <thread>
#include "Logger.h"

#define LOG(...) Logger::Log(__VA_ARGS__)

Game* game = nullptr;
using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;


int main(int argc, char *argv[]) {
    Logger::Init("server_log.txt");
    game = new Game();

    game->init();

    const int TICK_RATE = 60;
    const auto TICK_INTERVAL = std::chrono::duration<double>(1.0 / TICK_RATE);

    auto nextTick = Clock::now() + TICK_INTERVAL;

    while (game->running()) {
        auto now = Clock::now();

        if (now < nextTick) {
            std::this_thread::sleep_for(nextTick - now);
            continue;
        }

        game->deltaTime = static_cast<float>(TICK_INTERVAL.count());
        game->tick(game->deltaTime);

        nextTick += TICK_INTERVAL;
    }

    game->clean();
    Logger::Shutdown();
    return 0;

}

