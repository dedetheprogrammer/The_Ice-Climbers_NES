#pragma once
#include "EngineECS.h"
#include "raylib.h"
//#include "collider.h"

class MovementTopi : public Script {
private:
    // Variables para Popo:
    bool isGrounded;  // Telling us if the object is on the ground.
    int move;     // Telling us if the object is facing to the right.

    void Move() {
        float deltaTime = GetFrameTime();
        // Horizontal movement:
        auto dims = animator.GetDimensions();
        if ((transform.position.x + animator.GetViewDimensions().x) < 0 || transform.position.x > GetScreenWidth()) {
            animator.Flip();
            move *= -1;
            if(animator.InState("Stunned")) {
                animator["Walk"];
            }
        }
        rigidbody.velocity.x = move * rigidbody.acceleration.x;
        transform.position.x += rigidbody.velocity.x * deltaTime;
        //hitbox.Move(position);
    }

    void Draw() {
        animator.Play(transform.position, GetFrameTime());
        collider.Draw();
        rigidbody.Draw(transform.position + animator.GetViewDimensions());
    }

public:

    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más 
    // eficiente que acceder una y otra vez a los componentes cada vez que 
    // necesitamos hacer algo con uno de ellos.
    Animator& animator;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;
    std::vector<Collider2D*>& colliders;


    MovementTopi(GameObject& gameObject) : Script(gameObject), 
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>()),
        colliders(gameObject.colliders)
    {
        isGrounded  = false;
        move        = 1;
    }

    void Update() override {
        Move();
        Draw();
    }

};
