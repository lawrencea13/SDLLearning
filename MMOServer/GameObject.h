#pragma once

#include <tuple>
#include <vector>
#include <memory>
#include "Rect.h"

class Game;

class GameObject {
public:
	GameObject(int x, int y, int w, int h, Game& game);

	virtual ~GameObject() {}

	auto getLocation() {
		struct result { int x; int y; };
		return result{destRect.x, destRect.y};
	}

	auto getDimensions() {
		struct result { int x; int y; };
		return result{ destRect.w, destRect.h };
	}

	void setLocation(int x, int y) {
		destRect.x = x;
		destRect.y = y;
	}

	// probably the only thing that's necessary but I will leave location, dimensions and the full rect available.
	Rect& getRect() {
		return destRect;
	}

	virtual void Update();
	virtual void Collided(GameObject* obj);

	
	bool checkExternalCollision(const Rect& otherRect) const;


protected:

	Game& gameInstance;
	
	// all x/y vars to be implemented on a per object basis.
	// should be as close to destRect as possible
	int xpos;
	int ypos;

	// change in position this frame only, reset each frame
	float xchange;
	float ychange;

	// change in position from last frame, store in case we need to calculate acceleration or deceleration for advanced movement
	int lf_xchange;
	int lf_ychange;

	Rect srcRect, destRect;

	bool gravityEnabled = false;
	bool collisionEnabled = false;

	int baseGravity = 5;
	int currentGravity;
	
	void handleCollision();
	// distance squared, this is equal to 1280 pixels. A singular object should NEVER be larger than this.
	// An example of an issue this could cause is a level with a flat floor that spans the entire x axis.
	// This floor would need to be broken up into pieces no larger than 1280 in size.
	// This is not currently enforced at the object level but may be in the future.
	const double cullDistance = 16000000;

private:
	bool checkInternalCollision(Rect* myRect, Rect* otherRect);
	int sign(int value);

};
