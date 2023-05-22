#include "Player.h"

// ============================================================================
HoleBehavior::HoleBehavior(GameObject& gameObject) : Script(gameObject),
    transform(gameObject.getComponent<Transform2D>()) {}

HoleBehavior::HoleBehavior(GameObject& gameObject, HoleBehavior& behavior) : Script(gameObject),
    transform(gameObject.getComponent<Transform2D>()),
    original_block(behavior.original_block) {}

Component* HoleBehavior::Clone(GameObject& gameObject) {
    return new HoleBehavior(gameObject, *this);
}

// ============================================================================
BlockBehavior::BlockBehavior(GameObject& gameObject) : Script(gameObject),
    sprite(gameObject.getComponent<Sprite>()),
    transform(gameObject.getComponent<Transform2D>()) {}

BlockBehavior::BlockBehavior(GameObject& gameObject, BlockBehavior& behavior) : Script(gameObject),
    sprite(gameObject.getComponent<Sprite>()),
    transform(gameObject.getComponent<Transform2D>()),
    hole(behavior.hole) {}

Component* BlockBehavior::Clone(GameObject& gameObject) {
    return new BlockBehavior(gameObject, *this);
}

void BlockBehavior::OnCollision(Collision contact) {
    if (contact.gameObject.tag == "Player") {
        if (contact.contact_normal.y < 0 && !contact.gameObject.getComponent<Script, Player>().brokeBlock) {
            contact.gameObject.getComponent<Script, Player>().brokeBlock = true;
            contact.gameObject.getComponent<RigidBody2D>().velocity.y = 0;
            GameSystem::Instantiate(*hole, GameObjectOptions{.position = transform.position});
            if (current_blocks != nullptr) {
                (*current_blocks)--;
            }
            gameObject.Destroy();
        }
    }
}

// ============================================================================
SlidingBlockBehavior::SlidingBlockBehavior(GameObject& gameObject) : Script(gameObject),
    sprite(gameObject.getComponent<Sprite>()),
    transform(gameObject.getComponent<Transform2D>()),
    rigidbody(gameObject.getComponent<RigidBody2D>()) {}

SlidingBlockBehavior::SlidingBlockBehavior(GameObject& gameObject, SlidingBlockBehavior& behavior) : Script(gameObject),
    sprite(gameObject.getComponent<Sprite>()),
    transform(gameObject.getComponent<Transform2D>()),
    rigidbody(gameObject.getComponent<RigidBody2D>()),
    hole(behavior.hole) {}

Component* SlidingBlockBehavior::Clone(GameObject& gameObject) {
    return new SlidingBlockBehavior(gameObject, *this);
}

// ============================================================================
Player::Player(GameObject& gameObject, Controller& c) : Script(gameObject),
    animator(gameObject.getComponent<Animator>()),
    audioplayer(gameObject.getComponent<AudioPlayer>()),
    collider(gameObject.getComponent<Collider2D>()),
    rigidbody(gameObject.getComponent<RigidBody2D>()),
    transform(gameObject.getComponent<Transform2D>()),
    controller(c)
{
    lifes = 3;
    isGrounded  = false;
    isJumping   = false;
    isRight     = true;
    isAttacking = false;
    isCrouched  = false;
    hasBounced  = false;
    onCloud = false;
    isStunned = false;
    brokeBlock = false;
    collider_size = collider.size;
    collider_offset = collider.offset;
    last_collider_pos = *collider.pos;
    isBraking = false;
    last_tag = "";
    hasCollisioned = false;
    victory = false;
    puntuacion = false;
    bloquesDestruidos = 0;
    frutasRecogidas = 0;
    bonusLevel = false;
}

