#pragma once
#include "EngineECS.h"
#include <iostream>
//#include "raylib.h"
//#include "collider.h"

class MovementCone : public Script {
private:
    // Variables para Popo:
    int move;     // Telling us if the objsdect is facing to the right.
    Vector2 initialPos;

    void Draw() {
        collider.Draw();
        sprite.Draw();
    }

public:

    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más 
    // eficiente que acceder una y otra vez a los componentes cada vez que 
    // necesitamos hacer algo con uno de ellos.
    Sprite& sprite;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;


    MovementCone(GameObject& gameObject) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        move        = 1;
        initialPos  = transform.position;
        collider.active = false;
    }
    MovementCone(GameObject& gameObject, MovementCone& movement) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        move        = movement.move;
        initialPos  = movement.initialPos;
        collider.active = false;
    }

    Component* Clone(GameObject& gameObject) override {
        return new MovementCone(gameObject, *this);
    }

    void OnCollision(Collision contact) override {
        if (contact.gameObject.tag != "Floor") std::cout << "Soy ice cone, me choco con: " << contact.gameObject.tag << std::endl;

        // If there is side-to-side contact with the floor, set that floor to active state
        if (contact.gameObject.tag == "Floor") {
            auto floorCollider = contact.gameObject.getComponent<Collider2D>();

            if (contact.contact_normal.x && !floorCollider.active) {
                // Become invisible and inactive and wait for topi to collide
                sprite.ChangeTexture("Assets/Sprites/Ice_cone_invisible.png");
                collider.active = false;
                rigidbody.velocity.x = 0.0;
                floorCollider.active = true;
            }
        // If there is contact with the Topi
        } else if (contact.gameObject.tag == "Enemy") {
            auto topiCollider = contact.gameObject.getComponent<Collider2D>();
            auto topiTransform = contact.gameObject.getComponent<Transform2D>();

            std::cout << "Me choco con el topi " << std::endl;
            std::cout << "Mi collider activo? " << collider.active << std::endl;
            std::cout << "Collider del topi activo? " << topiCollider.active << std::endl;

            if (!collider.active && topiCollider.active && transform.position.x == initialPos.x) {
                std::cout << "Topi starts moving" << std::endl;
                rigidbody.velocity.x = rigidbody.max_velocity.x;

                float displacement = (move == 1)? topiCollider.size.x + 2 : -(collider.size.x + 2);
                transform.position.x = topiTransform.position.x + displacement;
                collider.active = true;
            } else if (!collider.active && transform.position.x != initialPos.x) {
                // Topi contact after having placed the block
                transform.position.x = initialPos.x;
                sprite.ChangeTexture("Assets/Sprites/Ice_cone.png");
                topiCollider.active = false;
            } else {
                std::cout << "No hace nada" << std::endl;
            }

        } else if (contact.gameObject.tag == "Player" && collider.active) { // Se ocupa el player
            
        }
    }

    void Update() override {
        if(collider.active) {
            float deltaTime = GetFrameTime();
            transform.position.x += rigidbody.velocity.x * move * deltaTime;
        }
    }

};
