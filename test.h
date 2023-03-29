#pragma once
#include "animator.h"
#include "audioplayer.h"
#include "./Colisiones/colisiones.h"
#include "raylib.h"

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
