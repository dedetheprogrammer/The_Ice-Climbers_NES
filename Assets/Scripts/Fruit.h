
#include "Dengine.h"
#include <iostream>
#include <random>

class FruitBehavior : public Script {
private:
    AudioPlayer& audioplayer;
    RigidBody2D& rigidbody;
    Sprite& sprite;
    Transform2D& transform;
    Collider2D collider;

public:
    FruitBehavior(GameObject& gameObject) : Script(gameObject),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()),
        collider(gameObject.getComponent<Collider2D>()) {}

    FruitBehavior(GameObject& gameObject, FruitBehavior behavior) : Script(gameObject),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()),
        collider(gameObject.getComponent<Collider2D>()) {}

    Component* Clone(GameObject& gameObject) {
        return new FruitBehavior(gameObject, *this);
    }

    void OnCollision(Collision contact) override {
        if (contact.gameObject.tag == "Player") {
            audioplayer["Pick"];
            gameObject.Destroy();
        }
    }
};