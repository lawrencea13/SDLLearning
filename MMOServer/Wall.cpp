#include "Wall.h"
#include "Game.h"


Wall::Wall(int x, int y, int w, int h, Game& game) : GameObject(x, y, w, h, game) {
	destRect.x = x;
	destRect.y = y;
	destRect.w = w;
	destRect.h = h;
	collisionEnabled = true;
}
