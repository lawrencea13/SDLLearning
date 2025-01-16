#include "Canvas.h"

Canvas::Canvas(SDL_Renderer* renderer) : renderer(renderer)
{
    // NO CHILDREN
}

Canvas::~Canvas()
{
}

void Canvas::draw()
{
    for (const auto& widget : widgets) {
        widget->drawIfVisible(renderer);
    }
}

void Canvas::update()
{
    for (auto& widget : widgets) {
        widget->update();
    }
}

void Canvas::addWidget(std::shared_ptr<Widget> widget)
{
    widgets.push_back(widget);
}

const std::vector<std::shared_ptr<Widget>>& Canvas::getWidgets() const
{
    return widgets;
}
