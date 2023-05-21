#pragma once
#include "EngineECS.h"
#include <iostream>
#include <random>

class CloudBehavior : public Script {
private:

    float current_cooldown;
    float cooldown;
    bool spawned;
    bool started;
    RigidBody2D& rigidbody;
    Sprite& sprite;
    Transform2D& transform;

    int random_sense();

public:
    int floor_level;
    CloudBehavior(GameObject& gameObject);
    CloudBehavior(GameObject& gameObject, CloudBehavior behavior);
    Component* Clone(GameObject& gameObject);
    void Update() override;
};