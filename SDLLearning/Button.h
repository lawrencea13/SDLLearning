#pragma once


#include "Widget.h"
#include <string>
#include <memory>
#include <functional>


class Button : public Widget {
public:
    Button(int x, int y, int width, int height, SDL_Color color, const std::string& label, InputHandler* inputMgr, std::shared_ptr<TTF_Font> font = nullptr);

    Button(int x, int y, int width, int height, std::shared_ptr<SDL_Texture> texture, const std::string& label);

    void drawImpl(SDL_Renderer* renderer) override;

    std::function<void()> onPressCallback;
    std::function<void()> onReleaseCallback;

    void setOnPress(std::function<void()> callback);
    void setOnRelease(std::function<void()> callback);

    void onPress() override;
    void onRelease() override;
    void onHoverStateChanged() override;

private:
    std::string label;
};