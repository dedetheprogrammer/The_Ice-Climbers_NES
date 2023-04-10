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
    bool isGamePadAxis(int binding) {
        return  binding == GAMEPAD_AXIS_LEFT_X  ||
                binding == GAMEPAD_AXIS_LEFT_Y  ||
                binding == GAMEPAD_AXIS_RIGHT_X ||
                binding == GAMEPAD_AXIS_RIGHT_Y;
    }
    bool isGamePadTrigger(int binding) {
        return  binding == GAMEPAD_AXIS_LEFT_TRIGGER    ||
                binding == GAMEPAD_AXIS_RIGHT_TRIGGER;
    }
public:
    enum Type{NO_TYPE=-1, CONTROLLER_0, CONTROLLER_1, CONTROLLER_2, CONTROLLER_3, KEYBOARD};
    Type type;
    enum Control{NO_CONTROL=-1, LEFT, RIGHT, DOWN, UP, ATTACK, JUMP};
    std::unordered_map<Control, int> controls;


    bool isPressed(Control c) {
        if (c == NO_CONTROL) return false;

        if (type == KEYBOARD) {
            return IsKeyPressed(controls[c]);
            
        } else if (type >= 0 && type <= 3) {
            // Gamepad checks
            if (!IsGamepadAvailable(type)) return false;

            if (isGamePadAxis(controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, controls[c]) < -0.5);
            else if (isGamePadAxis(controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, controls[c]) > 0.5);
            else if (isGamePadAxis(controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, controls[c]) < -0.5);
            else if (isGamePadAxis(controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, controls[c]) > 0.5);

            else if (isGamePadTrigger(controls[c])) return (GetGamepadAxisMovement(type, controls[c]) > 0.5);
            else return IsGamepadButtonPressed(type, controls[c]);

        } else return false;
    }
    
    bool isDown(Control c) {
        if (c == NO_CONTROL) return false;

        if (type == KEYBOARD) {
            return IsKeyDown(controls[c]);
            
        } else if (type >= 0 && type <= 3) {
            // Gamepad checks
            if (!IsGamepadAvailable(type)) return false;

            if (isGamePadAxis(controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, controls[c]) < 0.15);
            else if (isGamePadAxis(controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, controls[c]) > 0.15);
            else if (isGamePadAxis(controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, controls[c]) < 0.15);
            else if (isGamePadAxis(controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, controls[c]) > 0.15);

            else if (isGamePadTrigger(controls[c])) return (GetGamepadAxisMovement(type, controls[c]) > 0.15);
            else return IsGamepadButtonDown(type, controls[c]);

        } else return false;
    }
    /*
    bool isReleased(Control c) {
        if (c == NO_CONTROL) return false;

        if (type == KEYBOARD) {
            return IsKeyReleased(controls[c]);
            
        } else if (type >= 0 && type <= 3) {
            // Gamepad checks
            if (!IsGamepadAvailable(type)) return false;

            if (isGamePadAxis(controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, controls[c]) < 0);
            else if (isGamePadAxis(controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, controls[c]) > 0);
            else if (isGamePadAxis(controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, controls[c]) < 0);
            else if (isGamePadAxis(controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, controls[c]) > 0);
            else return IsGamepadButtonReleased(type, controls[c]);

        } else return false;
    }
    */
    /*
    bool isUp(Control c) {
        if (c == NO_CONTROL) return false;

        if (type == KEYBOARD) {
            return IsKeyUp(controls[c]);
            
        } else if (type >= 0 && type <= 3) {
            // Gamepad checks
            if (!IsGamepadAvailable(type)) return false;

            if (isGamePadAxis(controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, controls[c]) < 0);
            else if (isGamePadAxis(controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, controls[c]) > 0);
            else if (isGamePadAxis(controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, controls[c]) < 0);
            else if (isGamePadAxis(controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, controls[c]) > 0);
            else return IsGamepadButtonUp(type, controls[c]);

        } else return false;
    }
    */
};

Controller Controller_0;
Controller Controller_1;
Controller Controller_2;
Controller Controller_3;

Controller* controllers[] = {&Controller_0, &Controller_1, &Controller_2, &Controller_3};