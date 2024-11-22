#pragma once

#include "GameObject.h"
#include "InputHandling.h"

class Player : public GameObject {
public:
	// old and unused
	Player(const char* textureSheet, SDL_Renderer* rend, int x, int y, ObjectRegistry* reg, bool enableCollision);

	Player(int x, int y, int w, int h, SDL_Renderer* rend, std::shared_ptr<SDL_Texture> tex, ObjectRegistry* reg, bool enableCollision, InputHandler* inputManager);

	void Update() override;
	void Render() override;
private:
	// Instead of creating on a per object basis, this has been moved to Game
	InputHandler* input;
};
