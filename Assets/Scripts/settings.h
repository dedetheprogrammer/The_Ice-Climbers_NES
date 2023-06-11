#pragma once
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <vector>
#include "controllers.h"
#include "raylib.h"
#include "controllers.h"

//-----------------------------------------------------------------------------
// Game settings
// ----------------------------------------------------------------------------
using Config = std::unordered_map<std::string, std::unordered_map<std::string, std::variant<int, float, bool>>>;
Config ini;
std::unordered_map<std::string, std::string> types {
    {"AdvancedAI", "bool"},
    {"OldFashioned", "bool"},
    {"ScreenWidth", "int"},
    {"ScreenHeight", "int"},
    {"DisplayMode", "int"},
    {"FPSLimit", "int"},
    {"Volume", "float"},
    //Controller Settings
    {"ControllerType", "int"},
    {"GP_Left", "int"},
    {"GP_Right", "int"},
    {"GP_Down", "int"},
    {"GP_Up", "int"},
    {"GP_Jump", "int"},
    {"GP_Attack", "int"},
    {"KB_Left", "int"},
    {"KB_Right", "int"},
    {"KB_Down", "int"},
    {"KB_Up", "int"},
    {"KB_Jump", "int"},
    {"KB_Attack", "int"},
};
// -- GAMEPLAY
const bool DEFAULT_ADVANCED_AI     = false;
int P1_TOTAL_PTS                   = 0;
int P1_TOTAL_SEC                   = 0;
int P2_TOTAL_PTS                   = 0;
int P2_TOTAL_SEC                   = 0;
// -- GRAPHICS
const bool DEFAULT_OLD_STYLE       = false;
// ---- DISPLAY MODE
const int DEFAULT_DISPLAY_MODE     = 0;
int DISPLAY_MODE_OPTION            = 0;
enum DISPLAY_MODE_ENUM { WINDOWED = 0, WINDOWED_FULLSCREEN, FULLSCREEN };
std::string to_string(DISPLAY_MODE_ENUM dme) {
    if (dme == WINDOWED) {
        return "WINDOWED";
    } else if (dme == WINDOWED_FULLSCREEN) {
        return "FULLWINDOWED";
    } else if (dme == FULLSCREEN) {
        return "FULLSCREEN";
    } else {
        return "WHAT THE FUCK JUST HAPPENED HERE.";
    }
}
// ---- SCREEN SIZE
const int DEFAULT_SCREEN_WIDTH     = 900;
const int DEFAULT_SCREEN_HEIGHT    = 600;
int OLD_RESOLUTION_OPTION          = 2;
int RESOLUTION_OPTION              = 2;
std::vector<std::pair<int, int>> RESOLUTION_OPTIONS {
    {640, 480}, {800, 600}, {900, 600}, {1024, 768}, {1280, 720}, {1366, 768}, {1680, 1050}, {1920, 1080}
};
// ---- FPS LIMIT
const int DEFAULT_FPS_LIMIT        = 60;
int FPS_LIMIT_OPTION               = 0;
std::vector<int> FPS_LIMIT_OPTIONS { 15, 30, 60 };

// -- MUSIC/AUDIO:
const float DEFAULT_MUSIC_VOLUME   = 0.15f;
// Crear sistema que verifique nombres no reconocidos.
// Crear sistema que verifique valores sin sentido.

int& WINDOW_WIDTH    = std::get<int>(ini["Graphics"]["ScreenWidth"]);  // 240px
int& WINDOW_HEIGHT   = std::get<int>(ini["Graphics"]["ScreenHeight"]); // 160px
float MUSIC_VOLUME   = std::get<int>(ini["Audio"]["Volume"]);

