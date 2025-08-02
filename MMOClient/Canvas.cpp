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

std::vector<std::shared_ptr<Widget>> Canvas::getWidgetsbyLayer(const std::string& layer) const
{
    std::vector<std::shared_ptr<Widget>> filteredWidgets;

    for (const auto& widget : widgets) {
        if (widget->getLayer() == layer) {
            filteredWidgets.push_back(widget);
        }
    }

    return filteredWidgets;
}

std::vector<std::shared_ptr<Widget>> Canvas::getWidgetNotinLayer(const std::string& layer) const
{
    std::vector<std::shared_ptr<Widget>> filteredWidgets;

    for (const auto& widget : widgets) {
        if (widget->getLayer() != layer) {
            filteredWidgets.push_back(widget);
        }
    }

    return filteredWidgets;
}

void Canvas::setFocus(Widget* w)
{
    if (focusedWidget && focusedWidget != w)
        focusedWidget->setFocus(false);
    focusedWidget = w;
    if (focusedWidget)
        focusedWidget->setFocus(true);
}

void Canvas::clearFocus()
{
    if (focusedWidget) {
        focusedWidget->setFocus(false);
        focusedWidget = nullptr;
    }
}

bool Canvas::handleKeyInput(SDL_Keycode key)
{
    if (focusedWidget) {
        return focusedWidget->handleKeyInput(key);
    }

    return false;
}
