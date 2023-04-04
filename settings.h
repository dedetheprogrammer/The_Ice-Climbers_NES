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
#include "raylib.h"

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
    {"EffectsVolume", "float"}
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
    // Controles no implementados.
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
    os << "; [Controls]";
    os.close();
}

void init_config() {
    std::ifstream in("settings.ini");
    if (!in.is_open()) {
        default_config();
        save_config();
    } else{
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