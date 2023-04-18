#pragma once
#include "EngineECS.h"
//#include "raylib.h"
//#include "collider.h"

class MovementTopi : public Script {
private:
    // Variables para Popo:
    bool isGrounded;  // Telling us if the object is on the ground.
    bool isRunning;
    int move;     // Telling us if the objsdect is facing to the right.
    Vector2 initialPos;

    void Draw() {
        animator.Play();
        collider.Draw();
        rigidbody.Draw();
    }

public:

    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más 
    // eficiente que acceder una y otra vez a los componentes cada vez que 
    // necesitamos hacer algo con uno de ellos.
    Animator& animator;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;


    MovementTopi(GameObject& gameObject) : Script(gameObject), 
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        isGrounded  = false;
        isRunning   = false;
        move        = 1;
        initialPos  = {0,0};
    }
    MovementTopi(GameObject& gameObject, MovementTopi& movement) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        isGrounded  = movement.isGrounded;
        move        = movement.move;
        isRunning   = movement.isRunning;
        initialPos  = movement.initialPos;
    }

    Component* Clone(GameObject& gameObject) override {
        return new MovementTopi(gameObject, *this);
    }

    void OnCollision(Collision contact) override {
        if (contact.gameObject.tag != "Floor") std::cout << "Soy Topi, me choco con: " << contact.gameObject.tag << std::endl;

        if (contact.gameObject.tag == "Floor") {
            if (contact.contact_normal.y < 0) {
                if(!contact.gameObject.getComponent<Collider2D>().active) {
                    if(isRunning) {
                        animator["Stunned"];
                    } else {
                        rigidbody.max_velocity.x *= 2;
                        isRunning = true;
                        animator.Flip();
                        move *= -1;
                    }
                } else {
                    rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
                }
            }
        } else if (contact.gameObject.tag == "Player") { // Se ocupa el player
            
        }
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        // Horizontal movement:
        auto dims = animator.GetViewDimensions();
        if ((transform.position.x + dims.x) < 0 || transform.position.x > GetScreenWidth()) {
            if(isRunning) {
                isRunning = false;
                collider.active = true;
                rigidbody.max_velocity.x /= 2;
            }
            animator.Flip();
            move *= -1;
            
            if(animator.InState("Stunned")) {
                transform.position = initialPos;
                animator["Walk"];
            } else {
                initialPos = transform.position;
            }
        }
        
        rigidbody.max_velocity.x = abs(rigidbody.max_velocity.x);
        rigidbody.max_velocity.x *= move;
        transform.position.x += rigidbody.max_velocity.x * deltaTime;

        
        transform.position.y += rigidbody.velocity.y * deltaTime;
        rigidbody.velocity.y += rigidbody.gravity * deltaTime;
    }

    void Flip() {
        move *= -1;
    }

};
