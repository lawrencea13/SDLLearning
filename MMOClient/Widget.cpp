#include "Widget.h"

Widget::Widget(int x, int y, int width, int height, SDL_Color color, std::shared_ptr<TTF_Font> font)
    : x(x), y(y), width(width), height(height), color(color), texture(nullptr), hasTexture(false), colorModulation{ 0, 0, 0, 0 }, font(font) {}

Widget::Widget(int x, int y, int width, int height, std::shared_ptr<SDL_Texture> texture, std::shared_ptr<TTF_Font> font)
    : x(x), y(y), width(width), height(height), color({ 0, 0, 0, 0 }), texture(texture), hasTexture(true), colorModulation{ 0, 0, 0, 0 }, font(font) {}

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
    // instead of nesting, reset the button and return
    if (!isVisible()) {
        setPressed(false);
        setHovered(false);
        return;
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
	enabled = state;
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
        // Was initially set to call on release
        // However, after reading up on UI design and experimenting
        // The button will reset and no action will be taken since a press and unhover typically means a change of heart.
        //onRelease();
        // Instead, simply set pressed to false.
        pressed = false;
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
    visible = state;
}

const std::string& Widget::getLayer() const {
    return layer;
}

void Widget::setLayer(const std::string& newLayer) {
    layer = newLayer;
}
