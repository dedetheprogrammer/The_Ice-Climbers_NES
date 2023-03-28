#pragma once
#include "animator.h"
#include "colisiones.h"
#include "raylib.h"
#include "audioplayer.h"
#include "Colisiones/Colisiones/figuras.h"
#include <ctime>
#include <random>

// Other:
Font NES;

Vector2 direction(Vector2 o, Vector2 d) {
    auto max = __max(abs(d.x - o.x), abs(d.y - o.y));
    return Vector2{(d.x - o.x) / max, (d.y - o.y) / max};
}

// Movimiento.
int GetAxis(std::string axis, std::unordered_map<std::string, int> controls) {
    if (axis == "Horizontal") {
        bool left_key  = IsKeyDown(controls["Left"]);
        bool right_key = IsKeyDown(controls["Right"]);
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
        velocity = min_velocity;
        this->min_velocity = min_velocity;
        this->max_velocity = max_velocity;
        this->acceleration = acceleration;
    }

};


class GameObject {
private:
    //...
public:
    int numPlayer;
    // Controls
    std::unordered_map<std::string, int> controls; //The player's controls
    // Propiedades hardcodeadas.
    int isRight;              // Telling us if the object is facing to the right.
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
    Rectangulo hitbox;       // Collision component

    GameObject(float movement_speed, Vector2 position, Vector2 size, Animator animator, Audioplayer audioplayer, RigidBody rigidbody, int numPlayer, std::unordered_map<std::string, int> controls) {
        isRight = 1;
        isJumping = false;
        //isAttacking = false;
        this->movement_speed = movement_speed;
        // Jumping:
        jumping_dist = 0;
        max_jumping_dist = 250;
        this->jumping_force = 300;
        this->position = position;
        this->animator = animator;
        this->audioplayer = audioplayer;
        this->rigidbody   = rigidbody;
        this->numPlayer = numPlayer;
        this->controls = controls;
        this->animator.setPlayerColor(this->numPlayer);
        this->hitbox = Rectangulo(position, size * 3);
        std::cout << "Gameobject hitbox:";
        hitbox.Print();
    }

    void Move() {

        // Delta time:
        float deltaTime = GetFrameTime();

        // Horizontal movement:
        int move = 0;
        if (animator.HasFinished("Attack")) {
            move = GetAxis("Horizontal", controls);
            if (!move) {
                if (!animator.Trigger("Walk", "Brake") || isJumping == 0) {
                    //if (!isJumping) {
                        if ((rigidbody.velocity.x > rigidbody.min_velocity.x)) {
                            rigidbody.velocity.x -= rigidbody.acceleration.x * deltaTime;
                            position.x += isRight * rigidbody.velocity.x * deltaTime;
                        } else {
                            rigidbody.velocity.x = rigidbody.min_velocity.x;
                            if (!animator.InState("Stunned")) animator["Idle"];
                        }
                    //}
                }
            } else {
                if (animator.InState("Stunned")) {
                    animator["Walk"];
                }
                if (rigidbody.velocity.x < rigidbody.max_velocity.x) {
                    rigidbody.velocity.x += rigidbody.acceleration.x * deltaTime;
                } else {
                    rigidbody.velocity.x = rigidbody.max_velocity.x;
                }
                if (!isJumping) animator["Walk"];
                if (!animator.InState("Walk")) {
                }
                if ((move < 0 && isRight == 1) || (move > 0 && isRight == -1)) {
                    rigidbody.velocity.x = rigidbody.min_velocity.x;
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
            }/* else if (isJumping == 1) {
                if (jumping_dist < max_jumping_dist) {
                    jumping_dist += jumping_movement;
                } else {
                    jumping_dist = 0;
                    isJumping = 0;
                }
            }*/
            

            // Position actualization:
            auto dims = animator.GetDimensions();
            if ((position.x + dims.first) < -dims.first) {
                position.x = GetScreenWidth();
            } else if (position.x > (GetScreenWidth() + dims.first)) {
                position.x = -dims.first*2;
            } else {
                if (!isJumping && !animator.InState("Stunned") && IsKeyPressed(controls["Attack"])) {
                    animator["Attack"];
                    rigidbody.velocity.x = rigidbody.min_velocity.x;
                } else {
                    position.x += move * rigidbody.velocity.x * deltaTime;
                }
            }
            position.y += jumping_movement;
        }
        hitbox.Move(position);
    }

    void Draw() {
        animator.Play(Vector2{position.x, position.y});
        hitbox.Draw();
    }

};

class IAObject {
private:
    //...
public:
    // Propiedades hardcodeadas.
    int move;      // Telling us if the object is facing to the right.
    float speed;       // GameObject speed.
    bool lookingRight;

