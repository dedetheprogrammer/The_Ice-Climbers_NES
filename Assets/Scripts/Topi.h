#pragma once
#include "EngineECS.h"
#include "Popo.h"
//#include "raylib.h"
//#include "collider.h"

class MovementTopi : public Script {
private:
    // Variables para Popo:
    bool isGrounded;  // Telling us if the object is on the ground.
    bool isRunning;
    int move;     // Telling us if the objsdect is facing to the right.
    bool isOut;
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
        rigidbody.velocity.x = rigidbody.max_velocity.x;
        collider.active = false;
        isOut = false;
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
        rigidbody.velocity.x = rigidbody.max_velocity.x;
        collider.active = false;
        isOut       = false;
    }

    Component* Clone(GameObject& gameObject) override {
        return new MovementTopi(gameObject, *this);
    }

    void OnCollision(Collision contact) override {
        if (contact.gameObject.tag != "Floor" && contact.gameObject.tag != "Block") std::cout << "Soy Topi, me choco con: " << contact.gameObject.tag << std::endl;

        if (contact.gameObject.tag == "Floor" || contact.gameObject.tag == "Block") {
            if (contact.contact_normal.y < 0) {
                if(!contact.gameObject.getComponent<Collider2D>().active) {
                    if(isRunning && !animator.InState("Stunned")) {
                        std::cout << "Cae" << std::endl;
                        animator["Stunned"];
                    } else if(!isRunning && !animator.InState("Stunned")) {
                        std::cout << "Corre" << std::endl;
                        Flip();
                        rigidbody.velocity.x *= 2;
                        isRunning = true;
                    }
                } else {
                    rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
                }
            }
        } else if (contact.gameObject.tag == "Player") { // Se ocupa el player
            /*auto animatorPlayer = contact.gameObject.getComponent<Animator>();
            auto scriptPlayer = contact.gameObject.getComponent<Script, Movement>();
            auto isRight = scriptPlayer.getIsRight();
            if (!animatorPlayer.InState("Attack") && !animatorPlayer.InState("Stunned")) {
                std::cout << "\tNo está atacando lo pongo en estado de stunned" << std::endl;
                animatorPlayer["Stunned"];
                collider.size = animator.GetViewDimensions();
                scriptPlayer.setStunned(true);
                rigidbody.velocity.x = 0;
            }else if(!animator.InState("Stunned")){
                if ((contact.contact_normal.x < 0 && !isRight) || (contact.contact_normal.x > 0 && isRight)){
                    std::cout << "\tAtaco pero me ha chocado por atras =D " << std::endl;
                    animator["Stunned"];
                    scriptPlayer.setStunned(true);
                }else {
                    std::cout << "\tSe estunea el enemigo" << std::endl;
                    animator["Stunned"];
                    Flip();
                }
            }*/
        }
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        // Horizontal movement:
        auto dims = animator.GetViewDimensions();
        if ((transform.position.x + dims.x) < 0 || transform.position.x > GetScreenWidth()) {
            std::cout << "Sale" << std::endl;
            if(isOut) {
                std::cout << "Estado Fuera" << std::endl;
                isOut = false;
                Flip();
                if(isRunning) {
                    isRunning = false;
                    collider.active = true;
                    rigidbody.velocity.x /= 2;
                }
                
                if(animator.InState("Stunned")) {
                    transform.position = initialPos;
                    animator["Walk"];
                } else {
                    initialPos = transform.position;
                }
            }
        } else {
            std::cout << "Vuelvo a la pantalla" << std::endl;
            isOut = true;
        }
        
        //rigidbody.velocity.x = abs(rigidbody.max_velocity.x);
        //if(rigidbody.velocity.x > 0) rigidbody.velocity.x *= move;
        transform.position.x += rigidbody.velocity.x * deltaTime;

        
        transform.position.y += rigidbody.velocity.y * deltaTime;
        rigidbody.velocity.y += rigidbody.gravity * deltaTime;
    }

    void Flip() {
        animator.Flip();
        move *= -1;
        rigidbody.velocity.x *= move;
    }
};
