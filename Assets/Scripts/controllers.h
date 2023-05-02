#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include "raylib.h"


std::unordered_map<int, std::string> code2key = {
    {KEY_NULL, "NULL"},
    // Alphanumeric keys
    {KEY_APOSTROPHE, "APOSTROPHE"},
    {KEY_COMMA, "COMMA"},
    {KEY_MINUS, "MINUS"},
    {KEY_PERIOD, "PERIOD"},
    {KEY_SLASH, "SLASH"},
    {KEY_ZERO, "ZERO"},
    {KEY_ONE, "ONE"},
    {KEY_TWO, "TWO"},
    {KEY_THREE, "THREE"},
    {KEY_FOUR, "FOUR"},
    {KEY_FIVE, "FIVE"},
    {KEY_SIX, "SIX"},
    {KEY_SEVEN, "SEVEN"},
    {KEY_EIGHT, "EIGHT"},
    {KEY_NINE, "NINE"},
    {KEY_SEMICOLON, "SEMICOLON"},
    {KEY_A, "A"},
    {KEY_B, "B"},
    {KEY_C, "C"},
    {KEY_D, "D"},
    {KEY_E, "E"},
    {KEY_F, "F"},
    {KEY_G, "G"},
    {KEY_H, "H"},
    {KEY_I, "I"},
    {KEY_J, "J"},
    {KEY_K, "K"},
    {KEY_L, "L"},
    {KEY_M, "M"},
    {KEY_N, "N"},
    {KEY_O, "O"},
    {KEY_P, "P"},
    {KEY_Q, "Q"},
    {KEY_R, "R"},
    {KEY_S, "S"},
    {KEY_T, "T"},
    {KEY_U, "U"},
    {KEY_V, "V"},
    {KEY_W, "W"},
    {KEY_X, "X"},
    {KEY_Y, "Y"},
    {KEY_Z, "Z"},
    {KEY_LEFT_BRACKET, "LEFT_BRACKET"},
    {KEY_BACKSLASH, "BACKSLASH"},
    {KEY_RIGHT_BRACKET, "RIGHT_BRACKET"},
    {KEY_GRAVE, "GRAVE"},
    // Function keys
    {KEY_SPACE, "SPACE"},
    {KEY_ESCAPE, "ESCAPE"},
    {KEY_ENTER, "ENTER"},
    {KEY_TAB, "TAB"},
    {KEY_BACKSPACE, "BACKSPACE"},
    {KEY_RIGHT, "RIGHT"},
    {KEY_LEFT, "LEFT"},
    {KEY_DOWN, "DOWN"},
    {KEY_UP, "UP"},
    {KEY_PAGE_UP, "PAGE_UP"},
    {KEY_PAGE_DOWN, "PAGE_DOWN"},
    {KEY_HOME, "HOME"},
    {KEY_END, "END"},
    {KEY_CAPS_LOCK, "CAPS_LOCK"},
    {KEY_SCROLL_LOCK, "SCROLL_LOCK"},
    {KEY_NUM_LOCK, "NUM_LOCK"},
    {KEY_PRINT_SCREEN, "PRINT_SCREEN"},
    {KEY_PAUSE, "PAUSE"},
    {KEY_F1, "F1"},
    {KEY_F2, "F2"},
    {KEY_F3, "F3"},
    {KEY_F4, "F4"},
    {KEY_F5, "F5"},
    {KEY_F6, "F6"},
    {KEY_F7, "F7"},
    {KEY_F8, "F8"},
    {KEY_F9, "F9"},
    {KEY_F10, "F10"},
    {KEY_F11, "F11"},
    {KEY_F12, "F12"},
    {KEY_LEFT_SHIFT, "LEFT_SHIFT"},
    {KEY_LEFT_CONTROL, "LEFT_CONTROL"},
    {KEY_LEFT_ALT, "LEFT_ALT"},
    {KEY_LEFT_SUPER, "LEFT_SUPER"},
    {KEY_RIGHT_SHIFT, "RIGHT_SHIFT"},
    {KEY_RIGHT_CONTROL, "RIGHT_CONTROL"},
    {KEY_RIGHT_ALT, "RIGHT_ALT"},
    {KEY_RIGHT_SUPER, "RIGHT_SUPER"},
    {KEY_KB_MENU, "KB_MENU"},
    // Keypad keys
    {KEY_KP_0, "KP_0"},
    {KEY_KP_1, "KP_1"},
    {KEY_KP_2, "KP_2"},
    {KEY_KP_3, "KP_3"},
    {KEY_KP_4, "KP_4"},
    {KEY_KP_5, "KP_5"},
    {KEY_KP_6, "KP_6"},
    {KEY_KP_7, "KP_7"},
    {KEY_KP_8, "KP_8"},
    {KEY_KP_9, "KP_9"},
    {KEY_KP_DECIMAL, "KP_DECIMAL"},
    {KEY_KP_DIVIDE, "KP_DIVIDE"},
    {KEY_KP_MULTIPLY, "KP_MULTIPLY"},
    {KEY_KP_SUBTRACT, "KP_SUBTRACT"},
    {KEY_KP_ADD, "KP_ADD"},
    {KEY_KP_ENTER, "KP_ENTER"},
    {KEY_KP_EQUAL, "KP_EQUAL"},
    // Android key buttons
    {KEY_BACK, "BACK"},
    {KEY_MENU, "MENU"},
    {KEY_VOLUME_UP, "VOLUME_UP"},
    {KEY_VOLUME_DOWN, "VOLUME_DOWN"}
};
std::unordered_map<int, std::string> code2joy = {
    {GAMEPAD_BUTTON_UNKNOWN, "UNKNOWN"},
    {GAMEPAD_BUTTON_LEFT_FACE_UP, "LEFT_FACE_UP"},
    {GAMEPAD_BUTTON_LEFT_FACE_RIGHT, "LEFT_FACE_RIGHT"},
    {GAMEPAD_BUTTON_LEFT_FACE_DOWN, "LEFT_FACE_DOWN"},
    {GAMEPAD_BUTTON_LEFT_FACE_LEFT, "LEFT_FACE_LEFT"},
    {GAMEPAD_BUTTON_RIGHT_FACE_UP, "RIGHT_FACE_UP"},
    {GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, "RIGHT_FACE_RIGHT"},
    {GAMEPAD_BUTTON_RIGHT_FACE_DOWN, "RIGHT_FACE_DOWN"},
    {GAMEPAD_BUTTON_RIGHT_FACE_LEFT, "RIGHT_FACE_LEFT"},
    {GAMEPAD_BUTTON_LEFT_TRIGGER_1, "LEFT_TRIGGER_1"},
    {GAMEPAD_BUTTON_LEFT_TRIGGER_2, "LEFT_TRIGGER_2"},
    {GAMEPAD_BUTTON_RIGHT_TRIGGER_1, "RIGHT_TRIGGER_1"},
    {GAMEPAD_BUTTON_RIGHT_TRIGGER_2, "RIGHT_TRIGGER_2"},
    {GAMEPAD_BUTTON_MIDDLE_LEFT, "MIDDLE_LEFT"},
    {GAMEPAD_BUTTON_MIDDLE, "MIDDLE"},
    {GAMEPAD_BUTTON_MIDDLE_RIGHT, "MIDDLE_RIGHT"},
    {GAMEPAD_BUTTON_LEFT_THUMB, "LEFT_THUMB"},
    {GAMEPAD_BUTTON_RIGHT_THUMB, "RIGHT_THUMB"},
    {GAMEPAD_AXIS_LEFT_X, "AXIS_LEFT_X"},
    {GAMEPAD_AXIS_LEFT_Y, "AXIS_LEFT_Y"},
    {GAMEPAD_AXIS_RIGHT_X, "AXIS_RIGHT_X"},
    {GAMEPAD_AXIS_RIGHT_Y, "AXIS_RIGHT_Y"},
    {GAMEPAD_AXIS_LEFT_TRIGGER, "AXIS_LEFT_TRIGGER"},
    {GAMEPAD_AXIS_RIGHT_TRIGGER, "AXIS_RIGHT_TRIGGER"}
};
//-----------------------------------------------------------------------------
// Controller
//-----------------------------------------------------------------------------
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
    std::unordered_map<Control, int> kb_controls;
    std::unordered_map<Control, int> gp_controls;


    bool isPressed(Control c) {

        if (c == NO_CONTROL) return false;

        if (type == KEYBOARD) {
            return IsKeyPressed(kb_controls[c]);
            
        } else if (type >= 0 && type <= 3) {
            // Gamepad checks
            if (!IsGamepadAvailable(type)) return false;

            if (isGamePadAxis(gp_controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, gp_controls[c]) < -0.5);
            else if (isGamePadAxis(gp_controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, gp_controls[c]) > 0.5);
            else if (isGamePadAxis(gp_controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, gp_controls[c]) < -0.5);
            else if (isGamePadAxis(gp_controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, gp_controls[c]) > 0.5);

            else if (isGamePadTrigger(gp_controls[c])) return (GetGamepadAxisMovement(type, gp_controls[c]) > 0.5);
            else return IsGamepadButtonPressed(type, gp_controls[c]);

        } else return false;
    }
    
    bool isDown(Control c) {

        if (c == NO_CONTROL) return false;

        if (type == KEYBOARD) {
            return IsKeyDown(kb_controls[c]);
            
        } else if (type >= 0 && type <= 3) {
            // Gamepad checks
            if (!IsGamepadAvailable(type)) return false;

            if (isGamePadAxis(gp_controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, gp_controls[c]) < -0.15);
            else if (isGamePadAxis(gp_controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, gp_controls[c]) > 0.15);
            else if (isGamePadAxis(gp_controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, gp_controls[c]) < -0.15);
            else if (isGamePadAxis(gp_controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, gp_controls[c]) > 0.15);

            else if (isGamePadTrigger(gp_controls[c])) return (GetGamepadAxisMovement(type, gp_controls[c]) > 0.15);
            else return IsGamepadButtonDown(type, gp_controls[c]);

        } else return false;
    }

    std::string getActionBind(Control c) { return (type == Type::KEYBOARD) ? code2key[kb_controls[c]] : code2joy[gp_controls[c]]; }

    void printAll() {
        for (auto kv : kb_controls) {
            printKey(KEYBOARD, kv.first);
        }
        for (auto kv : gp_controls) {
            printKey(CONTROLLER_0, kv.first);
        }
    }

    void printKey(Type t, Control c) {
        std::string keybind = "NONE";
        std::string controllerName;
        bool isActive = false;
        if (t == Type::KEYBOARD) {
            keybind = code2key[kb_controls[c]];
            isActive = IsKeyDown(kb_controls[c]);
            controllerName = "Keyboard";
        } else {
            keybind = code2joy[gp_controls[c]];
            if (isGamePadAxis(gp_controls[c]) || isGamePadTrigger(gp_controls[c])) isActive = (std::fabs(GetGamepadAxisMovement(t, gp_controls[c])) > 0.15);
            else isActive = IsGamepadButtonDown(t, gp_controls[c]);
            controllerName = "Controller_" + std::to_string(t);
        }

        std::cout << "Controller: " << controllerName << ", Control: " << c << " , Keybind: " << keybind << " , Active: " << isActive << std::endl;
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

std::string to_string(Controller::Type type) {
    if (type == Controller::CONTROLLER_0) {
        return "CONTROLLER 1";
    } else if (type == Controller::CONTROLLER_1) {
        return "CONTROLLER 2";
    } else if (type == Controller::CONTROLLER_2) {
        return "CONTROLLER 3";
    } else if (type == Controller::CONTROLLER_3) {
        return "CONTROLLER 4";
    } else if (type == Controller::KEYBOARD) {
        return "KEYBOARD";
    } else if (type == Controller::NO_TYPE) {
        return "NOT DEFINED";
    } else {
        return "?!";
    }
}
Controller Controller_0;
Controller Controller_1;
Controller Controller_2;
Controller Controller_3;


Controller* controllers[] = {&Controller_0, &Controller_1, &Controller_2, &Controller_3};