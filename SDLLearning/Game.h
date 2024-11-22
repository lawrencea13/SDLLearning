#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include "ObjectRegistry.h"
#include "TextureManager.h"
#include "InputHandling.h"
#include "SDL_ttf.h"

class Game {
public:
	Game();
	~Game();

	void init(const char* title, int xpos, int ypos, int width, int height, int screenType /* used for fullscreen, borderless, windowed */);
	
	void handleEvents();
	void update();
	void render();
	void clean();

	bool registerDrawable();
	bool registerGameObject();
	bool registerPhysicsObject();

	bool running() { return isRunning; }

	static SDL_Event event;
	ObjectRegistry registry;

private:
	bool isRunning;
	SDL_Window* window;
	SDL_Renderer* renderer;
	TextureManager* textureManager;
	InputHandler inputManager;
};