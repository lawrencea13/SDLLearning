#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

class TextureManager {
public:
    explicit TextureManager(SDL_Renderer* rend);
    std::shared_ptr<SDL_Texture> getTexture(const std::string& texturePath);

private:
    SDL_Renderer* renderer;
    std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textureCache;

    std::shared_ptr<SDL_Texture> loadTexture(const std::string& texturePath);
};