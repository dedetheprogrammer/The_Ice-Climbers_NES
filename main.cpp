#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "raylib.h"

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




int main(void) {

    int WINDOW_WIDTH  = 720;
    int WINDOW_HEIGHT = 720;
    InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"COÑO");
    InitAudioDevice(); // Initialize audio device.

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
    UnloadTexture(Popo_sprite);
    UnloadTexture(Mountain_sprite);
    UnloadFont(NES);
    UnloadTexture(Snowball);
    UnloadMusicStream(music);  // Unload music stream buffers from RAM
    UnloadSound(jump);  // Unload music stream buffers from RAM
    CloseAudioDevice();
    CloseWindow();
}