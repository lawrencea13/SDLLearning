#pragma once

#include <vector>
#include <memory>
#include "GameObject.h"

class ObjectRegistry {
public:
    ObjectRegistry() { }
    ~ObjectRegistry(){ }


    void registerObject(GameObject* obj);
    void removeObject(GameObject* obj);
    void update();
    void draw();

    std::vector<GameObject*> getObjects() {
        return objects;
    }

private:
    std::vector<GameObject*> objects;
};