    std::mt19937 mt;
    std::uniform_real_distribution<> rand;
    std::uniform_int_distribution<> side;
    Vector2 position;   // IAObject position.
    Vector2 initialPos; // IAObject initial position.
    Animator animator;  // Animator component.
    RigidBody rigidbody;
    Rectangulo hitbox;
    float ratio;        // IAObject appearance ratio

    IAObject(Vector2 position, Vector2 size, Animator animator, RigidBody rigidbody, float ratio, int seed) :
        position(position), initialPos(position), animator(animator), rigidbody(rigidbody), ratio(ratio) {
        move = 1;
        lookingRight = true;
        std::mt19937 mt(seed);
        std::uniform_real_distribution<> rand(0.0,100.0);
        std::uniform_int_distribution<> side(0,1);
        this->mt = mt;
        this->rand = rand;
        this->side = side;
        this->hitbox = Rectangulo(position, size * 3);
    }

    void Draw() {
        animator.Play(position);
        hitbox.Draw();
    }

    void Flip() {
        animator.Flip();
        move *= -1;
    }

    Vector2 getPosition() {
        return position;
    }

    void setPosition(Vector2 position) {
        this->position = position;
    }

    void hit() {
        animator["Stunned"];
        move *= -1;
        animator.Flip();
    }

    virtual void Play() = 0;
};

class Topi : public IAObject {
private:
    bool running;
public:
    Topi(Vector2 position, Vector2 size, Animator animator, RigidBody rigidbody, float ratio, int seed) :
        IAObject(position, size, animator, rigidbody, ratio, seed), running(false) {}

    void run() {
        running = !running;
    }

    void Move() {
        float deltaTime = GetFrameTime();

        // Horizontal movement:
        auto dims = animator.GetDimensions();
        if ((position.x + dims.first) < 0 || position.x > GetScreenWidth()) {
            Flip();
            if(animator.InState("Stunned")) {
                animator["Walk"];
            }
        }
        position.x += move * rigidbody.velocity.x * deltaTime;
        hitbox.Move(position);
    }

    void Play() override {
        Move();
        Draw();
    }
};

class Joseph : public IAObject {
private:
    bool onScreen;
public:
    Joseph(Vector2 position, Vector2 size, Animator animator, RigidBody rigidbody, float ratio, int seed) :
        IAObject(position, size, animator, rigidbody, ratio, seed), onScreen(false) {
        this->hitbox.Print();
    }

    void Move() {
        float deltaTime = GetFrameTime();
        if(onScreen) {
            // Horizontal movement:
            auto dims = animator.GetDimensions();
            if ((position.x + dims.first) < 0 || position.x > GetScreenWidth()) {
                onScreen = !onScreen;
                position = initialPos;
                if(animator.InState("Stunned")) animator["Walk"];
            } else                position.x += move * rigidbody.velocity.x * deltaTime;

        } else {
            onScreen = abs(rand(mt) - rand(mt)) < ratio;
            if(onScreen) {
                move = side(mt);
                if(move != lookingRight) {
                    lookingRight = !lookingRight;
                    Flip();
                }
                if(!move) {
                    move = -1;
                    position.x = GetScreenWidth();
                }
                position.x += move * rigidbody.velocity.x * deltaTime;
            }
        }
        hitbox.Move(position);
    }

