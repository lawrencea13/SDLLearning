#include "ObjectRegistry.h"

void ObjectRegistry::registerObject(GameObject* obj) {
    objects.push_back(obj);
}

void ObjectRegistry::removeObject(GameObject* obj) {
    objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
}

void ObjectRegistry::update() {
    for (auto& obj : objects) {
        obj->Update();
    }
}