#pragma once

#include "GameObject.h"

class Wall : public GameObject {
public:
	Wall(int x, int y, int w, int h,Game& game);
};