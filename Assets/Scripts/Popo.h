#pragma once
#include "EngineECS.h"
#include "raylib.h"
#include "raylibx.h"

class PopoBehavior : public Script {
private:

    // Variables para Popo:
    bool hasBounced;
    bool isJumping;
    bool onCloud;
    Vector2 last_save_position;
    float momentum;
public:
    bool isBraking;
    int lifes;
    bool brokeBlock;
    bool isAttacking; // Telling us if the object is attacking.
    bool isRight;     // Telling us if the object is facing to the right.
    bool isStunned;
    Vector2 collider_size;
    Vector2 collider_offset;
    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más 
    // eficiente que acceder una y otra vez a los componentes cada vez que 
    // necesitamos hacer algo con uno de ellos.
    Animator& animator;
    AudioPlayer& audioplayer;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    bool isGrounded;  // Telling us if the object is on the ground.
    Transform2D& transform;

    PopoBehavior(GameObject& gameObject) : Script(gameObject), 
        animator(gameObject.getComponent<Animator>()),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        lifes = 3;
        isGrounded  = false;
        isJumping   = false;
        isRight     = true;
        isAttacking = false;
        hasBounced  = false;
        onCloud = false;
        isStunned = false;
        brokeBlock = false;
        collider_size = collider.size;
        collider_offset = collider.offset;
        isBraking = false;
    }
    
    PopoBehavior(GameObject& gameObject, PopoBehavior& behavior) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        lifes       = behavior.lifes;
        isJumping   = behavior.isJumping;
        hasBounced  = behavior.hasBounced;
        isGrounded  = behavior.isGrounded;
        isRight     = behavior.isRight;
        isAttacking = behavior.isAttacking;
        onCloud     = behavior.onCloud;
        isStunned   = behavior.isStunned;
        brokeBlock  = behavior.brokeBlock;
        collider_size = behavior.collider_size;
        collider_offset = behavior.collider_offset;
        isBraking = behavior.isBraking;
    }

    Component* Clone(GameObject& gameObject) override {
        return new PopoBehavior(gameObject, *this);
    }

    void OnCollision(Collision contact) {
        float deltaTime = GetFrameTime();
        int move = GetAxis("Horizontal");
        if (contact.gameObject.tag == "Floor") {
            if (contact.contact_normal.y != 0) {
                if (contact.contact_normal.y < 0) {
                    if (isJumping || isBraking) {
                        isBraking = true;
                        if (rigidbody.velocity.x > 0) {
                            rigidbody.velocity.x -= rigidbody.acceleration.x * deltaTime;
                            if (rigidbody.velocity.x < 0) {
                                rigidbody.velocity.x = 0;
                            }
                            animator["Brake"];
                        } else if (rigidbody.velocity.x < 0) {
                            rigidbody.velocity.x += rigidbody.acceleration.x * deltaTime;
                            if (rigidbody.velocity.x > 0) {
                                rigidbody.velocity.x = 0;
                            }
                            animator["Brake"];
                        } else {
                            isBraking = false;
                        }
                    } else {
                        if (!isStunned && !isAttacking && !move) {
                            rigidbody.velocity.x = 0;
                            animator["Idle"];
                        }
                    }
                    isJumping  = false;
                    hasBounced = false;
                    isGrounded = true;
                    brokeBlock = false;
                    last_save_position = transform.position;
                } else {
                    animator["Fall"];
                }
                rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
            }
            if (contact.contact_normal.x != 0 && ((transform.position.y + animator.GetViewDimensions().y) > contact.gameObject.getComponent<Transform2D>().position.y) //&&
                //(transform.position.y < (contact.gameObject.getComponent<Transform2D>().position.y + contact.gameObject.getComponent<Collider2D>().size.y))) {
            ){
                rigidbody.velocity.x += contact.contact_normal.x * std::abs(rigidbody.velocity.x) * (1 - contact.contact_time) * 1.05;
                if (!isGrounded) {
                    hasBounced = true;
                    rigidbody.velocity.x *= -1;
                    if ((rigidbody.velocity.x > 0 && !isRight) || (rigidbody.velocity.x < 0 && isRight)) {
                        isRight = !isRight;
                        animator.Flip();
                    }
                }
            }
        } else {
            if (isGrounded) {
                isGrounded = false;
                animator["Fall"];
            }
        }

        if (contact.gameObject.tag == "Wall") {
            rigidbody.velocity += contact.contact_normal * abs(rigidbody.velocity) * (1 - contact.contact_time) * 1.05;
        }

        if (contact.gameObject.tag == "Cloud") {
            if (contact.contact_normal.y < 0) {
                isGrounded = true;
                isJumping  = false;
                onCloud = true;
                if (!move) {
                    animator["Idle"];
                } else {
                    animator["Walk"];
                    if ((move > 0 && !isRight) || (move < 0 && isRight)) {
                        isRight = !isRight;
                        animator.Flip();
                    }
                }
                rigidbody.velocity.x = (move * rigidbody.acceleration.x + contact.gameObject.getComponent<RigidBody2D>().velocity.x);
            }
            if (contact.contact_normal.x != 0) {
                rigidbody.velocity.x = (/*move * rigidbody.acceleration.x +*/ contact.gameObject.getComponent<RigidBody2D>().velocity.x);
                if ((rigidbody.velocity.x > 0 && !isRight) || (rigidbody.velocity.x < 0 && isRight)) {
                    isRight = !isRight;
                    animator.Flip();
                }
            }
            rigidbody.velocity += contact.contact_normal * abs(rigidbody.velocity) * (1 - contact.contact_time) * 1.05;
        } else {
            onCloud = false;
        }
        if (contact.gameObject.tag == "Enemy") {
            if (!isStunned) {
                if (!isAttacking) {
                    std::cout << "Hola\n";
                    lifes--;
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                } else {
                    if (contact.gameObject.getComponent<RigidBody2D>().velocity.x < 0 && !isRight) {
                        lifes--;
                        animator["Stunned"];
                        isStunned = true;
                        rigidbody.velocity.x = 0;
                    } else if (contact.gameObject.getComponent<RigidBody2D>().velocity.x < 0 && !isRight) {
                        lifes--;
                        animator["Stunned"];
                        isStunned = true;
                        rigidbody.velocity.x = 0;
                    }
                } 
            }
        }

        if (contact.gameObject.tag == "Goal") {
            if (contact.contact_normal.y > 0) {
                std::cout << "VICTORY!";
                exit(0);
            }
        }
    }

    void Update() override {
        // Auxiliar variables:
        int move = 0;                     // Horizontal move sense.
        float deltaTime = GetFrameTime(); // Delta time

        if (!isAttacking) {
            // Horizontal movement:
            move = GetAxis("Horizontal");
            transform.position.x += rigidbody.velocity.x * deltaTime;
            if (!hasBounced && !onCloud) {
                if (move) {
                    if (!isJumping) {
                        isBraking = false;
                        isStunned = false;
                        rigidbody.velocity.x = move * rigidbody.acceleration.x;
                        momentum += move * rigidbody.acceleration.x * deltaTime;
                        if (momentum < -rigidbody.max_velocity.x) {
                            momentum = -rigidbody.max_velocity.x;
                        } else if (momentum > rigidbody.max_velocity.x) {
                            momentum = rigidbody.max_velocity.x;
                        }
                        if (isGrounded) {
                            isJumping = false;
                            animator["Walk"];
                            //collider.size = animator.GetViewDimensions();
                        }
                    } else {
                        rigidbody.velocity.x += move * rigidbody.acceleration.x/16 * deltaTime;
                        if (rigidbody.velocity.x < -rigidbody.max_velocity.x) {
                            rigidbody.velocity.x = -rigidbody.max_velocity.x;
                        } else if (rigidbody.velocity.x > rigidbody.max_velocity.x) {
                            rigidbody.velocity.x = rigidbody.max_velocity.x;
                        }
                    }
                } else {
                    if (!isJumping) {
                        momentum = 0;
                    }
                }
                if ((move > 0 && !isRight) || (move < 0 && isRight)) {
                    isRight = !isRight;
                    animator.Flip();
                }
            }
            if (transform.position.x > GetScreenWidth()) {
                transform.position.x = -animator.GetViewDimensions().x;
            } else if (transform.position.x + animator.GetViewDimensions().x < 0) {
                transform.position.x = GetScreenWidth();
            }


            // Vertical movement:
            if (!isJumping && isGrounded) {
                if (IsKeyDown(KEY_SPACE)) {
                    isGrounded = false;
                    isJumping  = true;
                    rigidbody.velocity.x += momentum;
                    if (rigidbody.velocity.x > rigidbody.max_velocity.x) {
                        rigidbody.velocity.x = rigidbody.max_velocity.x;
                    } else if (rigidbody.velocity.x < -rigidbody.max_velocity.x) {
                        rigidbody.velocity.x = -rigidbody.max_velocity.x;
                    }

                    rigidbody.velocity.y = -rigidbody.acceleration.y;
                    animator["Jump"];
                    audioplayer["Jump"];
                    //collider.size = animator.GetViewDimensions();
                } else if (IsKeyDown(KEY_E)) {
                    isAttacking = true;
                    //transform.position.y -= 3;
                    rigidbody.velocity.x = 0;
                    animator["Attack"];
                    //collider.size.x = animator.GetViewDimensions().x;
                    collider.size.x = animator.GetViewDimensions().x;
                    collider.offset = {0,0};
                }
            }
        } else if (animator.HasFinished("Attack")) {
            isAttacking = false;
            animator["Idle"];
            collider.size = collider_size;
            collider.offset = collider_offset;
        }

        // Colissions:
        transform.position.y += rigidbody.velocity.y * deltaTime;
        rigidbody.velocity.y += rigidbody.gravity * deltaTime;
        if (transform.position.y > GetScreenHeight()) {
            lifes--;
            isJumping = false;
            isGrounded = true;
            rigidbody.velocity = {0,0};
            transform.position = last_save_position;
            transform.position.y -= 150;
            animator["Fall"];
        }
    }

};

