#pragma once

#include "GameObject.h"

class Wall : public GameObject {
public:
	Wall(int x, int y, int w, int h, SDL_Color* newColor, SDL_Renderer* rend, std::shared_ptr<SDL_Texture> tex, bool enableCollision,Game& game);

	void Render() override;
};