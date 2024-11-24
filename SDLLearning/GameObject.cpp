#include "GameObject.h"
#include <iostream>
#include "Game.h"


void GameObject::Update() {}

void GameObject::Render() {
	// render examples //
	// SDL_RenderCopy(renderer, objTexture, &srcRect, &destRect);

	//SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	//SDL_RenderFillRect(renderer, &destRect);

}

void GameObject::Collided(GameObject* obj) {
	// to be implemented by objects that want to do something when colliding with another object, e.g. do damage to that object
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

GameObject::GameObject(int x, int y, int w, int h, SDL_Renderer* rend, std::shared_ptr<SDL_Texture> tex, bool enableCollision, Game& game) : gameInstance(game) {
	destRect.x = x;
	destRect.y = y;
	destRect.w = w;
	destRect.h = h;
	renderer = rend;
	texture = tex;
	collisionEnabled = enableCollision;
	std::cout << "Created new game object" << std::endl;
	gameInstance.onGameObjectCreated(this);
}

/// \ Can be called by the children of the class to effectively "enable collision"
/// Should be called after calculating all movements to be done, but before movement is applied to the rect.
///
void GameObject::handleCollision() {
	if (!collisionEnabled) {
		std::cout << "You called GameObject::handleCollision but did not enable collision." << std::endl;
		return;
	}
	//else if (!registry) {
	//	std::cout << "Collision is enabled, but you did not instantiate this GameObject with an ObjectRegistry." << std::endl;
	//	std::cout << "As a result, there is nothing to collide with." << std::endl;
	//	return;
	//}

	SDL_Rect x_Rect{ destRect.x + xchange, destRect.y, destRect.w, destRect.h };
	SDL_Rect y_Rect{ destRect.x, destRect.y + ychange, destRect.w, destRect.h };
	
	std::vector<GameObject*> objects = gameInstance.registry.getObjects();
	

	for (GameObject* obj : objects) {
		// make sure obj still valid, make sure the obj is not this obj
		// make sure obj collision is enabled
		if (obj && obj != this && obj->collisionEnabled) {  

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

