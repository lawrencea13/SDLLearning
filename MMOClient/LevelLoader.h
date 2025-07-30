#pragma once


#include <string>
#include <vector>
#include <memory>
#include <SDL.h>
#include "GameObject.h"
#include "nlohmann//json.hpp"

using json = nlohmann::json;

struct Wall {
    int x, y, width, height;
    SDL_Color color;
    std::string texture;
};

struct Platform {
    int x, y, width, height;
    std::string movementType;
    int range;
    int speed;
};

struct DestructibleBlock {
    int x, y, width, height;
    int health;
};

struct Hazard {
    int x, y, width, height;
    int damage;
    std::string type;
};

struct Enemy {
    std::string type;
    int x, y;
    std::string behavior;
    int range;
    int speed;
};