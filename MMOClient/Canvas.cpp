#include "Canvas.h"
#include "Logger.h"

#define LOG(...) Logger::Log(__VA_ARGS__)

Canvas::Canvas(SDL_Renderer* renderer, InputHandler* inputhandler, bool registerEventHandler) : renderer(renderer), inputHandler(inputhandler)
{
    if (registerEventHandler && inputHandler) {
        registerAsInputHandler();
    }
}

Canvas::~Canvas()
{
    if (inputHandler) {
		LOG("Removing input handlers upon canvas destruction with the following Handles: %zu, %zu, %zu, %zu, %zu",
            keyDownHandle, keyUpHandle, mouseDownHandle, mouseUpHandle, mouseMoveHandle);

		if (keyDownHandle != 0)
            inputHandler->removeKeyDownListener(keyDownHandle);

		if (keyUpHandle != 0)
		    inputHandler->removeKeyUpListener(keyUpHandle);

		if (mouseDownHandle != 0)
		    inputHandler->removeMouseDownListener(mouseDownHandle);

		if (mouseUpHandle != 0)
		    inputHandler->removeMouseUpListener(mouseUpHandle);

		if (mouseMoveHandle != 0)
		    inputHandler->removeMouseMoveListener(mouseMoveHandle);
    }
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
	std::cout << "Setting focus to a widget" << std::endl;
    if (focusedWidget && focusedWidget != w)
        focusedWidget->setFocus(false);
    focusedWidget = w;
    if (focusedWidget) {
        focusedWidget->setFocus(true);
        if(inputHandler) {
            inputHandler->setBlockPolledInput(focusedWidget->getConsumeInput());
		}
    }
        
}

void Canvas::clearFocus()
{
    if (focusedWidget) {
        focusedWidget->setFocus(false);
        focusedWidget = nullptr;
    }

    if (inputHandler) {
        inputHandler->setBlockPolledInput(false);
    }
}

void Canvas::handleKeyDown(SDL_Keycode key)
{
    if (focusedWidget) {
        focusedWidget->handleKeyDown(key);
    }
}

void Canvas::handleKeyUp(SDL_Keycode key)
{
    if (focusedWidget) {
        focusedWidget->handleKeyUp(key);
    }
}

void Canvas::handleMouseDown(Uint8 button)
{
    std::cout << "Canvas handling mouse down event" << std::endl;
    if (button == SDL_BUTTON_LEFT) {
        for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) {
            Widget* widget = it->get();
            if (widget->isVisible() && widget->containsPoint(mouseX, mouseY)) {
                widget->onPress();
                setFocus(widget);
                return;
            }
        }
        clearFocus();
    }
}

void Canvas::handleMouseUp(Uint8 button)
{
    if (button == SDL_BUTTON_LEFT && focusedWidget) {
        focusedWidget->onRelease();
        focusedWidget = nullptr;
    }
}

void Canvas::handleMouseMove(int x, int y)
{
	mouseX = x;
	mouseY = y;

    for (const auto& widget : widgets) {
        bool shouldBeHovered = widget->containsPoint(x, y) && widget->isVisible();
        if (widget->isHovered() != shouldBeHovered) {
            widget->setHovered(shouldBeHovered);
            // setHovered will call onHoverStateChanged()
        }
    }


    if (focusedWidget) {
        focusedWidget->handleMouseMove(x, y);
    }
}

void Canvas::registerAsInputHandler()
{
    if (!inputHandler || registeredAsInputHandler) return;
    registeredAsInputHandler = true;

    keyDownHandle = inputHandler->registerKeyDownListener([this](SDL_Keycode key) {
        this->handleKeyDown(key);
        });

    keyUpHandle = inputHandler->registerKeyUpListener([this](SDL_Keycode key) {
        this->handleKeyUp(key);
        });

    mouseDownHandle = inputHandler->registerMouseDownListener([this](Uint8 button) {
        this->handleMouseDown(button);
		});

    mouseUpHandle = inputHandler->registerMouseUpListener([this](Uint8 button) {
        this->handleMouseUp(button);
        });

    mouseMoveHandle = inputHandler->registerMouseMoveListener([this](int x, int y) {
        this->handleMouseMove(x, y);
		});
}
