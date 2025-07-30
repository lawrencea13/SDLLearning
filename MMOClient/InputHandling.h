#pragma once

#include <SDL.h>
#include <unordered_map>
#include <string>
#include <iostream>

class InputHandler {

public:
    void update(SDL_Event* event) {
		
        if (event->type == SDL_KEYDOWN) {
            keysDown[event->key.keysym.sym] = true;
        }
        else if (event->type == SDL_KEYUP) {
            keysDown[event->key.keysym.sym] = false;
        }


        if (event->type == SDL_MOUSEBUTTONDOWN) {
            mouseButtonDown[event->button.button] = true;
        }
        else if (event->type == SDL_MOUSEBUTTONUP) {
            mouseButtonDown[event->button.button] = false;
        }

        if (event->type == SDL_MOUSEMOTION) {
            mouseX = event->motion.x;
            mouseY = event->motion.y;
        }
    }

    bool isKeyDown(SDL_Keycode key) const {
        auto it = keysDown.find(key);
        return it != keysDown.end() && it->second;
    }

    bool isMouseButtonDown(Uint8 button) const {
        auto it = mouseButtonDown.find(button);
        return it != mouseButtonDown.end() && it->second;
    }

    int getMouseX() const { return mouseX; }
    int getMouseY() const { return mouseY; }

private:
    std::unordered_map<SDL_Keycode, bool> keysDown;
    std::unordered_map<Uint8, bool> mouseButtonDown;
    int mouseX = 0;
    int mouseY = 0;
};