void controller_default_config(int controller) {
    std::string controllerSection = "Controller_" + std::to_string(controller);
    ini[controllerSection]["ControllerType"]  = Controller::KEYBOARD;
    ini[controllerSection]["GP_Left"]  = GAMEPAD_BUTTON_LEFT_FACE_LEFT;
    ini[controllerSection]["GP_Right"] = GAMEPAD_BUTTON_LEFT_FACE_RIGHT;
    ini[controllerSection]["GP_Down"]  = GAMEPAD_BUTTON_LEFT_FACE_DOWN;
    ini[controllerSection]["GP_Up"]    = GAMEPAD_BUTTON_UNKNOWN;
    ini[controllerSection]["GP_Jump"]  = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
    ini[controllerSection]["GP_Attack"]= GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;

    switch (controller) {
        case 0:
            ini[controllerSection]["KB_Left"]  = KEY_A;
            ini[controllerSection]["KB_Right"] = KEY_D;
            ini[controllerSection]["KB_Down"]  = KEY_S;
            ini[controllerSection]["KB_Up"]    = KEY_NULL;
            ini[controllerSection]["KB_Jump"]  = KEY_SPACE;
            ini[controllerSection]["KB_Attack"]= KEY_E;
            break;
        case 1:
            ini[controllerSection]["KB_Left"]  = KEY_LEFT;
            ini[controllerSection]["KB_Right"] = KEY_RIGHT;
            ini[controllerSection]["KB_Down"]  = KEY_DOWN;
            ini[controllerSection]["KB_Up"]    = KEY_NULL;
            ini[controllerSection]["KB_Jump"]  = KEY_RIGHT_ALT;
            ini[controllerSection]["KB_Attack"]= KEY_RIGHT_CONTROL;
            break;
        case 2:
            ini[controllerSection]["KB_Left"]  = KEY_J;
            ini[controllerSection]["KB_Right"] = KEY_L;
            ini[controllerSection]["KB_Down"]  = KEY_K;
            ini[controllerSection]["KB_Up"]    = KEY_NULL;
            ini[controllerSection]["KB_Jump"]  = KEY_O;
            ini[controllerSection]["KB_Attack"]= KEY_P;
            break;
        case 3:
            ini[controllerSection]["KB_Left"]  = KEY_KP_4;
            ini[controllerSection]["KB_Right"] = KEY_KP_6;
            ini[controllerSection]["KB_Down"]  = KEY_KP_2;
            ini[controllerSection]["KB_Up"]    = KEY_NULL;
            ini[controllerSection]["KB_Jump"]  = KEY_KP_7;
            ini[controllerSection]["KB_Attack"]= KEY_KP_9;
            break;
    }
}

void default_config() {
    // Game settings:
    ini["Game"]["AdvancedAI"]       = DEFAULT_ADVANCED_AI;
    // Graphics settings:
    ini["Graphics"]["OldFashioned"] = DEFAULT_OLD_STYLE;
    ini["Graphics"]["ScreenWidth"]  = DEFAULT_SCREEN_WIDTH;
    ini["Graphics"]["ScreenHeight"] = DEFAULT_SCREEN_HEIGHT;
    ini["Graphics"]["DisplayMode"]  = DEFAULT_DISPLAY_MODE;
    ini["Graphics"]["FPSLimit"]     = DEFAULT_FPS_LIMIT;
    // Audio settings:
    ini["Audio"]["Volume"]          = DEFAULT_MUSIC_VOLUME;
    // Controller settings:
    controller_default_config(0);
    controller_default_config(1);
    controller_default_config(2);
    controller_default_config(3);
}

void save_controls(int controller) {
    std::string controllerSection = "Controller_" + std::to_string(controller);
    ini[controllerSection]["ControllerType"]  = controllers[controller]->type;

    if (controllers[controller]->type == Controller::Type::KEYBOARD) {
        ini[controllerSection]["KB_Left"]  = controllers[controller]->kb_controls[Controller::Control::LEFT];
        ini[controllerSection]["KB_Right"] = controllers[controller]->kb_controls[Controller::Control::RIGHT];
        ini[controllerSection]["KB_Down"]  = controllers[controller]->kb_controls[Controller::Control::DOWN];
        ini[controllerSection]["KB_Up"]    = controllers[controller]->kb_controls[Controller::Control::UP];
        ini[controllerSection]["KB_Jump"]  = controllers[controller]->kb_controls[Controller::Control::JUMP];
        ini[controllerSection]["KB_Attack"]= controllers[controller]->kb_controls[Controller::Control::ATTACK];
    } else {
        ini[controllerSection]["GP_Left"]  = controllers[controller]->gp_controls[Controller::Control::LEFT];
        ini[controllerSection]["GP_Right"] = controllers[controller]->gp_controls[Controller::Control::RIGHT];
        ini[controllerSection]["GP_Down"]  = controllers[controller]->gp_controls[Controller::Control::DOWN];
        ini[controllerSection]["GP_Up"]    = controllers[controller]->gp_controls[Controller::Control::UP];
        ini[controllerSection]["GP_Jump"]  = controllers[controller]->gp_controls[Controller::Control::JUMP];
        ini[controllerSection]["GP_Attack"]= controllers[controller]->gp_controls[Controller::Control::ATTACK];
    }
}

