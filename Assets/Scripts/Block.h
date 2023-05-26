#pragma once
#include "EngineECS.h"
#include "Controllers.h"
//#include "Popo.h"
#include "Cloud.h"
#include <iostream>
#include "random"
#include "raylib.h"
#include "raylibx.h"

class PopoBehavior;
class StalactiteBehavior : public Script {
private:
    float timeToSmall;
    float timeToMedium;
    float timeToBig;
    float timeToFall;
    float timeToBreak;
    float timeToDisappear;

    float timeCount;
public:
    float spawning_ratio = 0.002;

    Animator& animator;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;

    enum StalactiteStates {NONE = 0, SMALL = 1, MEDIUM = 2, BIG = 3, FALLING = 4, BREAKING = 5, BROKEN = 6};
    StalactiteStates state;

    StalactiteBehavior(GameObject& gameObject);

    StalactiteBehavior(GameObject& gameObject, StalactiteBehavior& behavior);
    
    Component* Clone(GameObject& gameObject);

    void OnCollision(Collision contact);

    void Update();

};

class HoleBehavior : public Script {
private:
    Transform2D& transform;
public:
    GameObject* original_block;

    HoleBehavior(GameObject& gameObject);

    HoleBehavior(GameObject& gameObject, HoleBehavior& behavior);


    Component* Clone(GameObject& gameObject);

    void OnCollision(Collision contact) override;
};

class BlockBehavior : public Script {
private:
    Sprite& sprite;
    Transform2D& transform;
    float spawning_cooldown;
    float spawning_timer;
    std::random_device rd;
    std::mt19937 e2;
    std::uniform_real_distribution<float> E;

    bool spawn_stalactite();
public:
    int *current_blocks = nullptr;
    int floor_level;
    GameObject* hole;
    GameObject* stalactiteTemplate;

    BlockBehavior(GameObject& gameObject);

    BlockBehavior(GameObject& gameObject, BlockBehavior& behavior);

    Component* Clone(GameObject& gameObject);

    void OnCollision(Collision contact) override;

    void Update() override;

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

    void OnCollision(Collision contact) override;
};

class FloorBehavior : public Script {
private:
    //...
public:
    Transform2D& transform;
    int floor_level;
    FloorBehavior(GameObject& gameObject);

    FloorBehavior(GameObject& gameObject, FloorBehavior& behavior);

    Component* Clone(GameObject& gameObject) override;
};

bool static final;

class PopoBehavior : public Script {
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
    //bool static final; 
    bool isBraking;
    int lifes;
    bool brokeBlock;
    bool isAttacking; // Telling us if the object is attacking.
    bool isCrouched;
    bool isRight;     // Telling us if the object is facing to the right.
    bool isStunned;
    bool isSliding;
    int frutasRecogidas;
    int bloquesDestruidos;
    int nutpickerGolpeados;
    int icicleDestruido;
    bool victory;
    bool puntuacion;
    bool bonusLevel;
    int timeDead;
    int timeStunned;
    int lastMove;
    int lastVelocity;
    int iceVelocity;
    int floor_level;
    int last_level;
    Vector2 collider_size;
    Vector2 collider_offset;
    Vector2 last_collider_pos;

    static void setFinal(bool b);
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

    PopoBehavior(GameObject& gameObject, Controller& c);

    PopoBehavior(GameObject& gameObject, PopoBehavior& behavior);

    Component* Clone(GameObject& gameObject) override;

    void OnCollision(Collision contact);

    void Update() override;
};
