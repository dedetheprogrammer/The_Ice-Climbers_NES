#pragma once
#include "EngineECS.h"

class MovementCloud : public Script {
private:

public:

    Sprite& sprite;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform; 

    MovementCloud(GameObject& gameObject) : Script(gameObject), 
        sprite(gameObject.getComponent<Sprite>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
    
    }
    MovementCloud(GameObject& gameObject, MovementCloud& movement) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
     
    }

    Component* Clone(GameObject& gameObject) override {
        return new MovementCloud(gameObject, *this);
    }
    
    void OnCollision(Collision contact) override {}

    void Update() override {
        float deltaTime = GetFrameTime();
        // Horizontal movement:

        if (transform.position.x > GetScreenWidth()) {
            transform.position.x = sprite.GetViewDimensions().x * -1;    
        }
        
        rigidbody.velocity.x = abs(rigidbody.max_velocity.x);
        transform.position.x += rigidbody.velocity.x * deltaTime;

    }

};