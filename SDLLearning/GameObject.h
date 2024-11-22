#pragma once

#include <tuple>
#include <vector>
#include "SDL.h"
#include <memory>

class ObjectRegistry;

class GameObject {
public:
	
	virtual ~GameObject() {}

	auto getLocation() {
		struct result { int x; int y; };
		return result{destRect.x, destRect.y};
	}

	auto getDimensions() {
		struct result { int x; int y; };
		return result{ destRect.w, destRect.h };
	}

	// probably the only thing that's necessary but I will leave location, dimensions and the full rect available.
	SDL_Rect& getRect() {
		return destRect;
	}

	virtual void Update();
	virtual void Render();

	
	bool checkExternalCollision(const SDL_Rect& otherRect) const;


protected:
	// different constructor options for different types of children
	// 
	// expected for an object with a texture to load
	GameObject(const char* textureSheet, SDL_Renderer* rend, int x, int y);
	// expected for a debug object to use or for simple solid color blocks if intended
	GameObject(int x, int y, int w, int h, SDL_Color* newColor, SDL_Renderer* rend);
	// second debug option, it offers baked in collision.
	GameObject(const char* textureSheet, SDL_Renderer* rend, int x, int y, ObjectRegistry* objReg, bool enableCollision);
	// New texture loader, old texture options may not work with new texture loader
	GameObject(int x, int y, int w, int h, SDL_Renderer* rend, std::shared_ptr<SDL_Texture> tex, ObjectRegistry* reg, bool enableCollision);

	// NOTE: WALL TYPES SHOULD BE NO LARGER THAN 1270x1270

	// all x/y vars to be implemented on a per object basis.
	// should be as close to destRect as possible
	int xpos;
	int ypos;

	// change in position this frame only, reset each frame
	int xchange;
	int ychange;

	// change in position from last frame, store in case we need to calculate acceleration or deceleration for advanced movement
	int lf_xchange;
	int lf_ychange;

	SDL_Rect srcRect, destRect;
	//SDL_Texture* objTexture;
	std::shared_ptr<SDL_Texture> texture;
	SDL_Renderer* renderer;
	SDL_Color* color;
	ObjectRegistry* registry;

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
	bool checkInternalCollision(SDL_Rect* myRect, SDL_Rect* otherRect);
	int sign(int value);

};