Player::Player(GameObject& gameObject, Player& behavior) : Script(gameObject),
    animator(gameObject.getComponent<Animator>()),
    audioplayer(gameObject.getComponent<AudioPlayer>()),
    collider(gameObject.getComponent<Collider2D>()),
    rigidbody(gameObject.getComponent<RigidBody2D>()),
    transform(gameObject.getComponent<Transform2D>()),
    controller(behavior.controller)
{
    lifes       = behavior.lifes;
    isJumping   = behavior.isJumping;
    hasBounced  = behavior.hasBounced;
    isGrounded  = behavior.isGrounded;
    isRight     = behavior.isRight;
    isAttacking = behavior.isAttacking;
    isCrouched  = behavior.isCrouched;
    onCloud     = behavior.onCloud;
    isStunned   = behavior.isStunned;
    brokeBlock  = behavior.brokeBlock;
    collider_size = behavior.collider_size;
    collider_offset = behavior.collider_offset;
    last_collider_pos = behavior.last_collider_pos;
    isBraking = behavior.isBraking;
    last_tag = behavior.last_tag;
    hasCollisioned = behavior.hasCollisioned;
    victory     = behavior.victory;
    puntuacion = false;
    bloquesDestruidos = 0;
    frutasRecogidas = 0;
    bonusLevel = false;
}

Component* Player::Clone(GameObject& gameObject) {
    return new Player(gameObject, *this);
}

