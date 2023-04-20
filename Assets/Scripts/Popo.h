#pragma once
#include "EngineECS.h"
//#include "raylib.h"
#include "raylibx.h"
#include "Topi.h"

class Movement : public Script {
private:
    // Variables para Popo:
    bool isGrounded;  // Telling us if the object is on the ground.
    bool isRight;     // Telling us if the object is facing to the right.
    bool isAttacking; // Telling us if the object is attacking.
    bool isStunned;   // Telling us if the object is stunned.
    bool isRebound;   // Telling us if the object is rebounded.

public:

    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más 
    // eficiente que acceder una y otra vez a los componentes cada vez que 
    // necesitamos hacer algo con uno de ellos.
    Animator& animator;
    AudioPlayer& audioplayer;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;

    int deadX, deadY, groundX, groundY, isRightGround;

    Movement(GameObject& gameObject) : Script(gameObject), 
        animator(gameObject.getComponent<Animator>()),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        isGrounded  = false;
        isRight     = true;
        isAttacking = false;
        isStunned = false;
        deadX = 0;
        deadY = 0;
        groundX = 0;
        groundY = 0;
        isRightGround = 0;
        isRebound = false;
    }
    Movement(GameObject& gameObject, Movement& movement) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        isGrounded  = movement.isGrounded;
        isRight     = movement.isRight;
        isAttacking = movement.isAttacking;
        isStunned   = movement.isStunned;
        deadX = 0;
        deadY = 0;
        isRightGround = 0;
        isRebound   = movement.isRebound;
    }

    Component* Clone(GameObject& gameObject) override {
        return new Movement(gameObject, *this);
    }

    void OnCollision(Collision contact) override {
        
        if(contact.gameObject.tag != "Floor") std::cout << "";
        if (contact.gameObject.tag == "Cloud") {
            if (!contact.contact_normal.x) {
                if (contact.contact_normal.y > 0) {
                    rigidbody.velocity.x = contact.gameObject.getComponent<RigidBody2D>().velocity.x;
                } else {
                    rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 2;
                }
            }
        } else if (contact.gameObject.tag == "Floor" || contact.gameObject.tag == "Block") {
            int move = GetAxis("Horizontal");
            float deltaTime = GetFrameTime();
            if (!contact.contact_normal.x && contact.gameObject.getComponent<Collider2D>().active && collider.active) {
                if (contact.contact_normal.y < 0) {
                    if (!isAttacking && !move) {
                        if (rigidbody.velocity.x > 0) {
                            rigidbody.velocity.x -= sgn(rigidbody.velocity.x) * rigidbody.acceleration.x * deltaTime;
                            if (rigidbody.velocity.x < 0) {
                                rigidbody.velocity.x = 0;
                            }
                            animator["Brake"];
                            collider.size = animator.GetViewDimensions();
                        } else if (rigidbody.velocity.x < 0) {
                            rigidbody.velocity.x -= sgn(rigidbody.velocity.x) * rigidbody.acceleration.x * deltaTime;
                            if (rigidbody.velocity.x > 0) {
                                rigidbody.velocity.x = 0;
                            }
                            animator["Brake"];
                            collider.size = animator.GetViewDimensions();
                        } else {
                            if(!isStunned) {
                                rigidbody.velocity.x = 0;
                                animator["Idle"];
                                collider.size = animator.GetViewDimensions();
                            }
                        }
                    }
                    
                    rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
                    isGrounded = true;
                    groundX = transform.position.x;
                    groundY = transform.position.y;
                    isRightGround = isRight;
                    isRebound = false;
                } else if (contact.contact_normal.y > 0) {
                    //rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 2;
                    rigidbody.velocity.y *= -1;
                    animator["Fall"];
                    if (contact.gameObject.tag == "Block") {
                        contact.gameObject.getComponent<Collider2D>().active = false;
                        //sprite.Unload();
                        contact.gameObject.getComponent<Sprite>().ChangeTexture("Assets/Sprites/block_invisible.png");
                    }
                } else {
                    std::cout << "Me he chocado? No me he chocado?" << std::endl;
                }
            }else if(!contact.gameObject.getComponent<Collider2D>().active) {
                isGrounded = false;
            } else if(!isGrounded || animator.InState("Fall")){
                rigidbody.velocity.x *= -1;
                isRebound = true;
                animator.Flip();
            }
        } else if (contact.gameObject.tag == "Enemy") {
            auto animatorEnemy = contact.gameObject.getComponent<Animator>();
            if(!animatorEnemy.InState("Stunned")) {
                if (!isAttacking && !isStunned) {
                    std::cout << "\tNo estoy atacando me pongo en estado de stunned" << std::endl;
                    animator["Stunned"];
                    collider.active = false;
                    deadX = transform.position.x;
                    deadY = transform.position.y;
                    collider.size = animator.GetViewDimensions();
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                }else if(!animator.InState("Stunned")){
                    if ((contact.contact_normal.x < 0 && !isRight) || (contact.contact_normal.x > 0 && isRight)){
                        std::cout << "\tAtaco pero me ha chocado por atras =D " << std::endl;
                        animator["Stunned"];
                        collider.active = false;
                        deadX = transform.position.x;
                        deadY = transform.position.y;
                        collider.size = animator.GetViewDimensions();
                        isStunned = true;
                    }else {
                        std::cout << "\tSe estunea el enemigo" << std::endl;
                        contact.gameObject.getComponent<Animator>()["Stunned"];
                        contact.gameObject.getComponent<Script, MovementTopi>().Flip();
                    }
                }
            }
        }else if (contact.gameObject.tag == "Wall") {
            if (!contact.contact_normal.x) {
                rigidbody.velocity.x *= -1;
            }else{
                rigidbody.velocity.y *= -1;
                animator["Fall"];
            }
        }
    }

    void Update() override {
        // Auxiliar variables:
        int move = 0;                     // Horizontal move sense.
        float deltaTime = GetFrameTime(); // Delta time
        
        // std::cout << "deltatime = " << deltaTime << std::endl;
        if(deltaTime > 0.2){
            deltaTime = 1.0 / GetFPS();   // Fix deltaTime to the period of one frame
        }

        if (!isAttacking) {
            // Horizontal movement:
            move = GetAxis("Horizontal");
            if (move && !isRebound) {
                if(isStunned) isStunned = !isStunned;
                rigidbody.velocity.x = move * rigidbody.acceleration.x;
                if (isGrounded) {
                    animator["Walk"];
                    collider.size = animator.GetViewDimensions();
                }
            }
            transform.position.x += rigidbody.velocity.x * deltaTime;
            if ((move > 0 && !isRight) || (move < 0 && isRight)) {
                isRight = !isRight;
                animator.Flip();
            }
            if (transform.position.x > GetScreenWidth()) {
                transform.position.x = -animator.GetViewDimensions().x;
                
            } else if (transform.position.x + animator.GetViewDimensions().x < 0) {
                transform.position.x = GetScreenWidth();
            }
            if(transform.position.y >GetScreenHeight()){
                if(animator.InState("Stunned")){
                    collider.active = true;
                    transform.position.x = deadX;
                    transform.position.y = deadY-20;
                    animator["Idle"];
                }else{
                    transform.position.x = groundX;
                    transform.position.y = groundY - 20;//+ isRightGround*40;
                    std::cout << "GroundX: " << groundX << " GroundY: " << groundY << std::endl;
                    std::cout << "GroundX: " << transform.position.x << " GroundY: " << transform.position.y << std::endl;
                    rigidbody.velocity.x = 0;
                }
            }

            // Vertical movement:
            if (isGrounded && !isStunned) {
                if (IsKeyDown(KEY_SPACE)) {
                    isGrounded = false;
                    rigidbody.velocity.y = -rigidbody.acceleration.y;
                    animator["Jump"];
                    audioplayer["Jump"];
                    collider.size = animator.GetViewDimensions();
                } else if (IsKeyDown(KEY_E)) {
                    isAttacking = true;
                    transform.position.y -= 3;
                    rigidbody.velocity.x = 0;
                    animator["Attack"];
                    collider.size = animator.GetViewDimensions();
                }
            }
        } else if (animator.HasFinished("Attack")) {
            isAttacking = false;
            animator["Idle"];
            collider.size = animator.GetViewDimensions();
            transform.position.y += 3;
        }
        
        // Colissions:
        transform.position.y += rigidbody.velocity.y * deltaTime;
        rigidbody.velocity.y += rigidbody.gravity * deltaTime;
    }

    bool getIsRight() {
        return isRight;
    }

    void setStunned(bool stunned) {
        isStunned = stunned;
    }
};

