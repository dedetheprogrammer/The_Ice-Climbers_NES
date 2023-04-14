#pragma once
#include "EngineECS.h"
#include "raylib.h"

class LockedPlatform : public Script {
private:

    void Move() {
        
    }

    //void Draw() {
    //    //animator.Play(transform.position, GetFrameTime());
    //    collider.Draw();
    //}

public:

    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más 
    // eficiente que acceder una y otra vez a los componentes cada vez que 
    // necesitamos hacer algo con uno de ellos.
    Collider2D& collider;

    LockedPlatform(GameObject& gameObject) : Script(gameObject),
        collider(gameObject.getComponent<Collider2D>()) {}

    void Update() override {
        Move();
        //Draw();
    }

};
