#pragma once
#ifndef ENGINEUI_H
#define ENGINEUI_H

#include "raylib.h"

enum WINDOW_BEHAVOR { COLLISION = 0x01, TRAVERSE = 0x02, IGNORE = 0x04 };
bool WINDOW_LIMITS_BEHAVOR (WINDOW_BEHAVOR flag) {
    return false;
}

class Flicker {
private:
    float current_time; // Current time in the current state.
public:
    bool  action;       // Indicates if is the moment to perform the action or not. 
    float trigger_time; // Time for triggering into the next state.

    Flicker() : action(false), current_time(0), trigger_time(0) {}
    Flicker(float trigger_time) : action(false), current_time(0), trigger_time(trigger_time) {}

    bool Trigger(float deltaTime) {
        current_time += deltaTime;
        if (current_time >= trigger_time) {
            action = !action;
            current_time = 0;
        }
        return action;
    }
};

/**
 * @brief (UNUSED) A Sprite instance.
 * 
 */
class Sprite {
private:
    Rectangle src; // Rectangle that selects the region of the sprite.
    Rectangle dst; // Rectangle that shows the region with the transformations.
public:

    Texture2D sprite;

    Sprite(const char *fileName) {
        sprite = LoadTexture(fileName);
        src = dst = {0,0,(float)sprite.width,(float)sprite.height};
    }

    Sprite(const char *fileName, float scale) {
        sprite = LoadTexture(fileName);
        src = {0,0,(float)sprite.width, (float)sprite.height};
        dst = {0,0,sprite.width*scale, sprite.height*scale};
    }

    Sprite(const char* fileName, float scale, Vector2 dest) {
        sprite = LoadTexture(fileName);
        src = {0,0,(float)sprite.width, (float)sprite.height};
        dst = {dest.x, dest.y, sprite.width*scale, sprite.height*scale};  
    }

    void Draw() {
        DrawTexturePro(sprite, src, dst, Vector2{0,0}, 0, WHITE);
    }
};

#endif