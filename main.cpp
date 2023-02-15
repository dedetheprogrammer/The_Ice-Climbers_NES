#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include "raylib.h"

int WINDOW_WIDTH  = 720;
int WINDOW_HEIGHT = 720;

/*
class Sprite {
private:
    Rectangle src; // Rectangle that selects the region of the sprite.
    Rectangle dst; // Rectangle that shows the region with the transformations.
public:

    Texture2D sprite;

    Sprite(const char *fileName) {
        sprite = LoadTexture(fileName);
        src = dst = {0,0,(float)sprite.width,(float)sprite.height};
    }

    Sprite(const char *fileName, float scale) {
        sprite = LoadTexture(fileName);
        src = {0,0,(float)sprite.width, (float)sprite.height};
        dst = {0,0,sprite.width*scale, sprite.height*scale};
    }

    Sprite(const char* fileName, float scale, Vector2 dest) {
        sprite = LoadTexture(fileName);
        src = {0,0,(float)sprite.width, (float)sprite.height};
        dst = {dest.x, dest.y, sprite.width*scale, sprite.height*scale};  
    }

    void Draw() {
        DrawTexturePro(sprite, src, dst, Vector2{0,0}, 0, WHITE);
    }
};
*/


class Projectile {
private:
    Rectangle src;
    Rectangle dst;
    float distance;
public:
    Texture2D sprite;
    float speed;
    Projectile(float speed, Vector2 position, Texture2D sprite) : speed(speed) {
        this->sprite = sprite;
        src = {0, 0, (float)sprite.width, (float)sprite.height};
        dst = {position.x, position.y, sprite.width*2.5f, sprite.height*2.5f};
        distance = 0;
    }

    bool Move() {
        if (dst.x + dst.width < 0 || dst.x > GetScreenWidth() || distance >= 100) {
            distance = 0;
            return false;
        }
        float offset = speed * GetFrameTime();
        distance += std::abs(offset);
        dst.x += offset;
        return true;
        
    }

    void Draw() {
        DrawTexturePro(sprite, src, dst, Vector2{0,0}, 0, WHITE);
    }

};

class Object {
private:
    // ...
    Rectangle src;
    Rectangle dst;
    bool jumping;
    bool jump_direction;
    float jump_height;
    float max_jump_height = 150;
    int sense;
public:

    float snowball_cooldown;
    std::vector<Projectile> snowballs;
    float speed;
    Vector2 position;
    Texture2D sprite, snowball;
    Sound jump;

    Object(float speed, Vector2 position, Texture2D sprite, Sound jump, Texture2D snowball) : position(position), sprite(sprite) {
        this->snowball = snowball;
        this->speed = speed;
        jumping = false;
        jump_height = 0;
        this->jump = jump;
        src = {0, 0, (float)sprite.width, (float)sprite.height};
        dst = {position.x, position.y, sprite.width*3.0f, sprite.height*3.0f};
        sense = -1;
        snowball_cooldown = 0;
    }

