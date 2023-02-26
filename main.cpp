#include <cmath>
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

// Dimensiones de la ventana.
int WINDOW_WIDTH  = 900; // 240px
int WINDOW_HEIGHT = 600; // 160px

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

class GameObject {
private:
    //...
public:
    // Propiedades hardcodeadas.
    int isRight;             // Telling us if the object is facing to the right.
    //bool isAttacking;        // Telling us if the object is attacking.
    float movement_speed;    // GameObject movement speed.
    // Braking:
    float braking_dist;      // Braking distance.
    float max_braking_dist;  // Max. braking distance.
    float braking_force;     // Coefficient to lowering the current speed.
    // Jumping:
    int isJumping;          // Telling us if the object is jumping. (0) if not, (-1) if going up and (1) if falling.
    float jumping_dist;     // Jumping distance.
    float max_jumping_dist; // Max. jumping distance.
    float jumping_force;    // Coefficient to highering the current height.
    Vector2 position;       // GameObject position.
    // Components:
    Animator animator;       // Animator component.
    Audioplayer audioplayer; // Audioplayer component.

    GameObject(float movement_speed, Vector2 position, Animator animator, Audioplayer audioplayer) {
        isRight = 1;
        isJumping = false;
        //isAttacking = false;
        this->movement_speed = movement_speed;
        // Braking:
        braking_dist = 0;
        max_braking_dist = 10;
        this->braking_force = 40;
        // Jumping:
        jumping_dist = 0;
        max_jumping_dist = 150;
        this->jumping_force = 300;
        this->position = position; 
        this->animator = animator;
        this->audioplayer = audioplayer;
    }

    void Move() {

        // Delta time:
        float deltaTime = GetFrameTime();

        // Horizontal movement:
        int move = 0;
        if (animator.HasFinished("Attack")) {
            move = GetAxis("Horizontal");
            if (!move) {
                if (!animator.Trigger("Walk", "Brake") || !animator.Trigger("Jump","Brake")) {
                    if (!isJumping) {
                        if (braking_dist <= 3) {
                            braking_dist = 0;
                            animator["Idle"];
                        } else {
                            braking_dist -= braking_force * deltaTime;
                            std::cout << "Braking dist: " << braking_dist << "\n";
                            position.x += (isRight * braking_dist);
                        }
                    }
                }
            } else {
                if (!isJumping) animator["Walk"];
                if (!animator.InState("Walk")) {
                    braking_dist = 0;
                }
                if ((move < 0 && isRight == 1) || (move > 0 && isRight == -1)) {
                    braking_dist = 0;
                    isRight *= -1;
                    animator.Flip();
                }
            }

            // Vertical movement (jump):
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

            // Position actualization:
            auto dims = animator.GetDimensions();
            if ((position.x + dims.first) < -dims.first) {
                position.x = GetScreenWidth();
            } else if (position.x > (GetScreenWidth() + dims.first)) {
                position.x = -dims.first*2;
            } else {
                if (!isJumping && IsKeyPressed(KEY_E)) {
                    animator["Attack"];
                    braking_dist = 0;
                } else {
                    float movement_dist;
                    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                        max_braking_dist = 15;
                        movement_dist    = (move * (movement_speed+80) * deltaTime);
                    } else {
                        max_braking_dist = 10;
                        if (braking_dist > max_braking_dist) braking_dist = max_braking_dist;
                        movement_dist    = (move * movement_speed * deltaTime);
                    }
                    if (braking_dist < max_braking_dist) braking_dist += std::abs(movement_dist);
                    position.x += movement_dist;
                }
            }
            position.y += jumping_movement;
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
    GameObject Popo(100, Vector2{0,(WINDOW_HEIGHT - Popo_sprite.height*2.0f)-91}, PopoAnimator, PopoFX);


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
        PopoAnimator.Play(Vector2{350,580});
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

int main() {

    //std::ostringstream s;
    InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"COÑO");
    InitAudioDevice(); // Initialize audio device.

    NES = LoadFont("Assets/NES - Ice Climber - Fonts/Pixel_NES/Pixel_NES.otf");
    // ---- Music
    Music ts_music = LoadMusicStream("Assets/NES - Ice Climber - Sound Effects/01-Main-Title.mp3");
    ts_music.looping = true;
    bool play_music = false;
    PlayMusicStream(ts_music);
    SetTargetFPS(60);

    // Title screen.
    // ---- Sprite
    Texture2D ts_bg = LoadTexture("Assets/NES - Ice Climber - Sprites/01-Title-screen.png");
    Rectangle ts_src{0, 0, (float)ts_bg.width, (float)ts_bg.height};
    Rectangle ts_dst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};

