#include "Controllers.h"

std::unordered_map<KeyboardKey, Texture2D> key_textures = {};
std::unordered_map<GamepadButton, Texture2D> button_textures = {};
std::unordered_map<int, bool> validKeys = {
    {KEY_NULL, false},
    // Alphanumeric keys
    {KEY_APOSTROPHE, true},
    {KEY_COMMA, true},
    {KEY_MINUS, true},
    {KEY_PERIOD, true},
    {KEY_SLASH, true},
    {KEY_ZERO, true},
    {KEY_ONE, true},
    {KEY_TWO, true},
    {KEY_THREE, true},
    {KEY_FOUR, true},
    {KEY_FIVE, true},
    {KEY_SIX, true},
    {KEY_SEVEN, true},
    {KEY_EIGHT, true},
    {KEY_NINE, true},
    {KEY_SEMICOLON, true},
    {KEY_A, true},
    {KEY_B, true},
    {KEY_C, true},
    {KEY_D, true},
    {KEY_E, true},
    {KEY_F, true},
    {KEY_G, true},
    {KEY_H, true},
    {KEY_I, true},
    {KEY_J, true},
    {KEY_K, true},
    {KEY_L, true},
    {KEY_M, true},
    {KEY_N, true},
    {KEY_O, true},
    {KEY_P, true},
    {KEY_Q, true},
    {KEY_R, true},
    {KEY_S, true},
    {KEY_T, true},
    {KEY_U, true},
    {KEY_V, true},
    {KEY_W, true},
    {KEY_X, true},
    {KEY_Y, true},
    {KEY_Z, true},
    {KEY_LEFT_BRACKET, true},
    {KEY_BACKSLASH, true},
    {KEY_RIGHT_BRACKET, true},
    {KEY_GRAVE, true},
    // Function keys
    {KEY_SPACE, true},
    {KEY_ESCAPE, false},
    {KEY_ENTER, true},
    {KEY_TAB, true},
    {KEY_BACKSPACE, true},
    {KEY_RIGHT, true},
    {KEY_LEFT, true},
    {KEY_DOWN, true},
    {KEY_UP, true},
    {KEY_PAGE_UP, false},
    {KEY_PAGE_DOWN, false},
    {KEY_HOME, false},
    {KEY_END, false},
    {KEY_CAPS_LOCK, true},
    {KEY_SCROLL_LOCK, false},
    {KEY_NUM_LOCK, false},
    {KEY_PRINT_SCREEN, false},
    {KEY_PAUSE, false},
    {KEY_F1, false},
    {KEY_F2, false},
    {KEY_F3, false},
    {KEY_F4, false},
    {KEY_F5, false},
    {KEY_F6, false},
    {KEY_F7, false},
    {KEY_F8, false},
    {KEY_F9, false},
    {KEY_F10, false},
    {KEY_F11, false},
    {KEY_F12, false},
    {KEY_LEFT_SHIFT, true},
    {KEY_LEFT_CONTROL, true},
    {KEY_LEFT_ALT, true},
    {KEY_LEFT_SUPER, false},
    {KEY_RIGHT_SHIFT, true},
    {KEY_RIGHT_CONTROL, false},
    {KEY_RIGHT_ALT, true},
    {KEY_RIGHT_SUPER, false},
    {KEY_KB_MENU, false},
    // Keypad keys
    {KEY_KP_0, true},
    {KEY_KP_1, true},
    {KEY_KP_2, true},
    {KEY_KP_3, true},
    {KEY_KP_4, true},
    {KEY_KP_5, true},
    {KEY_KP_6, true},
    {KEY_KP_7, true},
    {KEY_KP_8, true},
    {KEY_KP_9, true},
    {KEY_KP_DECIMAL, true},
    {KEY_KP_DIVIDE, true},
    {KEY_KP_MULTIPLY, true},
    {KEY_KP_SUBTRACT, true},
    {KEY_KP_ADD, true},
    {KEY_KP_ENTER, false},
    {KEY_KP_EQUAL, true},
    // Android key buttons
    {KEY_BACK, false},
    {KEY_MENU, false},
    {KEY_VOLUME_UP, false},
    {KEY_VOLUME_DOWN, false}
};
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

    {GAMEPAD_EJE_IZQ_X, "AXIS_LEFT_X"},
    {GAMEPAD_EJE_IZQ_Y, "AXIS_LEFT_Y"},
    {GAMEPAD_EJE_DER_X, "AXIS_RIGHT_X"},
    {GAMEPAD_EJE_DER_Y, "AXIS_RIGHT_Y"},
    {GAMEPAD_EJE_GAT_IZQ, "AXIS_LEFT_TRIGGER"},
    {GAMEPAD_EJE_GAT_DER, "AXIS_RIGHT_TRIGGER"}
};
//-----------------------------------------------------------------------------
// Controller
//-----------------------------------------------------------------------------
bool Controller::isGamePadAxis(int binding) {
    return  binding == GAMEPAD_EJE_IZQ_X    ||
            binding == GAMEPAD_EJE_IZQ_Y    ||
            binding == GAMEPAD_EJE_DER_X    ||
            binding == GAMEPAD_EJE_DER_Y;
}
bool Controller::isGamePadTrigger(int binding) {
    return  binding == GAMEPAD_EJE_GAT_IZQ  ||
            binding == GAMEPAD_EJE_GAT_DER;
}
bool Controller::isPressed(Control c) {

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
bool Controller::isDown(Control c) {

    if (c == NO_CONTROL) return false;

    if (type == KEYBOARD) {
        return IsKeyDown(kb_controls[c]);
        
    } else if (type >= 0 && type <= 3) {
        // Gamepad checks
        if (!IsGamepadAvailable(type)) return false;

        if (isGamePadAxis(gp_controls[c]) && c%2 == 0) return (GetGamepadAxisMovement(type, gp_controls[c] - GAMEPAD_EJE_IZQ_X) < -0.15);
        else if (isGamePadAxis(gp_controls[c]) && c%2 == 1) return (GetGamepadAxisMovement(type, gp_controls[c] - GAMEPAD_EJE_IZQ_X) > 0.15);

        else if (isGamePadTrigger(gp_controls[c])) return (GetGamepadAxisMovement(type, gp_controls[c] - GAMEPAD_EJE_IZQ_X) > 0.15);
        else return IsGamepadButtonDown(type, gp_controls[c]);

    } else return false;
}
std::string Controller::getActionBind(Control c) { 
    return (type == Type::KEYBOARD) ? code2key[kb_controls[c]] : code2joy[gp_controls[c]]; 
}
Texture2D Controller::getActionTexture(Control c) {
    return (type == Type::KEYBOARD) ? key_textures[(KeyboardKey)kb_controls[c]] : button_textures[(GamepadButton)gp_controls[c]];
}
void Controller::printAll() {
    std::cout << "Keyboard bindings:" << std::endl;
    for (auto kv : kb_controls) {
        printKey(KEYBOARD, kv.first);
    }
    std::cout << "Controller bindings:" << std::endl;
    for (auto kv : gp_controls) {
        printKey(CONTROLLER_0, kv.first);
    }
}
void Controller::printKey(Type t, Control c) {
    std::string keybind = "NONE";
    std::string controllerName;
    bool isActive = false;
    if (t == Type::KEYBOARD) {
        keybind = code2key[kb_controls[c]];
        isActive = IsKeyDown(kb_controls[c]);
        controllerName = "Keyboard";
    } else {
        keybind = code2joy[gp_controls[c]];
        if (isGamePadAxis(gp_controls[c]) || isGamePadTrigger(gp_controls[c])) isActive = (std::fabs(GetGamepadAxisMovement(t, gp_controls[c] - GAMEPAD_EJE_IZQ_X)) > 0.15);
        else isActive = IsGamepadButtonDown(t, gp_controls[c]);
        controllerName = "Controller_" + std::to_string(t);
    }

    std::cout << "Controller: " << controllerName << ", Control: " << c << " , Keybind: " << keybind << " , Active: " << isActive << std::endl;
}

void init_input_textures() {
    key_textures[KEY_COMMA] = LoadTexture("Assets/Sprites/Keys/,.png");
    key_textures[KEY_MINUS] = LoadTexture("Assets/Sprites/Keys/-.png");
    key_textures[KEY_ZERO] = LoadTexture("Assets/Sprites/Keys/0.png");
    key_textures[KEY_ONE] = LoadTexture("Assets/Sprites/Keys/1.png");
    key_textures[KEY_TWO] = LoadTexture("Assets/Sprites/Keys/2.png");
    key_textures[KEY_THREE] = LoadTexture("Assets/Sprites/Keys/3.png");
    key_textures[KEY_FOUR] = LoadTexture("Assets/Sprites/Keys/4.png");
    key_textures[KEY_FIVE] = LoadTexture("Assets/Sprites/Keys/5.png");
    key_textures[KEY_SIX] = LoadTexture("Assets/Sprites/Keys/6.png");
    key_textures[KEY_SEVEN] = LoadTexture("Assets/Sprites/Keys/7.png");
    key_textures[KEY_EIGHT] = LoadTexture("Assets/Sprites/Keys/8.png");
    key_textures[KEY_NINE] = LoadTexture("Assets/Sprites/Keys/9.png");
    key_textures[KEY_A] = LoadTexture("Assets/Sprites/Keys/a.png");
    key_textures[KEY_B] = LoadTexture("Assets/Sprites/Keys/b.png");
    key_textures[KEY_C] = LoadTexture("Assets/Sprites/Keys/c.png");
    key_textures[KEY_D] = LoadTexture("Assets/Sprites/Keys/d.png");
    key_textures[KEY_E] = LoadTexture("Assets/Sprites/Keys/e.png");
    key_textures[KEY_F] = LoadTexture("Assets/Sprites/Keys/f.png");
    key_textures[KEY_G] = LoadTexture("Assets/Sprites/Keys/g.png");
    key_textures[KEY_H] = LoadTexture("Assets/Sprites/Keys/h.png");
    key_textures[KEY_I] = LoadTexture("Assets/Sprites/Keys/i.png");
    key_textures[KEY_J] = LoadTexture("Assets/Sprites/Keys/j.png");
    key_textures[KEY_K] = LoadTexture("Assets/Sprites/Keys/k.png");
    key_textures[KEY_L] = LoadTexture("Assets/Sprites/Keys/l.png");
    key_textures[KEY_M] = LoadTexture("Assets/Sprites/Keys/m.png");
    key_textures[KEY_N] = LoadTexture("Assets/Sprites/Keys/n.png");
    key_textures[KEY_O] = LoadTexture("Assets/Sprites/Keys/o.png");
    key_textures[KEY_P] = LoadTexture("Assets/Sprites/Keys/p.png");
    key_textures[KEY_Q] = LoadTexture("Assets/Sprites/Keys/q.png");
    key_textures[KEY_R] = LoadTexture("Assets/Sprites/Keys/r.png");
    key_textures[KEY_S] = LoadTexture("Assets/Sprites/Keys/s.png");
    key_textures[KEY_T] = LoadTexture("Assets/Sprites/Keys/t.png");
    key_textures[KEY_U] = LoadTexture("Assets/Sprites/Keys/u.png");
    key_textures[KEY_V] = LoadTexture("Assets/Sprites/Keys/v.png");
    key_textures[KEY_W] = LoadTexture("Assets/Sprites/Keys/w.png");
    key_textures[KEY_X] = LoadTexture("Assets/Sprites/Keys/x.png");
    key_textures[KEY_Y] = LoadTexture("Assets/Sprites/Keys/y.png");
    key_textures[KEY_Z] = LoadTexture("Assets/Sprites/Keys/z.png");
    key_textures[KEY_SPACE] = LoadTexture("Assets/Sprites/Keys/space.png");
    key_textures[KEY_TAB] = LoadTexture("Assets/Sprites/Keys/tab.png");
    key_textures[KEY_BACKSPACE] = LoadTexture("Assets/Sprites/Keys/backspace.png");
    key_textures[KEY_RIGHT] = LoadTexture("Assets/Sprites/Keys/pader.png");
    key_textures[KEY_LEFT] = LoadTexture("Assets/Sprites/Keys/paizq.png");
    key_textures[KEY_DOWN] = LoadTexture("Assets/Sprites/Keys/pabajo.png");
    key_textures[KEY_UP] = LoadTexture("Assets/Sprites/Keys/parriba.png");
    key_textures[KEY_CAPS_LOCK] = LoadTexture("Assets/Sprites/Keys/bloqmayus.png");
    key_textures[KEY_LEFT_SHIFT] = LoadTexture("Assets/Sprites/Keys/leftshift.png");
    key_textures[KEY_LEFT_CONTROL] = LoadTexture("Assets/Sprites/Keys/leftctrl.png");
    key_textures[KEY_LEFT_ALT] = LoadTexture("Assets/Sprites/Keys/alt.png");
    key_textures[KEY_RIGHT_SHIFT] = LoadTexture("Assets/Sprites/Keys/rightshift.png");
    key_textures[KEY_RIGHT_ALT] = LoadTexture("Assets/Sprites/Keys/altgr.png");
    key_textures[KEY_KP_0] = LoadTexture("Assets/Sprites/Keys/kp0.png");
    key_textures[KEY_KP_1] = LoadTexture("Assets/Sprites/Keys/kp1.png");
    key_textures[KEY_KP_2] = LoadTexture("Assets/Sprites/Keys/kp2.png");
    key_textures[KEY_KP_3] = LoadTexture("Assets/Sprites/Keys/kp3.png");
    key_textures[KEY_KP_4] = LoadTexture("Assets/Sprites/Keys/kp4.png");
    key_textures[KEY_KP_5] = LoadTexture("Assets/Sprites/Keys/kp5.png");
    key_textures[KEY_KP_6] = LoadTexture("Assets/Sprites/Keys/kp6.png");
    key_textures[KEY_KP_7] = LoadTexture("Assets/Sprites/Keys/kp7.png");
    key_textures[KEY_KP_8] = LoadTexture("Assets/Sprites/Keys/kp8.png");
    key_textures[KEY_KP_9] = LoadTexture("Assets/Sprites/Keys/kp9.png");
    key_textures[KEY_KP_ENTER] = LoadTexture("Assets/Sprites/Keys/enter.png");
    button_textures[GAMEPAD_BUTTON_LEFT_FACE_UP] = LoadTexture("Assets/Sprites/Buttons/LFUp(PS4).png");
    button_textures[GAMEPAD_BUTTON_LEFT_FACE_RIGHT] = LoadTexture("Assets/Sprites/Buttons/LFRight(PS4).png");
    button_textures[GAMEPAD_BUTTON_LEFT_FACE_DOWN] = LoadTexture("Assets/Sprites/Buttons/LFDown(PS4).png");
    button_textures[GAMEPAD_BUTTON_LEFT_FACE_LEFT] = LoadTexture("Assets/Sprites/Buttons/LFLeft(PS4).png");
    button_textures[GAMEPAD_BUTTON_RIGHT_FACE_UP] = LoadTexture("Assets/Sprites/Buttons/RFUp(PS4).png");
    button_textures[GAMEPAD_BUTTON_RIGHT_FACE_RIGHT] = LoadTexture("Assets/Sprites/Buttons/RFRight(PS4).png");
    button_textures[GAMEPAD_BUTTON_RIGHT_FACE_DOWN] = LoadTexture("Assets/Sprites/Buttons/RFDown(PS4).png");
    button_textures[GAMEPAD_BUTTON_RIGHT_FACE_LEFT] = LoadTexture("Assets/Sprites/Buttons/RFLeft(PS4).png");
    button_textures[GAMEPAD_BUTTON_LEFT_TRIGGER_1] = LoadTexture("Assets/Sprites/Buttons/LT1(PS4).png");
    button_textures[GAMEPAD_BUTTON_LEFT_TRIGGER_2] = LoadTexture("Assets/Sprites/Buttons/LT2(PS4).png");
    button_textures[GAMEPAD_BUTTON_RIGHT_TRIGGER_1] = LoadTexture("Assets/Sprites/Buttons/RT1(PS4).png");
    button_textures[GAMEPAD_BUTTON_RIGHT_TRIGGER_2] = LoadTexture("Assets/Sprites/Buttons/RT2(PS4).png");
    button_textures[GAMEPAD_BUTTON_MIDDLE_LEFT] = LoadTexture("Assets/Sprites/Buttons/ML(PS4).png");
    button_textures[GAMEPAD_BUTTON_MIDDLE] = LoadTexture("Assets/Sprites/Buttons/M(PS4).png");
    button_textures[GAMEPAD_BUTTON_MIDDLE_RIGHT] = LoadTexture("Assets/Sprites/Buttons/MR(PS4).png");
    button_textures[GAMEPAD_BUTTON_LEFT_THUMB] = LoadTexture("Assets/Sprites/Buttons/LTh(PS4).png");
}

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

std::string to_string(Controller::Control control) {
    switch (control) {
        case Controller::NO_CONTROL: return "NO CONTROL";
        case Controller::LEFT: return "MOVE LEFT";
        case Controller::RIGHT: return "MOVE RIGHT";
        case Controller::DOWN: return "CROUCH";
        case Controller::UP: return "MOVE UP";
        case Controller::JUMP: return "JUMP";
        case Controller::ATTACK: return "ATTACK";
        default: return "?!";
    }
}

Controller Controller_0;
Controller Controller_1;
Controller Controller_2;
Controller Controller_3;
Controller* controllers[] = {&Controller_0, &Controller_1, &Controller_2, &Controller_3};

// Returns the controller number and control with which the binding is conflicted
// (only the first one if there are multiple and excepting 'control')
// if there is no conflict returns the pair (-1, NO_CONTROL)
std::pair<int, Controller::Control> isBindingConflicted(int controllerNumber, Controller::Control control, int binding) {
    if (binding <= 0) return std::pair(-1, Controller::NO_CONTROL);

    auto controller = controllers[controllerNumber];

    if (controller->type == Controller::KEYBOARD) {
        for (int i = 0; i < 4; i++) {
            for (auto kv : controllers[i]->kb_controls) {
                if (kv.first == Controller::NO_CONTROL || (controllerNumber == i && kv.first == control)) continue;
                if (binding == kv.second) return std::pair(i, kv.first);
            }
        }
    } else {
        for (auto kv : controller->gp_controls) {
            if (kv.first == Controller::NO_CONTROL || kv.first == control) continue;
            if (binding == kv.second) return std::pair(controllerNumber, kv.first);
        }
    }
    return std::pair(-1, Controller::NO_CONTROL);
}