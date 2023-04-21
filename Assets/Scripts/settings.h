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
#include "raylib.h"
#include "controllers.h"

//-----------------------------------------------------------------------------
// Game settings
// ----------------------------------------------------------------------------
using Config = std::unordered_map<std::string, std::unordered_map<std::string, std::variant<int, float, bool>>>;
Config ini;
std::unordered_map<std::string, std::string> types {
    {"OldFashioned", "bool"},
    {"ScreenWidth", "int"},
    {"ScreenHeight", "int"},
    {"DisplayMode", "int"},
    {"Vsync", "bool"},
    {"FPSLimit", "int"},
    {"MusicVolume", "float"},
    {"EffectsVolume", "float"},
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
int RESOLUTION_OPTION              = 0;
std::vector<std::pair<int, int>> RESOLUTION_OPTIONS {
    {640, 480}, {800, 600}, {900, 600}, {1024, 768}, {1280, 720}, {1920, 1080}
};
// ---- VSYNC
const bool DEFAULT_VSYNC           = false;
// ---- FPS LIMIT
const int DEFAULT_FPS_LIMIT        = 30;
int FPS_LIMIT_OPTION               = 0;
std::vector<int> FPS_LIMIT_OPTIONS { 15, 30, 60 };

// -- MUSIC/AUDIO:
const float DEFAULT_MUSIC_VOLUME   = 1.0f;
const float DEFAULT_EFFECTS_VOLUME = 1.0f;
// Crear sistema que verifique nombres no reconocidos.
// Crear sistema que verifique valores sin sentido.

int& WINDOW_WIDTH    = std::get<int>(ini["Graphics"]["ScreenWidth"]);  // 240px
int& WINDOW_HEIGHT   = std::get<int>(ini["Graphics"]["ScreenHeight"]); // 160px
float MUSIC_VOLUME   = 1.0f;
float EFFECTS_VOLUME = 1.0f;

void controller_default_config(int controller) {
    std::string controllerSection = "Controller_" + std::to_string(controller);
    ini[controllerSection]["ControllerType"]  = Controller::KEYBOARD;
    ini[controllerSection]["GP_Left"]  = GAMEPAD_BUTTON_LEFT_FACE_LEFT;
    ini[controllerSection]["GP_Right"] = GAMEPAD_BUTTON_LEFT_FACE_RIGHT;
    ini[controllerSection]["GP_Down"]  = GAMEPAD_BUTTON_LEFT_FACE_DOWN;
    ini[controllerSection]["GP_Up"]    = GAMEPAD_BUTTON_LEFT_FACE_UP;
    ini[controllerSection]["GP_Jump"]  = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;
    ini[controllerSection]["GP_Attack"]= GAMEPAD_BUTTON_RIGHT_FACE_RIGHT;

    ini[controllerSection]["KB_Left"]  = KEY_A;
    ini[controllerSection]["KB_Right"] = KEY_D;
    ini[controllerSection]["KB_Down"]  = KEY_S;
    ini[controllerSection]["KB_Up"]    = KEY_W;
    ini[controllerSection]["KB_Jump"]  = KEY_SPACE;
    ini[controllerSection]["KB_Attack"]= KEY_E;
}

void default_config() {
    // Graphics settings:
    ini["Graphics"]["OldFashioned"] = DEFAULT_OLD_STYLE;
    ini["Graphics"]["ScreenWidth"]  = DEFAULT_SCREEN_WIDTH;
    ini["Graphics"]["ScreenHeight"] = DEFAULT_SCREEN_HEIGHT;
    ini["Graphics"]["DisplayMode"]  = DEFAULT_DISPLAY_MODE;
    ini["Graphics"]["Vsync"]        = DEFAULT_VSYNC;
    ini["Graphics"]["FPSLimit"]     = DEFAULT_FPS_LIMIT;
    // Audio settings:
    ini["Audio"]["MusicVolume"]     = DEFAULT_MUSIC_VOLUME;
    ini["Audio"]["EffectsVolume"]   = DEFAULT_EFFECTS_VOLUME;
    // Controller settings:
    controller_default_config(0);
    controller_default_config(1);
    controller_default_config(2);
    controller_default_config(3);
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

void save_config() {
    std::ofstream os("settings.ini");
    os << "; Probando probando...\n";
    os << "[Graphics]"
        << "\nOldFashioned="  << std::get<bool>(ini["Graphics"]["OldFashioned"])
        << "\nScreenWidth="   << std::get<int>(ini["Graphics"]["ScreenWidth"])
        << "\nScreenHeight="  << std::get<int>(ini["Graphics"]["ScreenHeight"])
        << "\nDisplayMode="   << std::get<int>(ini["Graphics"]["DisplayMode"]) // 0: Full screen, 1: Full screen with borders, 2: Windowed.
        << "\nVsync="         << std::get<bool>(ini["Graphics"]["Vsync"])
        << "\nFPSLimit="      << std::get<int>(ini["Graphics"]["FPSLimit"]) << std::endl;
    os << "[Audio]" 
        << "\nMusicVolume="   << std::fixed << std::setprecision(2) << std::get<float>(ini["Audio"]["MusicVolume"])
        << "\nEffectsVolume=" << std::fixed << std::setprecision(2) << std::get<float>(ini["Audio"]["EffectsVolume"]) << std::endl;
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

    if (type == Controller::KEYBOARD) {

        controller.controls[Controller::LEFT] = std::get<int>(ini[controllerSection]["KB_Left"]);
        controller.controls[Controller::RIGHT] = std::get<int>(ini[controllerSection]["KB_Right"]);
        controller.controls[Controller::DOWN] = std::get<int>(ini[controllerSection]["KB_Down"]);
        controller.controls[Controller::UP] = std::get<int>(ini[controllerSection]["KB_Up"]);
        controller.controls[Controller::JUMP] = std::get<int>(ini[controllerSection]["KB_Jump"]);
        controller.controls[Controller::ATTACK] = std::get<int>(ini[controllerSection]["KB_Attack"]);

    } else if (type >= Controller::CONTROLLER_0 && type <= Controller::CONTROLLER_3) {

        controller.controls[Controller::LEFT] = std::get<int>(ini[controllerSection]["GP_Left"]);
        controller.controls[Controller::RIGHT] = std::get<int>(ini[controllerSection]["GP_Right"]);
        controller.controls[Controller::DOWN] = std::get<int>(ini[controllerSection]["GP_Down"]);
        controller.controls[Controller::UP] = std::get<int>(ini[controllerSection]["GP_Up"]);
        controller.controls[Controller::JUMP] = std::get<int>(ini[controllerSection]["GP_Jump"]);
        controller.controls[Controller::ATTACK] = std::get<int>(ini[controllerSection]["GP_Attack"]);
    }
}

void init_config() {
    std::ifstream in("settings.ini");
    if (!in.is_open()) {
        default_config();
        save_config();
    } else {
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
    int aux_0 = std::get<int>(ini["Graphics"]["ScreenWidth"]);
    int aux_1 = std::get<int>(ini["Graphics"]["ScreenHeight"]);
    DISPLAY_MODE_OPTION = std::get<int>(ini["Graphics"]["DisplayMode"]);
    if (DISPLAY_MODE_OPTION == WINDOWED) {
        InitWindow(aux_0, aux_1, "COÑO");
        SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    } else if (DISPLAY_MODE_OPTION == WINDOWED_FULLSCREEN) {
        if (IsWindowFullscreen()) {
            ToggleFullscreen();
        }
        InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "COÑO");
        SetWindowPosition(0, 30);
    } else if (DISPLAY_MODE_OPTION == FULLSCREEN) {
        InitWindow(aux_0, aux_1, "COÑO");
        if (!IsWindowFullscreen()) {
            ToggleFullscreen();
        }
    }
    //---- Screen size
    for (size_t i = 0; i < RESOLUTION_OPTIONS.size(); i++) {
        if (RESOLUTION_OPTIONS[i].first == aux_0 && RESOLUTION_OPTIONS[i].second == aux_1) {
            RESOLUTION_OPTION = i;
            break;
        }
    }
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
    controller_init_config(Controller_0, 0, in);
    controller_init_config(Controller_1, 1, in);
    controller_init_config(Controller_2, 2, in);
    controller_init_config(Controller_3, 3, in);
    
    //if (VSYNC) {
    //    SetConfigFlags(FLAG_VSYNC_HINT);
    //} else {
    //    SetConfigFlags(~FLAG_VSYNC_HINT);
    //}
    //-- Audio:
    InitAudioDevice(); // Initialize audio device.
    //-- Controls:
    SetExitKey(KEY_NULL);
}