#pragma once

#include <unordered_map>
#include <string>
#include "raylib.h"

//-----------------------------------------------------------------------------
// Controller
//-----------------------------------------------------------------------------
/**
 * @brief Controller component
 * 
 */
class Controller {
private:
    //...
public:
    enum Type{NO_TYPE=-1, CONTROLLER_0, CONTROLLER_1, CONTROLLER_2, CONTROLLER_3, KEYBOARD};
    Type type;
    enum Control{NO_CONTROL=-1, LEFT, RIGHT, DOWN, UP, JUMP, ATTACK};
    std::unordered_map<Control, int> controls;

    bool operator[](Control c) {
        if (c == NO_CONTROL) return false;

        if (type == KEYBOARD) {
            return IsKeyDown(controls[c]);
            
        } else if (type >= 0 && type <= 3) {
            // Gamepad checks
            if (c == LEFT) return (GetGamepadAxisMovement(type, controls[c]) < 0);
            else if (c == RIGHT) return (GetGamepadAxisMovement(type, controls[c]) > 0);
            else if (c == DOWN) return (GetGamepadAxisMovement(type, controls[c]) < 0);
            else if (c == UP) return (GetGamepadAxisMovement(type, controls[c]) > 0);
            else return IsGamepadButtonDown(type, controls[c]);

        } else return false;
    }
    
    int GetAxis(std::string axis) {
        if (axis == "Horizontal") {
            bool left_key  = operator[](LEFT);
            bool right_key = operator[](RIGHT);
            if (left_key && right_key) return 0;
            else if (left_key) return -1;
            else if (right_key) return 1;
        } else if (axis == "Vertical") {
            bool down_key  = operator[](DOWN);
            bool up_key    = operator[](UP);
            if (down_key && up_key) return 0;
            if (down_key) return -1;
            else if (up_key) return 1;
        }
        return 0;
    }
};

Controller Controller_0;
Controller Controller_1;
Controller Controller_2;
Controller Controller_3;