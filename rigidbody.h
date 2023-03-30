#pragma once
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "raylib.h"
#include "raylib_ext.h"

class RigidBody2D {
private:
    // ...
public:
    float mass;           // Mass of the object.
    float gravity;        // Gravity acceleration.
    Vector2 velocity;     // Current speed of the object.
    Vector2 max_velocity; // Max velocity that the object can reach.
    Vector2 acceleration; // Object accelerations.

    RigidBody2D() : mass(0), gravity(0), velocity({}) {}
    RigidBody2D(float mass, float gravity, Vector2 max_velocity, Vector2 acceleration) {
        this->mass    = mass;
        this->gravity = gravity;
        velocity = {0,0};
        this->max_velocity = max_velocity;
        this->acceleration = acceleration;
    }

    void Draw(Vector2 center) {
        DrawLineEx(center, center + velocity, 3.0f, BLUE);
        DrawLineEx(center, center + (Vector2){velocity.x, 0.0f}, 3.0f, RED);
        DrawLineEx(center, center + (Vector2){0.0f, velocity.y}, 3.0f, GREEN);
    }
};

#endif