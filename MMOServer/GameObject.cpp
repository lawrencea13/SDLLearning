#include "GameObject.h"
#include <iostream>
#include "Game.h"


void GameObject::Update() {}


void GameObject::Collided(GameObject* obj) {
	// to be implemented by objects that want to do something when colliding with another object, e.g. do damage to that object
}


bool GameObject::checkExternalCollision(const Rect& otherRect) const {
	return destRect.intersects(otherRect);
}

GameObject::GameObject(int x, int y, int w, int h, Game& game) : gameInstance(game)
{
	destRect.x = x;
	destRect.y = y;
	destRect.w = w;
	destRect.h = h;
	std::cout << "Created new game object" << std::endl;
	game.onGameObjectCreated(this);
}

/// \ Can be called by the children of the class to effectively "enable collision"
/// Should be called after calculating all movements to be done, but before movement is applied to the rect.
///
void GameObject::handleCollision() {
	if (!collisionEnabled) {
		std::cout << "You called GameObject::handleCollision but did not enable collision." << std::endl;
		return;
	}


	// Old logic
	/*
	*
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
			//std::cout << std::fixed << distSquared << std::endl;

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
	*/

	destRect.x += xchange;
	for (GameObject* obj : gameInstance.registry.getObjects()) {
		if (obj && obj != this && obj->collisionEnabled) {
			if (checkInternalCollision(&destRect, &(obj->getRect()))) {
				// Collided in X, move back
				destRect.x -= xchange;
				xchange = 0;
				break;
			}
		}
	}

	// Move Y
	destRect.y += ychange;
	for (GameObject* obj : gameInstance.registry.getObjects()) {
		if (obj && obj != this && obj->collisionEnabled) {
			if (checkInternalCollision(&destRect, &(obj->getRect()))) {
				// Collided in Y, move back
				destRect.y -= ychange;
				ychange = 0;
				break;
			}
		}
	}
}

bool GameObject::checkInternalCollision(Rect* myRect, Rect* otherRect) {
	return myRect->intersects(*otherRect);
}

int GameObject::sign(int value) {
	if (value > 0) return 1;
	if (value < 0) return -1;
	return 0;
}

