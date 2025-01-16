#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include "InputHandling.h"

class Widget {
public:
    // Constructor for color-only widget
    Widget(int x, int y, int width, int height, SDL_Color color, InputHandler* input, std::shared_ptr<TTF_Font> font = nullptr);

    // Constructor for texture-based widget
    Widget(int x, int y, int width, int height, std::shared_ptr<SDL_Texture> texture, std::shared_ptr<TTF_Font> font = nullptr);

    virtual ~Widget() = default;

    virtual void draw(SDL_Renderer* renderer);
    virtual void update();

    // Position and size setters
    void setPosition(int x, int y);
    void setSize(int width, int height);

    // Color modulation (for state-based color changes)
    void setColorModulation(Uint8 r, Uint8 g, Uint8 b);

    // Getters for position and size
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;

    // State setters and getters
    bool isEnabled() const;
    void setEnabled(bool state);

    bool isHovered() const;
    void setHovered(bool state);

    bool isPressed() const;
    void setPressed(bool state);

    bool isVisible() const;
    void setVisible(bool state);

    // \This function should not be called by children
    // \Implement Widget::drawImpl() instead.
    // \The container that manages drawing of the widgets should be calling this
    void drawIfVisible(SDL_Renderer* rend) {
        if (!visible) { return; }
        drawImpl(rend);
    }

    // Event handlers
    virtual void onPress() = 0;
    virtual void onRelease() = 0;
    // Called when Element is Hovered and again when it is not hovered anymore
    virtual void onHoverStateChanged() = 0;

    // Drag and drop events (no implementation in base class)
    virtual void drag(int startX, int startY, int endX, int endY) {}
    virtual void drop(int dropX, int dropY) {}

    // Key input handling for text input or other keyboard interactions
    virtual void handleKeyInput(SDL_Keycode key) {}

protected:
    int x, y;
    int width, height;
    SDL_Color color;
    SDL_Color colorModulation; // For applying hue adjustments

    std::shared_ptr<TTF_Font> font;
    std::shared_ptr<SDL_Texture> texture;
    bool hasTexture;

    // Widget states
    bool enabled = true;
    bool hovered = false;
    bool pressed = false;
    bool visible = true;

    // added later to avoid m1 being pressed before hovering over button for activation
    bool canPress = false;

    InputHandler* input;

    virtual void drawImpl(SDL_Renderer* renderer) = 0;

    
};