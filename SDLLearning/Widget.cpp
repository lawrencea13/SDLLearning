#include "Widget.h"

Widget::Widget(int x, int y, int width, int height, SDL_Color color)
    : x(x), y(y), width(width), height(height), color(color), texture(nullptr), hasTexture(false), colorModulation{ 255, 255, 255, 255 } /*color mod not really used for solid colors*/ {}

Widget::Widget(int x, int y, int width, int height, std::shared_ptr<SDL_Texture> texture)
    : x(x), y(y), width(width), height(height), color({ 0, 0, 0, 0 }), texture(texture), hasTexture(true), colorModulation{ 255, 255, 255, 255 } {}

// \Implementation of basic draw lacks any additional checks
// \Requires own implementation of things like a visibility check.
void Widget::draw(SDL_Renderer* renderer) {
    // 10-29-2024 I'm not a big fan of this, the color drawing is for early building and debug so I am leaving it in
    // Children should override this instead of allowing this to draw
    // 10-31-2024 HA I found a way to get out of using this crap
/*    if (hasTexture && texture) {
        SDL_SetTextureColorMod(texture.get(), colorModulation.r, colorModulation.g, colorModulation.b);

        SDL_Rect destRect = { x, y, width, height };
        SDL_RenderCopy(renderer, texture.get(), nullptr, &destRect);

        SDL_SetTextureColorMod(texture.get(), 255, 255, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect rect = { x, y, width, height };
        SDL_RenderFillRect(renderer, &rect);
    }*/
}

void Widget::update()
{
}

void Widget::setPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

void Widget::setSize(int width, int height) {
    this->width = width;
    this->height = height;
}


void Widget::setColorModulation(Uint8 r, Uint8 g, Uint8 b) {
    colorModulation.r = r;
    colorModulation.g = g;
    colorModulation.b = b;
}

int Widget::getX() const { return x; }
int Widget::getY() const { return y; }
int Widget::getWidth() const { return width; }
int Widget::getHeight() const { return height; }

bool Widget::isEnabled() const
{
    return false;
}

void Widget::setEnabled(bool state)
{
}

bool Widget::isHovered() const
{
    return hovered;
}

void Widget::setHovered(bool state)
{
    if (hovered == state) return;
    if (!hovered) {
        onHover();
    }
    else {

    }
    hovered = state;
}

bool Widget::isPressed() const
{
    return false;
}

void Widget::setPressed(bool state)
{
}

bool Widget::isVisible() const
{
    return false;
}

void Widget::setVisible(bool state)
{
}
