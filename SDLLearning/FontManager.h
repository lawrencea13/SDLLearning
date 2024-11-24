#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <SDL_ttf.h>
#include <iostream>

// I'm really not a big fan of this because it's the same as the TextureManager just for fonts
// However, I really didn't feel like modifying the TextureManager to be an AssetManager or using something like inheritance LMAO

class FontManager {
public:
    explicit FontManager();
    ~FontManager();

    std::shared_ptr<TTF_Font> getFont(const std::string& fontPath, int fontSize);

private:
    std::unordered_map<std::string, std::shared_ptr<TTF_Font>> fontCache;
    std::string generateKey(const std::string& fontPath, int fontSize) const;
    std::shared_ptr<TTF_Font> loadFont(const std::string& fontPath, int fontSize);
};