#include "Wall.h"


Wall::Wall(int x, int y, int w, int h, SDL_Color* newColor, SDL_Renderer* rend, bool enableCollision,Game& game) : GameObject(x, y, w, h, rend, nullptr, true, game) {
	color = newColor;
	destRect.x = x;
	destRect.y = y;
	destRect.w = w;
	destRect.h = h;
}

void Wall::Render() {
	SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
	SDL_RenderFillRect(renderer, &destRect);
}
