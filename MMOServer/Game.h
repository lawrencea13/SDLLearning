#pragma once

#include <iostream>
#include "ObjectRegistry.h"
#include "NetworkManager.h"
#include <steam/steam_api.h>
#include "PlayerManager.h"

class Game {
public:
	Game();
	~Game();

	void init(); // probably redundant with the constructor, but whatever - this came from a combined server/client codebase.
	
	void handleEvents();
	void update();
	void clean();
	void SteamLogin();
	void handleNewPlayerConnection(uint64_t steamID, ENetPeer* peer);


	bool onGameObjectCreated(GameObject* obj);
	bool registerGameObject(GameObject* obj);

	bool running() { return isRunning; }

	ObjectRegistry registry;

	NetworkManager& getNetworkManager() { return networkManager; }
	std::shared_ptr<PlayerManager> getPlayerManager() const { return playerManager; }


	// frametime info
	float deltaTime = 0.0f;
	float accumulator = 0.0f;
	float getDeltaTime() { return deltaTime; }
	uint32_t getFrameCount() const { return frameCount; }
	void tick(float fixedDeltaTime);

private:
	bool isRunning;
	NetworkManager networkManager;
	std::weak_ptr<Player> localPlayer;
	std::shared_ptr<PlayerManager> playerManager;
	uint32_t frameCount = 0;
	bool deletePlayerObject(uint64_t steamID);
	void broadcastPlayerStates();

};