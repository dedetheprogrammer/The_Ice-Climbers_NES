#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <variant>
#include <vector>
#include "test.h"

//-----------------------------------------------------------------------------
// Otros
//-----------------------------------------------------------------------------
class Parpadeo {
public:
    bool  show; 
    float current_time;
    float trigger_time;

    Parpadeo() : show(false), current_time(0), trigger_time(0) {}
    Parpadeo(float trigger_time) {
        show = false;
        current_time = 0;
        this->trigger_time = trigger_time;
    }

    bool Trigger(float deltaTime) {
        current_time += deltaTime;
        if (current_time >= trigger_time) {
            show = !show;
            current_time = 0;
        }
        return show;
    }
};

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

std::unordered_map<std::string, int> PopoControl {
    {"Right", KEY_D},
    {"Left", KEY_A},
    {"Jump", KEY_W},
    {"Attack", KEY_E},
};
std::unordered_map<std::string, int> NanaControl {
    {"Right", KEY_L},
    {"Left", KEY_J},
    {"Jump", KEY_O},
    {"Attack", KEY_P},
};
std::unordered_map<std::string, int> AmamControl {
    {"Right", KEY_RIGHT},
    {"Left", KEY_LEFT},
    {"Jump", KEY_UP},
    {"Attack", KEY_KP_0},
};
std::unordered_map<std::string, int> fifiControl {
    {"Right", KEY_L},
    {"Left", KEY_J},
    {"Jump", KEY_O},
    {"Attack", KEY_P},
};
std::vector<std::unordered_map<std::string,int>> Controls {PopoControl, NanaControl, AmamControl, fifiControl};

//-----------------------------------------------------------------------------
// Menus
// ----------------------------------------------------------------------------
enum MENU_ENUM { MAIN_MENU, NEW_GAME, NORMAL_GAME, SETTINGS, VIDEO_SETTINGS, AUDIO_SETTINGS, CONTROL_SETTINGS };