bool GamepadDetected(int gamepad) {
    if (IsGamepadAvailable(gamepad)) {
        if (IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_UP) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_TRIGGER_2) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_MIDDLE_LEFT) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_MIDDLE_RIGHT) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_THUMB) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_THUMB) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_AXIS_LEFT_X) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_AXIS_LEFT_Y) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_AXIS_RIGHT_X) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_AXIS_RIGHT_Y) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_AXIS_LEFT_TRIGGER) ||
            IsGamepadButtonPressed(gamepad, GAMEPAD_AXIS_RIGHT_TRIGGER)
        ) {
            return true;
        }
    }
    return false;
}

bool KeyboardDetected() {
    if (IsKeyPressed(KEY_APOSTROPHE) ||
        IsKeyPressed(KEY_COMMA) ||
        IsKeyPressed(KEY_MINUS) ||
        IsKeyPressed(KEY_PERIOD) ||
        IsKeyPressed(KEY_SLASH) ||
        IsKeyPressed(KEY_ZERO) ||
        IsKeyPressed(KEY_ONE) ||
        IsKeyPressed(KEY_TWO) ||
        IsKeyPressed(KEY_THREE) ||
        IsKeyPressed(KEY_FOUR) ||
        IsKeyPressed(KEY_FIVE) ||
        IsKeyPressed(KEY_SIX) ||
        IsKeyPressed(KEY_SEVEN) ||
        IsKeyPressed(KEY_EIGHT) ||
        IsKeyPressed(KEY_NINE) ||
        IsKeyPressed(KEY_SEMICOLON) ||
        IsKeyPressed(KEY_EQUAL) ||
        IsKeyPressed(KEY_A) ||
        IsKeyPressed(KEY_B) ||
        IsKeyPressed(KEY_C) ||
        IsKeyPressed(KEY_D) ||
        IsKeyPressed(KEY_E) ||
        IsKeyPressed(KEY_F) ||
        IsKeyPressed(KEY_G) ||
        IsKeyPressed(KEY_H) ||
        IsKeyPressed(KEY_I) ||
        IsKeyPressed(KEY_J) ||
        IsKeyPressed(KEY_K) ||
        IsKeyPressed(KEY_L) ||
        IsKeyPressed(KEY_M) ||
        IsKeyPressed(KEY_N) ||
        IsKeyPressed(KEY_O) ||
        IsKeyPressed(KEY_P) ||
        IsKeyPressed(KEY_Q) ||
        IsKeyPressed(KEY_R) ||
        IsKeyPressed(KEY_S) ||
        IsKeyPressed(KEY_T) ||
        IsKeyPressed(KEY_U) ||
        IsKeyPressed(KEY_V) ||
        IsKeyPressed(KEY_W) ||
        IsKeyPressed(KEY_X) ||
        IsKeyPressed(KEY_Y) ||
        IsKeyPressed(KEY_Z) ||
        IsKeyPressed(KEY_LEFT_BRACKET) ||
        IsKeyPressed(KEY_BACKSLASH) ||
        IsKeyPressed(KEY_RIGHT_BRACKET) ||
        IsKeyPressed(KEY_GRAVE) ||
        IsKeyPressed(KEY_SPACE) ||
        IsKeyPressed(KEY_ESCAPE) ||
        IsKeyPressed(KEY_ENTER) ||
        IsKeyPressed(KEY_TAB) ||
        IsKeyPressed(KEY_BACKSPACE) ||
        IsKeyPressed(KEY_INSERT) ||
        IsKeyPressed(KEY_DELETE) ||
        IsKeyPressed(KEY_RIGHT) ||
        IsKeyPressed(KEY_LEFT) ||
        IsKeyPressed(KEY_DOWN) ||
        IsKeyPressed(KEY_UP) ||
        IsKeyPressed(KEY_PAGE_UP) ||
        IsKeyPressed(KEY_PAGE_DOWN) ||
        IsKeyPressed(KEY_HOME) ||
        IsKeyPressed(KEY_END) ||
        IsKeyPressed(KEY_CAPS_LOCK) ||
        IsKeyPressed(KEY_SCROLL_LOCK) ||
        IsKeyPressed(KEY_NUM_LOCK) ||
        IsKeyPressed(KEY_PRINT_SCREEN) ||
        IsKeyPressed(KEY_PAUSE) ||
        IsKeyPressed(KEY_F1) ||
        IsKeyPressed(KEY_F2) ||
        IsKeyPressed(KEY_F3) ||
        IsKeyPressed(KEY_F4) ||
        IsKeyPressed(KEY_F5) ||
        IsKeyPressed(KEY_F6) ||
        IsKeyPressed(KEY_F7) ||
        IsKeyPressed(KEY_F8) ||
        IsKeyPressed(KEY_F9) ||
        IsKeyPressed(KEY_F10) ||
        IsKeyPressed(KEY_F11) ||
        IsKeyPressed(KEY_F12) ||
        IsKeyPressed(KEY_LEFT_SHIFT) ||
        IsKeyPressed(KEY_LEFT_CONTROL) ||
        IsKeyPressed(KEY_LEFT_ALT) ||
        IsKeyPressed(KEY_LEFT_SUPER) ||
        IsKeyPressed(KEY_RIGHT_SHIFT) ||
        IsKeyPressed(KEY_RIGHT_CONTROL) ||
        IsKeyPressed(KEY_RIGHT_ALT) ||
        IsKeyPressed(KEY_RIGHT_SUPER) ||
        IsKeyPressed(KEY_KB_MENU) ||
        IsKeyPressed(KEY_KP_0) ||
        IsKeyPressed(KEY_KP_1) ||
        IsKeyPressed(KEY_KP_2) ||
        IsKeyPressed(KEY_KP_3) ||
        IsKeyPressed(KEY_KP_4) ||
        IsKeyPressed(KEY_KP_5) ||
        IsKeyPressed(KEY_KP_6) ||
        IsKeyPressed(KEY_KP_7) ||
        IsKeyPressed(KEY_KP_8) ||
        IsKeyPressed(KEY_KP_9) ||
        IsKeyPressed(KEY_KP_DECIMAL) ||
        IsKeyPressed(KEY_KP_DIVIDE) ||
        IsKeyPressed(KEY_KP_MULTIPLY) ||
        IsKeyPressed(KEY_KP_SUBTRACT) ||
        IsKeyPressed(KEY_KP_ADD) ||
        IsKeyPressed(KEY_KP_ENTER) ||
        IsKeyPressed(KEY_KP_EQUAL)
    ) {
        return true;
    }
    return false;
}

