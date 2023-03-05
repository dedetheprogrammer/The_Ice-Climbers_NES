#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <vector>

#include "animator.h"
#include "audioplayer.h"
#include "colisiones.h"
#include "raylib.h"

// Settings:
int WINDOW_WIDTH  = 900; // 240px
int WINDOW_HEIGHT = 600; // 160px
bool VSYNC        = false;
int FPS_LIMIT     = 30;
enum DISPLAY_MODE_ENUM { 
    FULL_WINDOW, FULL_WINDOW_BORDERS, WINDOWED,
} DISPLAY_MODE;
float MUSIC_VOLUME = 1.0f;
float EFFECTS_VOLUME = 1.0f;

// Other:
Font NES;

// Movimiento.
int GetAxis(std::string axis) {
    if (axis == "Horizontal") {
        bool left_key  = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        bool right_key = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT); 
        if (left_key && right_key) return 0;
        else if (left_key) return -1;
        else if (right_key) return 1;
    } else if (axis == "Vertical") {
        bool down_key = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
        bool up_key   = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP); 
        if (down_key && up_key) return 0;
        if (down_key) return -1;
        else if (up_key) return 1;
    }
    return 0;
}

// Test de colisiones.
//Rectangle techo{200, WINDOW_HEIGHT - 200, 100, 50};
//Rectangle suelo{0, WINDOW_HEIGHT - 67, WINDOW_WIDTH, 67};

enum WINDOW_BEHAVOR { COLLISION = 0x01, TRAVERSE = 0x02, IGNORE = 0x04 };
bool WINDOW_LIMITS_BEHAVOR (WINDOW_BEHAVOR flag) {
    return false;
}

class RigidBody {
private:
    //...
public:
    float mass;
    float gravity;
    Vector2 velocity;
    Vector2 min_velocity;
    Vector2 max_velocity;
    Vector2 acceleration;

    RigidBody() {
        mass = 0;
        gravity = 0;
        velocity = {0,0};
        min_velocity = {0,0};
        max_velocity = {0,0};
        acceleration = {0,0};
    }

    RigidBody(float mass, float gravity, Vector2 min_velocity, Vector2 max_velocity, Vector2 acceleration) {
        this->mass = mass;
        this->gravity = gravity;
        velocity = {0,0};
        this->min_velocity = min_velocity;
        this->max_velocity = max_velocity;
        this->acceleration = acceleration;
    }

};

class GameObject {
private:
    //...
public:
    // Propiedades hardcodeadas.
    int isRight;             // Telling us if the object is facing to the right.
    //bool isAttacking;        // Telling us if the object is attacking.
    float movement_speed;    // GameObject movement speed.
    // Jumping:
    int isJumping;          // Telling us if the object is jumping. (0) if not, (-1) if going up and (1) if falling.
    float jumping_dist;     // Jumping distance.
    float max_jumping_dist; // Max. jumping distance.
    float jumping_force;    // Coefficient to highering the current height.
    Vector2 position;       // GameObject position.
    // Components:
    Animator animator;       // Animator component.
    Audioplayer audioplayer; // Audioplayer component.
    RigidBody rigidbody;     // Phisics component.

    GameObject(float movement_speed, Vector2 position, Animator animator, Audioplayer audioplayer, RigidBody rigidbody) {
        isRight = 1;
        isJumping = false;
        //isAttacking = false;
        this->movement_speed = movement_speed;
        // Jumping:
        jumping_dist = 0;
        max_jumping_dist = 150;
        this->jumping_force = 300;
        this->position = position; 
        this->animator = animator;
        this->audioplayer = audioplayer;
        this->rigidbody   = rigidbody;
    }

