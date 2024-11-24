#include "Widget.h"

Widget::Widget(int x, int y, int width, int height, SDL_Color color, InputHandler* inputMgr, std::shared_ptr<TTF_Font> font)
    : x(x), y(y), width(width), height(height), color(color), texture(nullptr), hasTexture(false), colorModulation{ 255, 255, 255, 255 }, input(inputMgr), font(font) {}

Widget::Widget(int x, int y, int width, int height, std::shared_ptr<SDL_Texture> texture, std::shared_ptr<TTF_Font> font)
    : x(x), y(y), width(width), height(height), color({ 0, 0, 0, 0 }), texture(texture), hasTexture(true), colorModulation{ 255, 255, 255, 255 }, font(font) {}

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
    SDL_Point mouseLocation = SDL_Point{ input->getMouseX(),input->getMouseY() };
    if (mouseLocation.x > x && mouseLocation.x < x + width && mouseLocation.y > y && mouseLocation.y < y + height) {
        // mouse is over the widget
        setHovered(true);
        if (input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
            setPressed(true);
        }
        else {
            setPressed(false);
        }
    }
    else {
        setHovered(false);
    }
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
    hovered = state;
    if (!hovered && pressed) {
        onRelease();
    }
    onHoverStateChanged();
}

bool Widget::isPressed() const
{
    return pressed;
}

void Widget::setPressed(bool state)
{
    if (pressed == state) return;
    pressed = state;
    if (pressed) {
        onPress();
    }
    else {
        onRelease();
    }
}

bool Widget::isVisible() const
{
    return visible;
}

void Widget::setVisible(bool state)
{
}
