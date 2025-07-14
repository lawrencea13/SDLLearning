#pragma once

#include "GameObject.h"

class Wall : public GameObject {
public:
	Wall(int x, int y, int w, int h, SDL_Color* newColor, std::shared_ptr<SDL_Texture> tex,Game& game);

	void Render() override;
};