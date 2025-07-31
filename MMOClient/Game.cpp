#include "Game.h"
#include "Player.h"
#include "Wall.h"
#include "Button.h"
#include "Logger.h"

#define LOG(...) Logger::Log(__VA_ARGS__)

class GameObject;

SDL_Event Game::event;

Game::Game() : isRunning(false), renderer(nullptr), textureManager(nullptr), window(nullptr), camera(0,0,1920,1080), fontManager(nullptr) {
	
}
Game::~Game() {

}

void Game::init(const char* title, int xpos, int ypos, int width, int height, int screenType /* used for fullscreen, borderless, windowed */) {
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

	SteamLogin();
	if (!networkManager.startClient("127.0.0.1", 1234)) {
		if (!networkManager.startClient("192.168.1.74", 1234)) {
			std::cerr << "Failed to connect to server." << std::endl;
			isRunning = false;
			return;
		}
	}

	networkManager.onPlayerStateReceived = [this](const ServerStatePacket& state) {
		this->clientHandleInitialConnect(state);
		};
	
	networkManager.serverStateCallback = [this](const ServerStatePacket state) {
		this->receiveServerStateUpdate(state);
		};

	networkManager.otherPlayerConnected = [this](const ServerStatePacket& state) {
		this->otherPlayerConnected(state);
		};

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Subsystem initialized." << std::endl;
		isRunning = true;
		window = SDL_CreateWindow(title, xpos, ypos, width, height, screenType);

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (!renderer) {
			isRunning = false;
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		registry = (*new ObjectRegistry());
		inputManager = InputHandler();
		
	}
	else {
		std::cerr << "Failed to initialize sybsystem. Shutting down." << std::endl;
		return;
	}

	if (TTF_Init() == 0) {
		std::cout << "Text subsystem initialized." << std::endl;
	}
	else {
		std::cerr << "Failed to initialize the text subsystem. Shutting down." << std::endl;
		isRunning = false;
		return;
	}



	textureManager = new TextureManager(renderer);
	fontManager = new FontManager();
	widgetScreen = std::make_unique<Canvas>(renderer);



	// All of the things below this are for testing
	// Ultimately this will load and cache textures then start at the main menu, and things will go from there.
	

	//camera.followTarget(new Player(100, 100, 64, 64, texture, *this));

	

	//SDL_Color* testColor = new SDL_Color{ 0, 0, 255 ,255 };
	//new Wall(0, 500, 1280, 300, testColor, renderer, nullptr, true, *this);
	/*std::shared_ptr<TTF_Font> font = fontManager->getFont("Assets\\IMMORTAL.ttf", 24);
	std::shared_ptr<TTF_Font> largeFont = fontManager->getFont("Assets\\IMMORTAL.ttf", 30);

	if (font) {
		std::cerr << "Font was loaded succesfully" << std::endl;
	}
	
	std::shared_ptr<Button> testButton = std::make_shared<Button>(100, 100, 200, 100, SDL_Color{ 0, 100, 100, 100 }, "Test Button", &inputManager, font);
	testButton->setOnRelease([]() {
		std::cout << "Start button pressed!" << std::endl;

		});

	widgetScreen->addWidget(testButton);
	widgetScreen->addWidget(std::make_shared<Button>(400, 100, 200, 100, SDL_Color{ 0, 100, 100, 100 }, "Test Button", &inputManager, largeFont));*/

	

}


void Game::handleEvents() {
	//SDL_PollEvent(&event);
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		default:
			break;
		}
		inputManager.update(&event);
	}
}

void Game::tick(float fixedDeltaTime)
{
	LOG("[CLIENT] Tick: %llu at %.3f", ++clientTickNum, SDL_GetTicks() / 1000.0f);
	deltaTime = fixedDeltaTime;
	// handled at 128 hz from main loop rather than the 30 hz update tick rate we see here
	//handleEvents(); // Input
	update();
	frameCount++;
	enet_host_flush(networkManager.getClient());
}

