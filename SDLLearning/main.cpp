#include "Game.h"
#include "SDL_ttf.h"


Game* game = nullptr;

int main(int argc, char *argv[]) {


//#ifdef DEDICATED_SERVER
//    game = new Game();
//    game->init("Dedicated Server", 0, 0, 0, 0, 0); // Params won't matter
//
//    const int TICK_RATE = 30; // 30 server ticks per second
//    const float TICK_INTERVAL = 1.0f / TICK_RATE;
//
//    Uint32 tickStart = SDL_GetTicks();
//    float accumulator = 0.0f;
//
//    while (game->running()) {
//        Uint32 tickTime = SDL_GetTicks();
//        float deltaTime = (tickTime - tickStart) / 1000.0f;
//        tickStart = tickTime;
//
//        accumulator += deltaTime;
//        while (accumulator >= TICK_INTERVAL) {
//            game->update();
//            accumulator -= TICK_INTERVAL;
//        }
//
//        SDL_Delay(1);
//    }
//
//    game->clean();
//    return 0;
//
//#else
//    // Regular client build
//    const int FPS = 128;
//    const float fixedDeltaTime = 1.0f / FPS;
//
//    game = new Game();
//    game->init("Test title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 16);
//
//    Uint32 frameStart = SDL_GetTicks();
//    float accumulator = 0.0f;
//
//    while (game->running()) {
//        Uint32 frameTime = SDL_GetTicks();
//        float deltaTime = (frameTime - frameStart) / 1000.0f;
//        frameStart = frameTime;
//
//        accumulator += deltaTime;
//        while (accumulator >= fixedDeltaTime) {
//            game->handleEvents();
//            game->update();
//            game->render();
//            accumulator -= fixedDeltaTime;
//            
//        }
//
//        
//    }
//
//    game->clean();
//    return 0;
//#endif
    game = new Game();

#ifdef DEDICATED_SERVER
    game->init("Dedicated Server", 0, 0, 0, 0, 0);

    const int TICK_RATE = 30;
    const float TICK_INTERVAL = 1.0f / TICK_RATE;

    Uint32 lastTick = SDL_GetTicks();

    while (game->running()) {
        Uint32 now = SDL_GetTicks();
        float frameTime = (now - lastTick) / 1000.0f;
        lastTick = now;

        game->deltaTime = frameTime;
        game->accumulator += frameTime;

        game->tick(TICK_INTERVAL);
        SDL_Delay(1); // Prevent CPU spin
    }

#else
    game->init("Client Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 16);

    const int FPS = 128;
    const float FRAME_INTERVAL = 1.0f / FPS;

    Uint32 lastTick = SDL_GetTicks();

    while (game->running()) {
        Uint32 now = SDL_GetTicks();
        float frameTime = (now - lastTick) / 1000.0f;
        lastTick = now;

        game->deltaTime = frameTime;
        game->accumulator += frameTime;

        game->tick(FRAME_INTERVAL);
        //SDL_Delay(1);
        // Enforce 128 FPS max
        float frameDuration = SDL_GetTicks() - now;
        float targetFrameTimeMs = 1000.0f / FPS;
        if (frameDuration < targetFrameTimeMs) {
            SDL_Delay(static_cast<Uint32>(targetFrameTimeMs - frameDuration));
        }
    }
#endif

    game->clean();
    return 0;

}