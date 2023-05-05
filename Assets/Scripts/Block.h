#pragma once
#include "EngineECS.h"
#include "Popo.h"
#include <iostream>

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
public:
    GameObject* hole;
    BlockBehavior(GameObject& gameObject) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()) {}

    BlockBehavior(GameObject& gameObject, BlockBehavior& behavior) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()),
        hole(behavior.hole) {}

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