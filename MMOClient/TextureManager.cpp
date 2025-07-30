#include "TextureManager.h"

TextureManager::TextureManager(SDL_Renderer* rend) {
    std::cout << "Instantiating the texture manager" << std::endl;
    if (!rend) {
        std::cerr << "The Renderer pointer in texture manager was invalid" << std::endl;
    }
    renderer = rend;
}

std::shared_ptr<SDL_Texture> TextureManager::getTexture(const std::string& texturePath) {
    // Check if the texture already exists in the cache
    auto it = textureCache.find(texturePath);
    if (it != textureCache.end()) {
        return it->second;
    }

    // Load the texture, cache it, and return it
    auto newTexture = loadTexture(texturePath);
    if (newTexture) {
        textureCache[texturePath] = newTexture;
    }
    return newTexture;
}

std::shared_ptr<SDL_Texture> TextureManager::loadTexture(const std::string& texturePath) {
    SDL_Surface* tempSurface = IMG_Load(texturePath.c_str());
    if (!tempSurface) {
        std::cerr << "Failed to load surface: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* rawTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (!rawTexture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }



    // Wrap raw texture in a shared_ptr with custom deleter to manage cleanup
    return std::shared_ptr<SDL_Texture>(rawTexture, SDL_DestroyTexture);
}