    void Move() {
        
        if (IsGamepadAvailable(0)) {
            float x_move = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
            if (x_move > 0) {
                sense =  1;
                if (src.width > 0) {
                    src.width *= -1;
                }
            } else if (x_move < 0) {
                sense = -1;
                if (src.width < 0) {
                    src.width *= -1;
                }
            }
            if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_THUMB)) {
                dst.x += x_move * (speed + 80) * GetFrameTime();
            } else {
                dst.x += x_move * speed * GetFrameTime();
            }
            if (!jumping && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                PlaySound(jump);
                jumping = true;
                jump_direction = true;
            } else if (jumping) {
                float offset = (speed*3) * GetFrameTime();
                if (jump_height <= max_jump_height && jump_direction == true) {
                    jump_height += offset;
                    dst.y -= offset;
                } else {
                    jump_direction = false;
                    if (jump_height > 0) {
                        jump_height -= offset;
                        dst.y += offset;
                    } else {
                        jump_height = 0;
                        jumping = false;
                    }
                }
            }
            if (snowball_cooldown <= 0 && snowballs.size() < 3 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                snowball_cooldown = 0.6;
                if (sense > 0) {
                    snowballs.push_back(Projectile(200, Vector2{dst.x + dst.width + 5, dst.y + 30}, snowball));
                } else if (sense < 0) {
                    snowballs.push_back(Projectile(-200, Vector2{dst.x - snowball.width - 5, dst.y + 30}, snowball));
                }
            }
        } else {
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                sense = 1;
                if (src.width > 0) {
                    src.width *= -1;
                }
                if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                    dst.x += sense * (speed + 80) * GetFrameTime();
                } else {
                    dst.x += sense * speed * GetFrameTime();
                }
            } else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                sense = -1;
                if (src.width < 0) {
                    src.width *= -1;
                }
                if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                    dst.x += sense * (speed + 80) * GetFrameTime();
                } else {
                    dst.x += sense * speed * GetFrameTime();
                }
            }

            if (!jumping && IsKeyPressed(KEY_SPACE)) {
                PlaySound(jump);
                jumping = true;
                jump_direction = true;
            } else if (jumping) {
                float offset = (speed*3) * GetFrameTime();
                if (jump_height <= max_jump_height && jump_direction == true) {
                    jump_height += offset;
                    dst.y -= offset;
                } else {
                    jump_direction = false;
                    if (jump_height > 0) {
                        jump_height -= offset;
                        dst.y += offset;
                    } else {
                        jump_height = 0;
                        jumping = false;
                    }
                }
            }
        }
        for (auto it = snowballs.begin(); it != snowballs.end(); /*it++*/) {
            if(!it->Move()) {
                it = snowballs.erase(it);
            } else {
                it++;
            }
        }
        snowball_cooldown -= GetFrameTime();
    }

    void Draw() {
        if (dst.x + dst.width < 0) {
            dst.x = GetScreenWidth();
        } if (dst.x > GetScreenWidth()) {
            dst.x = -dst.width;
        }
        DrawTexturePro(sprite, src, dst, Vector2{0,0}, 0, WHITE);
        for (auto& s : snowballs) {
            s.Draw();
        }
    }

};

void game() {

    Sound jump  = LoadSound("NES - Ice Climber - Sound Effects/09-Jump.wav");
    Music music = LoadMusicStream("NES - Ice Climber - Sound Effects/03-Play-BGM.ogg");
    music.looping = true;
    Texture2D Snowball = LoadTexture("NES - Ice Climber - Sprites/03-Snowball.png");
    Texture2D Pause_frame = LoadTexture("NES - Ice Climber - Sprites/04-Small-frame.png");
    float paused_showtime = 0.75;
    bool show = true;
    Texture2D Popo_sprite = LoadTexture("NES - Ice Climber - Sprites/02-Popo-Idle.png");
    Texture2D Mountain_sprite = LoadTexture("NES - Ice Climber - Sprites/01-Mountain.png");
    Rectangle src{0, (float)(Mountain_sprite.height - Mountain_sprite.width), (float)Mountain_sprite.width, (float)Mountain_sprite.width};
    Rectangle dst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    Rectangle src_0{0, 0, (float)Pause_frame.width, (float)Pause_frame.height};
    Rectangle dst_1{(WINDOW_WIDTH - Pause_frame.width*3.0f)/2.0f + 4, (WINDOW_HEIGHT - Pause_frame.height)/2.0f - 3, Pause_frame.width*3.0f, Pause_frame.height*3.0f};
    Object Popo(100, Vector2{(WINDOW_WIDTH - Popo_sprite.width*2.0f)/2,(WINDOW_HEIGHT - Popo_sprite.height*2.0f)-91}, Popo_sprite, jump, Snowball);
    Font NES = LoadFont("NES - Ice Climber - Fonts/Pixel_NES/Pixel_NES.otf");

    PlayMusicStream(music);
    bool paused = false;
    while(!WindowShouldClose()) {
        UpdateMusicStream(music);
        BeginDrawing();
        ClearBackground(BLACK);
        if (IsGamepadAvailable(0)) {
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                paused = !paused;
            }
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
        
        DrawTexturePro(Mountain_sprite, src, dst, Vector2{0,0}, 0, WHITE);
        EndDrawing();
    }
    UnloadFont(NES);
    UnloadTexture(Popo_sprite);
    UnloadTexture(Mountain_sprite);
    UnloadTexture(Snowball);
    UnloadMusicStream(music);  // Unload music stream buffers from RAM
    UnloadSound(jump);  // Unload music stream buffers from RAM

}