void controller_save_config(int controller, std::ofstream& os) {
    std::string controllerSection = "Controller_" + std::to_string(controller);
    os  << "[" << controllerSection << "]"
        << "\nControllerType="  << std::get<int>(ini[controllerSection]["ControllerType"])
        << "\nGP_Left="         << std::get<int>(ini[controllerSection]["GP_Left"])
        << "\nGP_Right="        << std::get<int>(ini[controllerSection]["GP_Right"])
        << "\nGP_Down="         << std::get<int>(ini[controllerSection]["GP_Down"])
        << "\nGP_Up="           << std::get<int>(ini[controllerSection]["GP_Up"])
        << "\nGP_Jump="         << std::get<int>(ini[controllerSection]["GP_Jump"])
        << "\nGP_Attack="       << std::get<int>(ini[controllerSection]["GP_Attack"])

        << "\nKB_Left="         << std::get<int>(ini[controllerSection]["KB_Left"])
        << "\nKB_Right="        << std::get<int>(ini[controllerSection]["KB_Right"])
        << "\nKB_Down="         << std::get<int>(ini[controllerSection]["KB_Down"])
        << "\nKB_Up="           << std::get<int>(ini[controllerSection]["KB_Up"])
        << "\nKB_Jump="         << std::get<int>(ini[controllerSection]["KB_Jump"])
        << "\nKB_Attack="       << std::get<int>(ini[controllerSection]["KB_Attack"])<< std::endl;
}

