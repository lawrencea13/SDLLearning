#include "FontManager.h"

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
}

std::shared_ptr<TTF_Font> FontManager::getFont(const std::string& fontPath, int fontSize)
{
    std::string key = generateKey(fontPath, fontSize);
    auto it = fontCache.find(key);
    if (it != fontCache.end()) {
        return it->second;
    }

    std::shared_ptr<TTF_Font> font = loadFont(fontPath, fontSize);
    if (font) {
        fontCache[key] = font;
    }

    return font;
}

// Path + size to avoid overwriting font when loading different sizes
std::string FontManager::generateKey(const std::string& fontPath, int fontSize) const
{
    return fontPath + ":" + std::to_string(fontSize);
}

std::shared_ptr<TTF_Font> FontManager::loadFont(const std::string& fontPath, int fontSize)
{
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
        std::cerr << "Failed to load font '" << fontPath << "': " << TTF_GetError() << std::endl;
        return nullptr;
    }

    return std::shared_ptr<TTF_Font>(font, TTF_CloseFont);
}
