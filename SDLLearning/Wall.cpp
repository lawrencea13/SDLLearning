#include "Wall.h"
#include "Game.h"


Wall::Wall(int x, int y, int w, int h, SDL_Color* newColor, SDL_Renderer* rend, std::shared_ptr<SDL_Texture> tex, bool enableCollision,Game& game) : GameObject(x, y, w, h, rend, tex, true, game) {
	color = newColor;
	destRect.x = x;
	destRect.y = y;
	destRect.w = w;
	destRect.h = h;
}

void Wall::Render() {
	Camera& camera = gameInstance.getCamera();
	SDL_Rect screenRect = camera.apply(destRect);
	SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
	SDL_RenderFillRect(renderer, &screenRect);
}
