#pragma once

#include <SDL.h>

class GameObject;

class Camera {
public:
    Camera(int x, int y, int width, int height);

    Camera() : viewport{ 0, 0, 1920, 1080 }, targetOffset{ 0, 0 }, isFollowing(false), followBounds{ 0,0 }, targetPosition{ 0,0 }
    {

    }

    // Manual set, will either modify this to lerp over time or create a new method for smooth transition
    void setPosition(int x, int y);

    // Sets target, not called every frame
    void followTarget(GameObject* object);
    
    // Clears target
    void stopFollowingTarget();

    void setFollowBounds(SDL_Rect newFollowBounds);

    void setViewportSize(int width, int height);

    SDL_Point getPosition() const;

    SDL_Rect apply(const SDL_Rect& worldRect) const;

    SDL_Point toScreenCoordinates(const SDL_Point& worldPoint) const;

    void update();

private:
    SDL_Rect viewport;
    SDL_Point targetOffset;
    SDL_Rect followBounds;
    bool isFollowing;
    SDL_Point targetPosition;
    GameObject* targetObject = nullptr;
};