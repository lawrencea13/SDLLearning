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
#include "Canvas.h"
#include "NetworkManager.h"
#include <steam/steam_api.h>
#include "PlayerManager.h"

class Game {
public:
	Game();
	~Game();

	void init(const char* title, int xpos, int ypos, int width, int height, int screenType /* used for fullscreen, borderless, windowed */);
	
	void handleEvents();
	void update();
	void render();
	void clean();
	void SteamLogin();
	void handleNewPlayerConnection(uint64_t steamID, ENetPeer* peer);
	void clientHandleInitialConnect(const ServerStatePacket& state);
	std::shared_ptr<Player> getLocalPlayer() { return localPlayer.lock(); }
	void setLocalPlayer(std::shared_ptr<Player> player) { localPlayer = player; }

	bool onGameObjectCreated(GameObject* obj);
	bool registerGameObject(GameObject* obj);

	bool running() { return isRunning; }

	static SDL_Event event;
	ObjectRegistry registry;
	std::unique_ptr<Canvas> widgetScreen;
	Camera& getCamera() { return camera; }
	SDL_Renderer* getRenderer() { return renderer; }
	InputHandler* getInputHandler() { return &inputManager; }

	NetworkManager& getNetworkManager() { return networkManager; }
	std::shared_ptr<PlayerManager> getPlayerManager() const { return playerManager; }

private:
	bool isRunning;
	SDL_Window* window;
	SDL_Renderer* renderer;
	TextureManager* textureManager;
	FontManager* fontManager;
	InputHandler inputManager;
	Camera camera;
	NetworkManager networkManager;
	std::weak_ptr<Player> localPlayer;
	std::shared_ptr<PlayerManager> playerManager;

	void handlePlayerInput(const PlayerInputPacket& input);
	void receiveServerStateUpdate(const ServerStatePacket& state);
};