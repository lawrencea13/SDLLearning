//#include "TextureManager.h"
//
//SDL_Texture* TextureManager::LoadTexture(const char* texture, SDL_Renderer* rend) {
//	SDL_Surface* tempSurface = IMG_Load(texture);
//	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, tempSurface);
//	SDL_FreeSurface(tempSurface);
//
//	// for now, if there is an issue with loading the texture,
//	// print out the most recent error and return null instead of the texture
//	if (!tex) {
//		std::cout << SDL_GetError();
//		return NULL;
//	}
//
//	return tex;
//}

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
        return it->second; // Return the cached texture
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