    void Move() {

        // Delta time:
        float deltaTime = GetFrameTime();

        // Horizontal movement:
        int move = 0;
        if (animator.HasFinished("Attack")) {
            move = GetAxis("Horizontal");
            if (!move) {
                if (!animator.Trigger("Walk", "Brake") /*|| !animator.Trigger("Jump","Brake")*/) {
                    //if (!isJumping) {
                        if ((rigidbody.velocity.x > rigidbody.min_velocity.x)) {
                            rigidbody.velocity.x -= rigidbody.acceleration.x * deltaTime;
                            position.x += isRight * rigidbody.velocity.x * deltaTime;
                        } else {
                            rigidbody.velocity.x = rigidbody.min_velocity.x;
                            animator["Idle"];
                        }
                        std::cout << "Velocity in x: " << rigidbody.velocity.x << "\n";
                    //}
                }
            } else {
                if (rigidbody.velocity.x < rigidbody.max_velocity.x) {
                    rigidbody.velocity.x += rigidbody.acceleration.x * deltaTime;
                } else {
                    rigidbody.velocity.x = rigidbody.max_velocity.x;
                }
                if (!isJumping) animator["Walk"];
                if (!animator.InState("Walk")) {
                }
                if ((move < 0 && isRight == 1) || (move > 0 && isRight == -1)) {
                    isRight *= -1;
                    animator.Flip();
                }
            }

            // Vertical movement (jump):
            /*
            if (!isJumping && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W))) {
                isJumping = -1;
                audioplayer["Jump"];
                animator["Jump"];
            }
            float jumping_movement = isJumping * jumping_force * deltaTime;
            float jumping_offset = max_jumping_dist - std::abs(jumping_dist);
            if (std::abs(jumping_movement) > jumping_offset) {
                jumping_movement = isJumping * jumping_offset;
            }
            if (isJumping == -1) {
                if (jumping_dist > -max_jumping_dist) {
                    jumping_dist += jumping_movement;
                } else {
                    jumping_dist = 0;
                    isJumping = 1;
                }
            } else if (isJumping == 1) {
                if (jumping_dist < max_jumping_dist) {
                    jumping_dist += jumping_movement;
                } else {
                    jumping_dist = 0;
                    isJumping = 0;
                }
            }
            */

            // Position actualization:
            auto dims = animator.GetDimensions();
            if ((position.x + dims.first) < -dims.first) {
                position.x = GetScreenWidth();
            } else if (position.x > (GetScreenWidth() + dims.first)) {
                position.x = -dims.first*2;
            } else {
                if (!isJumping && IsKeyPressed(KEY_E)) {
                    animator["Attack"];
                    rigidbody.velocity.x = rigidbody.min_velocity.x;
                } else {
                    position.x += move * rigidbody.velocity.x * deltaTime;
                }
            }
            //position.y += jumping_movement;
        }
    }

    void Draw() {
        animator.Play(Vector2{position.x, position.y});
    }

};

