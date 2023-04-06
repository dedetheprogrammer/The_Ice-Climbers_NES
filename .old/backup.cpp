// ************===================*****************=========================
// NO TOCAR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ============*******************=================*************************
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

    std::map<std::string, Animation> Animator;

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
                        if (offset < jump_height) {
                            jump_height -= offset;
                            dst.y += offset;
                        } else {
                            dst.y += jump_height;
                            jump_height = 0;
                        }
                        
                    } else {
                        jump_height = 0;
                        jumping = false;
                        std::cout << dst.y << "\n";
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
                        if (offset < jump_height) {
                            jump_height -= offset;
                            dst.y += offset;
                        } else {
                            dst.y += jump_height;
                            jump_height = 0;
                        }
                    } else {
                        jump_height = 0;
                        jumping = false;
                    }
                }
            }
        //}
        
        for (auto it = snowballs.begin(); it != snowballs.end();) {
            if(!it->Move()) {
                it = snowballs.erase(it);
            } else {
                it++;
            }
        }
        snowball_cooldown -= GetFrameTime();
        
    }

    */