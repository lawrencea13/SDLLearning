#pragma once


struct Rect {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    Rect() = default;
    Rect(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_) {}

    bool intersects(const Rect& other) const {
        return !(x + w <= other.x || x >= other.x + other.w ||
            y + h <= other.y || y >= other.y + other.h);
    }

    bool contains(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};