void game() {

    // Audio. Source/Sound player component?
    MusicSource BGM("Assets/NES - Ice Climber - Sound Effects/Go Go Go - Nightcore.mp3", true);
    
    // Textures. Sprite component?
    //Texture2D Snowball = LoadTexture("Assets/NES - Ice Climber - Sprites/03-Snowball.png");
    Texture2D Pause_frame = LoadTexture("Assets/NES - Ice Climber - Sprites/04-Small-frame.png");
    Texture2D Mountain_sprite = LoadTexture("Assets/NES - Ice Climber - Sprites/Mountain - Background 01.png");
    Texture2D Popo_sprite = LoadTexture("Assets/NES - Ice Climber - Sprites/Popo - Spritesheet 01 - Idle.png");

    // Animations & Animator component.
    Animator PopoAnimator(
        "Idle", 
        {
            {"Idle", Animation("Assets/NES - Ice Climber - Sprites/Popo - Spritesheet 01 - Idle.png", 16, 24, 3, 0.75, true)},
            {"Walk", Animation("Assets/NES - Ice Climber - Sprites/Popo - Spritesheet 02 - Walk.png", 16, 24, 3, 0.135, true)},
            {"Brake", Animation("Assets/NES - Ice Climber - Sprites/Popo - Spritesheet 03 - Brake.png", 16, 24, 3, 0.3, true)},
            {"Jump", Animation("Assets/NES - Ice Climber - Sprites/Popo - Spritesheet 04 - Jump.png", 20, 25, 3, 0.9, false)},
            {"Attack", Animation("Assets/NES - Ice Climber - Sprites/Popo - Spritesheet 05 - Attack.png", 21, 25, 3, 0.3, true)},
        }
    );

    // Sounds & Audioplayer component.
    Audioplayer PopoFX(
        {
            {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/NES - Ice Climber - Sound Effects/09-Jump.wav"))},
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
    RigidBody rigidbody(1, 9.8, {0,0}, {150,0}, {500,100});
    GameObject Popo(100, Vector2{0,(WINDOW_HEIGHT - Popo_sprite.height*2.0f)-107}, PopoAnimator, PopoFX, rigidbody);


    PopoAnimator["Attack"];
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
    
        DrawTexturePro(Mountain_sprite, Mountain_src, Mountain_dst, Vector2{0,0}, 0, WHITE);
        if (IsGamepadAvailable(0)) {
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                paused = !paused;
            }
        } else if (IsKeyPressed(KEY_BACKSPACE)){
            paused = !paused;
        }
        if (!paused) {
            Popo.Move();
            Popo.Draw();
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
        PopoAnimator.Play(Vector2{350,440});
        EndDrawing();
    }
    UnloadTexture(Popo_sprite);
    UnloadTexture(Mountain_sprite);
    UnloadTexture(Pause_frame);
    //UnloadTexture(Snowball);
    PopoAnimator.Unload();
    PopoFX.Unload();
    BGM.Unload();
}

void NewGame() {

}

//-----------------------------------------------------------------------------
// Game settings
// ----------------------------------------------------------------------------
using Settings = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;

void export_settings(Settings& s) {
    std::ofstream os("settings.ini");
    os << "; Probando probando...\n";
    os << "[Graphics]"
       << "\nOldFashion="   << s["Graphics"]["OldFashion"]
       << "\nScreenWidth="  << s["Graphics"]["ScreenWidth"]
       << "\nScreenHeight=" << s["Graphics"]["ScreenHeight"]
       << "\nDisplayMode="  << s["Graphics"]["DisplayMode"] // 0: Full screen, 1: Full screen with borders, 2: Windowed.
       << "\nVsync="        << s["Graphics"]["Vsync"]
       << "\nFPSLimit="     << s["Graphics"]["FPSLimit"] << std::endl;
    os << "[Audio]" 
       << "\nMusicVolume="   << s["Audio"]["MusicVolume"]
       << "\nEffectsVolume=" << s["Audio"]["EffectsVolume"] << std::endl;
    os << "; [Controls]";
}

void import_settings(Settings& s) {
    std::ifstream in("settings.ini");
    if (!in.is_open()) {
        // Graphics settings:
        s["Graphics"]["OldFashion"] = "false";
        s["Graphics"]["ScreenWidth"] = "900";
        s["Graphics"]["ScreenHeight"] = "600";
        s["Graphics"]["DisplayMode"] = "2"; // 0: Full screen, 1: Full screen with borders, 2: Windowed.
        s["Graphics"]["Vsync"] = "false";
        s["Graphics"]["FPSLimit"] = "30";
        // Audio:
        s["Audio"]["MusicVolume"] = "100";
        s["Audio"]["EffectsVolume"] = "100";
        // Controls: not implemented.
        export_settings(s);
    } else {
        std::streamsize bufferSize = 1024 * 1024; // 1 MB
        std::vector<char> buffer(bufferSize);
        in.rdbuf()->pubsetbuf(buffer.data(), bufferSize);

        std::string currentSection;
        for (std::string line; std::getline(in, line); ) {
            // Comprobar si la línea es una sección
            if (line.size() > 2 && line.front() == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }

            // Comprobar si la línea es una clave-valor
            std::istringstream lineStream(line);
            std::string key, value;

            if (std::getline(lineStream >> std::ws, key, '=') && std::getline(lineStream >> std::ws, value)) {
                s[currentSection][key] = value;
            }
        }
    }
}

void apply_settings(Settings& s) {
    WINDOW_WIDTH  = std::stoi(s["Graphics"]["ScreenWidth"]);
    WINDOW_HEIGHT = std::stoi(s["Graphics"]["ScreenHeight"]);
    if (s["Graphics"]["DisplayMode"] == "0") DISPLAY_MODE = FULL_WINDOW;
    else if (s["Graphics"]["DisplayMode"] == "1") DISPLAY_MODE = FULL_WINDOW_BORDERS;
    else DISPLAY_MODE = WINDOWED;
    VSYNC          = s["Graphics"]["Vsync"] == "true";
    FPS_LIMIT      = std::stoi(s["Graphics"]["FPSLimit"]);
    MUSIC_VOLUME   = std::stoi(s["Audio"]["MusicVolume"])/100.0f;
    EFFECTS_VOLUME = std::stoi(s["Audio"]["EffectsVolume"])/100.0f;
}

enum MENU_ENUM { START, NEW_GAME, NORMAL_GAME, SETTINGS};

int main() {

    Settings s;
    import_settings(s);
    apply_settings(s);


    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<float> D(0, (float)WINDOW_WIDTH);

    InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"COÑO");
    SetExitKey(KEY_NULL);
    SetTargetFPS(FPS_LIMIT);
    if (VSYNC) {
        SetConfigFlags(FLAG_VSYNC_HINT);
    } else {
        SetConfigFlags(~FLAG_VSYNC_HINT);
    }
    NES = LoadFont("Assets/NES - Ice Climber - Fonts/Pixel_NES/Pixel_NES.otf");

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

    // UI
    Texture2D Cross = LoadTexture("Assets/SPRITES/UI_Cross.png"); 
    Rectangle CrossSrc{0, 0, (float)Cross.width,  (float)Cross.height};
    Texture2D Transparent = LoadTexture("Assets/SPRITES/UI_Transparent.png"); 
    Rectangle TransparentSrc{0, 0, (float)Transparent.width,  (float)Transparent.height};
    Rectangle TransparentDst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    Animation OptionHammer("Assets/SPRITES/UI_Hammer_Spritesheet.png", 40, 24, 1.5, 0.5, true);


    //bool two_players  = false;
    bool close_window = false;
    bool speed_run = false;
    int current_option = 0;
    int OPTIONS = 4;
    MENU_ENUM CURRENT_MENU = START;
    while(!WindowShouldClose() && !close_window) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(Fields, FieldsSrc, FieldsDst, {0,0}, 0, WHITE);
        MountainDst.x -= MountainSpeed;
        if (MountainDst.x + MountainDst.width < 0) MountainDst.x = GetScreenWidth();
        DrawTexturePro(Mountain, MountainSrc, MountainDst, {0,0}, 0, WHITE);
        SnowSrc.x -= SnowSpeed;
        SnowSrc.y -= SnowSpeed;
        DrawTexturePro(Snow, SnowSrc, SnowDst, {0,0}, 0, WHITE);
        MidPinesSrc.x += MidPinesSpeed;
        DrawTexturePro(MidPines, MidPinesSrc, MidPinesDst, {0,0}, 0, WHITE);
        ForePinesSrc.x += ForePinesSpeed;
        DrawTexturePro(ForePines, ForePinesSrc, ForePinesDst, {0,0}, 0, WHITE);
        DrawTexturePro(Transparent, TransparentSrc, TransparentDst, {0,0}, 0, WHITE);
        DrawTextEx(NES, "ICE CLIMBER", {500, 224}, 35, 5, BLUE);

        switch (CURRENT_MENU) {
        case START:
            DrawTextEx(NES, "CONTINUE", {500, 282}, 35, 2, GRAY);
            DrawTextEx(NES, "NEW GAME", {500, 340}, 35, 2, WHITE);
            DrawTextEx(NES, "SETTINGS", {500, 398}, 35, 2, WHITE);
            DrawTextEx(NES, "EXIT",     {500, 456}, 35, 2, WHITE);
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
            DrawTextEx(NES, "NORMAL GAME",  {500, 282}, 35, 2, WHITE);
            DrawTextEx(NES, "BRAWL!",       {500, 340}, 35, 2, GRAY);
            DrawTextEx(NES, "ENDLESS MODE", {500, 398}, 35, 2, GRAY);
            DrawTextEx(NES, "RETURN",       {500, 456}, 35, 2, WHITE);
            if (IsKeyPressed(KEY_ENTER)) {
                switch (current_option) {
                case 0: 
                    CURRENT_MENU = NORMAL_GAME;
                    OPTIONS = 3;
                    current_option = 0;
                    break;
                case 1: case 2:
                    break;
                case 3:
                    CURRENT_MENU   = START;
                    current_option = 1;
                    break;
                }
            } else if (IsKeyPressed(KEY_ESCAPE)) {
                CURRENT_MENU   = START;
                current_option = 1;
            }
            break;
        case NORMAL_GAME:
            DrawTextEx(NES, "START!",      {500, 282}, 35, 2, GRAY);
            DrawTextEx(NES, "SPEED RUN?", {500, 340}, 35, 2, WHITE);
            DrawRectangleV({755, 349}, {20, 20}, WHITE);
            DrawRectangleV({756, 350}, {15, 15}, BLACK);
            if (speed_run) {
                DrawTexturePro(Cross, CrossSrc, {751, 345, CrossSrc.width * 2.0f, CrossSrc.height * 2.0f}, {0,0}, 0, WHITE);
            }
            DrawTextEx(NES, "RETURN",     {500, 398}, 35, 2, WHITE);
            DrawRectangleV({100, 250}, {120, 140}, GRAY);
            DrawTextEx(NES, "1P", {140, 400}, 35, 2, GRAY);
            DrawRectangleV({250, 250}, {120, 140}, GRAY);
            DrawTextEx(NES, "2P", {290, 400}, 35, 2, GRAY);
            if (IsKeyPressed(KEY_ENTER)) {
                switch (current_option) {
                case 0:
                    break;
                case 1:
                    speed_run = !speed_run;
                    break;
                case 2: 
                    CURRENT_MENU   = NEW_GAME;
                    OPTIONS = 4;
                    current_option = 0;
                }
            } else if (IsKeyPressed(KEY_ESCAPE)) {
                CURRENT_MENU   = NEW_GAME;
                OPTIONS = 4;
                current_option = 0;
            }
            break;
        case SETTINGS:
            DrawTextEx(NES, "VIDEO",    {500, 282}, 35, 2, GRAY);
            DrawTextEx(NES, "AUDIO",    {500, 340}, 35, 2, GRAY);
            DrawTextEx(NES, "CONTROLS", {500, 398}, 35, 2, GRAY);
            DrawTextEx(NES, "RETURN",   {500, 456}, 35, 2, WHITE);
            if (IsKeyPressed(KEY_ENTER)) {
                switch (current_option) {
                case 0: case 1: case 2:
                    std::cout << "Hola\n";
                    break;
                case 3: 
                    CURRENT_MENU   = START;
                    current_option = 2;
                    break;
                }
            } else if (IsKeyPressed(KEY_ESCAPE)) {
                CURRENT_MENU   = START;
                current_option = 2;
            }
        }

        if (IsKeyPressed(KEY_DOWN)) {
            current_option = ((current_option+1)%OPTIONS);
        }
        if (IsKeyPressed(KEY_UP)) {
            current_option = ((current_option-1)%OPTIONS + OPTIONS) % OPTIONS;
        }
        
        OptionHammer.Play({420, 282 + (58.0f * current_option)});
        EndDrawing();
    }
    OptionHammer.Unload();
    UnloadFont(NES);

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
    // 640x480
    // 800x600
    // 900x600
    // 1024x768
    // 1280x720
    // 1920x1080

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
