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
    // Old logic
    //SDL_Point mouseLocation = SDL_Point{ input->getMouseX(),input->getMouseY() };
    //// get the info from the prior frame to determine what we can do on current frame
    //bool priorFrameCanPress = canPress;

    //if (isVisible()) {
    //    /*
    //    * There might be a better way to do this but this is what I came up with to retain all functionality without much compromise.
    //    * canPress itself is always set for the NEXT frame, meaning it is never to be checked here, instead we use the local priorFrameCanPress
    //    * If the mouse is not already hovering the widget, we don't want a mouse down to trigger the button
    //    * Previously, This worked EXCEPT for when holding M1 and dragging across the screen until it hits the button
    //    * Now, we only set canPress to true if we are hovering AND not clicked
    //    * The canPress var may be set multiple times in a frame, but it should always produce the correct result.
    //    */
    //    if (mouseLocation.x > x && mouseLocation.x < x + width && mouseLocation.y > y && mouseLocation.y < y + height) {
    //        // mouse is over the widget
    //        setHovered(true);
    //        canPress = true;

    //        if (input->isMouseButtonDown(SDL_BUTTON_LEFT) && priorFrameCanPress) { 
    //            setPressed(true);
    //        }
    //        else {
    //            setPressed(false);
    //            canPress = true;
    //        }

    //        if (input->isMouseButtonDown(SDL_BUTTON_LEFT) && !priorFrameCanPress) {
    //            canPress = false;
    //        }

    //    }
    //    else {
    //        setHovered(false);
    //        if (input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
    //            canPress = false;
    //        }

    //        // can't press if not hovering
    //        // Doesn't disable on press but prevents the mouse from accidentally activating.
    //        canPress = false;
    //    }
    //}
    //else {
    //    // reset state, does nothing if state already reset
    //    setPressed(false);
    //    setHovered(false);
    //    // can't press if invisible
    //    canPress = false;
    //}

    // new logic, much more readable :)
    // Gonna leave the old logic in because I plan to have it looked at

    SDL_Point mouseLocation = { input->getMouseX(), input->getMouseY() };
    bool priorFrameCanPress = canPress; 

    // instead of nesting, reset the button and return
    if (!isVisible()) {
        setPressed(false);
        setHovered(false);
        canPress = false;
        return;
    }

    bool isHovering = (mouseLocation.x > x && mouseLocation.x < x + width &&
        mouseLocation.y > y && mouseLocation.y < y + height);

    setHovered(isHovering);

    if (isHovering) {
        if (input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
            if (priorFrameCanPress) {
                setPressed(true);
            }
            else {
                canPress = false;
            }
        }
        else {
            setPressed(false);
            canPress = true;
        }
    }
    else {
        setPressed(false);
        // Instead of doing a bunch of complex checks, just set canPress to the opposite of mouse down since that/
        //  makes the widget not clickable if you were holding M1 before you dragged...DUH
        canPress = !input->isMouseButtonDown(SDL_BUTTON_LEFT);
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
}
