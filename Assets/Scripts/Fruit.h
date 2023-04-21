
#include "EngineECS.h"
#include <iostream>
#include <random>

class FruitBehavior : public Script {
private:

    RigidBody2D& rigidbody;
    Sprite& sprite;
    Transform2D& transform;
    Collider2D collider;


public:
    FruitBehavior(GameObject& gameObject) : Script(gameObject),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        collider(gameObject.getComponent<Collider2D>()),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        
    }

    FruitBehavior(GameObject& gameObject, FruitBehavior behavior) : Script(gameObject),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        collider(gameObject.getComponent<Collider2D>()),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>())
    {
 
    }

    Component* Clone(GameObject& gameObject) {
        return new FruitBehavior(gameObject, *this);
    }

    void Update() override {
       
    }

    void OnCollision(Collision contact) override {
        if (contact.gameObject.tag == "Player") {
            gameObject.Destroy();
        }
    }
};