void game(int players) {

    // Audio. Source/Sound player component?
    MusicSource BGM("Assets/NES - Ice Climber - Sound Effects/Go Go Go - Nightcore.mp3", true);

    // Sounds & Audioplayer component.
    Audioplayer PopoFX(
        {
            {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/NES - Ice Climber - Sound Effects/09-Jump.wav"))},
        }
    );
    
    // Textures. Sprite component?
    //Texture2D Snowball = LoadTexture("Assets/OLD SPRITES/03-Snowball.png");
    Texture2D Pause_frame = LoadTexture("Assets/OLD SPRITES/04-Small-frame.png");
    Texture2D Mountain_sprite = LoadTexture("Assets/OLD SPRITES/Mountain - Background 01.png");
    Texture2D Popo_sprite = LoadTexture("Assets/OLD SPRITES/Popo - Spritesheet 03 - Brake.png");
    Texture2D Topi_sprite = LoadTexture("Assets/OLD SPRITES/Topi - Spritesheet 02 - Stunned.png");
    Texture2D Joseph_sprite = LoadTexture("Assets/OLD SPRITES/Joseph - Spritesheet 02 - Stunned.png");
    Texture2D Nutpicker_sprite = LoadTexture("Assets/OLD SPRITES/Nutpicker - Spritesheet 02 - Stunned.png");
    Texture2D GreenBlock_sprite = LoadTexture("Assets/OLD SPRITES/Brick - Green 01.png");

    // Animations & Animator component.
    Animator PopoAnimator(
        "Idle", 
        {
            {"Idle", Animation("Assets/OLD SPRITES/Popo - Spritesheet 01 - Idle.png", 16, 24, 3, 0.75, true)},
            {"Walk", Animation("Assets/OLD SPRITES/Popo - Spritesheet 02 - Walk.png", 16, 24, 3, 0.135, true)},
            {"Brake", Animation("Assets/OLD SPRITES/Popo - Spritesheet 03 - Brake.png", 16, 24, 3, 0.3, true)},
            {"Jump", Animation("Assets/OLD SPRITES/Popo - Spritesheet 04 - Jump.png", 20, 25, 3, 0.9, false)},
            {"Attack", Animation("Assets/OLD SPRITES/Popo - Spritesheet 05 - Attack.png", 21, 25, 3, 0.3, true)},
            {"Stunned", Animation("Assets/OLD SPRITES/Popo - Spritesheet 06 - Stunned.png", 16, 24, 3, 0.3, true)},
        }
    );
    Animator NanaAnimator(
        "Idle", 
        {
            {"Idle", Animation("Assets/OLD SPRITES/Popo - Spritesheet 01 - Idle.png", 16, 24, 3, 0.75, true)},
            {"Walk", Animation("Assets/OLD SPRITES/Popo - Spritesheet 02 - Walk.png", 16, 24, 3, 0.135, true)},
            {"Brake", Animation("Assets/OLD SPRITES/Popo - Spritesheet 03 - Brake.png", 16, 24, 3, 0.3, true)},
            {"Jump", Animation("Assets/OLD SPRITES/Popo - Spritesheet 04 - Jump.png", 20, 25, 3, 0.9, false)},
            {"Attack", Animation("Assets/OLD SPRITES/Popo - Spritesheet 05 - Attack.png", 21, 25, 3, 0.3, true)},
            {"Stunned", Animation("Assets/OLD SPRITES/Popo - Spritesheet 06 - Stunned.png", 16, 24, 3, 0.3, true)},
        }
    );
    Animator AmamAnimator(
        "Idle", 
        {
            {"Idle", Animation("Assets/OLD SPRITES/Popo - Spritesheet 01 - Idle.png", 16, 24, 3, 0.75, true)},
            {"Walk", Animation("Assets/OLD SPRITES/Popo - Spritesheet 02 - Walk.png", 16, 24, 3, 0.135, true)},
            {"Brake", Animation("Assets/OLD SPRITES/Popo - Spritesheet 03 - Brake.png", 16, 24, 3, 0.3, true)},
            {"Jump", Animation("Assets/OLD SPRITES/Popo - Spritesheet 04 - Jump.png", 20, 25, 3, 0.9, false)},
            {"Attack", Animation("Assets/OLD SPRITES/Popo - Spritesheet 05 - Attack.png", 21, 25, 3, 0.3, true)},
            {"Stunned", Animation("Assets/OLD SPRITES/Popo - Spritesheet 06 - Stunned.png", 16, 24, 3, 0.3, true)},
        }
    );

    Animator TopiAnimator(
        "Walk", 
        {
            {"Walk", Animation("Assets/OLD SPRITES/Topi - Spritesheet 01 - Walk.png", 16, 16, 3, 0.3, true)},
            {"Stunned", Animation("Assets/OLD SPRITES/Topi - Spritesheet 02 - Stunned.png", 16, 16, 3, 0.2, true)}
        }
    );
    
    Animator JosephAnimator(
        "Walk", 
        {
            {"Walk", Animation("Assets/OLD SPRITES/Joseph - Spritesheet 01 - Walk.png", 16, 31, 3, 0.2, true)},
            {"Stunned", Animation("Assets/OLD SPRITES/Joseph - Spritesheet 02 - Stunned.png", 16, 31, 3, 0.2, true)}
        }
    );
    
    Animator NutpickerAnimator(
        "Walk", 
        {
            {"Walk", Animation("Assets/OLD SPRITES/Nutpicker - Spritesheet 01 - Fly.png", 16, 31, 3, 0.2, true)},
            {"Stunned", Animation("Assets/OLD SPRITES/Nutpicker - Spritesheet 02 - Stunned.png", 16, 31, 3, 0.2, true)}
        }
    );
    
    Animator GreenBlockAnimator(
        "Idle", 
        {
            {"Idle", Animation("Assets/OLD SPRITES/Brick - Green 01.png", 8, 8, 3, 0.2, true)}
        }
    );

    // Rectangles = Sprites component?
    // Mountain background:
    float Mountain_view_height = (Mountain_sprite.width * WINDOW_HEIGHT)/(float)WINDOW_WIDTH;
    Rectangle Mountain_src{0, Mountain_sprite.height - Mountain_view_height, (float)Mountain_sprite.width, Mountain_view_height};
    Rectangle Mountain_dst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    
    // PAUSE frame:
    float paused_showtime = 0.75;
    bool show = true;
    Rectangle src_0{0, 0, (float)Pause_frame.width, (float)Pause_frame.height};
    Rectangle dst_1{(WINDOW_WIDTH - Pause_frame.width*3.0f)/2.0f + 4, (WINDOW_HEIGHT - Pause_frame.height)/2.0f - 3, Pause_frame.width*3.0f, Pause_frame.height*3.0f};

    // GameObject.
    //GameObject Popo(100, Vector2{(WINDOW_WIDTH - Popo_sprite.width*2.0f)/2,(WINDOW_HEIGHT - Popo_sprite.height*2.0f)-91}, PopoAnimator);
    RigidBody rigidbody(1, 9.8, {0,0}, {150,100}, {500,100});
    GameObject Popo(100, Vector2{0,(WINDOW_HEIGHT - Popo_sprite.height*2.0f)-107}, Vector2{(float)Popo_sprite.width, (float)Popo_sprite.height}, PopoAnimator, PopoFX, rigidbody, 1,Controls[0]);
    auto GameObjects = std::vector<GameObject>();
    GameObjects.push_back(Popo);
    if(players > 1) {
        RigidBody rigidbody2(1, 9.8, {0,0}, {150,0}, {500,100});
        GameObject Nana(100, Vector2{0,(WINDOW_HEIGHT - Popo_sprite.height*2.0f)-107}, Vector2{(float)Popo_sprite.width, (float)Popo_sprite.height}, NanaAnimator, PopoFX, rigidbody2, 2, Controls[1]);
        GameObjects.push_back(Nana);
    }
    if (/*players > 2*/ true) {
        /*RigidBody rigidbody3(1, 9.8, {0,0}, {150,0}, {500,100});
        GameObject Amam(100, Vector2{0,(WINDOW_HEIGHT - Popo_sprite.height*2.0f)-107}, Vector2{(float)Popo_sprite.width, (float)Popo_sprite.height}, AmamAnimator, PopoFX, rigidbody3, 3, Controls[2]);
        GameObjects.push_back(Amam);*/
    }

    // IAs
    
    std::mt19937 mtSuper(time(0));
    std::uniform_int_distribution<> randSuper(0, INT_MAX);

    RigidBody rigidbodyTopi(1, 9.8, {60,0}, {300,0}, {500,100});
    RigidBody rigidbodyJoseph(1, 9.8, {80,0}, {300,0}, {500,100});
    RigidBody rigidbodyNutpicker(1, 9.8, {80,0}, {300,0}, {500,100});

    Topi TopiIA(Vector2{Topi_sprite.width * (-1.0f), (WINDOW_HEIGHT - Topi_sprite.height*2.0f)-83}, Vector2{(float)Topi_sprite.width, (float)Topi_sprite.height}, TopiAnimator, rigidbodyTopi, 0.0, randSuper(mtSuper));
    Joseph JosephIA(Vector2{Joseph_sprite.width * (-1.0f), (WINDOW_HEIGHT - Joseph_sprite.height*2.0f)-98}, Vector2{(float)Joseph_sprite.width, (float)Joseph_sprite.height}, JosephAnimator, rigidbodyJoseph, 0.01, randSuper(mtSuper));
    Nutpicker NutpickerIA(Vector2{Nutpicker_sprite.width * (-1.0f), GetScreenHeight() / 4.0f}, Vector2{(float)Nutpicker_sprite.width, (float)Nutpicker_sprite.height}, NutpickerAnimator, rigidbodyNutpicker, 0.01, randSuper(mtSuper));
    
    auto Enemies = std::vector<IAObject*>();
    Enemies.push_back(&TopiIA);
    Enemies.push_back(&JosephIA);

    auto GreenBlock = WorldObject(Vector2{WINDOW_WIDTH/2 - 100.0f, WINDOW_HEIGHT - 300.0f}, Vector2{float(GreenBlock_sprite.width), float(GreenBlock_sprite.height)}, GreenBlockAnimator);
    auto Blocks = std::vector<WorldObject>();
    Blocks.push_back(GreenBlock);

    auto Floor = Rectangulo(0, WINDOW_HEIGHT - 89, WINDOW_WIDTH, 89);

    bool play_music = false;
    bool paused = false;
    BGM.Init();
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        if (IsKeyPressed(KEY_M)) {
            play_music = !play_music;
        }
        if (play_music) {
            BGM.Play();
        }
    
        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }

        DrawTexturePro(Mountain_sprite, Mountain_src, Mountain_dst, Vector2{0,0}, 0, WHITE);
        if (IsGamepadAvailable(0)) {
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                paused = !paused;
            }
        } else if (IsKeyPressed(KEY_BACKSPACE)){
            paused = !paused;
        }
        if (!paused) {
            Floor.Draw();
            for(auto& obj : GameObjects) {
                auto prevy = obj.position.y;
                //auto prevx = obj.position.x;
                obj.Move();
                for(auto& enemy : Enemies) {
                    if(Collides(obj.hitbox, enemy->hitbox)) {
                        if(obj.animator.InState("Attack") && !enemy->animator.InState("Stunned") && (obj.isRight != enemy->move))
                            enemy->hit();
                        else if(!enemy->animator.InState("Stunned"))
                            obj.animator["Stunned"];
                    }
                }
                bool freefall = true;
                if (Collides(obj.hitbox, Floor)) {
                    freefall = false;
                    obj.position.y = Floor.y - obj.hitbox.height;
                } else {
                    for(auto block : Blocks) {
                        if(Collides(obj.hitbox, block.hitbox)) {
                            if(block.position.y > prevy + obj.hitbox.height) {          //Popo choca encima del bloque
                                obj.position.y = block.position.y - obj.hitbox.height + 1;
                                obj.isJumping = 0;
                                obj.jumping_dist = 0;
                                //obj.animator["Idle"];
                            } else if(block.position.y + block.hitbox.height < prevy) {   //Popo choca debajo del bloque
                                obj.isJumping = 1;
                                obj.animator["Falling"];
                            } //else if(block.position.x  < prevx + obj.hitbox.weidth)
                            freefall = false;
                        }
                    }
                }
                if(freefall) {
                    obj.isJumping = 1;
                    obj.animator["Falling"];
                }
                obj.Draw();
            }
            for(auto& enemy : Enemies) {
                enemy->Play();
            }
            for(auto& block : Blocks) {
                block.Draw();
            }

        } else {
            DrawTexturePro(Pause_frame, src_0, dst_1, Vector2{0,0}, 0, WHITE);
            if (show) {
                if (paused_showtime <= 0) {
                    paused_showtime = 0.75;
                    show = false;
                } else {
                    DrawTextPro(NES, "PAUSED", Vector2{WINDOW_WIDTH/2.0f-55, WINDOW_HEIGHT/2.0f}, Vector2{0,0}, 0, 30, 1.5, WHITE);
                }
            } else {
                if (paused_showtime <= 0){
                    paused_showtime = 0.75;
                    show = true;
                }
            }
            paused_showtime -= GetFrameTime();
        }
        DrawText("Press [M] to mute the music", 20, 20, 20, WHITE);
        //DrawRectangle(techo.x, techo.y, techo.width, techo.height, BLUE);
        //DrawRectangle(suelo.x, suelo.y, suelo.width, suelo.height, RED);
        EndDrawing();
    }
    UnloadTexture(Popo_sprite);
    UnloadTexture(Mountain_sprite);
    UnloadTexture(Pause_frame);
    UnloadTexture(Topi_sprite);
    UnloadTexture(Joseph_sprite);
    //UnloadTexture(Snowball);
    PopoAnimator.Unload();
    NanaAnimator.Unload();
    AmamAnimator.Unload();
    TopiAnimator.Unload();
    JosephAnimator.Unload();
    PopoFX.Unload();
    BGM.Unload();
}

