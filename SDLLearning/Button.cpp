#include "Button.h"
#include <iostream>


Button::Button(int x, int y, int width, int height, SDL_Color color, const std::string& label)
    : Widget(x, y, width, height, color), label(label) {}

Button::Button(int x, int y, int width, int height, std::shared_ptr<SDL_Texture> texture, const std::string& label)
    : Widget(x, y, width, height, texture), label(label) {}

// Draw implementation for the button
void Button::drawImpl(SDL_Renderer* renderer) {
    SDL_Rect destRect = { x, y, width, height };
    //SDL_RenderCopy(renderer, texture.get(), nullptr, &destRect);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &destRect);

    //std::cout << "Button: " << label << " drawn at position (" << x << ", " << y << ")" << std::endl;
}

// Press method for the button
void Button::onPress() {
    std::cout << "Button: " << label << " is pressed." << std::endl;
}

// Release method for the button
void Button::onRelease() {
    std::cout << "Button: " << label << " is released." << std::endl;
}