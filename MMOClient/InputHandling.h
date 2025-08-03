#pragma once

#include <SDL.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>

class InputHandler {

public:
    using KeyCallback = std::function<void(SDL_Keycode)>;
    using MouseMoveCallback = std::function<void(int, int)>;
    using MouseButtonCallback = std::function<void(Uint8)>;
    using ListenerHandle = size_t;

    static constexpr size_t MAX_KEYS = 10;

    void update(SDL_Event* event) {

    }

    void clearPressedKeys() {
        pressedKeys.clear();
    }

    bool isKeyDown(SDL_Keycode key, bool ignoreBlocking = false) const {
        if (blockPolledInput && !ignoreBlocking) {
            return false; // Blocked input, return false
		}
        return std::find(pressedKeys.begin(), pressedKeys.end(), key) != pressedKeys.end();
    }

    const std::vector<SDL_Keycode>& getPressedKeys() const {
        return pressedKeys;
    }


    bool isMouseButtonDown(Uint8 button, bool ignoreBlocking = false) const {
        if (blockPolledInput && !ignoreBlocking) {
            return false; // Blocked input, return false
        }
        auto it = mouseButtonDown.find(button);
        return it != mouseButtonDown.end() && it->second;
    }

    int getMouseX() const { return mouseX; }
    int getMouseY() const { return mouseY; }

    void onKeyDown(SDL_Keycode key) {
        for (const auto& pair : keyDownListeners) {
            pair.second(key);
        }

        if (std::find(pressedKeys.begin(), pressedKeys.end(), key) == pressedKeys.end() &&
            pressedKeys.size() < MAX_KEYS) {
            pressedKeys.push_back(key);
        }
    }

    void onKeyUp(SDL_Keycode key) {
        for (const auto& pair : keyUpListeners) {
            pair.second(key);
        }

        auto it = std::remove(pressedKeys.begin(), pressedKeys.end(), key);
        pressedKeys.erase(it, pressedKeys.end());
    }

    void onMouseButtonDown(Uint8 button) {
        for (const auto& pair : mouseDownListeners) {
            pair.second(button);
        }
        mouseButtonDown[button] = true;
	}

    void onMouseButtonUp(Uint8 button) {
        for (const auto& pair : mouseUpListeners) {
            pair.second(button);
        }
        mouseButtonDown[button] = false;
    }

    void onMouseMotion(int x, int y) {
        for (const auto& pair : mouseMoveListeners) {
            pair.second(x, y);
        }

        mouseX = x;
        mouseY = y;
	}

    void setBlockPolledInput(bool block) {
        blockPolledInput = block;
    }

#pragma region Key Press listeners

    ListenerHandle  registerKeyDownListener(const KeyCallback& cb) {
        keyDownListeners[nextKeyDownHandle] = cb;
        return nextKeyDownHandle++;
    }
    ListenerHandle  registerKeyUpListener(const KeyCallback& cb) {
        keyUpListeners[nextKeyUpHandle] = cb;
        return nextKeyUpHandle++;
    }

    void removeKeyDownListener(ListenerHandle handle) {
        keyDownListeners.erase(handle);
    }
    void removeKeyUpListener(ListenerHandle handle) {
        keyUpListeners.erase(handle);
    }
#pragma endregion

#pragma region Mouse Listeners button/movement

    ListenerHandle  registerMouseDownListener(const MouseButtonCallback& cb) {
        mouseDownListeners[nextMouseDownHandle] = cb;
        return nextMouseDownHandle++;
    }
    ListenerHandle  registerMouseUpListener(const MouseButtonCallback& cb) {
        mouseUpListeners[nextMouseUpHandle] = cb;
        return nextMouseUpHandle++;
    }
    ListenerHandle  registerMouseMoveListener(const MouseMoveCallback& cb) {
        mouseMoveListeners[nextmouseMoveHandle] = cb;
        return nextmouseMoveHandle++;
    }

    void removeMouseUpListener(ListenerHandle handle) {
        keyDownListeners.erase(handle);
    }
    void removeMouseDownListener(ListenerHandle handle) {
        mouseDownListeners.erase(handle);
    }
    void removeMouseMoveListener(ListenerHandle handle) {
        mouseMoveListeners.erase(handle);
    }
  
#pragma endregion


private:
    std::vector<SDL_Keycode> pressedKeys;
    // simpler, order not needed so map works fine.
    std::unordered_map<Uint8, bool> mouseButtonDown;
    int mouseX = 0;
    int mouseY = 0;

    std::unordered_map<ListenerHandle, KeyCallback> keyDownListeners;
    std::unordered_map<ListenerHandle, KeyCallback> keyUpListeners;
    std::unordered_map<ListenerHandle, MouseMoveCallback> mouseMoveListeners;
    std::unordered_map<ListenerHandle, MouseButtonCallback> mouseDownListeners;
    std::unordered_map<ListenerHandle, MouseButtonCallback> mouseUpListeners;

    // store handles for deregistration as needed
    ListenerHandle nextKeyDownHandle = 1;
    ListenerHandle nextKeyUpHandle = 1;
    ListenerHandle nextmouseMoveHandle = 1;
    ListenerHandle nextMouseDownHandle = 1;
    ListenerHandle nextMouseUpHandle = 1;

    bool blockPolledInput = false;
};