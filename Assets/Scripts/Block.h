#pragma once
#include "EngineECS.h"
#include "raylib.h"

class BlockActions : public Script {
private:
    // Variables para Popo:
    bool isGrounded;  // Telling us if the object is on the ground.
    bool isRight;     // Telling us if the object is facing to the right.
    bool isAttacking; // Telling us if the object is attacking.

    void Move() {
        
    }

    void Draw() {
        animator.Play(transform.position, GetFrameTime());
        //collider.Draw(RED);
    }

public:

    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más 
    // eficiente que acceder una y otra vez a los componentes cada vez que 
    // necesitamos hacer algo con uno de ellos.
    Animator& animator;
    Collider2D& collider;
    Transform2D& transform;
    std::vector<GameObject*>& gameObjects; 

    BlockActions(GameObject& gameObject) : Script(gameObject), 
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        transform(gameObject.getComponent<Transform2D>()),
        gameObjects(gameObject.gameObjects)
    {
        isGrounded  = false;
        isRight     = true;
        isAttacking = false;
    }

    void Update() override {
        Move();
        Draw();
    }

};