void save_sav() {
    std::ofstream os("iceclimber.sav");
    if (os.is_open()) {
        os << "P1_TOTAL_PTS=" << P1_TOTAL_PTS
           << "\nP1_TOTAL_SEC=" << P1_TOTAL_SEC
           << "\nP2_TOTAL_PTS=" << P2_TOTAL_PTS
           << "\nP2_TOTAL_SEC=" << P2_TOTAL_SEC;
        os.close();
    }
}

void backup_sav() {
    std::ofstream os("iceclimber.sav.old");
    if (os.is_open()) {
        os << "P1_TOTAL_PTS=" << P1_TOTAL_PTS
           << "\nP1_TOTAL_SEC=" << P1_TOTAL_SEC
           << "\nP2_TOTAL_PTS=" << P2_TOTAL_PTS
           << "\nP2_TOTAL_SEC=" << P2_TOTAL_SEC;
        os.close();
    }
}

void save_config() {
    std::ofstream os("settings.ini");
    os << "; Probando probando...\n";
    os << "[Game]"
        << "\nAdvancedAI="    << std::get<bool>(ini["Game"]["AdvancedAI"]) << std::endl;
    os << "[Graphics]"
        << "\nOldFashioned="  << std::get<bool>(ini["Graphics"]["OldFashioned"])
        << "\nScreenWidth="   << std::get<int>(ini["Graphics"]["ScreenWidth"])
        << "\nScreenHeight="  << std::get<int>(ini["Graphics"]["ScreenHeight"])
        << "\nDisplayMode="   << std::get<int>(ini["Graphics"]["DisplayMode"]) // 0: Full screen, 1: Full screen with borders, 2: Windowed.
        << "\nFPSLimit="      << std::get<int>(ini["Graphics"]["FPSLimit"]) << std::endl;
    os << "[Audio]"
        << "\nVolume="   << std::get<float>(ini["Audio"]["Volume"]) << std::endl;
    controller_save_config(0, os);
    controller_save_config(1, os);
    controller_save_config(2, os);
    controller_save_config(3, os);
    os.close();
}

void controller_init_config(Controller& controller, int controllerNumber, std::ifstream& in) {
    std::string controllerSection = "Controller_" + std::to_string(controllerNumber);
    int type = std::get<int>(ini[controllerSection]["ControllerType"]);
    controller.type = (Controller::Type)type;

    controller.kb_controls[Controller::LEFT] = std::get<int>(ini[controllerSection]["KB_Left"]);
    controller.kb_controls[Controller::RIGHT] = std::get<int>(ini[controllerSection]["KB_Right"]);
    controller.kb_controls[Controller::DOWN] = std::get<int>(ini[controllerSection]["KB_Down"]);
    controller.kb_controls[Controller::UP] = std::get<int>(ini[controllerSection]["KB_Up"]);
    controller.kb_controls[Controller::JUMP] = std::get<int>(ini[controllerSection]["KB_Jump"]);
    controller.kb_controls[Controller::ATTACK] = std::get<int>(ini[controllerSection]["KB_Attack"]);

    controller.gp_controls[Controller::LEFT] = std::get<int>(ini[controllerSection]["GP_Left"]);
    controller.gp_controls[Controller::RIGHT] = std::get<int>(ini[controllerSection]["GP_Right"]);
    controller.gp_controls[Controller::DOWN] = std::get<int>(ini[controllerSection]["GP_Down"]);
    controller.gp_controls[Controller::UP] = std::get<int>(ini[controllerSection]["GP_Up"]);
    controller.gp_controls[Controller::JUMP] = std::get<int>(ini[controllerSection]["GP_Jump"]);
    controller.gp_controls[Controller::ATTACK] = std::get<int>(ini[controllerSection]["GP_Attack"]);
}

void init_sav() {
    std::ifstream in("iceclimber.sav");
    if (in.is_open()) {
        std::streamsize buffersize = 1024 * 1024; // 1MB.
        std::vector<char> buffer(buffersize);
        in.rdbuf()->pubsetbuf(buffer.data(), buffersize);

        int i = 0;
        for (std::string line; std::getline(in, line); i++) {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss >> std::ws, key, '=') && std::getline(iss >> std::ws, value)) {
                if (i == 0) {
                    if (key == "P1_TOTAL_PTS") {
                        P1_TOTAL_PTS = std::stoi(value);
                    } else {
                        in.close();
                        save_sav();
                        return;
                    }
                } else if (i == 1) {
                    if (key == "P1_TOTAL_SEC") {
                        P1_TOTAL_SEC = std::stoi(value);
                    } else {
                        in.close();
                        save_sav();
                        return;
                    }
                } else if (i == 2) {
                    if (key == "P2_TOTAL_PTS") {
                        P2_TOTAL_PTS = std::stoi(value);
                    } else {
                        in.close();
                        save_sav();
                        return;
                    }
                } else {
                    if (key == "P2_TOTAL_SEC") {
                        P2_TOTAL_SEC = std::stoi(value);
                    } else {
                        in.close();
                        save_sav();
                        return;
                    }
                }
            }
        }
        in.close();
    }
}

