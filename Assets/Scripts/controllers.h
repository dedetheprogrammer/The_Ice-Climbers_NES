#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include "raylib.h"

typedef enum {
    GAMEPAD_EJE_IZQ_X = 18,
    GAMEPAD_EJE_IZQ_Y,
    GAMEPAD_EJE_DER_X,
    GAMEPAD_EJE_DER_Y,
    GAMEPAD_EJE_GAT_IZQ,
    GAMEPAD_EJE_GAT_DER
} EjesGamepad;

extern std::unordered_map<KeyboardKey, Texture2D> key_textures;
extern std::unordered_map<GamepadButton, Texture2D> button_textures;
extern std::unordered_map<int, bool> validKeys;
extern std::unordered_map<int, std::string> code2key;
extern std::unordered_map<int, std::string> code2joy;

//-----------------------------------------------------------------------------
// Controller
//-----------------------------------------------------------------------------
class Controller {
private:
    bool isGamePadAxis(int binding);
    bool isGamePadTrigger(int binding);
public:
    enum Type{NO_TYPE=-1, CONTROLLER_0, CONTROLLER_1, CONTROLLER_2, CONTROLLER_3, KEYBOARD};
    Type type;
    enum Control{NO_CONTROL=-1, LEFT, RIGHT, DOWN, UP, ATTACK, JUMP};
    std::unordered_map<Control, int> kb_controls;
    std::unordered_map<Control, int> gp_controls;

    bool isPressed(Control c);
    bool isDown(Control c);
    std::string getActionBind(Control c);
    Texture2D getActionTexture(Control c);
    void printAll();
    void printKey(Type t, Control c);
};

void init_input_textures();

std::string to_string(Controller::Type type);

std::string to_string(Controller::Control control);

extern Controller Controller_0;
extern Controller Controller_1;
extern Controller Controller_2;
extern Controller Controller_3;
extern Controller* controllers[];

// Returns the controller number and control with which the binding is conflicted
// (only the first one if there are multiple and excepting 'control')
// if there is no conflict returns the pair (-1, NO_CONTROL)
std::pair<int, Controller::Control> isBindingConflicted(int controllerNumber, Controller::Control control, int binding);