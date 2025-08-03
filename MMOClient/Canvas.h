#pragma once
#include <vector>
#include <memory>
#include "Widget.h"
#include "InputHandling.h"

/*
Canvas is more of a layer to handle the management of events between widgets and the game itself rather than a direct drawing surface.
Instead of widgets directly handling events through event listeners, the Canvas will handle the events and pass them to the widgets.
*/

class Canvas {
public:
    Canvas(SDL_Renderer* renderer, InputHandler* inputhandler, bool registerEventHandler = true);
    //Canvas() { }
    ~Canvas();

    void draw();
    void update();
    void addWidget(std::shared_ptr<Widget> widget);

    const std::vector<std::shared_ptr<Widget>>& getWidgets() const;
    std::vector<std::shared_ptr<Widget>> getWidgetsbyLayer(const std::string& layer) const;
    std::vector<std::shared_ptr<Widget>> getWidgetNotinLayer(const std::string& layer) const;
    void setFocus(Widget* w);
    Widget* getFocusedWidget() const { return focusedWidget; }
    void clearFocus();
    void handleKeyDown(SDL_Keycode key);
    void handleKeyUp(SDL_Keycode key);

    void handleMouseDown(Uint8 button);
    void handleMouseUp(Uint8 button);

	void handleMouseMove(int x, int y);

    void registerAsInputHandler();


private:
    SDL_Renderer* renderer;
    InputHandler* inputHandler = nullptr;
    std::vector<std::shared_ptr<Widget>> widgets;
    Widget* focusedWidget = nullptr;

	int mouseX = 0;
	int mouseY = 0;
    
    // name conflicts with the InputHandler class kind of, this just means the canvas is receiving input events from inputHandler
	bool registeredAsInputHandler = false;

    size_t keyDownHandle = 0;
    size_t keyUpHandle = 0;
    size_t mouseDownHandle = 0;
    size_t mouseUpHandle = 0;
    size_t mouseMoveHandle = 0;
};