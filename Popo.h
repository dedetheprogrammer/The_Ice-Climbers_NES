#pragma once
#include "EngineECS.h"
#include "raylib.h"
//#include "collider.h"

class Movement : public Script {
private:
    // Variables para Popo:
    bool isGrounded;  // Telling us if the object is on the ground.
    bool isRight;     // Telling us if the object is facing to the right.
    bool isAttacking; // Telling us if the object is attacking.
    bool isFalling;

    void Move() {
        // Auxiliar variables:
        int move = 0;                     // Horizontal move sense.
        float deltaTime = GetFrameTime(); // Delta time
        if (!isAttacking) {
            // Horizontal movement:
            move = GetAxis("Horizontal");
            if (move) {
                rigidbody.velocity.x = move * rigidbody.acceleration.x;
                if (isGrounded) {
                    animator["Walk"];
                    collider.size = animator.GetViewDimensions();
                }
            }
            transform.position.x += rigidbody.velocity.x * deltaTime;
            if (move > 0 && !isRight || move < 0 && isRight) {
                isRight = !isRight;
                animator.Flip();
            }
            if (transform.position.x > GetScreenWidth()) {
                transform.position.x = -animator.GetViewDimensions().x;
            } else if (transform.position.x + animator.GetViewDimensions().x < 0) {
                transform.position.x = GetScreenWidth();
            }

            // Vertical movement:
            if (isGrounded) {
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
        
        auto floor = colliders[0];
        // Colissions:
        float ct; Vector2 cp, cn;
        bool collision;
        for(auto colliderAux : colliders) {
            collision = CollisionSystem.Collides(collider, rigidbody.velocity * deltaTime, *colliderAux, cp, cn, ct) && ct < 1.0f;
            if(collision){
                //std::cout << colliderAux->type << std::endl;
                switch (colliderAux->type){
                    case COLLIDER_ENUM::FLOOR:
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
                                rigidbody.velocity.x = 0;
                                animator["Idle"];
                                collider.size = animator.GetViewDimensions();
                            }
                        }
                        rigidbody.velocity.y += cn.y * std::abs(rigidbody.velocity.y) * (1 - ct);
                        isGrounded = true;
                        break;

                    case COLLIDER_ENUM::PLATFORM:
                        if(cn.y < 0) { //Popo choca encima del bloque
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
                                    rigidbody.velocity.x = 0;
                                    animator["Idle"];
                                    collider.size = animator.GetViewDimensions();
                                }
                            }
                            isGrounded = true;
                            rigidbody.velocity.y += cn.y * std::abs(rigidbody.velocity.y) * (1 - ct);
                        }else if(cn.y > 0) {   //Popo choca debajo del bloque
                            //std::cout << "abajo" << std::endl;
                            rigidbody.velocity.y *= -1;
                        }/*else if(cn.x < 0){      //Popo choca a la izq del bloque
                            //std::cout << "izquierda" << std::endl;
                            rigidbody.velocity.x += cn.x * std::abs(rigidbody.velocity.x) * (1 - ct);
                            //rigidbody.velocity.x *= -1;
                            //transform.position.x = block.position.x - obj.hitbox.size.x - 1;
                        }else if(cn.x > 0){      //Popo choca a la der del bloque
                            //std::cout << "derecha" << std::endl;
                            rigidbody.velocity.x += cn.x * std::abs(rigidbody.velocity.x) * (1 - ct);
                            //rigidbody.velocity.x *= -1;
                            //transform.position.x = block.position.x + block.hitbox.size.x + 1;
                        }*/
                        
                        break;

                    case COLLIDER_ENUM::ENEMY:
                        break;
                }
                isFalling = false;
                break;
            }
        }
        if (!collision) {
            //std::cout << "No Colisiona" << std::endl;
            transform.position.y += rigidbody.velocity.y * deltaTime;
            rigidbody.velocity.y += rigidbody.gravity * deltaTime;
            isGrounded = false;
            if(isFalling) animator["Fall"];
            isFalling = true;
        } /*else {
            //std::cout << "Colisiona" << std::endl;
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
                    rigidbody.velocity.x = 0;
                    animator["Idle"];
                    collider.size = animator.GetViewDimensions();
                }
            }
            rigidbody.velocity.y += cn.y * std::abs(rigidbody.velocity.y) * (1 - ct);
            isGrounded = true;
        }*/
    }

    void Draw() {
        animator.Play(transform.position, GetFrameTime());
        //collider.Draw();
        rigidbody.Draw(transform.position + animator.GetViewDimensions());
    }

public:

    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más 
    // eficiente que acceder una y otra vez a los componentes cada vez que 
    // necesitamos hacer algo con uno de ellos.
    Animator& animator;
    AudioPlayer& audioplayer;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;
    std::vector<Collider2D*>& colliders;

    Movement(GameObject& gameObject) : Script(gameObject), 
        animator(gameObject.getComponent<Animator>()),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>()),
        colliders(gameObject.colliders)
    {
        isGrounded  = false;
        isRight     = true;
        isAttacking = false;
        isFalling = false;
    }

    void Update() override {
        Move();
        Draw();
    }

};