void Game::update() {
	// order matters for how we look at handling "updates"
	// At this scope, we should get inputs before we update objects to avoid being a frame behind
	// Above this scope, we call update and then render.
	// This means we determine all of the logic then render
	// This avoids being a frame behind on rendering, and overall 2 frames behind from input.
	// This would account for 33.34 ms of input delay just because of how we ordered logic at 60 fps, higher fps counts would be lower.
	if (networkManager.isClientStarted()) {
		networkManager.serviceNetwork();
	}

	registry.update();
	widgetScreen->update();
	// camera must be kept separate as it's not a game object
	camera.update();

}

void Game::render() {
	// Clear screen before rendering new screen
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	//put stuff to render here
	//player->Render();
	registry.draw();
	widgetScreen->draw();
	SDL_RenderPresent(renderer);
}


void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	SDL_Quit();
	enet_deinitialize();
}

void Game::SteamLogin()
{

	if (SteamAPI_Init())
	{
		std::cout << "Steam Initialized." << std::endl;


		if (SteamUser() && SteamUser()->BLoggedOn())
		{
			CSteamID steamID = SteamUser()->GetSteamID();
			std::cout << "Logged in as: " << steamID.ConvertToUint64() << std::endl;

			// You can access the user's name and other info here.
			const char* userName = SteamFriends()->GetPersonaName();
			std::cout << "Username: " << userName << std::endl;
		}
		else
		{
			std::cerr << "Steam is not logged in. Please login to Steam." << std::endl;
		}

	}
	else
	{
		std::cerr << "Failed to initialize Steam. Steam might not be running." << std::endl;
	}

}


void Game::clientHandleInitialConnect(const ServerStatePacket& state)
{
	std::cout << "Received initial connect state for player SteamID: " << state.steamID << " at position (" << state.posX << ", " << state.posY << ")\n";
	
	std::shared_ptr<SDL_Texture> texture = textureManager->getTexture("Assets\\testSprite.png");
	if (state.steamID == networkManager.getLocalSteamID()) {
		auto player = playerManager->CreatePlayer(
			state.steamID,
			static_cast<int>(state.posX),
			static_cast<int>(state.posY),
			64, 64,
			texture,
			*this
		);
		player->setSteamID(state.steamID);
		player->setIsLocalPlayer(true);
		setLocalPlayer(player);
	}
	else {
		auto player = playerManager->CreatePlayer(
			state.steamID,
			static_cast<int>(state.posX),
			static_cast<int>(state.posY),
			64, 64,
			texture,
			*this
		);
		player->setSteamID(state.steamID);
	}


}

bool Game::onGameObjectCreated(GameObject* obj)
{
	return registerGameObject(obj);
}

bool Game::registerGameObject(GameObject* obj)
{
	registry.registerObject(obj);
	std::cout << "GameObject Registered" << std::endl;
	return true;
}



void Game::receiveServerStateUpdate(const ServerStatePacket state)
{
	auto player = playerManager->GetPlayer(state.steamID);
	LOG("[CLIENT] Game::receiveServerStateUpdate Received server state at tick=%llu (t=%.3f) pos=(%.3f,%.3f)", clientTickNum, SDL_GetTicks() / 1000.0f, state.posX, state.posY);

	if (!player) {
		// We don’t have a reference yet — might not have received the spawn message?
		std::cerr << "Warning: Received update for unknown player: " << state.steamID << "\n";
		return;
	}

	if (player->getIsLocalTesting()) {
		return; // Don't apply server state updates in local testing mode
	}

	// handle through prediction and reconciliation if this is the local player
	if (player->getIsLocalPlayer()) {
		
	}
	else {
		// Directly set the position for other clients
		//player->setLocation(static_cast<int>(state.posX), static_cast<int>(state.posY));
	}

	player->ApplyServerState(state);


}

void Game::otherPlayerConnected(const ServerStatePacket& state)
{
	if (state.steamID == localPlayer.lock()->getSteamID()) return; // Don't spawn yourself again
	std::shared_ptr<SDL_Texture> texture = textureManager->getTexture("Assets\\testSprite.png");
	auto player = playerManager->CreatePlayer(
		state.steamID,
		static_cast<int>(state.posX),
		static_cast<int>(state.posY),
		64, 64,
		texture,
		*this
	);

	player->setSteamID(state.steamID);
	//registry.registerObject(player.get());

	std::cout << "Spawned new player with SteamID: " << state.steamID << "\n";

}