void init_config() {
    std::ifstream in("settings.ini");
    if (!in.is_open()) {
        default_config();
        save_config();
        save_sav();
    } else {
        // Init save data:
        init_sav();
        // Init config:
        std::streamsize buffersize = 1024 * 1024; // 1MB.
        std::vector<char> buffer(buffersize);
        in.rdbuf()->pubsetbuf(buffer.data(), buffersize);

        std::string section;
        for (std::string line; std::getline(in, line); ) {
            if (line.size() > 2 && line[0] == '[' && line.back() == ']') {
                section = line.substr(1, line.size() - 2);
                continue;
            }

            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss >> std::ws, key, '=') && std::getline(iss >> std::ws, value)) {
                if (types[key] == "bool") ini[section][key] = (value == "1");
                else if (types[key] == "int") ini[section][key] = std::stoi(value);
                else if (types[key] == "float") ini[section][key] = std::stof(value);
            }
        }
    }
    //-- Graphics
    InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "(THE) ICE CLIMBER(S)");
    Image icon = LoadImage("Assets/Sprites/iceclimber.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    int aux_0 = std::get<int>(ini["Graphics"]["ScreenWidth"]);
    int aux_1 = std::get<int>(ini["Graphics"]["ScreenHeight"]);
    //---- Screen size
    for (size_t i = 0; i < RESOLUTION_OPTIONS.size(); i++) {
        if (RESOLUTION_OPTIONS[i].first == aux_0 && RESOLUTION_OPTIONS[i].second == aux_1) {
            RESOLUTION_OPTION = i;
            break;
        }
    }
    SetConfigFlags(FLAG_VSYNC_HINT);
    //---- FPS limit
    aux_0 = std::get<int>(ini["Graphics"]["FPSLimit"]);
    SetTargetFPS(aux_0);
    for (size_t i = 0; i < FPS_LIMIT_OPTIONS.size(); i++) {
        if (FPS_LIMIT_OPTIONS[i] == aux_0) {
            FPS_LIMIT_OPTION = i;
            break;
        }
    }
    //---- Controller Settings
    init_input_textures();
    controller_init_config(Controller_0, 0, in);
    controller_init_config(Controller_1, 1, in);
    controller_init_config(Controller_2, 2, in);
    controller_init_config(Controller_3, 3, in);
    //-- Audio:
    InitAudioDevice(); // Initialize audio device.
    SetMasterVolume(std::get<float>(ini["Audio"]["Volume"]));
    //-- Controls:
    SetExitKey(KEY_NULL);
}

void update_window() {
    DISPLAY_MODE_OPTION = std::get<int>(ini["Graphics"]["DisplayMode"]);
    if (DISPLAY_MODE_OPTION == WINDOWED) {
        SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        SetWindowPosition((GetMonitorWidth(0) - WINDOW_WIDTH)/2, (GetMonitorHeight(0) - WINDOW_HEIGHT)/2 + 30);
    } else if (DISPLAY_MODE_OPTION == WINDOWED_FULLSCREEN) {
        if (IsWindowFullscreen()) {
            ToggleFullscreen();
        }
        SetWindowPosition(0, 30);
        SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
        OLD_RESOLUTION_OPTION = RESOLUTION_OPTION;
        RESOLUTION_OPTION = RESOLUTION_OPTIONS.size()-1;
        
    } else if (DISPLAY_MODE_OPTION == FULLSCREEN) {
        if (!IsWindowFullscreen()) {
            SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
            OLD_RESOLUTION_OPTION = RESOLUTION_OPTION;
            RESOLUTION_OPTION = RESOLUTION_OPTIONS.size()-1;
            ToggleFullscreen();
        }
    }
}