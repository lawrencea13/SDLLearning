#pragma once

#include "GameObject.h"
#include "InputHandling.h"

class Player : public GameObject {
public:
	Player(int x, int y, int w, int h, SDL_Renderer* rend, std::shared_ptr<SDL_Texture> tex, bool enableCollision, InputHandler* inputManager, Game& game);

	void Update() override;
	void Render() override;
private:
	// Instead of creating on a per object basis, this has been moved to Game
	InputHandler* input;
};
