#include "Game.h"
#include "Player.h"
#include "Logger.h"

#define LOG(...) Logger::Log(__VA_ARGS__)

class GameObject;


inline static double NowSeconds() {
	using namespace std::chrono;
	static auto start = steady_clock::now();
	return duration<double>(steady_clock::now() - start).count();
}


Game::Game() : isRunning(false) {
	
}
Game::~Game() {

}

void Game::init() {
	isRunning = false;

	if (enet_initialize() != 0) {
		std::cerr << "An error occurred while initializing ENet.\n";
		isRunning = false;
		return;
	}
	else {
		std::cout << "ENet successfully initialized.\n";
	}

	playerManager = std::make_shared<PlayerManager>();

	std::cout << "Starting server (dedicated build)..." << std::endl;
	if (!networkManager.startServer(1234)) {
		std::cerr << "Failed to start server!" << std::endl;
		isRunning = false;
		return;
	}
	std::cout << "Server started successfully!" << std::endl;
	isRunning = true;
	SteamLogin();

	networkManager.onPlayerAuthenticated = [this](uint64_t steamID, ENetPeer* peer) {
		this->handleNewPlayerConnection(steamID, peer);
		};

	networkManager.inputCallback = [this](uint64_t steamID, const PlayerInputPacket& input) {
		auto p = playerManager->GetPlayer(steamID);
		if (!p) return;
		p->ApplyInput(input);
		};

	networkManager.disconnectCallback = [this](uint64_t steamID) {
		if (!deletePlayerObject(steamID)) {
			// Optional: log warning
			std::cerr << "Tried to delete unknown player " << steamID << "\n";
		}
		else {
			networkManager.broadcastToAll(
				PACKET_PLAYER_DISCONNECTED,
				&steamID,
				sizeof(steamID),
				0,
				ENET_PACKET_FLAG_RELIABLE
			);
		}
		};

	return;
}


void Game::handleEvents() {
	// was handling SDL events, but server doesn't need SDL, leaving here in  case we want to handle events later.
	// TODO: implement a proper shutdown mechanism

}

void Game::update() {

	registry.update();

	if (networkManager.isClientStarted()) {
		networkManager.serviceNetwork();

		broadcastPlayerStates();
	}

}



void Game::clean() {

	enet_deinitialize();
}

void Game::SteamLogin()
{

	if (SteamAPI_Init())
	{
		std::cout << "Steam Initialized." << std::endl;
		
		networkManager.initSteamServer();

	}
	else
	{
		std::cerr << "Failed to initialize Steam. Steam might not be running." << std::endl;
	}

}

void Game::handleNewPlayerConnection(uint64_t steamID, ENetPeer* peer)
{

	int spawnX = 100, spawnY = 100;
	// nullptr for now on texture, don't really care on the server what the texture is tbh
	// server will track somehow down the road when it becomes relevant
	// this logic is gonna get tore up again anyway when the saving/loading system is implemented
	auto newPlayer = playerManager->CreatePlayer(steamID, spawnX, spawnY, 64, 64, *this);
	newPlayer->setSteamID(steamID);

	// Associate this player with the SteamID in the NetworkManager
	//networkManager.associatePlayer(steamID, newPlayer.get());

	// Register with the game registry if needed
	//registry.registerObject(newPlayer.get());

	// 1. Send all current players (including new one) to the new peer
	std::vector<ServerStatePacket> allPlayers;

	//std::cout << "New player location: " << spawnX << ", " << spawnY << std::endl;

	for (const auto& [id, playerPtr] : playerManager->GetAllPlayers()) {
		if (!playerPtr) continue;

		ServerStatePacket info;
		info.steamID = id;
		info.posX = static_cast<float>(playerPtr->getLocation().x);
		info.posY = static_cast<float>(playerPtr->getLocation().y);
		std::cout << "Sending player info: " << info.steamID << " at (" << info.posX << ", " << info.posY << ")\n";
		uint8_t* raw = reinterpret_cast<uint8_t*>(&info);

		allPlayers.push_back(info);
	}

	if (!allPlayers.empty()) {
		networkManager.sendPacketToPeer(
			PACKET_CONNECTED_PLAYER_LIST,
			allPlayers.data(),
			allPlayers.size() * sizeof(ServerStatePacket),
			peer,
			0
		);
	}

	// 2. Broadcast new player info to all other clients
	ServerStatePacket newPlayerInfo;
	newPlayerInfo.steamID = steamID;
	newPlayerInfo.posX = static_cast<float>(spawnX);
	newPlayerInfo.posY = static_cast<float>(spawnY);

	networkManager.broadcastToAllExcept(
		PACKET_NEW_PLAYER_CONNECTED,
		&newPlayerInfo,
		sizeof(newPlayerInfo),
		peer,
		0
	);


}

bool Game::deletePlayerObject(uint64_t steamID)
{
	auto playerPtr = playerManager->GetPlayer(steamID);
	if (!playerPtr) {
		// No such player
		return false;
	}
	registry.removeObject(playerPtr.get());
	playerManager->RemovePlayer(steamID);

	return true;
}

void Game::broadcastPlayerStates() {
	std::vector<ServerStatePacket> allStates;

	for (const auto& [steamID, playerPtr] : playerManager->GetAllPlayers()) {
		if (!playerPtr) continue;
		ServerStatePacket state;
		state.steamID = steamID;
		state.posX = static_cast<float>(playerPtr->getLocation().x);
		state.posY = static_cast<float>(playerPtr->getLocation().y);
		state.inputFrame = playerPtr->getLastReceivedFrame();
		allStates.push_back(state);
	}

	if (!allStates.empty()) {
		networkManager.broadcastToAll(
			PACKET_SERVER_STATE,
			allStates.data(),
			allStates.size() * sizeof(ServerStatePacket),
			0,
			ENET_PACKET_FLAG_UNSEQUENCED
		);
	}
}



bool Game::onGameObjectCreated(GameObject* obj)
{
	// potentially do some other logic here, this codebase was originally combined with client logic, so this may be redundant. Always returns true for now.
	return registerGameObject(obj);
}

bool Game::registerGameObject(GameObject* obj)
{
	registry.registerObject(obj);
	std::cout << "GameObject Registered" << std::endl;
	return true;
}

void Game::tick(float fixedDeltaTime)
{
	//static uint64_t tickNum = 0;
	//LOG("[SERVER] Tick: %llu at %.3f", ++tickNum, NowSeconds());
	update();
	frameCount++;
	enet_host_flush(networkManager.getServer());
}



