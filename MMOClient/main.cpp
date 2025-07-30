#include "Game.h"
#include "SDL_ttf.h"
#include <chrono>
#include <thread>


int main(int argc, char *argv[]) {

    

    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<double>;
    constexpr double SIM_HZ = 60.0;
    constexpr double RENDER_HZ = 128.0;

    const Duration simDt{ 1.0 / SIM_HZ };
    const Duration renderDt{ 1.0 / RENDER_HZ };


    //auto lastTime = Clock::now();
    //const float SIM_DT = 1.0f / 30.0f;   // 30 Hz simulation
    //const float MAX_FPS = 128.0f;
    //const float FRAME_DT = 1.0f / MAX_FPS;
    //float accumulator = 0.0f;

    Game game;


    game.init("Client Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 16);

    auto lastSimTime = Clock::now();
    auto nextRenderTime = lastSimTime;

    while (game.running()) {

        auto now = Clock::now();
        Duration realDt = now - lastSimTime;
        lastSimTime = now;

        game.handleEvents();

        static Duration simAccumulator{ 0 };
        simAccumulator += realDt;
        while (simAccumulator >= simDt) {
            game.tick(simDt.count());
            simAccumulator -= simDt;
        }


        if (now < nextRenderTime) {
            std::this_thread::sleep_until(nextRenderTime);
            now = nextRenderTime;
        }

        auto renderDtNative = std::chrono::duration_cast<Clock::duration>(renderDt);
        nextRenderTime = now + renderDtNative;

        game.render();
    }


    /*
    while (game.running()) {
        auto now = Clock::now();
        float realDt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        // 2) Accumulate and run fixed‐dt updates
        accumulator += realDt;
        while (accumulator >= SIM_DT) {
            game.tick(SIM_DT);
            accumulator -= SIM_DT;
        }

        // 3) Render as fast as you like (up to MAX_FPS)
        game.render();

        // 4) Throttle render loop to cap at ~128 FPS
        auto afterRender = Clock::now();
        float frameTime = std::chrono::duration<float>(afterRender - now).count();
        if (frameTime < FRAME_DT) {
            std::this_thread::sleep_for(
                std::chrono::duration<float>(FRAME_DT - frameTime)
            );
        }
    }
    */

    game.clean();
    return 0;

}