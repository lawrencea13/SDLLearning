#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include "ObjectRegistry.h"
#include "TextureManager.h"
#include "InputHandling.h"
#include "SDL_ttf.h"
#include "Camera.h"
#include "FontManager.h"

class Game {
public:
	Game();
	~Game();

	void init(const char* title, int xpos, int ypos, int width, int height, int screenType /* used for fullscreen, borderless, windowed */);
	
	void handleEvents();
	void update();
	void render();
	void clean();

	bool onGameObjectCreated(GameObject* obj);
	bool registerGameObject(GameObject* obj);

	bool running() { return isRunning; }

	static SDL_Event event;
	ObjectRegistry registry;
	Camera& getCamera() { return camera; }

private:
	bool isRunning;
	SDL_Window* window;
	SDL_Renderer* renderer;
	TextureManager* textureManager;
	FontManager* fontManager;
	InputHandler inputManager;
	Camera camera;
};