#include "Camera.h"
#include "GameObject.h"
#include <iostream>

Camera::Camera(int x, int y, int width, int height) : viewport{ x, y, width, height }, targetOffset{ 0, 0 }, isFollowing(false), followBounds{ 0,0 }, targetPosition{ 0,0 }
{

}


void Camera::setPosition(int x, int y)
{
	viewport.x = x;
	viewport.y = y;
}


void Camera::followTarget(GameObject* object)
{
	targetObject = object;
	isFollowing = true;
}

void Camera::stopFollowingTarget()
{
	isFollowing = false;
}

void Camera::setFollowBounds(SDL_Rect newFollowBounds)
{
    followBounds = newFollowBounds;
}

void Camera::setViewportSize(int width, int height)
{
    viewport.w = width;
    viewport.h = height;
}

SDL_Point Camera::getPosition() const
{
	return { viewport.x, viewport.y };
}

SDL_Rect Camera::apply(const SDL_Rect& worldRect) const
{
	SDL_Rect screenRect = worldRect;
	screenRect.x -= viewport.x;
	screenRect.y -= viewport.y;
	return screenRect;
}

SDL_Point Camera::toScreenCoordinates(const SDL_Point& worldPoint) const
{
	return { worldPoint.x - viewport.x, worldPoint.y - viewport.y };
}

void Camera::update()
{
    if (targetObject) {
        // Get the center of the target object
        SDL_Rect targetRect = targetObject->getRect();
        int targetX = targetRect.x + targetRect.w / 2;
        int targetY = targetRect.y + targetRect.h / 2;

        // Adjust viewport to follow the target within the tolerance range
        if (targetX < viewport.x + followBounds.x) {
            viewport.x = targetX - followBounds.x;
        }
        else if (targetX > viewport.x + viewport.w - followBounds.x ) {
            viewport.x = targetX - (viewport.w - followBounds.x);
        }

        if (targetY < viewport.y + followBounds.y) {
            viewport.y = targetY - followBounds.y;
        }
        else if (targetY > viewport.y + viewport.h - followBounds.y) {
            viewport.y = targetY - (viewport.h - followBounds.y);
        }
    }
}