    Texture2D PinesFore = LoadTexture("Assets/NES - Ice Climber - Sprites/Titlescreen - Pines - 01.png");
    float PinesForeHeight = (WINDOW_WIDTH * PinesFore.height)/(float)(PinesFore.width);
    Rectangle PinesForeSrc{0, 0, (float)PinesFore.width, (float)PinesFore.height};
    Rectangle PinesForeDst{0, WINDOW_HEIGHT - PinesForeHeight, (float)WINDOW_WIDTH, PinesForeHeight};
    float PinesForeSpeed = 0.6;
    Texture2D PinesMid = LoadTexture("Assets/NES - Ice Climber - Sprites/Titlescreen - Pines - 02.png");
    float PinesMidHeight = (WINDOW_WIDTH * PinesMid.height)/(float)(PinesMid.width);
    Rectangle PinesMidSrc{0, 0, (float)PinesMid.width, (float)PinesMid.height};
    Rectangle PinesMidDst{0, WINDOW_HEIGHT - PinesMidHeight, (float)WINDOW_WIDTH, PinesMidHeight};
    float PinesMidSpeed = 0.3;
    Texture2D Mountain = LoadTexture("Assets/NES - Ice Climber - Sprites/Titlescreen - Mountain - 03.png");
    float MountainHeight = (WINDOW_WIDTH * Mountain.height)/(float)(Mountain.width);
    Rectangle MountainSrc{0, 0, (float)Mountain.width, (float)Mountain.height};
    Rectangle MountainDst{0, WINDOW_HEIGHT - (MountainHeight * 0.75), (float)WINDOW_WIDTH * 0.75, MountainHeight * 0.75};
    float MountainSpeed = 0.1;
    Texture2D Fields = LoadTexture("Assets/NES - Ice Climber - Sprites/Titlescreen - Fields - 04.png");
    float FieldsHeight = (WINDOW_WIDTH * Fields.height)/(float)(Fields.width);
    Rectangle FieldsSrc{0, 0, (float)Fields.width, (float)Fields.height};
    Rectangle FieldsDst{0, WINDOW_HEIGHT - FieldsHeight, (float)WINDOW_WIDTH, FieldsHeight};
    Texture2D Snow = LoadTexture("Assets/NES - Ice Climber - Sprites/Titlescreen - Snow - 05.png");
    Rectangle SnowSrc{0, 0, (float)Snow.width,  (float)Snow.height};
    Rectangle SnowDst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    float SnowSpeed = 0.1;
    //Rectangle ts_pines_01_src0{0,0,(float)ts_pines_01.width,(float)ts_pines_01.height};
    //Rectangle ts_pines_01_src1{0,0,0,0};
    
    //float ts_pines_01_height = (WINDOW_WIDTH * ts_pines_01.height)/(float)(ts_pines_01.width);
    //Rectangle ts_pines_01_dst0{0,WINDOW_HEIGHT - ts_pines_01_height,(float)WINDOW_WIDTH, ts_pines_01_height};
    //Rectangle ts_pines_01_dst1{0,WINDOW_HEIGHT - ts_pines_01_height,(float)WINDOW_WIDTH, ts_pines_01_height};
    float scrollingFore = 0;

    while(!WindowShouldClose()) {

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


        DrawText("Press [M] to mute the music", 20, 20, 20, WHITE);
        EndDrawing();
        if (IsKeyPressed(KEY_ENTER)) {
            StopMusicStream(ts_music);
            game();
            PlayMusicStream(ts_music);
        }
    }

    // Title screen.
    UnloadFont(NES);
    UnloadTexture(ts_bg);
    UnloadMusicStream(ts_music);
    CloseAudioDevice();
    CloseWindow();

}