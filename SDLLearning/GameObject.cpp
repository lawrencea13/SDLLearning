#include "GameObject.h"
#include <iostream>
#include "ObjectRegistry.h"

GameObject::GameObject(const char* textureSheet, SDL_Renderer* rend, int x, int y) {
	renderer = rend;
	//objTexture = TextureManager::LoadTexture(textureSheet, rend);

	xpos = x;
	ypos = y;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 296;
	srcRect.h = 296;
	destRect.w = 64;
	destRect.h = 64;
	
	//input = new InputHandler();
}

GameObject::GameObject(const char* textureSheet, SDL_Renderer* rend, int x, int y, ObjectRegistry* objReg, bool enableCollision = true) {
	renderer = rend;
	//objTexture = TextureManager::LoadTexture(textureSheet, rend);

	xpos = x;
	ypos = y;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 296;
	srcRect.h = 296;
	destRect.w = 64;
	destRect.h = 64;
	registry = objReg;
	collisionEnabled = enableCollision;
	//input = new InputHandler();
}

GameObject::GameObject(int x, int y, int w, int h, SDL_Color* newColor, SDL_Renderer* rend) {
	destRect.x = x;
	destRect.y = y;
	destRect.w = w;
	destRect.h = h;
	color = newColor;
	renderer = rend;
}

GameObject::GameObject(int x, int y, int w, int h, SDL_Renderer* rend, std::shared_ptr<SDL_Texture> tex, ObjectRegistry* reg, bool enableCollision) {
	destRect.x = x;
	destRect.y = y;
	destRect.w = w;
	destRect.h = h;
	renderer = rend;
	texture = tex;
	registry = reg;
	collisionEnabled = enableCollision;

}

void GameObject::Update() {}

void GameObject::Render() {
	// render examples //
	// SDL_RenderCopy(renderer, objTexture, &srcRect, &destRect);

	//SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	//SDL_RenderFillRect(renderer, &destRect);

}


bool GameObject::checkExternalCollision(const SDL_Rect& otherRect) const {
	if (destRect.x < otherRect.x + otherRect.w &&
		destRect.x + destRect.w > otherRect.x &&
		destRect.y < otherRect.y + otherRect.h &&
		destRect.y + destRect.h > otherRect.y) {
		return true; // Collision detected
	}
	return false; // No collision
}

/// \ Can be called by the children of the class to effectively "enable collision"
/// Should be called after calculating all movements to be done, but before movement is applied to the rect.
///
void GameObject::handleCollision() {
	if (!collisionEnabled) {
		std::cout << "You called GameObject::handleCollision but did not enable collision." << std::endl;
		return;
	}
	else if (!registry) {
		std::cout << "Collision is enabled, but you did not instantiate this GameObject with an ObjectRegistry." << std::endl;
		std::cout << "As a result, there is nothing to collide with." << std::endl;
		return;
	}

	SDL_Rect x_Rect{ destRect.x + xchange, destRect.y, destRect.w, destRect.h };
	SDL_Rect y_Rect{ destRect.x, destRect.y + ychange, destRect.w, destRect.h };
	std::vector<GameObject*> objects = registry->getObjects();
	for (GameObject* obj : objects) {
		if (obj && obj != this) {  

			SDL_Rect* otherRect = &(obj->getRect());

			int xdist = otherRect->x - destRect.x;
			int ydist = otherRect->y - destRect.y;
			// distance between the x,y of the player and the object.
			double distSquared = (xdist + ydist) * (xdist + ydist);

			//std::cout << distSquared << std::endl;
			std::cout << std::fixed << distSquared << std::endl;

			if (distSquared > cullDistance) {
				continue;
			}

			// should also cull between the distance between the effective x,y of the width/heigh of the object incase it's a huge block

			if (checkInternalCollision(&y_Rect, otherRect)) {
				while (checkInternalCollision(&y_Rect, otherRect)) {
					y_Rect.y -= sign(ychange);
				}
				destRect.y = y_Rect.y;
				ychange = 0;
			}


			if (checkInternalCollision(&x_Rect, otherRect)) {
				while (checkInternalCollision(&x_Rect, otherRect)) {
					x_Rect.x -= sign(xchange);
				}
				destRect.x = x_Rect.x;
				xchange = 0;
			}

		}
	}

}

bool GameObject::checkInternalCollision(SDL_Rect* myRect, SDL_Rect* otherRect) {
	if (myRect->x < otherRect->x + otherRect->w &&
		myRect->x + myRect->w > otherRect->x &&
		myRect->y < otherRect->y + otherRect->h &&
		myRect->y + myRect->h > otherRect->y) {
		return true; // Collision detected
	}
	return false; // No collision
}

int GameObject::sign(int value) {
	if (value > 0) return 1;
	if (value < 0) return -1;
	return 0;
}

