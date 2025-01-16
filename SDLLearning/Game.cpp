#include "Game.h"
#include "Player.h"
#include "Wall.h"
#include "Button.h"

class GameObject;

SDL_Event Game::event;

Game::Game() : isRunning(false), renderer(nullptr), textureManager(nullptr), window(nullptr), camera(0,0,1920,1080) {
	
}
Game::~Game() {

}

void Game::init(const char* title, int xpos, int ypos, int width, int height, int screenType /* used for fullscreen, borderless, windowed */) {
	isRunning = false;
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Subsystem initialized." << std::endl;
		isRunning = true;
		/* 
		* 1 = fullscreen // ACTS THE SAME TO EXCLUSIVE FULL SCREEN ICKY AHHHHHHHHH
		* 16 = borderless
		* 4097 = exclusive fullscreen
		*/
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

	std::shared_ptr<TTF_Font> font = fontManager->getFont("Assets\\IMMORTAL.ttf", 24);
	std::shared_ptr<TTF_Font> largeFont = fontManager->getFont("Assets\\IMMORTAL.ttf", 30);

	// All of the things below this are for testing
	// Ultimately this will load and cache textures then start at the main menu, and things will go from there.
	//std::shared_ptr<SDL_Texture> texture = textureManager->getTexture("Assets\\testSprite.png");
	
	//camera.followTarget(new Player(0, 0, 64, 64, renderer, texture, true, &inputManager, *this));
	//camera.setFollowBounds(SDL_Rect{ 200,200 });
	camera.setViewportSize(1280, 720);
	//SDL_Color* testColor = new SDL_Color{ 0, 0, 255 ,255 };
	//new Wall(0, 500, 1280, 300, testColor, renderer, nullptr, true, *this);
	if (font) {
		std::cerr << "Font was loaded succesfully" << std::endl;
	}
	widgetScreen->addWidget(std::make_shared<Button>(100, 100, 200, 100, SDL_Color{ 0, 100, 100, 100 }, "Test Button", &inputManager, font));
	widgetScreen->addWidget(std::make_shared<Button>(400, 100, 200, 100, SDL_Color{ 0, 100, 100, 100 }, "Test Button", &inputManager, largeFont));


}



void Game::handleEvents() {
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}
	inputManager.update(&event);
}

void Game::update() {
	// order matters for how we look at handling "updates"
	// At this scope, we should get inputs before we update objects to avoid being a frame behind
	// Above this scope, we call update and then render.
	// This means we determine all of the logic then render
	// This avoids being a frame behind on rendering, and overall 2 frames behind from input.
	// This would account for 33.34 ms of input delay just because of how we ordered logic at 60 fps, higher fps counts would be lower.
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

