#pragma once
#include "EngineECS.h"
#include "Popo.h"
#include <iostream>
#include "random"

class StalactiteBehavior : public Script {
private:
    float timeToSmall;
    float timeToMedium;
    float timeToBig;
    float timeToFall;
    float timeToBreak;

    float timeCount;
public:
    Animator& animator;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;

    enum StalactiteStates {NONE = 0, SMALL = 1, MEDIUM = 2, BIG = 3, FALLING = 4, BREAKING = 5, BROKEN = 6};
    StalactiteStates state;

    StalactiteBehavior(GameObject& gameObject) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        timeToSmall     = 1.0f;
        timeToMedium    = 1.0f;
        timeToBig       = 1.0f;
        timeToFall      = 1.0f;
        timeToBreak     = 0.5f;

        timeCount       = 0.0f;
        state           = NONE;
    }

    StalactiteBehavior(GameObject& gameObject, StalactiteBehavior& behavior) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        timeToSmall     = behavior.timeToSmall;
        timeToMedium    = behavior.timeToMedium;
        timeToBig       = behavior.timeToBig;
        timeToFall      = behavior.timeToFall;
        timeToBreak     = behavior.timeToBreak;

        timeCount       = behavior.timeCount;
        state           = behavior.state;
    }
    
    Component* Clone(GameObject& gameObject) override {
        return new StalactiteBehavior(gameObject, *this);
    }

    void OnCollision(Collision contact) override {
        
        if (state == FALLING
            &&  (contact.gameObject.tag == "Player"
                ||  (timeCount > 0.1
                    &&  (contact.gameObject.tag == "Floor"
                        || contact.gameObject.tag == "Wall"
                        || contact.gameObject.tag == "SlidingFloor")
                    )
                )
            )
        {
            state = BREAKING;
            animator["BREAKING"];
            timeCount = 0.0f;
            rigidbody.velocity = {0.0f, 0.0f};
        }
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        timeCount += deltaTime;

        switch (state) {
            case NONE:
                if (timeCount > timeToSmall) { state = SMALL; animator["SMALL"]; timeCount = 0.0f; }
                break;
            case SMALL:
                if (timeCount > timeToMedium) { state = MEDIUM; animator["MEDIUM"]; timeCount = 0.0f; }
                break;
            case MEDIUM:
                if (timeCount > timeToBig) { state = BIG; animator["BIG"]; timeCount = 0.0f; }
                break;
            case BIG:
                if (timeCount > timeToFall) { state = FALLING; timeCount = 0.0f; }
                break;
            case FALLING:
                transform.position.y += rigidbody.velocity.y * deltaTime;
                rigidbody.velocity.y += rigidbody.gravity    * deltaTime;
                if (rigidbody.velocity.y > rigidbody.max_velocity.y) rigidbody.velocity.y = rigidbody.max_velocity.y;
                break;
            case BREAKING:
                if (timeCount > timeToBreak) { state = BROKEN; animator["NONE"]; timeCount = 0.0f; gameObject.Destroy(); }
                break;
            default: break;
        }
    }

};

class HoleBehavior : public Script {
private:
    Transform2D& transform;
public:
    GameObject* original_block;

    HoleBehavior(GameObject& gameObject) : Script(gameObject),
        transform(gameObject.getComponent<Transform2D>()) {}

    HoleBehavior(GameObject& gameObject, HoleBehavior& behavior) : Script(gameObject),
        transform(gameObject.getComponent<Transform2D>()),
        original_block(behavior.original_block) {}


    Component* Clone(GameObject& gameObject) {
        return new HoleBehavior(gameObject, *this);
    }

    void OnCollision(Collision contact) override {
       //if (contact.gameObject.tag == "Icicle") {
       //    GameSystem::Instantiate(original_block, GameObjectOptions{.position=transform.position});
       //    //gameObject.Destroy();
       //    //contact.gameObject.Destroy();
       //}
    }
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
private:
    bool spawn_stalactite() { 
        return E(e2) < 0.002;   // probability to spawn a stalactite each frame
    }
public:
    GameObject* hole;
    GameObject* stalactiteTemplate;

    BlockBehavior(GameObject& gameObject) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()),
        spawning_cooldown(8.0f),
        spawning_timer(7.0f),
        e2(rd()),
        E(0,100) {}

    BlockBehavior(GameObject& gameObject, BlockBehavior& behavior) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()),
        spawning_cooldown(8.0f),
        spawning_timer(7.0f),
        e2(rd()),
        E(0,100),
        hole(behavior.hole),
        stalactiteTemplate(behavior.stalactiteTemplate) {}

    Component* Clone(GameObject& gameObject) {
        return new BlockBehavior(gameObject, *this);
    }

    void OnCollision(Collision contact) override {
        if (contact.gameObject.tag == "Player") {
            if (contact.contact_normal.y < 0 && !contact.gameObject.getComponent<Script, PopoBehavior>().brokeBlock) {
                contact.gameObject.getComponent<Script, PopoBehavior>().brokeBlock = true;
                contact.gameObject.getComponent<RigidBody2D>().velocity.y = 0;
                GameSystem::Instantiate(*hole, GameObjectOptions{.position = transform.position});
                gameObject.Destroy();
            }
        }
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        spawning_timer += deltaTime;
        
        if (spawning_timer > spawning_cooldown && spawn_stalactite()) {
            Vector2 spawnPosition = transform.position + Vector2{0.0f, transform.size.y};
            GameSystem::Instantiate(*stalactiteTemplate, GameObjectOptions{.position = spawnPosition});
            spawning_timer = 0.0f;
        }
    }

};

class SlidingBlockBehavior : public Script {
private:
    Sprite& sprite;
    Transform2D& transform;
    RigidBody2D& rigidbody;
public:
    GameObject* hole;
    SlidingBlockBehavior(GameObject& gameObject) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()) {}

    SlidingBlockBehavior(GameObject& gameObject, SlidingBlockBehavior& behavior) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        hole(behavior.hole) {}

    Component* Clone(GameObject& gameObject) {
        return new SlidingBlockBehavior(gameObject, *this);
    }
};