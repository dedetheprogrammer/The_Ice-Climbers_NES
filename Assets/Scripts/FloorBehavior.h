#pragma once
#include <random>
#include "EngineECS.h"
#include "raylib.h"

class FloorBehavior : public Script {
private:
    //...
public:
    Transform2D& transform;
    int floor_level;
    FloorBehavior(GameObject& gameObject) : Script(gameObject),
        transform(gameObject.getComponent<Transform2D>()) {}

    FloorBehavior(GameObject& gameObject, FloorBehavior& behavior) : Script(gameObject),
        transform(gameObject.getComponent<Transform2D>()) {}

    Component* Clone(GameObject& gameObject) override {
        return new FloorBehavior(gameObject, *this);
    }
};