#pragma once
#include <vector>
#include <memory>
#include "Widget.h"

class Canvas {
public:
    Canvas(SDL_Renderer* renderer);
    Canvas() { }
    ~Canvas();

    void draw();
    void update();
    void addWidget(std::shared_ptr<Widget> widget);

    const std::vector<std::shared_ptr<Widget>>& getWidgets() const;
    std::vector<std::shared_ptr<Widget>> getWidgetsbyLayer(const std::string& layer) const;
    std::vector<std::shared_ptr<Widget>> getWidgetNotinLayer(const std::string& layer) const;

private:
    SDL_Renderer* renderer;
    std::vector<std::shared_ptr<Widget>> widgets;
};