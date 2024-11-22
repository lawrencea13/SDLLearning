#include "Game.h"
#include "SDL_ttf.h"


Game* game = nullptr;

int main(int argc, char *argv[]) {

	const int FPS = 128;
	const float fixedDeltaTime = 1.0f / FPS; // Time per physics update


	game = new Game();

	// Borderless fullscreen window for 1080p, may need to get player screen dimensions
	//game->init("Test title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, 16);
	// Exclusive fullscreen window, the width and heigh will be scaled to the correct resolution
	//game->init("Test title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, 1);

	// Borderless but not full screen, good for testing.
	game->init("Test title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 16);

	Uint32 frameStart = SDL_GetTicks();
	// accumulates "time" until it hits fixed delta time targeting logic updates to occur at a fixed rate
	float accumulator = 0.0f;

	while (game->running()) {

		Uint32 frameTime = SDL_GetTicks();
		float deltaTime = (frameTime - frameStart) / 1000.0f;
		frameStart = frameTime;
		
		

		accumulator += deltaTime;
		while (accumulator >= fixedDeltaTime) {
			game->handleEvents();
			game->update();
			accumulator -= fixedDeltaTime;
		}

		
		
		game->render();

		
	}

	game->clean();

	return 0;
}