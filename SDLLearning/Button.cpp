#include "Button.h"
#include <iostream>


Button::Button(int x, int y, int width, int height, SDL_Color color, const std::string& label, InputHandler* inputMgr, std::shared_ptr<TTF_Font> font)
    : Widget(x, y, width, height, color, inputMgr, font), label(label)
{

}

Button::Button(int x, int y, int width, int height, std::shared_ptr<SDL_Texture> texture, const std::string& label)
    : Widget(x, y, width, height, texture), label(label) {}

// Draw implementation for the button
void Button::drawImpl(SDL_Renderer* renderer) {
    SDL_Rect destRect = { x, y, width, height };
    //SDL_RenderCopy(renderer, texture.get(), nullptr, &destRect);

    SDL_SetRenderDrawColor(renderer, color.r + colorModulation.r, color.g + colorModulation.g, color.b + colorModulation.b, color.a);
    SDL_RenderFillRect(renderer, &destRect);

    

    if (font && !label.empty()) {
        
        SDL_Surface* textSurface = TTF_RenderText_Blended(font.get(), label.c_str(), SDL_Color{255,255,255,255});
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_FreeSurface(textSurface);

            SDL_Rect textRect;
            TTF_SizeText(font.get(), label.c_str(), &textRect.w, &textRect.h);
            textRect.x = x + (width - textRect.w) / 2; // Center text horizontally
            textRect.y = y + (height - textRect.h) / 2; // Center text vertically

            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_DestroyTexture(textTexture);
        }
    }

}

void Button::setOnPress(std::function<void()> callback)
{
    onPressCallback = callback;
}

void Button::setOnRelease(std::function<void()> callback)
{
    onReleaseCallback = callback;
}

void Button::onPress() {
    if (onPressCallback) onPressCallback();
}

void Button::onRelease() {
    if (onReleaseCallback) onReleaseCallback();
}



void Button::onHoverStateChanged()
{
    if (hovered) {
        setColorModulation(50, 50, 50);
    }
    else {
        setColorModulation(0, 0, 0);
    }
}