int main() {

    std::ostringstream s;
    InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"COÑO");
    InitAudioDevice(); // Initialize audio device.

    Font NES = LoadFont("NES - Ice Climber - Fonts/Pixel_NES/Pixel_NES.otf");

    // Title screen.
    // ---- Sprite
    Texture2D ts_bg = LoadTexture("NES - Ice Climber - Sprites/01-Title-screen.png");
    Texture2D ts_hammer = LoadTexture("NES - Ice Climber - Sprites/05-Menu-hammer.png");
    Rectangle ts_src{0, 0, (float)ts_bg.width, (float)ts_bg.height};
    Rectangle ts_dst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    Rectangle ts_hammer_src{0, 0, (float)ts_hammer.width, (float)ts_hammer.height};
    int option = 0, options = 2;
    float option_change_timer = 0;
    Rectangle ts_hammer_dst_0{161, 388, ts_hammer.width*3.0f, ts_hammer.height*3.0f};
    Rectangle ts_hammer_dst_1{161, 438, ts_hammer.width*3.0f, ts_hammer.height*3.0f};
    //Rectangle ts_hammer_dst_2{161, 488, ts_hammer.width*3.0f, ts_hammer.height*3.0f};
    int mountain = 0, mountains = 32;

    // ---- Music
    Music ts_music = LoadMusicStream("NES - Ice Climber - Sound Effects/01-Main-Title.mp3");
    ts_music.looping = true;
    bool play_music = false;

    PlayMusicStream(ts_music);

    SetTargetFPS(30);
    bool JOYSTICK_USED = false;
    while(!WindowShouldClose()) {
        if (IsKeyPressed(KEY_M)) {
            play_music = !play_music;
        }
        if (play_music) {
            UpdateMusicStream(ts_music);
        }
        BeginDrawing();
        DrawTexturePro(ts_bg, ts_src, ts_dst, Vector2{0,0}, 0, WHITE);
        if (IsGamepadAvailable(0)) {
            if (option_change_timer <= 0) {
                option_change_timer = 0.12;
                int move = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
                if (move < 0) {
                    option = (((option-1)%options) + options) % options;
                } else if (move > 0) {
                    option = (option+1)%options;
                }
            }
            if (IsGamepadButtonPressed(0,GAMEPAD_BUTTON_LEFT_FACE_UP)) {
                option = (((option-1)%options) + options) % options;
            }
            if (IsGamepadButtonPressed(0,GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
                option = (option+1)%options;
            }
            if (IsGamepadButtonPressed(0,GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                mountain = (((mountain-1)%mountains) + mountains) % mountains;
            }
            if (IsGamepadButtonPressed(0,GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                mountain = (mountain+1)%mountains;
            }
        }
        if (option == 0) {
            DrawTexturePro(ts_hammer, ts_hammer_src, ts_hammer_dst_0, Vector2{0,0}, 0, WHITE);
        } else if (option == 1) {
            DrawTexturePro(ts_hammer, ts_hammer_src, ts_hammer_dst_1, Vector2{0,0}, 0, WHITE);
        }
        DrawText("Press [M] to mute the music", 20, 20, 20, WHITE);
        s << std::setw(2) << std::setfill('0') << mountain+1;
        //DrawTextPro(NES, "PAUSED", Vector2{WINDOW_WIDTH/2.0f-55, WINDOW_HEIGHT/2.0f}, Vector2{0,0}, 0, 30, 1.5, WHITE);
        DrawTextPro(NES, s.str().c_str(), Vector2{427, 485}, Vector2{0,0}, 0, 30, 4, Color{168,228,252,255});
        s.str("");
        EndDrawing();
        option_change_timer -= GetFrameTime();
        if (IsGamepadAvailable(0)) {
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                StopMusicStream(ts_music);
                game();
                PlayMusicStream(ts_music);
            }
        }
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