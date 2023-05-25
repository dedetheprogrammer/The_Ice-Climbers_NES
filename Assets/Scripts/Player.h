#pragma once
#include "EngineECS.h"
#include "Cloud.h"
#include "Controllers.h"
#include "FloorBehavior.h"
#include "raylib.h"
#include "raylibx.h"

class Player;
class HoleBehavior : public Script {
private:
    Transform2D& transform;
public:
    GameObject* original_block;
    HoleBehavior(GameObject& gameObject);
    HoleBehavior(GameObject& gameObject, HoleBehavior& behavior);
    Component* Clone(GameObject& gameObject);
};

class BlockBehavior : public Script {
private:
    Sprite& sprite;
    Transform2D& transform;
public:
    int *current_blocks = nullptr;
    int floor_level;
    GameObject* hole;

    BlockBehavior(GameObject& gameObject);
    BlockBehavior(GameObject& gameObject, BlockBehavior& behavior);
    Component* Clone(GameObject& gameObject);
    void OnCollision(Collision contact) override;
};

class SlidingBlockBehavior : public Script {
private:
    Sprite& sprite;
    Transform2D& transform;
    RigidBody2D& rigidbody;
public:
    GameObject* hole;
    int *current_blocks = nullptr;
    int floor_level;
    SlidingBlockBehavior(GameObject& gameObject);
    SlidingBlockBehavior(GameObject& gameObject, SlidingBlockBehavior& behavior);
    Component* Clone(GameObject& gameObject);
};

// ============================================================================
class Player : public Script {
private:

    // Variables para Popo:
    bool hasBounced;
    bool isJumping;
    bool onCloud;
    Vector2 last_save_position;
    float momentum;
    std::string last_tag;
    bool hasCollisioned;
public:
    bool isBraking;
    int lifes;
    bool brokeBlock;
    bool isAttacking; // Telling us if the object is attacking.
    bool isCrouched;
    bool isRight;     // Telling us if the object is facing to the right.
    bool isStunned;
    int frutasRecogidas;
    int bloquesDestruidos;
    bool victory;
    bool puntuacion;
    bool bonusLevel;
    int floor_level;
    int last_level;
    Vector2 collider_size;
    Vector2 collider_offset;
    Vector2 last_collider_pos;
    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más
    // eficiente que acceder una y otra vez a los componentes cada vez que
    // necesitamos hacer algo con uno de ellos.
    Animator& animator;
    AudioPlayer& audioplayer;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    bool isGrounded;  // Telling us if the object is on the ground.
    Transform2D& transform;
    Controller& controller; // Pointer to the controller instance.

    Player(GameObject& gameObject, Controller& c);
    Player(GameObject& gameObject, Player& behavior);
    Component* Clone(GameObject& gameObject) override;
    void OnCollision(Collision contact) override;
    void Update() override;

};
