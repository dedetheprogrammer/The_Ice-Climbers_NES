#pragma once
#include "animator.h"
#include "audio_player.h"
#include "collider.h"
#include "rigidbody.h"
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

enum WINDOW_BEHAVOR { COLLISION = 0x01, TRAVERSE = 0x02, IGNORE = 0x04 };
bool WINDOW_LIMITS_BEHAVOR (WINDOW_BEHAVOR flag) {
    return false;
}

class GameObject {
private:
    //...
public:
    // Popo hardcoded properties.
    bool isGrounded;  // Telling us if the object is on the ground.
    bool isRight;     // Telling us if the object is facing to the right.
    bool isAttacking; // Telling us if the object is attacking.
    // Transform variables.
    Vector2 position; // GameObject position.
    // Components:
    Animator animator; // Animator component.
    Audioplayer audioplayer; // Audioplayer component.
    RigidBody2D rigidbody;   // Phisics component.
    Collider collider; 

    GameObject(Vector2 position, Animator animator, Audioplayer audioplayer, RigidBody2D rigidbody) {
        isRight     = true;
        isGrounded  = false;
        isAttacking = false;

        this->position = position; 
        this->animator = animator;
        this->audioplayer = audioplayer;
        this->rigidbody = rigidbody;
        collider = Collider(&this->position, animator.GetViewDimensions());
    }

    void Move(Collider& floor) {

        // Auxiliar variables:
        int move = 0;                     // Horizontal move sense.
        float deltaTime = GetFrameTime(); // Delta time

        if (!isAttacking) {
            // Horizontal movement:
            move = GetAxis("Horizontal");
            if (move) {
                rigidbody.velocity.x = move * rigidbody.acceleration.x;
                if (isGrounded) {
                    animator["Walk"];
                    collider.size = animator.GetViewDimensions();
                }
            }
            position.x += rigidbody.velocity.x * deltaTime;
            if (move > 0 && !isRight || move < 0 && isRight) {
                isRight = !isRight;
                animator.Flip();
            }
            if (position.x > WINDOW_WIDTH) {
                position.x = -animator.GetViewDimensions().x;
            } else if (position.x + animator.GetViewDimensions().x < 0) {
                position.x = WINDOW_WIDTH;
            }

            // Vertical movement:
            if (isGrounded) {
                if (IsKeyDown(KEY_SPACE)) {
                    isGrounded = false;
                    rigidbody.velocity.y = -rigidbody.acceleration.y;
                    animator["Jump"];
                    collider.size = animator.GetViewDimensions();
                } else if (IsKeyDown(KEY_E)) {
                    isAttacking = true;
                    position.y -= 3;
                    rigidbody.velocity.x = 0;
                    animator["Attack"];
                    collider.size = animator.GetViewDimensions();
                }
            }
        } else if (animator.HasFinished("Attack")) {
            isAttacking = false;
            animator["Idle"];
            collider.size = animator.GetViewDimensions();
            position.y += 3;
        }

        // Colissions:
        float ct; Vector2 cp, cn;
        if (!Collides(collider, rigidbody.velocity * deltaTime, floor, cp, cn, ct) || ct >= 1.0f) {
            position.y += rigidbody.velocity.y * deltaTime;
            rigidbody.velocity.y += rigidbody.gravity * deltaTime;
        } else {
            if (!isAttacking && !move) {
                if (rigidbody.velocity.x > 0) {
                    rigidbody.velocity.x -= sgn(rigidbody.velocity.x) * rigidbody.acceleration.x * deltaTime;
                    if (rigidbody.velocity.x < 0) {
                        rigidbody.velocity.x = 0;
                    }
                    animator["Brake"];
                    collider.size = animator.GetViewDimensions();
                } else if (rigidbody.velocity.x < 0) {
                    rigidbody.velocity.x -= sgn(rigidbody.velocity.x) * rigidbody.acceleration.x * deltaTime;
                    if (rigidbody.velocity.x > 0) {
                        rigidbody.velocity.x = 0;
                    }
                    animator["Brake"];
                    collider.size = animator.GetViewDimensions();
                } else {
                    rigidbody.velocity.x = 0;
                    animator["Idle"];
                    collider.size = animator.GetViewDimensions();
                }
            }
            rigidbody.velocity.y += cn.y * std::abs(rigidbody.velocity.y) * (1 - ct);
            isGrounded = true;
        }
    }

    void Draw() {
        animator.Play(position);
        collider.Draw();
        rigidbody.Draw(position + animator.GetViewDimensions());
    }

};
