#include "Player.h"
#include "Game.h"


Player::Player(int x, int y, int w, int h, SDL_Renderer* rend, std::shared_ptr<SDL_Texture> tex, bool enableCollision, InputHandler* inputManager, Game& game) : GameObject(x, y, w, h, rend, tex, enableCollision, game) {
	//input = InputHandler();
	collisionEnabled = true;
	input = inputManager;
	//std::cout << "Player Instantiated Texture Reference Count:" << tex.use_count() << std::endl;
}

void Player::Update() {
	// remnant of old input system
	//input.update();
	// set to prior frame change
	lf_xchange = xchange;
	lf_ychange = ychange;

	// reset on new frame
	xchange = 0;
	ychange = 0;

	if (input->isKeyDown(SDLK_w)) {
		ychange -= 10;
	}
	if (input->isKeyDown(SDLK_a)) {
		xchange -= 10;
	}
	if (input->isKeyDown(SDLK_s)) {
		ychange += 10;
	}
	if (input->isKeyDown(SDLK_d)) {
		xchange += 10;
	}


	handleCollision();

	
	destRect.x += xchange;
	destRect.y += ychange;
	
}

void Player::Render() {
	//std::cout << "Player is trying to draw" << std::endl;
	/*if (!texture) {
		std::cout << "Shared pointer for the texture is bad" << std::endl;
	}
	else if (!texture.get()) {
		std::cout << "Wasn't able to 'get the texture' but the pointer was valid?" << std::endl;
	}
	else {
		std::cout << "Texture and the getter technically were valid?" << std::endl;
	}*/
	Camera& camera = gameInstance.getCamera();
	SDL_Rect screenRect = camera.apply(destRect);

	SDL_RenderCopy(renderer, texture.get(), nullptr, &screenRect);

	//std::cout << SDL_GetError() << std::endl;
}