    void Play() override {
        Move();
        if(onScreen) {
            Draw();
        }
    }
};

class Nutpicker : public IAObject {
private:
    bool onScreen;
    std::vector<Vector2> flightPoints;
public:
    Nutpicker(Vector2 position, Vector2 size, Animator animator, RigidBody rigidbody, float ratio, int seed) :
        IAObject(position, size, animator, rigidbody, ratio, seed), onScreen(false) {
            flightPoints = { {GetScreenWidth() / 6.0f, GetScreenHeight() / 4.0f}, {GetScreenWidth() - (GetScreenWidth() / 6.0f), GetScreenHeight() / 6.0f},
                {GetScreenWidth() - (GetScreenWidth() / 6.0f), GetScreenHeight() - (GetScreenHeight() / 3.0f)}, {GetScreenWidth() / 4.0f, GetScreenHeight() - (GetScreenHeight() / 4.0f)} };
    }

    void Move() {
        float deltaTime = GetFrameTime();
        if(onScreen) {
            // Horizontal movement:
            auto dims = animator.GetDimensions();
            if ((position.x + dims.first) < 0 || position.x > GetScreenWidth()) {
                onScreen = !onScreen;
                position = initialPos;
                if(animator.InState("Stunned")) animator["Walk"];
            } else
                position.x += move * rigidbody.velocity.x * deltaTime;

        } else {
            onScreen = rand(mt) < ratio;
            if(onScreen) {
                move = side(mt);
                if(move != lookingRight) {
                    lookingRight = !lookingRight;
                    Flip();
                }
                if(!move) {
                    move = -1;
                    position.x = GetScreenWidth();
                }
                position.x += move * rigidbody.velocity.x * deltaTime;
            }
        }
        hitbox.Move(position);
    }

    void Play() override {
        Move();
        if(onScreen) {
            Draw();
        }
    }
};

class WorldObject {
private:
    //...
public:
    Vector2 position;   // WorldObject position.
    Animator animator;  // Animator component.
    Rectangulo hitbox;

    WorldObject(Vector2 position, Vector2 size, Animator animator) :
        position(position), animator(animator) {
        this->hitbox = Rectangulo(position, size * 3);
    }

    void Draw() {
        animator.Play(position);
        hitbox.Draw();
    }

    void Flip() {
        animator.Flip();
    }

    Vector2 getPosition() {
        return position;
    }

