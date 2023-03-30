#pragma once
#ifndef EFFECTS_H
#define EFFECTS_H

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


#endif