void Player::OnCollision(Collision contact) {
    float deltaTime = GetFrameTime();
    //int move = GetAxis("Horizontal");
    int move = 0;
    if (controller.isDown(Controller::LEFT)) move -= 1;
    if (controller.isDown(Controller::RIGHT)) move += 1;

    if (contact.gameObject.tag == "Floor") {
        if (contact.contact_normal.y != 0) {
            if (contact.contact_normal.y < 0) {

                if (contact.gameObject.hasSecondTag("Block")) {
                    floor_level = contact.gameObject.getComponent<Script, BlockBehavior>().floor_level;
                } else {
                    floor_level = contact.gameObject.getComponent<Script, FloorBehavior>().floor_level;
                }

                if (isJumping || isBraking) {
                    if(contact.gameObject.name == "Bonus 0" || contact.gameObject.name == "Bonus 1")
                        bonusLevel = true;
                    isBraking = true;
                    if (rigidbody.velocity.x > 0) {
                        rigidbody.velocity.x -= rigidbody.acceleration.x * deltaTime;
                        momentum -= rigidbody.acceleration.x * deltaTime;
                        if (rigidbody.velocity.x < 0) {
                            rigidbody.velocity.x = 0;
                        }
                        if (momentum < 0) {
                            momentum = 0;
                        }
                        animator["Brake"];
                    } else if (rigidbody.velocity.x < 0) {
                        rigidbody.velocity.x += rigidbody.acceleration.x * deltaTime;
                        momentum += rigidbody.acceleration.x * deltaTime;
                        if (rigidbody.velocity.x > 0) {
                            rigidbody.velocity.x = 0;
                        }
                        if (momentum > 0) {
                            momentum = 0;
                        }
                        animator["Brake"];
                    } else {
                        isBraking = false;
                    }
                } else {
                    if (!isStunned && !isAttacking && !move && !isCrouched) {
                        rigidbody.velocity.x = 0;
                        animator["Idle"];
                        collider.size = collider_size;
                        collider.offset = collider_offset;
                    } else if(!isStunned && !isAttacking && !move && isCrouched && !animator.InState("Crouch")) {
                        animator["Crouch"];
                        collider.size = animator.GetViewDimensions();
                        collider.size.y -= 45;
                        collider.offset.y += 45;
                    }
                }
                isJumping  = false;
                hasBounced = false;
                isGrounded = true;
                brokeBlock = false;
                last_save_position = transform.position;
                rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
            } else{
                if(contact.gameObject.name[0] != 'L') bloquesDestruidos += 1;
                animator["Fall"];
                rigidbody.velocity.y *= -0.7;
            }
        }
        if (contact.contact_normal.x != 0/* && ((transform.position.y + animator.GetViewDimensions().y) > contact.gameObject.getComponent<Transform2D>().position.y) &&
            (transform.position.y < (contact.gameObject.getComponent<Transform2D>().position.y + contact.gameObject.getComponent<Collider2D>().size.y))*/)
        {
            rigidbody.velocity.x += contact.contact_normal.x * std::abs(rigidbody.velocity.x) * (1 - contact.contact_time) * 1.02;
            if (!isGrounded) {
                hasBounced = true;
                rigidbody.velocity.x *= -1;
                if ((rigidbody.velocity.x > 0 && !isRight) || (rigidbody.velocity.x < 0 && isRight)) {
                    isRight = !isRight;
                    animator.Flip();
                }
            }
        }
    }

    if (contact.gameObject.tag == "Hole") {
        if (contact.contact_normal.y < 0 && !isGrounded) {
            animator["Fall"];
            isGrounded = false;
        }
    }
    
    if (contact.gameObject.tag == "Wall") {
        rigidbody.velocity += contact.contact_normal * abs(rigidbody.velocity) * (1 - contact.contact_time) * 1.05;
        if (contact.contact_normal.y != 0) {
            if(!isGrounded && !hasBounced) {
                hasBounced = true;
                rigidbody.velocity.x *= -1;
                if ((rigidbody.velocity.x > 0 && !isRight) || (rigidbody.velocity.x < 0 && isRight)) {
                    isRight = !isRight;
                    animator.Flip();
                }
            }
        }
    }

    if (contact.gameObject.tag == "SlidingFloor") {
        if (contact.contact_normal.y < 0) {
            isGrounded = true;
            brokeBlock = false;
            isJumping  = false;
            onCloud = false;
            floor_level = contact.gameObject.getComponent<Script, SlidingBlockBehavior>().floor_level;

            if (!move) {
                animator["Idle"];
                collider.size = collider_size;
                collider.offset = collider_offset;
            } else {
                animator["Walk"];
                if ((move > 0 && !isRight) || (move < 0 && isRight)) {
                    isRight = !isRight;
                    animator.Flip();
                }
            }
            rigidbody.velocity.x = (move * rigidbody.acceleration.x + contact.gameObject.getComponent<RigidBody2D>().max_velocity.x);
        }
        if (contact.contact_normal.x != 0) {
            rigidbody.velocity.x = (/*move * rigidbody.acceleration.x +*/ contact.gameObject.getComponent<RigidBody2D>().max_velocity.x);
            if ((rigidbody.velocity.x > 0 && !isRight) || (rigidbody.velocity.x < 0 && isRight)) {
                isRight = !isRight;
                animator.Flip();
            }
        }
        rigidbody.velocity += contact.contact_normal * abs(rigidbody.velocity) * (1 - contact.contact_time) * 1.05;
    }

    if (contact.gameObject.tag == "Cloud") {
        floor_level = contact.gameObject.getComponent<Script, CloudBehavior>().floor_level;
        if (contact.contact_normal.y < 0) {
            isGrounded = true;
            brokeBlock = false;
            isJumping  = false;
            onCloud = false;//true;
            if (!move) {
                animator["Idle"];
                collider.size = collider_size;
                collider.offset = collider_offset;
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

    if ((contact.gameObject.tag == "Enemy" && !contact.gameObject.getComponent<Animator>().InState("Stunned"))) {
        if (!isStunned) {
            if (!isAttacking && !(contact.gameObject.name == "Nutpicker" && !isGrounded && contact.contact_normal.y)) {
                lifes--;
                animator["Stunned"];
                isStunned = true;
                rigidbody.velocity.x = 0;
            }else if(contact.contact_normal.x){
                if (contact.gameObject.getComponent<RigidBody2D>().velocity.x < 0 && !isRight) {
                    lifes--;
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                } else if (contact.gameObject.getComponent<RigidBody2D>().velocity.x > 0 && isRight) {
                    lifes--;
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                }
            }
        }
    }

    if (contact.gameObject.tag == "Player") {
        if (!isStunned) {
            if (!isAttacking && !(contact.gameObject.name == "Nutpicker" && !isGrounded && contact.contact_normal.y)) {
                lifes--;
                animator["Stunned"];
                isStunned = true;
                rigidbody.velocity.x = 0;
            }else if(contact.contact_normal.x){
                if (contact.gameObject.getComponent<RigidBody2D>().velocity.x < 0 && !isRight) {
                    lifes--;
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                } else if (contact.gameObject.getComponent<RigidBody2D>().velocity.x > 0 && isRight) {
                    lifes--;
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                }
            }
        }
    }

    if (contact.gameObject.tag == "Icicle") {
        if (!isStunned) {
            if (!isAttacking) {
                lifes--;
                animator["Stunned"];
                isStunned = true;
                rigidbody.velocity.x = 0;
            } else if(isAttacking && contact.contact_normal.x){
                if (contact.gameObject.getComponent<RigidBody2D>().velocity.x < 0 && !isRight) {
                    lifes--;
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                } else if (contact.gameObject.getComponent<RigidBody2D>().velocity.x > 0 && isRight) {
                    lifes--;
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                } else{
                    contact.gameObject.Destroy();
                }
            }
        }
    }

    if (contact.gameObject.tag == "Goal") {
        if (floor_level == last_level) {
            if (contact.contact_normal.y > 0) {
                rigidbody.velocity = {0,0};
                rigidbody.acceleration = {0,0};
                rigidbody.gravity = 0;
                victory = true;
            }
        }
    }

    if (contact.gameObject.tag == "Fruit") {
        frutasRecogidas += 1;
    }

    last_tag = contact.gameObject.tag;
    hasCollisioned = true;
}

void Player::Update() {
    // Auxiliar variables:
    int move = 0;                     // Horizontal move sense.
    float deltaTime = GetFrameTime(); // Delta time

    if (controller.isDown(Controller::DOWN)) {
        if(!isCrouched && isGrounded)
            isCrouched = true;
    } else if(isCrouched) {
        isCrouched = false;
    }

    if (!isAttacking && !isCrouched) {
        if (controller.isDown(Controller::LEFT)) move -= 1;
        if (controller.isDown(Controller::RIGHT)) move += 1;

        transform.position.x += rigidbody.velocity.x * deltaTime;
        if (!hasBounced && !onCloud) {
            if (move) {
                if (!isJumping) {
                    isBraking = false;
                    isStunned = false;
                    rigidbody.velocity.x = move * rigidbody.acceleration.x;
                    momentum += move * rigidbody.acceleration.x/1.5 * deltaTime;
                    if (momentum < -rigidbody.max_velocity.x) {
                        momentum = -rigidbody.max_velocity.x;
                    } else if (momentum > rigidbody.max_velocity.x) {
                        momentum = rigidbody.max_velocity.x;
                    }
                    if (isGrounded) {
                        isJumping = false;
                        animator["Walk"];
                    }
                } else {
                    rigidbody.velocity.x += move * rigidbody.acceleration.x/4 * deltaTime;
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
            if (controller.isDown(Controller::JUMP)) {
                isGrounded = false;
                isJumping  = true;
                rigidbody.velocity.x = momentum;
                if (rigidbody.velocity.x > rigidbody.max_velocity.x) {
                    rigidbody.velocity.x = rigidbody.max_velocity.x;
                } else if (rigidbody.velocity.x < -rigidbody.max_velocity.x) {
                    rigidbody.velocity.x = -rigidbody.max_velocity.x;
                }

                rigidbody.velocity.y = -rigidbody.acceleration.y;
                animator["Jump"];
                audioplayer["Jump"];
            } else if (controller.isDown(Controller::ATTACK) && !bonusLevel) {
                isAttacking = true;
                rigidbody.velocity.x = 0;
                animator["Attack"];
                collider.size.x = animator.GetViewDimensions().x;
                collider.offset = {0,0};
            }
        }
    } else if (animator.HasFinished("Attack")) {
        isAttacking = false;
        if(!isCrouched) {
            animator["Idle"];
            collider.size = collider_size;
            collider.offset = collider_offset;
        } else if(!animator.InState("Crouch")) {
            animator["Crouch"];
            collider.size.y -= 45;
            collider.offset.y += 45;
        }

    }

    // Colissions:
    transform.position.y += rigidbody.velocity.y * deltaTime;
    rigidbody.velocity.y += rigidbody.gravity * deltaTime;
    if (transform.position.y > GetScreenHeight() && !victory) {
        lifes--;
        isJumping = false;
        isGrounded = true;
        rigidbody.velocity = {0,0};
        transform.position = last_save_position;
        transform.position.y -= 150;
        animator["Fall"];
    }

    if (hasCollisioned) {
        hasCollisioned = false;
    } else {
        if (last_tag == "Floor") {
            if (isGrounded) {
                isGrounded = false;
                animator["Fall"];
                rigidbody.velocity.x = momentum;
                if (rigidbody.velocity.x > rigidbody.max_velocity.x) {
                    rigidbody.velocity.x = rigidbody.max_velocity.x;
                } else if (rigidbody.velocity.x < -rigidbody.max_velocity.x) {
                    rigidbody.velocity.x = -rigidbody.max_velocity.x;
                }
                isJumping = true;
            }
        }
    }
}