    void setPosition(Vector2 position) {
        this->position = position;
    }
};

/*Collision Collides(GameObject p, IAObject i) {
    return CollidesV(p.hitbox, p.rigidbody.velocity, i.hitbox, i.rigidbody.velocity);
}*/

static bool Collides(Rectangulo r1, Rectangulo r2)
{
    return !(r1.x + r1.width < r2.x		// r1 is to the left of r2
        || r2.x + r2.width < r1.x		// r1 is to the right of r2
        || r1.y + r1.height < r2.y		// r1 is above r2 
        || r2.y + r2.height < r1.y);	// r1 is below r2
}

/*static Collision CollidesV(Rectangulo r1, Vector2 v1, Rectangulo r2, Vector2 v2)
{
    // Caso en el que ya se estaba colisionando al principio del frame
    if (Collides(r1, r2))
    {
        return Collision(true);
    }

    // Si no colisiona después de desplazarse, entonces no hace falta calcular nada más
    // asumimos los objetos no se mueven a grandes velocidades
    Rectangulo r1Desp = Rectangulo(r1.x + v1.x, r1.y + v1.y, r1.height, r1.width);
    Rectangulo r2Desp = Rectangulo(r2.x + v2.x, r2.y + v2.y, r2.height, r2.width);
    if (!Collides(r1Desp, r2Desp))
    {
        return Collision(false);
    }

    Collision retCol = Collision(false);

    // Encontrar instante temporal en el que la cara superior e inferior se cruzan en y
    // y la cara derecha con la izquierda se cruzan en x, para cada combinación, es decir 4
    // (x'-x) = (v'-v) * t -> t = (x'-x) / (v'-v)
    float tsup1_x_inf2 = (std::fabs(v2.y - v1.y) > 0.00001f) ? ((r2.y + r2.height) - r1.y) / (v1.y - v2.y) : -1.0f;
    float tinf1_x_sup2 = (std::fabs(v2.y - v1.y) > 0.00001f) ? (r2.y - (r1.y + r1.height)) / (v1.y - v2.y) : -1.0f;
    float tizq1_x_der2 = (std::fabs(v2.x - v1.x) > 0.00001f) ? ((r2.x + r2.width) - r1.x) / (v1.x - v2.x) : -1.0f;
    float tder1_x_izq2 = (std::fabs(v2.x - v1.x) > 0.00001f) ? (r2.x - (r1.x + r1.width)) / (v1.x - v2.x) : -1.0f;

    // Verificar si en los tiempos calculados, realmente hay colision
    r1Desp.Move(r1.x + tsup1_x_inf2 * v1.x, r1.y + tsup1_x_inf2 * v1.y);
    r2Desp.Move(r2.x + tsup1_x_inf2 * v2.x, r2.y + tsup1_x_inf2 * v2.y);
    tsup1_x_inf2 = (Collides(r1Desp, r2Desp)) ? tsup1_x_inf2 : -1.0f;

    r1Desp.Move(r1.x + tinf1_x_sup2 * v1.x, r1.y + tinf1_x_sup2 * v1.y);
    r2Desp.Move(r2.x + tinf1_x_sup2 * v2.x, r2.y + tinf1_x_sup2 * v2.y);
    tinf1_x_sup2 = (Collides(r1Desp, r2Desp)) ? tinf1_x_sup2 : -1.0f;

    r1Desp.Move(r1.x + tizq1_x_der2 * v1.x, r1.y + tizq1_x_der2 * v1.y);
    r1Desp.Move(r2.x + tizq1_x_der2 * v2.x, r2.y + tizq1_x_der2 * v2.y);
    tizq1_x_der2 = (Collides(r1Desp, r2Desp)) ? tizq1_x_der2 : -1.0f;

    r1Desp.Move(r1.x + tder1_x_izq2 * v1.x, r1.y + tder1_x_izq2 * v1.y);
    r1Desp.Move(r2.x + tder1_x_izq2 * v2.x, r2.y + tder1_x_izq2 * v2.y);
    tder1_x_izq2 = (Collides(r1Desp, r2Desp)) ? tder1_x_izq2 : -1.0f;


    retCol.debugMsg =  "t1: " + std::to_string(tsup1_x_inf2)
                    + " t2: " + std::to_string(tinf1_x_sup2)
                    + " t3: " + std::to_string(tizq1_x_der2)
                    + " t4: " + std::to_string(tder1_x_izq2);

    // Calcular el minimo no negativo de ellos
    float tmin = 2.0f;

    if (tsup1_x_inf2 >= 0.0f && tsup1_x_inf2 <= 1.0f)
        { tmin = tsup1_x_inf2; retCol.colNormal = { 0.0f, 1.0f }; }
    if (tinf1_x_sup2 >= 0.0f && tinf1_x_sup2 <= 1.0f && tinf1_x_sup2 < tmin)
        { tmin = tinf1_x_sup2; retCol.colNormal = { 0.0f, -1.0f }; }
    if (tizq1_x_der2 >= 0.0f && tizq1_x_der2 <= 1.0f && tizq1_x_der2 < tmin)
        { tmin = tizq1_x_der2; retCol.colNormal = { 1.0f, 0.0f }; }
    if (tder1_x_izq2 >= 0.0f && tder1_x_izq2 <= 1.0f && tder1_x_izq2 < tmin)
        { tmin = tder1_x_izq2; retCol.colNormal = { -1.0f, 0.0f }; }

    if (tmin >= 0.0f && tmin <= 1.0f)
    {
        retCol.valid = true;
        retCol.despCol = { tmin * v1.x, tmin * v1.y };
        retCol.tCol = tmin;
    }
    return retCol;
}*/

/*void game() {

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
}*/
