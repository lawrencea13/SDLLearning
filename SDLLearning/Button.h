#pragma once

#pragma once

#include "Widget.h"
#include <string>
#include <memory>

class Button : public Widget {
public:
    Button(int x, int y, int width, int height, SDL_Color color, const std::string& label);

    Button(int x, int y, int width, int height, std::shared_ptr<SDL_Texture> texture, const std::string& label);

    void drawImpl(SDL_Renderer* renderer) override;

    void onPress() override;
    void onRelease() override;

private:
    std::string label;
};