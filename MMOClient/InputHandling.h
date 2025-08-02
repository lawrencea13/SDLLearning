#pragma once

#include <SDL.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <algorithm>

class InputHandler {

public:
    using KeyCallback = std::function<void(SDL_Keycode)>;

    static constexpr size_t MAX_KEYS = 10;

    void update(SDL_Event* event) {
        if (event->type == SDL_MOUSEMOTION) {
            mouseX = event->motion.x;
            mouseY = event->motion.y;
        }
    }

    void clearPressedKeys() {
        pressedKeys.clear();
    }

    bool isKeyDown(SDL_Keycode key) const {
        return std::find(pressedKeys.begin(), pressedKeys.end(), key) != pressedKeys.end();
    }

    const std::vector<SDL_Keycode>& getPressedKeys() const {
        return pressedKeys;
    }


    bool isMouseButtonDown(Uint8 button) const {
        auto it = mouseButtonDown.find(button);
        return it != mouseButtonDown.end() && it->second;
    }

    int getMouseX() const { return mouseX; }
    int getMouseY() const { return mouseY; }

    void onKeyDown(SDL_Keycode key) {
        if (std::find(pressedKeys.begin(), pressedKeys.end(), key) == pressedKeys.end() &&
            pressedKeys.size() < MAX_KEYS) {
            pressedKeys.push_back(key);
        }

        for (auto& cb : keyDownListeners) {
            cb(key);
        }
    }

    void onKeyUp(SDL_Keycode key) {
        auto it = std::remove(pressedKeys.begin(), pressedKeys.end(), key);
        pressedKeys.erase(it, pressedKeys.end());

        for (auto& cb : keyUpListeners) {
            cb(key);
        }
    }

    void onMouseButtonDown(Uint8 button) {
        mouseButtonDown[button] = true;
	}

    void onMouseButtonUp(Uint8 button) {
        mouseButtonDown[button] = false;
    }

    // callback registration
    void registerKeyDownListener(const KeyCallback& cb) {
        keyDownListeners.push_back(cb);
    }
    void registerKeyUpListener(const KeyCallback& cb) {
        keyUpListeners.push_back(cb);
    }

private:
    std::vector<SDL_Keycode> pressedKeys;
    // simpler, order not needed so map works fine.
    std::unordered_map<Uint8, bool> mouseButtonDown;
    int mouseX = 0;
    int mouseY = 0;

    std::vector<KeyCallback> keyDownListeners;
    std::vector<KeyCallback> keyUpListeners;
};