int main() {

    init_config();
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<float> D(0, (float)WINDOW_WIDTH);
    NES = LoadFont("Assets/NES - Ice Climber - Fonts/Pixel_NES.otf");

    // ---- Music
    Music ts_music = LoadMusicStream("Assets/NES - Ice Climber - Sound Effects/01-Main-Title.mp3");
    ts_music.looping = true;
    bool play_music = false;

    // Initial trailer --------------------------------------------------------
    int state = 0, shown = 0;
    Texture2D NintendoLogo = LoadTexture("Assets/SPRITES/Nintendo_logo.png");
    float nintendologo_fade = 0;
    float nintendologo_fade_add = 0.4;
    Rectangle NintendoLogoSrc{0, 0, (float)NintendoLogo.width, (float)NintendoLogo.height};
    Rectangle NintendoLogoDst{(WINDOW_WIDTH - NintendoLogo.width*0.3f)/2.0f, (WINDOW_HEIGHT - NintendoLogo.height*0.3f)/2.0f, NintendoLogo.width*0.3f, NintendoLogo.height*0.3f};
    Texture2D TeamLogo = LoadTexture("Assets/SPRITES/Team_logo.png");
    float teamlogo_fade = 0;
    float teamlogo_fade_add = 0.3;
    Rectangle TeamLogoSrc{0, 0, (float)TeamLogo.width, (float)TeamLogo.height};
    Rectangle TeamLogoDst{(WINDOW_WIDTH - TeamLogo.width*2.8f)/2.0f, (WINDOW_HEIGHT - TeamLogo.height*2.8f)/2.0f, TeamLogo.width*2.8f, TeamLogo.height*2.8f};
    float full_black_fade = 1.0f;
    float full_black_fade_add = 0.3;
    Vector2 text_measures = MeasureTextEx(NES, "PRESS <ENTER> TO START", 35, 2);
    Parpadeo pstart(1);
    bool first_enter   = false;
    float current_key_cooldown = 0, key_cooldown_add = 0.2, key_cooldown = 0.1;

    // Titlescreen ------------------------------------------------------------
    // - Fore pines
    Texture2D ForePines = LoadTexture("Assets/SPRITES/Titlescreen_01_Fore_Pines.png");
    float ForePinesSpeed = 0.6;
    float ForePinesHeight = (WINDOW_WIDTH * ForePines.height)/(float)(ForePines.width);
    Rectangle ForePinesSrc{0, 0, (float)ForePines.width, (float)ForePines.height};
    Rectangle ForePinesDst{0, WINDOW_HEIGHT - ForePinesHeight, (float)WINDOW_WIDTH, ForePinesHeight};
    // - Mid pines
    Texture2D MidPines = LoadTexture("Assets/SPRITES/Titlescreen_02_Mid_Pines.png");
    float MidPinesSpeed = 0.3;
    float MidPinesHeight = (WINDOW_WIDTH * MidPines.height)/(float)(MidPines.width);
    Rectangle MidPinesSrc{0, 0, (float)MidPines.width, (float)MidPines.height};
    Rectangle MidPinesDst{0, WINDOW_HEIGHT - MidPinesHeight + 5, (float)WINDOW_WIDTH, MidPinesHeight};
    // - Mountain
    Texture2D Mountain = LoadTexture("Assets/SPRITES/Titlescreen_03_Mountain.png");
    float MountainSpeed = 0.1;
    float MountainHeight = (WINDOW_WIDTH * Mountain.height)/(float)(Mountain.width);
    Rectangle MountainSrc{0, 0, (float)Mountain.width, (float)Mountain.height};
    Rectangle MountainDst{D(e2), WINDOW_HEIGHT - (MountainHeight * 0.75f), WINDOW_WIDTH * 0.75f, MountainHeight * 0.75f};
    // - Background fields
    Texture2D Fields = LoadTexture("Assets/SPRITES/Titlescreen_04_Fields.png");
    float FieldsHeight = (WINDOW_WIDTH * Fields.height)/(float)(Fields.width);
    Rectangle FieldsSrc{0, 0, (float)Fields.width, (float)Fields.height};
    Rectangle FieldsDst{0, WINDOW_HEIGHT - FieldsHeight, (float)WINDOW_WIDTH, FieldsHeight};
    // - Falling snow
    float SnowSpeed = 0.1;
    Texture2D Snow = LoadTexture("Assets/SPRITES/Titlescreen_05_Snow.png");
    Rectangle SnowSrc{0, 0, (float)Snow.width,  (float)Snow.height};
    Rectangle SnowDst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    // - Sign
    float SignSpeed = 105, height_dst = 30;
    Texture2D Sign = LoadTexture("Assets/SPRITES/Titlescreen_06_Sign.png");
    Rectangle SignSrc{0, 0, (float)Sign.width,  (float)Sign.height};
    Rectangle SignDst{(WINDOW_WIDTH - Sign.width*2.5f)/2.0f, -Sign.height * 2.5f, Sign.width * 2.5f, Sign.height * 2.5f};

    Texture2D OldBackground = LoadTexture("Assets/SPRITES/Titlescreen_Old_01_Background.png");
    Rectangle OldBackgroundSrc{0, 0, (float)OldBackground.width,  (float)OldBackground.height};
    Rectangle OldBackgroundDst{0, 0, (float)WINDOW_WIDTH, WINDOW_HEIGHT - 90.0f};
    float water_speed = 5;
    Texture2D OldWater = LoadTexture("Assets/SPRITES/Titlescreen_Old_02_Water.png");
    Rectangle OldWaterSrc{0, 0, (float)OldWater.width,  (float)OldWater.height};
    Rectangle OldWaterDst{0,  WINDOW_HEIGHT - 90.0f, (float)WINDOW_WIDTH, 90.0f};
    Texture2D OldSign = LoadTexture("Assets/SPRITES/Titlescreen_Old_03_Sign.png");
    Rectangle OldSignSrc{0, 0, (float)OldSign.width,  (float)OldSign.height};
    Rectangle OldSignDst{(WINDOW_WIDTH - OldSign.width*2.5f)/2.0f, -OldSign.height * 2.5f, OldSign.width * 2.5f, OldSign.height * 2.5f};
    bool show_background = false;
    // - Copyright
    float CopySpeed = 30, copy_height_dst = WINDOW_HEIGHT - 50;
    Texture2D Copy = LoadTexture("Assets/SPRITES/Titlescreen_07_Copyright.png");
    Rectangle CopySrc{0, 0, (float)Copy.width, (float)Copy.height};
    Rectangle CopyDst{(WINDOW_WIDTH - Copy.width*3.0f)/2.0f, WINDOW_HEIGHT + (float)Copy.height*3.0f, Copy.width * 3.0f, Copy.height * 3.0f};

    // UI
    Texture2D Cross = LoadTexture("Assets/SPRITES/UI_Cross.png"); 
    Rectangle CrossSrc{0, 0, (float)Cross.width,  (float)Cross.height};
    Texture2D Transparent = LoadTexture("Assets/SPRITES/UI_Transparent.png"); 
    Rectangle TransparentSrc{0, 0, (float)Transparent.width,  (float)Transparent.height};
    Rectangle TransparentDst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    Texture2D Arrow = LoadTexture("Assets/SPRITES/UI_Arrow.png");
    Rectangle ArrowSrc{0, 0, (float)Arrow.width,  (float)Arrow.height};
    Rectangle ArrowSrcInv{0, 0, -(float)Arrow.width,  (float)Arrow.height};
    Animation OptionHammer("Assets/SPRITES/UI_Hammer_Spritesheet.png", 40, 24, 1.5, 0.5, true);
    Texture2D OldOptionHammer = LoadTexture("Assets/SPRITES/UI_Old_Hammer.png");
    Rectangle OldOptionHammerSrc{0, 0, (float)OldOptionHammer.width, (float)OldOptionHammer.height};
    Texture2D Enterkey = LoadTexture("Assets/SPRITES/Keys/enter.png");
    Rectangle EnterkeySrc{0, 0, (float)Enterkey.width, (float)Enterkey.height};
    Texture2D Spacekey  = LoadTexture("Assets/SPRITES/Keys/space.png");
    Rectangle SpacekeySrc{0, 0, (float)Spacekey.width, (float)Spacekey.height};

    // Efectos?
    std::vector<Parpadeo> parpadeos(2, Parpadeo(0.75));

    int menu_start = 224, menu_height = 290;
    bool fst_player = false, snd_player = false;
    bool close_window = false;
    bool speed_run = false;
    int current_option = 0;
    int OPTIONS = 4;
    int option_offset = menu_height/(OPTIONS+1);
    int option_drift  = 0;
    MENU_ENUM CURRENT_MENU = MAIN_MENU;


    while(!WindowShouldClose() && !close_window) {

        // Delta time:
        float deltaTime = GetFrameTime();

        // Background:
        if (show_background) {
            if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                DrawTexturePro(Fields, FieldsSrc, FieldsDst, {0,0}, 0, WHITE);
                MountainDst.x -= MountainSpeed;
                if (MountainDst.x + MountainDst.width < 0) MountainDst.x = GetScreenWidth();
                DrawTexturePro(Mountain, MountainSrc, MountainDst, {0,0}, 0, WHITE);
                SnowSrc.x += SnowSpeed;
                SnowSrc.y -= SnowSpeed;
                DrawTexturePro(Snow, SnowSrc, SnowDst, {0,0}, 0, WHITE);
                MidPinesSrc.x += MidPinesSpeed;
                DrawTexturePro(MidPines, MidPinesSrc, MidPinesDst, {0,0}, 0, WHITE);
                ForePinesSrc.x += ForePinesSpeed;
                DrawTexturePro(ForePines, ForePinesSrc, ForePinesDst, {0,0}, 0, WHITE);
            } else {
                DrawTexturePro(OldBackground, OldBackgroundSrc, OldBackgroundDst, {0,0}, 0, WHITE);
                OldWaterSrc.x += water_speed * deltaTime;
                DrawTexturePro(OldWater, OldWaterSrc, OldWaterDst, {0,0}, 0, WHITE);
            }
        }

        if (play_music) {
            UpdateMusicStream(ts_music);
        }

        // Begin drawing:
        BeginDrawing();
        ClearBackground(BLACK);
        if (state == 0) {
            DrawTexturePro(NintendoLogo, NintendoLogoSrc, NintendoLogoDst, {0,0}, 0, Fade(WHITE, nintendologo_fade));
            if (!shown) {
                if (nintendologo_fade <= 1.0f) {
                    nintendologo_fade += nintendologo_fade_add * deltaTime;
                } else {
                    shown = 1;
                }
            } else if (nintendologo_fade >= 0.0f) {
                nintendologo_fade -= nintendologo_fade_add * deltaTime;
            } else {
                state = 1;
                shown = 0;
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                state = 1;
                shown = 0;
            }
        } else if (state == 1) {
            DrawTexturePro(TeamLogo, TeamLogoSrc, TeamLogoDst, {0,0}, 0, Fade(WHITE, teamlogo_fade));
            if (!shown) {
                if (teamlogo_fade <= 1.0f) {
                    teamlogo_fade += teamlogo_fade_add * deltaTime;
                } else {
                    shown = 1;
                }
            } else if (teamlogo_fade >= 0.0f) {
                teamlogo_fade -= teamlogo_fade_add * deltaTime;
            } else {
                state = 2;
                shown = 0;
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                state = 2;
                shown = 0;
            }
        } else if (state == 2) {
            if (full_black_fade <= 0.9) {
                if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                    DrawTexturePro(Sign, SignSrc, SignDst, {0,0}, 0, WHITE);
                    if (!show_background) {
                        if (SignDst.y < height_dst) {
                            SignDst.y += SignSpeed * deltaTime;
                        } else {
                            show_background = true;
                            PlayMusicStream(ts_music);
                        }
                        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                            SignDst.y = height_dst;
                            CopyDst.y = copy_height_dst;
                            show_background = true;
                            full_black_fade = 0;
                            first_enter = true;
                            PlayMusicStream(ts_music);
                            play_music = true;
                        }
                    }
                } else {
                    DrawTexturePro(OldSign, OldSignSrc, OldSignDst, {0,0}, 0, WHITE);
                    if (!show_background) {
                        if (OldSignDst.y < height_dst) {
                            OldSignDst.y += SignSpeed * deltaTime;
                        } else {
                            show_background = true;
                            PlayMusicStream(ts_music);
                            play_music = true;
                        }
                        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                            OldSignDst.y = height_dst;
                            CopyDst.y = copy_height_dst;
                            show_background = true;
                            full_black_fade = 0;
                            first_enter = true;
                            PlayMusicStream(ts_music);
                            play_music = true;
                        }
                    } 
                }
                DrawTexturePro(Copy, CopySrc, CopyDst, {0,0}, 0, WHITE);
                if (CopyDst.y > copy_height_dst) {
                    CopyDst.y -= CopySpeed * deltaTime;
                }
            }
            if (!show_background) {
                DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, full_black_fade));
                full_black_fade -= full_black_fade_add * deltaTime;
            } else {
                if (pstart.Trigger(deltaTime)) {
                    if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                        auto aux = MeasureTextEx(NES, "PRESS", 35, 5);
                        DrawTextEx(NES, "PRESS", {WINDOW_WIDTH/2.0f - aux.x - (Enterkey.width * 1.7f) + 10, WINDOW_HEIGHT - 120.f}, 35,2, BLUE);
                        DrawTexturePro(Enterkey, EnterkeySrc, {WINDOW_WIDTH/2.0f - (Enterkey.width * 1.7f) + 10, WINDOW_HEIGHT - 120.0f, Enterkey.width * 1.7f, Enterkey.height * 1.7f}, {0,0}, 0, WHITE);
                        DrawTextEx(NES, "TO START", { WINDOW_WIDTH/2.0f + 30, WINDOW_HEIGHT - 120.f}, 35, 2, BLUE);

                    } else {
                        DrawTextEx(NES, "PRESS <ENTER> TO START", {(WINDOW_WIDTH - text_measures.x)/2.0f, WINDOW_HEIGHT - 200.0f}, 35, 2, BLUE);
                    }
                }
                if (!first_enter && IsKeyPressed(KEY_ENTER)) {
                    state = -1;
                } else {
                    if (current_key_cooldown < key_cooldown) {
                        current_key_cooldown += key_cooldown_add * deltaTime;
                    } else {
                        first_enter = false;
                    }
                }
            }
        } else {
            DrawTexturePro(Transparent, TransparentSrc, {0, WINDOW_HEIGHT - 45.0f, (float)WINDOW_WIDTH, 45.0f}, {0,0}, 0, WHITE);
            if (!std::get<bool>(ini["Graphics"]["OldFashioned"]) && show_background) {
                DrawTexturePro(Transparent, TransparentSrc, TransparentDst, {0,0}, 0, WHITE);
                DrawRectangle(464, menu_start, 1, menu_height - 20, LIGHTGRAY);
                DrawTexturePro(Enterkey, EnterkeySrc, {WINDOW_WIDTH - 180.0f, WINDOW_HEIGHT - 32.0f, Enterkey.width * 0.9f, Enterkey.height * 0.9f}, {0,0}, 0, WHITE);
            } else {
                DrawTextEx(NES, "<ENTER>", {WINDOW_WIDTH - 195.0f, WINDOW_HEIGHT - 28.0f}, 13, 1, WHITE);
            }
            DrawTextEx(NES, "Select option", {WINDOW_WIDTH - 127.0f, WINDOW_HEIGHT - 28.0f}, 13, 1, LIGHTGRAY);


            switch (CURRENT_MENU) {
            case MAIN_MENU:
                DrawTextEx(NES, "ICE CLIMBER", {500, (float)menu_start}, 35, 5, BLUE);
                DrawTextEx(NES, "CONTINUE", {500, menu_start + (float)option_offset}, 35, 2, GRAY);
                DrawTextEx(NES, "NEW GAME", {500, menu_start + (option_offset * 2.0f)}, 35, 2, WHITE);
                DrawTextEx(NES, "SETTINGS", {500, menu_start + (option_offset * 3.0f)}, 35, 2, WHITE);
                DrawTextEx(NES, "EXIT",     {500, menu_start + (option_offset * 4.0f)}, 35, 2, WHITE);
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (current_option) {
                    case 0:
                        std::cout << "Hola\n";
                        break;
                    case 1:
                        CURRENT_MENU   = NEW_GAME;
                        current_option = 0;
                        break;
                    case 2: 
                        CURRENT_MENU   = SETTINGS;
                        current_option = 0;
                        break;
                    case 3:
                        close_window = true;
                    }
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    close_window = true;
                }
                break;
            case NEW_GAME:
                DrawTextEx(NES, "NEW GAME", {500, (float)menu_start}, 35, 5, BLUE);
                DrawTextEx(NES, "NORMAL MODE",  {500, menu_start + (float)option_offset}, 35, 2, WHITE);
                DrawTextEx(NES, "BRAWL!",       {500, menu_start + (option_offset * 2.0f)}, 35, 2, GRAY);
                DrawTextEx(NES, "ENDLESS MODE", {500, menu_start + (option_offset * 3.0f)}, 35, 2, GRAY);
                DrawTextEx(NES, "RETURN",       {500, menu_start + (option_offset * 4.0f)}, 35, 2, WHITE);
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (current_option) {
                    case 0: 
                        CURRENT_MENU = NORMAL_GAME;
                        OPTIONS = 3;
                        current_option = 0;
                        option_offset = menu_height/(OPTIONS+1);
                        break;
                    case 1: case 2:
                        break;
                    case 3:
                        CURRENT_MENU   = MAIN_MENU;
                        current_option = 1;
                        break;
                    }
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    CURRENT_MENU   = MAIN_MENU;
                    current_option = 1;
                }
                break;
            case NORMAL_GAME:
                DrawTextEx(NES, "NORMAL MODE", {500, (float)menu_start}, 35, 5, BLUE);
                DrawTextEx(NES, "START!", {500, menu_start + (float) option_offset}, 35, 2, fst_player ? WHITE : GRAY);
                DrawTextEx(NES, "SPEED RUN?", {500, menu_start + (option_offset * 2.0f)}, 35, 2, WHITE);
                DrawRectangleV({755, 349}, {20, 20}, WHITE);
                DrawRectangleV({756, 350}, {15, 15}, BLACK);
                if (speed_run) {
                    DrawTexturePro(Cross, CrossSrc, {751, 345, CrossSrc.width * 2.0f, CrossSrc.height * 2.0f}, {0,0}, 0, WHITE);
                }
                DrawTextEx(NES, "RETURN", {500, menu_start + (option_offset * 3.0f)}, 35, 2, WHITE);
                DrawRectangleV({100, 250}, {120, 140}, GRAY);
                if (!fst_player && parpadeos[0].Trigger(deltaTime)) {
                    DrawTextEx(NES, "PRESS", {126, 282}, 20, 2, WHITE);
                    DrawTexturePro(Spacekey, SpacekeySrc, {118, 312, SpacekeySrc.width * 1.75f, SpacekeySrc.height * 1.75f}, {0,0}, 0, WHITE);
                }
                DrawTextEx(NES, "1P", {140, 400}, 35, 2, fst_player ? WHITE : GRAY);
                DrawRectangleV({250, 250}, {120, 140}, GRAY);
                if (!snd_player && parpadeos[1].Trigger(deltaTime)) {
                    DrawTextEx(NES, "PRESS", {276, 282}, 20, 2, WHITE);
                    DrawTexturePro(Spacekey, SpacekeySrc, {268, 312, SpacekeySrc.width * 1.75f, SpacekeySrc.height * 1.75f}, {0,0}, 0, WHITE);
                }
                DrawTextEx(NES, "2P", {290, 400}, 35, 2, snd_player ? WHITE : GRAY);
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (current_option) {
                    case 0:
                        if(snd_player) game(2);
                        else if(fst_player) game(1);
                        break;
                    case 1:
                        speed_run = !speed_run;
                        break;
                    case 2: 
                        CURRENT_MENU   = NEW_GAME;
                        OPTIONS = 4;
                        option_offset  = menu_height/(OPTIONS+1);
                        current_option = 0;
                    }
                } else if (IsKeyPressed(KEY_SPACE)) {
                    if (!fst_player) {
                        fst_player = true;
                    } else if (!snd_player) {
                        snd_player = true;
                    }
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    CURRENT_MENU   = NEW_GAME;
                    OPTIONS = 4;
                    option_offset  = menu_height/(OPTIONS+1);
                    current_option = 0;
                }
                break;
            case SETTINGS:
                DrawTextEx(NES, "SETTINGS", {500, (float)menu_start}, 35, 5, BLUE);
                DrawTextEx(NES, "VIDEO",    {500, menu_start + (float)option_offset}, 35, 2, WHITE);
                DrawTextEx(NES, "AUDIO",    {500, menu_start + (option_offset * 2.0f)}, 35, 2, GRAY);
                DrawTextEx(NES, "CONTROLS", {500, menu_start + (option_offset * 3.0f)}, 35, 2, GRAY);
                DrawTextEx(NES, "RETURN",   {500, menu_start + (option_offset * 4.0f)}, 35, 2, WHITE);
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (current_option) {
                    case 0: 
                        CURRENT_MENU   = VIDEO_SETTINGS; 
                        OPTIONS        = 6;
                        current_option = 0;
                        option_offset  = menu_height/(OPTIONS+1);
                        option_drift   = 3;
                        break;
                    case 1: case 2:
                        std::cout << "Hola\n";
                        break;
                    case 3: 
                        CURRENT_MENU   = MAIN_MENU;
                        current_option = 2;
                        break;
                    }
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    CURRENT_MENU   = MAIN_MENU;
                    current_option = 2;
                }
                break;
            case VIDEO_SETTINGS:
                // MENU TITLE:
                DrawTextEx(NES, "VIDEO SETTINGS", {500, (float)menu_start}, 35, 5, BLUE);

                // OLD STYLE OPTION:
                DrawTextEx(NES, "OLD STYLE?", {500, menu_start + (float)option_offset}, 30, 1, WHITE);
                DrawRectangleV({805, menu_start + (float)option_offset + 6}, {20, 20}, WHITE);
                DrawRectangleV({806, menu_start + (float)option_offset + 8}, {15, 15}, BLACK);
                if (std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                    DrawTexturePro(Cross, CrossSrc, {800, menu_start + (float)option_offset + 3, CrossSrc.width * 2.0f, CrossSrc.height * 2.0f}, {0,0}, 0, WHITE);
                }

                // DISPLAY MODE OPTION: 0, 1, 2. Look up for the options explanation.
                DISPLAY_MODE_OPTION = std::get<int>(ini["Graphics"]["DisplayMode"]);
                DrawTextEx(NES, "DISPLAY MODE", {500, menu_start + (option_offset * 2.0f)}, 30, 1, WHITE);
                DrawTexturePro(Arrow, ArrowSrc, {750, menu_start + (option_offset * 2.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTexturePro(Arrow, ArrowSrcInv, {870, menu_start + (option_offset * 2.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTextEx(NES, to_string((DISPLAY_MODE_ENUM)DISPLAY_MODE_OPTION).c_str(),
                    {770, menu_start + (option_offset * 2.0f)}, 17, 1, WHITE);

                // RESOLUTION. 640x480, 800x600, 900x600, 1024x768, 1280x720, 1920x1080:
                DrawTextEx(NES, "RESOLUTION", {500, menu_start + (option_offset * 3.0f)}, 30, 1, WHITE);
                DrawTexturePro(Arrow, ArrowSrc, {750, menu_start + (option_offset * 3.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTexturePro(Arrow, ArrowSrcInv, {870, menu_start + (option_offset * 3.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTextEx(NES, (std::to_string(std::get<int>(ini["Graphics"]["ScreenWidth"])) + "x" + std::to_string(std::get<int>(ini["Graphics"]["ScreenHeight"]))).c_str(), 
                    {770, menu_start + (option_offset * 3.0f) + 5}, 17, 1, WHITE);

                // VSYNC:
                DrawTextEx(NES, "VSYNC", {500, menu_start + (option_offset * 4.0f)}, 30, 1, GRAY);
                DrawRectangleV({805, menu_start + (option_offset * 4.0f) + 6}, {20, 20}, WHITE);
                DrawRectangleV({806, menu_start + (option_offset * 4.0f) + 8}, {15, 15}, BLACK);
                if (std::get<bool>(ini["Graphics"]["Vsync"])) {
                    DrawTexturePro(Cross, CrossSrc, {800, menu_start + (option_offset * 4.0f) + 3, CrossSrc.width * 2.0f, CrossSrc.height * 2.0f}, {0,0}, 0, WHITE);
                }

                // FPS LIMIT
                DrawTextEx(NES, "FPS LIMIT", {500, menu_start + (option_offset * 5.0f)}, 30, 1, WHITE);
                DrawTextEx(NES, std::to_string(std::get<int>(ini["Graphics"]["FPSLimit"])).c_str(), {803, menu_start + (option_offset * 5.0f) + 3}, 20, 1, WHITE);
                DrawTexturePro(Arrow, ArrowSrc, {760, menu_start + (option_offset * 5.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTexturePro(Arrow, ArrowSrcInv, {860, menu_start + (option_offset * 5.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);

                DrawTextEx(NES, "RETURN", {500, menu_start + (option_offset * 6.0f)}, 30, 1, WHITE);
                switch (current_option) {
                case 0:
                    if (IsKeyPressed(KEY_ENTER)) {
                        ini["Graphics"]["OldFashioned"] = !std::get<bool>(ini["Graphics"]["OldFashioned"]);
                    }
                    break;
                case 1:
                    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                        DISPLAY_MODE_OPTION = (DISPLAY_MODE_OPTION+1)%3;
                        ini["Graphics"]["DisplayMode"] = DISPLAY_MODE_OPTION;
                        if (DISPLAY_MODE_OPTION == WINDOWED) {
                            if (IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                            SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                        } else if (DISPLAY_MODE_OPTION == WINDOWED_FULLSCREEN) {
                            if (IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                            SetWindowPosition(0, 30);
                            SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                        } else if (DISPLAY_MODE_OPTION == FULLSCREEN) {
                            if (!IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                        }
                    }
                    if (IsKeyPressed(KEY_LEFT)) {
                        DISPLAY_MODE_OPTION = (DISPLAY_MODE_OPTION+2)%3;
                        ini["Graphics"]["DisplayMode"] = DISPLAY_MODE_OPTION;
                        if (DISPLAY_MODE_OPTION == WINDOWED) {
                            if (IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                            SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                        } else if (DISPLAY_MODE_OPTION == WINDOWED_FULLSCREEN) {
                            if (IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                            SetWindowPosition(0, 30);
                            SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                        } else if (DISPLAY_MODE_OPTION == FULLSCREEN) {
                            if (!IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                        }
                    }
                    break;
                case 2:
                    if (DISPLAY_MODE_OPTION != WINDOWED_FULLSCREEN) {
                        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                            RESOLUTION_OPTION = (RESOLUTION_OPTION+1)%RESOLUTION_OPTIONS.size();
                            ini["Graphics"]["ScreenWidth"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].first;
                            ini["Graphics"]["ScreenHeight"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].second;
                            SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                        }
                        if (IsKeyPressed(KEY_LEFT)) {
                            RESOLUTION_OPTION = (RESOLUTION_OPTION+RESOLUTION_OPTIONS.size()-1)%RESOLUTION_OPTIONS.size();
                            ini["Graphics"]["ScreenWidth"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].first;
                            ini["Graphics"]["ScreenHeight"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].second;
                            SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                        }
                    }
                    break;
                case 3:
                    if (IsKeyPressed(KEY_ENTER)) {
                        ini["Graphics"]["Vsync"] = !std::get<bool>(ini["Graphics"]["Vsync"]);
                    }
                    break;
                case 4:
                    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                        FPS_LIMIT_OPTION = ((FPS_LIMIT_OPTION+1)%FPS_LIMIT_OPTIONS.size());
                        ini["Graphics"]["FPSLimit"] = FPS_LIMIT_OPTIONS[FPS_LIMIT_OPTION];
                        SetTargetFPS(std::get<int>(ini["Graphics"]["FPSLimit"]));
                    }
                    if (IsKeyPressed(KEY_LEFT)) {
                        FPS_LIMIT_OPTION = ((FPS_LIMIT_OPTION + FPS_LIMIT_OPTIONS.size() - 1) % FPS_LIMIT_OPTIONS.size());
                        ini["Graphics"]["FPSLimit"] = FPS_LIMIT_OPTIONS[FPS_LIMIT_OPTION];
                        SetTargetFPS(std::get<int>(ini["Graphics"]["FPSLimit"]));
                    }
                    break;
                case 5:
                    if (IsKeyPressed(KEY_ENTER)) {
                        CURRENT_MENU   = SETTINGS;
                        OPTIONS        = 4;
                        current_option = 0;
                        option_offset  = menu_height/(OPTIONS+1);
                        option_drift   = 0;
                    }
                    break;
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    CURRENT_MENU   = SETTINGS;
                    OPTIONS        = 4;
                    current_option = 0;
                    option_offset  = menu_height/(OPTIONS+1);
                    option_drift   = 0;
                }
                break;
            case AUDIO_SETTINGS: case CONTROL_SETTINGS:
                break;
            }

            if (IsKeyPressed(KEY_DOWN)) {
                current_option = ((current_option+1)%OPTIONS);
            }
            if (IsKeyPressed(KEY_UP)) {
                current_option = ((current_option-1)%OPTIONS + OPTIONS) % OPTIONS;
            }
            
            if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                OptionHammer.Play({420, (float)menu_start + (option_offset * (current_option+1) - option_drift)});
            } else {
                DrawTexturePro(OldOptionHammer, OldOptionHammerSrc,
                    {450, (float)menu_start + (option_offset * (current_option+1) - option_drift), OldOptionHammer.width * 4.0f, OldOptionHammer.height * 4.0f}, {0,0}, 0, WHITE);
            }
            DrawText("Elements in gray are not available yet.", 20, 20, 25, WHITE);
        }
        EndDrawing();
    }
    OptionHammer.Unload();
    UnloadTexture(NintendoLogo);
    UnloadTexture(TeamLogo);
    UnloadTexture(Sign);
    UnloadTexture(OldSign);
    UnloadTexture(Copy);
    UnloadTexture(Arrow);
    UnloadTexture(Mountain);
    UnloadTexture(OldOptionHammer);
    UnloadTexture(Fields);
    UnloadTexture(ForePines);
    UnloadTexture(MidPines);
    UnloadTexture(Transparent);
    UnloadTexture(Cross);
    UnloadTexture(Snow);
    UnloadTexture(Spacekey);
    UnloadTexture(Enterkey);
    UnloadFont(NES);
    UnloadMusicStream(ts_music);
    CloseAudioDevice();
    save_config();
}

/*int main() {

    //std::ostringstream s;
    InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"COÑO");
    SetExitKey(KEY_NULL);
    InitAudioDevice(); // Initialize audio device.

    NES = LoadFont("Assets/NES - Ice Climber - Fonts/Pixel_NES/Pixel_NES.otf");
    // ---- Music
    Music ts_music = LoadMusicStream("Assets/NES - Ice Climber - Sound Effects/01-Main-Title.mp3");
    ts_music.looping = true;
    bool play_music = false;
    PlayMusicStream(ts_music);
    SetTargetFPS(30);

    // Title screen.
    // ---- Sprite
    //Texture2D ts_bg = LoadTexture("Assets/NES - Ice Climber - Sprites/01-Title-screen.png");
    //Rectangle ts_src{0, 0, (float)ts_bg.width, (float)ts_bg.height};
    //Rectangle ts_dst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    // Resouluciones:

    Texture2D PinesFore = LoadTexture("Assets/NES - Ice Climber - Sprites/1_new/Titlescreen - 01 - Pines.png");
    float PinesForeHeight = (WINDOW_WIDTH * PinesFore.height)/(float)(PinesFore.width);
    Rectangle PinesForeSrc{0, 0, (float)PinesFore.width, (float)PinesFore.height};
    Rectangle PinesForeDst{0, WINDOW_HEIGHT - PinesForeHeight, (float)WINDOW_WIDTH, PinesForeHeight};
    float PinesForeSpeed = 0.6;
    Texture2D PinesMid = LoadTexture("Assets/NES - Ice Climber - Sprites/1_new/Titlescreen - 02 - Pines.png");
    float PinesMidHeight = (WINDOW_WIDTH * PinesMid.height)/(float)(PinesMid.width);
    Rectangle PinesMidSrc{0, 0, (float)PinesMid.width, (float)PinesMid.height};
    Rectangle PinesMidDst{0, WINDOW_HEIGHT - PinesMidHeight + 5, (float)WINDOW_WIDTH, PinesMidHeight};
    float PinesMidSpeed = 0.3;
    Texture2D Mountain = LoadTexture("Assets/NES - Ice Climber - Sprites/1_new/Titlescreen - 03 - Mountain.png");
    float MountainHeight = (WINDOW_WIDTH * Mountain.height)/(float)(Mountain.width);
    Rectangle MountainSrc{0, 0, (float)Mountain.width, (float)Mountain.height};
    Rectangle MountainDst{0, WINDOW_HEIGHT - (MountainHeight * 0.75f), WINDOW_WIDTH * 0.75f, MountainHeight * 0.75f};
    float MountainSpeed = 0.1;
    Texture2D Fields = LoadTexture("Assets/NES - Ice Climber - Sprites/1_new/Titlescreen - 04 - Fields.png");
    float FieldsHeight = (WINDOW_WIDTH * Fields.height)/(float)(Fields.width);
    Rectangle FieldsSrc{0, 0, (float)Fields.width, (float)Fields.height};
    Rectangle FieldsDst{0, WINDOW_HEIGHT - FieldsHeight, (float)WINDOW_WIDTH, FieldsHeight};
    Texture2D Snow = LoadTexture("Assets/NES - Ice Climber - Sprites/1_new/Titlescreen - 05 - Snow.png");
    Rectangle SnowSrc{0, 0, (float)Snow.width,  (float)Snow.height};
    Rectangle SnowDst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    float SnowSpeed = 0.1;
    Texture2D Letter = LoadTexture("Assets/NES - Ice Climber - Sprites/1_new/Titlescreen - 06 - Letter.png");
    float LetterWidthPos = (WINDOW_WIDTH - (Letter.width*2.5f))/2.0f;
    Rectangle LetterSrc{0, 0, (float)Letter.width,  (float)Letter.height};
    Rectangle LetterDst{LetterWidthPos, 30, Letter.width * 2.5f, Letter.height * 2.5f};
    Texture2D ObscureLayer = LoadTexture("Assets/NES - Ice Climber - Sprites/1_new/Titlescreen - 07 - Obscure layer.png"); 
    Rectangle ObscureLayerSrc{0, 0, (float)ObscureLayer.width,  (float)ObscureLayer.height};
    Rectangle ObscureLayerDst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    //Texture2D SelectionHammer = LoadTexture("Assets/NES - Ice Climber - Sprites/1_new/Titlescreen - 08 - Selection hammer.png"); 
    //Rectangle SelectionHammerSrc{0, 0, (float)SelectionHammer.width, (float)SelectionHammer.height};
    //Rectangle SelectionHammerDst{LetterWidthPos + 80, 282, SelectionHammer.width*1.5f, SelectionHammer.height*1.5f};
    Animation SelectionHammer("Assets/NES - Ice Climber - Sprites/1_new/Titlescreen - 08 - Selection hammer Animation.png", 40, 24, 1.5, 0.5, true);
    Texture2D ObscureSelection = LoadTexture("Assets/NES - Ice Climber - Sprites/1_new/Titlescreen - 09 - Obscure Selection.png");
    Rectangle ObscureSelectionSrc{0, 0, (float)ObscureSelection.width, (float)ObscureSelection.height};
    Rectangle ObscureSelectionDst{LetterWidthPos + 75, 277, ObscureSelection.width*3.15f, ObscureSelection.height*1.5f};

    Texture2D Settings = LoadTexture("Assets/NES - Ice Climber - Sprites/1_new/Settings.png");
    Rectangle SettingsSrc{0, 0, (float)Settings.width, (float)Settings.height};
    Rectangle SettingsDst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
 
    int option = 0;
    int OPTIONS = 4;
    bool settings = false;
    bool new_game  = false;
    bool closeWindow = false;
    while(!WindowShouldClose() && !closeWindow) {

        if (scrollingFore <= -PinesFore.width * 2) scrollingFore = 0;

        if (IsKeyPressed(KEY_M)) {
            play_music = !play_music;
        }
        if (play_music) {
            UpdateMusicStream(ts_music);
        }
        BeginDrawing();
        ClearBackground(BLACK);
        //DrawTexturePro(ts_bg, ts_src, ts_dst, Vector2{0,0}, 0, WHITE);
        //DrawTextureEx(PinesFore, (Vector2){ scrollingFore, 70 }, 0, 2, WHITE);
        //DrawTextureEx(PinesFore, (Vector2){ PinesFore.width*2 + scrollingFore, 70 }, 0, 2, WHITE);

        if (!new_game) {
            DrawTexturePro(Fields, FieldsSrc, FieldsDst, Vector2{0,0}, 0, WHITE);
            MountainDst.x -= MountainSpeed;
            if (MountainDst.x + MountainDst.width < 0) MountainDst.x = GetScreenWidth();
            DrawTexturePro(Mountain, MountainSrc, MountainDst, Vector2{0,0}, 0, WHITE);
            SnowSrc.x -= SnowSpeed;
            SnowSrc.y -= SnowSpeed;
            DrawTexturePro(Snow, SnowSrc, SnowDst, Vector2{0,0}, 0, WHITE);
            PinesMidSrc.x += PinesMidSpeed;
            DrawTexturePro(PinesMid, PinesMidSrc, PinesMidDst, Vector2{0,0}, 0, WHITE);
            PinesForeSrc.x += PinesForeSpeed;
            DrawTexturePro(PinesFore, PinesForeSrc, PinesForeDst, Vector2{0,0}, 0, WHITE);
            if (!settings) {
                if (IsKeyPressed(KEY_DOWN)) {
                    option = ((option+1)%OPTIONS);
                    ObscureSelectionDst.y = 277 + (58.0f * option);
                }
                if (IsKeyPressed(KEY_UP)) {
                    option = ((option-1)%OPTIONS + OPTIONS) % OPTIONS;
                    ObscureSelectionDst.y = 277 + (58.0f * option);
                }
                DrawTexturePro(ObscureSelection, ObscureSelectionSrc, ObscureSelectionDst, Vector2{0,0}, 0, WHITE);
                DrawTexturePro(Letter, LetterSrc, LetterDst, Vector2{0,0}, 0, WHITE);
                SelectionHammer.Play({LetterWidthPos + 80, 282 + (58.0f * option)});
                DrawTextEx(NES, "CONTINUE", {LetterWidthPos + 150, 282}, 35, 2, GRAY);
                DrawTextEx(NES, "NEW GAME", {LetterWidthPos + 150, 340}, 35, 2, WHITE);
                DrawTextEx(NES, "SETTINGS", {LetterWidthPos + 150, 398}, 35, 2, WHITE);
                DrawTextEx(NES, "EXIT",     {LetterWidthPos + 150, 456}, 35, 2, WHITE);
                DrawTexturePro(PinesFore, PinesForeSrc, PinesForeDst, Vector2{0,0}, 0, WHITE);
                DrawTextEx(NES, "(C) 2023 NINTENDO", {LetterWidthPos + 80, 550}, 35, 2, WHITE);
                DrawTextEx(NES, "Press [M] to mute",  {10, 10}, 20, 2, WHITE);
            } else {
                DrawTexturePro(Settings, SettingsSrc, SettingsDst, Vector2{0,0}, 0, WHITE);
            }
        } else {
            DrawTextEx(NES, "NORMAL MODE",  {LetterWidthPos + 150, 282}, 35, 2, WHITE);
            DrawTextEx(NES, "BRAWL MODE",   {LetterWidthPos + 150, 340}, 35, 2, GRAY);
            DrawTextEx(NES, "ENDLESS MODE", {LetterWidthPos + 150, 398}, 35, 2, GRAY);
        }
        EndDrawing();

        if (IsKeyPressed(KEY_ENTER)) {
            switch(option) {
                case 0:
                    std::cout << "Hola!\n";
                    break;
                case 1: 
                    StopMusicStream(ts_music);
                    game();
                    PlayMusicStream(ts_music);
                    break;
                case 2:
                    settings = !settings;
                    break;
                case 3:
                    closeWindow = true;
                    break;
            }

        }
    }

    // Title screen.
    UnloadFont(NES);
    //UnloadTexture(ts_bg);
    UnloadMusicStream(ts_music);
    CloseAudioDevice();
    CloseWindow();

}
*/
