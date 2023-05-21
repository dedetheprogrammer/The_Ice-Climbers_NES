#pragma once
#include "EngineECS.h"
#include "Block.h"
#include "Popo.h"
#include "settings.h"
#include <iostream>
#include <ctime>
#include "random"

class JosephBehavior : public Script {
private:
    int random_sense() {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> E(0, 1);
        return (E(e2) < 0.5) ? -1 : 1;
    }
    float random_range(float min, float max) {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<float> E(min, max);
        return E(e2);
    }
private:
    bool isGrounded;  // Telling us if the object is on the ground.
    bool isRight;     // Telling us if the objsdect is facing to the right.
    int last_sense;
    bool hasFallen;
    bool wantsToFall;   // Telling us if joseph wants to fall
                        // either to chase popo or to cause a rumbling
    bool isStunned;
    float current_cooldown;
    float cooldown;
    float current_chase_cooldown;
    float chase_cooldown;
    bool chasePlayer;
    bool ignoreFloor;
    float lastTrackY;
    float trackTimer;
    bool firstTimeAirborn;
public:
    Animator& animator;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;
    
    std::vector<Transform2D*> playerTransforms;
    bool shakeGround;

    JosephBehavior(GameObject& gameObject) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        isGrounded  = true;
        isRight     = true;
        last_sense  = 0;
        hasFallen   = false;
        wantsToFall = false;
        isStunned   = false;
        current_cooldown = 0.0f;
        cooldown    = 3.0f;
        current_chase_cooldown = 0.0f;
        chase_cooldown = 4.0f;
        chasePlayer = false;
        shakeGround = false;
        ignoreFloor = false;
        lastTrackY  = transform.position.y;
        trackTimer  = 0.0f;
        firstTimeAirborn = true;
    }

    JosephBehavior(GameObject& gameObject, JosephBehavior& behavior) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>()),
        playerTransforms(behavior.playerTransforms),
        shakeGround(behavior.shakeGround)
    {
        isGrounded  = behavior.isGrounded;
        isRight     = behavior.isRight;
        last_sense  = behavior.last_sense;
        hasFallen   = behavior.hasFallen;
        wantsToFall = behavior.wantsToFall;
        isStunned   = behavior.isStunned;
        current_cooldown = behavior.current_cooldown;
        cooldown    = behavior.cooldown;
        current_chase_cooldown = behavior.current_chase_cooldown;
        chase_cooldown = behavior.chase_cooldown;
        chasePlayer = behavior.chasePlayer;
        ignoreFloor = behavior.ignoreFloor;
        lastTrackY  = behavior.transform.position.y;
        trackTimer  = behavior.trackTimer;
        firstTimeAirborn = true;
    }
    
    Component* Clone(GameObject& gameObject) override {
        return new JosephBehavior(gameObject, *this);
    }

    
    void Start() override {
        rigidbody.velocity.x = random_sense() * rigidbody.acceleration.x;
        if (rigidbody.velocity.x < 0) {
            transform.position.x = GetScreenWidth();
            if (isRight) {
                isRight = !isRight;
                animator.Flip();
            }
        } else {
            transform.position.x = -(animator.GetViewDimensions().x);
        }
    }

    void OnCollision(Collision contact) override {
        if ((contact.gameObject.tag == "Floor" || contact.gameObject.tag == "Wall" || contact.gameObject.tag == "SlidingFloor")) {
            rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
            isGrounded = true;
            firstTimeAirborn = false;
            if (hasFallen) {
                rigidbody.velocity.x = last_sense * rigidbody.acceleration.x;
                hasFallen = false;
                shakeGround = true;
                wantsToFall = false;
                animator["Walk"];
            }
        }
        if (contact.gameObject.tag == "Hole") {
            int pos_x1 = contact.gameObject.getComponent<Collider2D>().Pos().x;
            int pos_x2 = pos_x1 + contact.gameObject.getComponent<Collider2D>().size.x;
            int pos_p1 = collider.Pos().x;
            int pos_p2 = pos_p1 + collider.size.x;
            if (isGrounded) {
                bool isGoingToFall = (pos_x1 < pos_p1) && (pos_x2 > pos_p2);
                if (isGoingToFall) {
                    animator["Falling"];
                    last_sense = sgn(rigidbody.velocity.x);
                    rigidbody.velocity.x = 0;
                    isGrounded = false;
                    hasFallen = true;
                } else {
                    bool headingToFall =    (isRight && pos_p1 < pos_x1) ||
                                            (!isRight && pos_p2 > pos_x2);
                    if ((wantsToFall && !headingToFall) || (!wantsToFall && headingToFall)) {
                        isRight = !isRight;
                        animator.Flip();
                        rigidbody.velocity.x *= -1;
                    }
                }
            }
        }
        if (contact.gameObject.tag == "Player") {
            if (!isStunned && contact.gameObject.getComponent<Script, PopoBehavior>().isAttacking) {
                if (contact.contact_normal.x < 0 && !contact.gameObject.getComponent<Script, PopoBehavior>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x *= -1;
                }
                if (contact.contact_normal.x > 0 && contact.gameObject.getComponent<Script, PopoBehavior>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x *= -1;
                }
            }
        }
    }

    void Update() override {
        /*
        // delete itself if out of frame
        if (transform.position.y > GetScreenHeight() + 10.0f) {
            gameObject.Destroy();
            return;
        }
        */

        float deltaTime = GetFrameTime();

        // Extra check to see if joseph is falling
        if (trackTimer > 0.2f) {
            //std::cout << "Check: " << transform.position.y << " vs " << lastTrackY << std::endl;
            if (transform.position.y > lastTrackY + 40.0f
                && transform.position.y < lastTrackY + 80.0f) {
                animator["Falling"];
                last_sense = sgn(rigidbody.velocity.x);
                rigidbody.velocity.x = 0;
                isGrounded = false;
                hasFallen = true;
            }
            lastTrackY = transform.position.y;
            trackTimer = 0.0f;
        } else if (!firstTimeAirborn) {
            trackTimer += deltaTime;
        }

        if (isStunned) {
            rigidbody.velocity.x = 0.0f;
            if (current_cooldown > cooldown && isGrounded) {
                isStunned = false;
                animator["Walk"];
                current_cooldown = 0.0f;
                rigidbody.velocity.x = random_sense() * rigidbody.acceleration.x;

                if (isRight && rigidbody.velocity.x < 0.0f) {
                    animator.Flip();
                    isRight = false;
                } else if (!isRight && rigidbody.velocity.x > 0.0f) {
                    animator.Flip();
                    isRight = true;
                }
            } else current_cooldown += deltaTime;
        }
        
        if (current_chase_cooldown > chase_cooldown) {
            chasePlayer = !chasePlayer;
            current_chase_cooldown = 0.0f;
            chase_cooldown = random_range(2.0f, 8.0f);
        } else current_chase_cooldown += deltaTime;

        if (isGrounded) {

            float closest_player_x = 0.0f;
            float closest_player_dist = 10000.0f;
            bool allplayersAbove = true;
            for (auto p_trans : playerTransforms) {

                wantsToFall |= p_trans->position.y + p_trans->size.y >
                            transform.position.y + transform.size.y + 5.0f;

                allplayersAbove &= p_trans->position.y + p_trans->size.y < transform.position.y;

                float player_dist = mod(p_trans->position - transform.position);
                if (player_dist < closest_player_dist) {
                    closest_player_dist = player_dist;
                    closest_player_x = p_trans->position.x;
                }
            }

            if (allplayersAbove && transform.position.y < GetScreenHeight()) {
                // Make it harder for the players by removing one of the levels and hoping someone falls
                wantsToFall = true;
            }
            if (chasePlayer && !allplayersAbove) {
                bool headingToPlayer = (isRight && closest_player_x > transform.position.x) ||
                                    (!isRight && closest_player_x < transform.position.x);
                if (fabs(closest_player_x - transform.position.x) < 1.0f)
                    rigidbody.velocity.x = 0.0f;
                else {
                    rigidbody.velocity.x = sgn(closest_player_x - transform.position.x) * rigidbody.acceleration.x;
                    if (!headingToPlayer) {
                        isRight = !isRight;
                        animator.Flip();
                    }
                }
            } else if (rigidbody.velocity.x == 0.0f) {
                rigidbody.velocity.x = random_sense() * rigidbody.acceleration.x;
                if ((isRight && rigidbody.velocity.x < 0.0f) ||
                    (!isRight && rigidbody.velocity.x > 0.0f)) {
                    isRight = !isRight;
                    animator.Flip();
                }
            }
        }
        
        transform.position.x += rigidbody.velocity.x * deltaTime;
        if (!isRight && (transform.position.x + transform.size.x + 1.0f < 0.0f))
            transform.position.x = GetScreenWidth();
        if (isRight && (transform.position.x + 1.0f > GetScreenWidth()))
            transform.position.x = -transform.size.x;

        transform.position.y += rigidbody.velocity.y * deltaTime;
        rigidbody.velocity.y += rigidbody.gravity    * deltaTime;
    }
};

class RedCondorBehavior : public Script {
private:
    bool turning;
    Animator& animator;
    RigidBody2D& rigidbody;
    Transform2D& transform;
public:

    RedCondorBehavior(GameObject& gameObject) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        turning = false;
        rigidbody.velocity.x = rigidbody.acceleration.x;
    }

    RedCondorBehavior(GameObject& gameObject, RedCondorBehavior behavior) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        turning = behavior.turning;
        rigidbody.velocity.x = rigidbody.acceleration.x;
    }

    Component* Clone(GameObject& gameObject) override {
        return new RedCondorBehavior(gameObject);
    }

    void OnCollision(Collision contact) {
        if (contact.contact_normal.y < 0) {
            rigidbody.velocity = {0,0};
            rigidbody.acceleration = {0,0};
        }
    }

    void Update() {
        transform.position.x += rigidbody.velocity.x * GetFrameTime();
        if (!turning) {
            if (rigidbody.velocity.x > 0 &&
                ((GetScreenWidth()-50) < (transform.position.x + animator.GetViewDimensions().x))) {
                rigidbody.velocity.x *= -1;
                animator.Flip();
            } else if (rigidbody.velocity.x < 0 && transform.position.x < 50) {
                rigidbody.velocity.x *= -1;
                animator.Flip();
            }
            turning = true;
        } else {
            if (rigidbody.velocity.x > 0 && ((transform.position.x + animator.GetViewDimensions().x) > 50)) {
                turning = false;
            }
            if (rigidbody.velocity.x < 0 && ((GetScreenWidth()-50) > transform.position.x)) {
                turning = false;
            }
        }
    }
};

class IcicleBehavior : public Script {
private:
    RigidBody2D& rigidbody;
    Transform2D& transform;
public:
    IcicleBehavior(GameObject& gameObject) : Script(gameObject),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {

    }

    IcicleBehavior(GameObject& gameObject, IcicleBehavior& behavior) : Script(gameObject),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {

    }

    Component* Clone(GameObject& gameObject) override {
        return new IcicleBehavior(gameObject, *this);
    }

    void OnCollision(Collision contact) override {
        //if (contact.gameObject.tag == "Floor") {
        //    rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
        //}
        if (contact.gameObject.tag == "Hole") {
            auto position = contact.gameObject.getComponent<Transform2D>().position;
            GameSystem::Instantiate(*contact.gameObject.getComponent<Script, HoleBehavior>().original_block, GameObjectOptions{.position=position});
            contact.gameObject.Destroy();
            gameObject.Destroy();
        }
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        transform.position.x += rigidbody.velocity.x * deltaTime;
        //transform.position.y += rigidbody.velocity.y * deltaTime;
        //rigidbody.velocity.y += rigidbody.gravity * deltaTime;
    }

};

class TopiBehavior : public Script {
private:
    // Variables para Popo:
    float original_level;
    float current_cooldown;
    float cooldown;
    bool started;
    bool spawned;
    bool isGrounded;  // Telling us if the object is on the ground.
    bool isRunning;
    bool isRight;     // Telling us if the objsdect is facing to the right.
    bool isStunned;
    bool ignoreFloor;
    int last_sense;
    bool hasFallen;
    bool needIcicle;

    GameObject& Icicle;

    int random_sense() {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> E(0, 1);
        return (E(e2) < 0.5) ? -1 : 1;
    }

public:
    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más
    // eficiente que acceder una y otra vez a los componentes cada vez que
    // necesitamos hacer algo con uno de ellos.
    Animator& animator;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;

    TopiBehavior(GameObject& gameObject, GameObject& Icicle) : Script(gameObject),
        Icicle(Icicle),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        original_level = transform.position.y;
        cooldown = current_cooldown = 3.0f;
        started     = false;
        spawned     = false;
        isGrounded  = false;
        isRunning   = false;
        isRight     = true;
        isStunned   = false;
        ignoreFloor = false;
        last_sense  = 0;
        hasFallen = false;
        needIcicle = false;

    }

    TopiBehavior(GameObject& gameObject, TopiBehavior& behavior) : Script(gameObject),
        Icicle(behavior.Icicle),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        cooldown = current_cooldown = 3.0f;
        original_level = transform.position.y;
        ignoreFloor = behavior.ignoreFloor;
        started     = behavior.started;
        spawned     = behavior.spawned;
        isGrounded  = behavior.isGrounded;
        isRight     = behavior.isRight;
        isRunning   = behavior.isRunning;
        isStunned   = behavior.isStunned;
        last_sense  = behavior.last_sense;
        hasFallen   = behavior.hasFallen;
        needIcicle  = behavior.needIcicle;

    }

    Component* Clone(GameObject& gameObject) override {
        return new TopiBehavior(gameObject, *this);
    }

    void Start() override {
        original_level = transform.position.y;
        rigidbody.velocity.x = random_sense() * rigidbody.acceleration.x;
        if (rigidbody.velocity.x < 0) {
            transform.position.x = GetScreenWidth();
            if (isRight) {
                isRight = !isRight;
                animator.Flip();
            }
        } else {
            transform.position.x = -(animator.GetViewDimensions().x);
        }
    }

    void OnCollision(Collision contact) override {

        if ((contact.gameObject.tag == "Floor" || contact.gameObject.tag == "Wall" || contact.gameObject.tag == "SlidingFloor") && !ignoreFloor) {
            rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
            isGrounded = true;
            if (hasFallen) {
                rigidbody.velocity.x = last_sense * rigidbody.acceleration.x;
            }
        }
        if (contact.gameObject.tag == "Hole") {
            int pos_x1 = contact.gameObject.getComponent<Collider2D>().Pos().x, pos_x2 = pos_x1 + contact.gameObject.getComponent<Collider2D>().size.x,
                dis_x1x2_2 = (pos_x2 - pos_x1)/2,
                pos_p2 = collider.Pos().x + collider.size.x;
            if (!hasFallen && !isRunning) {
                if (((isRight && (pos_p2 <= pos_x1-1)) || (!isRight && (collider.Pos().x >= pos_x2)))) {
                    rigidbody.velocity.x *= -2;
                    if (rigidbody.velocity.x > 0 && !isRight) {
                        isRight = !isRight;
                        animator.Flip();
                    }
                    if (rigidbody.velocity.x < 0 && isRight) {
                        isRight = !isRight;
                        animator.Flip();
                    }
                    isRunning = true;
                    last_sense = sgn(rigidbody.velocity.x);
                    needIcicle = true;
                } else {
                    animator["Stunned"];
                    last_sense = sgn(rigidbody.velocity.x);
                    rigidbody.velocity.x = 0;
                    isGrounded = false;
                    hasFallen = true;
                    ignoreFloor = true;
                }
            } else {
                if ((isRight && (pos_p2 < pos_x2 && pos_p2 >= (pos_x1+dis_x1x2_2)))
                    || (!isRight && (collider.Pos().x > pos_x1 && collider.Pos().x <= (pos_x1+dis_x1x2_2))))
                {
                    animator["Stunned"];
                    last_sense = sgn(rigidbody.velocity.x);
                    rigidbody.velocity.x = 0;
                    isGrounded = false;
                    ignoreFloor = true;
                } else {
                    rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
                }
            }
        }
        if (contact.gameObject.tag == "Player") {
            if (!isStunned && contact.gameObject.getComponent<Script, PopoBehavior>().isAttacking) {
                if (contact.contact_normal.x < 0 && !contact.gameObject.getComponent<Script, PopoBehavior>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x *= -1;
                }
                if (contact.contact_normal.x > 0 && contact.gameObject.getComponent<Script, PopoBehavior>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x *= -1;
                }
            }
        }
        if (contact.gameObject.tag == "Icicle") {
            contact.gameObject.getComponent<RigidBody2D>().velocity.x = rigidbody.velocity.x;
        }
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        if (!started) {
            if ((current_cooldown >= cooldown) || needIcicle) {
                needIcicle = false;
                current_cooldown = 0.0f;
                started = true;
                isStunned = false;
                hasFallen = false;
                animator["Walk"];
                if (isRunning) {
                    isRunning = false;
                    rigidbody.velocity.x = -last_sense * rigidbody.acceleration.x;
                    auto icicle_size = Icicle.getComponent<Sprite>().GetViewDimensions();
                    if (last_sense > 0) {
                        transform.position.x = GetScreenWidth() + 70;
                        GameSystem::Instantiate(Icicle, GameObjectOptions{.position{transform.position.x - 40, original_level + (animator.GetViewDimensions().y - icicle_size.y) + 2}});
                        if (isRight) {
                            isRight = !isRight;
                            animator.Flip();
                        }
                    } else {
                        transform.position.x = -(animator.GetViewDimensions().x + 70);
                        GameSystem::Instantiate(Icicle, GameObjectOptions{.position{transform.position.x + 40, original_level + (animator.GetViewDimensions().y - icicle_size.y) + 2}});
                        if (!isRight) {
                            isRight = !isRight;
                            animator.Flip();
                        }
                    }
                } else {
                    rigidbody.velocity.x = random_sense() * rigidbody.acceleration.x;
                    if (rigidbody.velocity.x < 0) {
                        transform.position.x = GetScreenWidth();
                        if (isRight) {
                            isRight = !isRight;
                            animator.Flip();
                        }
                    } else {
                        transform.position.x = -(animator.GetViewDimensions().x);
                        if (!isRight) {
                            isRight = !isRight;
                            animator.Flip();
                        }
                    }
                }

                transform.position.y = original_level;
            } else {
                current_cooldown += GetFrameTime();
            }
        } else {
            if(rigidbody.velocity.x == 0)
                rigidbody.velocity.x = last_sense * rigidbody.acceleration.x;
            transform.position.x += rigidbody.velocity.x * deltaTime;
            if (!spawned){
                if (transform.position.x < (GetScreenWidth()+10) && rigidbody.velocity.x < 0) {
                    spawned = true;
                }
                if ((transform.position.x + animator.GetViewDimensions().x) > 10 && rigidbody.velocity.x > 0) {
                    spawned = true;
                }
            } else {
                if (transform.position.x > GetScreenWidth() && rigidbody.velocity.x > 0) {
                    started = spawned = false;
                }
                if (transform.position.x + animator.GetViewDimensions().x <= 0 && rigidbody.velocity.x < 0) {
                    started = spawned = false;
                }
            }
        }
        transform.position.y += rigidbody.velocity.y * deltaTime;
        rigidbody.velocity.y += rigidbody.gravity    * deltaTime;
        if (ignoreFloor) {
            ignoreFloor = !ignoreFloor;
        }
    }

    void Move(Vector2 traslation) {
        original_level += traslation.y;
    }
};

class NutpickerBehavior : public Script {
private:
    // Variables para Nutpicker:
    bool isStunned;
    float time;
    float timeStunned;
    float t;
    float tStunned;
    float isRight;
    int cont;
    unsigned t0, t1;

public:
    // ¿Que usa Popo? Guardamos las referencias de sus componentes ya que es más
    // eficiente que acceder una y otra vez a los componentes cada vez que
    // necesitamos hacer algo con uno de ellos.
    Animator& animator;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;

    NutpickerBehavior(GameObject& gameObject, GameObject& Icicle) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        isStunned = true;
        time = rand() % 10 + 15;
        timeStunned = rand() % 25 + 20;
        t = 0;
        tStunned = 0;
        isRight = 1;
        cont = 0;
    }

    NutpickerBehavior(GameObject& gameObject, NutpickerBehavior& behavior) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        isStunned = true;
        time = behavior.time;
        timeStunned = behavior.timeStunned;
        t = behavior.t;
        tStunned = behavior.tStunned;
        t0 = behavior.t0;
        t1 = behavior.t1;
        rigidbody.velocity.x = 80;
        isRight = behavior.isRight;
        cont = behavior.cont;
    }

    Component* Clone(GameObject& gameObject) override {
        return new NutpickerBehavior(gameObject, *this);
    }


    void OnCollision(Collision contact) override {

        if (contact.gameObject.tag == "Player") {
            if (!isStunned && contact.gameObject.getComponent<Script, PopoBehavior>().isAttacking) {
                if (contact.contact_normal.x < 0 && !contact.gameObject.getComponent<Script, PopoBehavior>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                    t0 = clock();
                }
                if (contact.contact_normal.x > 0 && contact.gameObject.getComponent<Script, PopoBehavior>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                    t0 = clock();
                }
            }
            if(!isStunned && (!contact.gameObject.getComponent<Script, PopoBehavior>().isGrounded) && (contact.contact_normal.y < 0)){
                animator["Stunned"];
                isStunned = true;
                rigidbody.velocity.x = 0;
                t0 = clock();
            }
        }
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        if(t0 == 0) t0 = clock();
        bool primer = true;

        for (auto& [check_name, check_ref] : GameSystem::GameObjects["Player"]) {
            if(primer){
                primer = false;
                if(!isStunned){
                    t1 = clock();
                    auto elapsed_time = (double(t1-t0)/CLOCKS_PER_SEC);

                    if(elapsed_time < time) {
                        if(transform.position.y > check_ref->getComponent<Transform2D>().position.y + check_ref->getComponent<Animator>().GetViewDimensions().y + 50){
                            rigidbody.velocity.y = -20;
                        }else if(transform.position.y + animator.GetViewDimensions().y + 50 < check_ref->getComponent<Transform2D>().position.y){
                            rigidbody.velocity.y = 20;
                        }
                        if(abs(transform.position.x - check_ref->getComponent<Transform2D>().position.x) > 2*WINDOW_WIDTH/5){
                            if((check_ref->getComponent<Transform2D>().position.x > transform.position.x && rigidbody.velocity.x < 0) || (check_ref->getComponent<Transform2D>().position.x < transform.position.x && rigidbody.velocity.x > 0)){
                                animator.Flip();
                                isRight *= -1;
                                rigidbody.velocity.x *= -1;
                            }
                        }
                    }else if(transform.position.x + animator.GetViewDimensions().x < 0 || transform.position.x > WINDOW_WIDTH ||
                                 transform.position.y + animator.GetViewDimensions().y < 0 || transform.position.y > WINDOW_HEIGHT) {
                        isStunned = true;
                        t0 = clock();
                    }
                }else{
                    t1 = clock();
                    auto elapsed_time = (double(t1-t0)/CLOCKS_PER_SEC);
                    rigidbody.velocity.y = 200;
                    cont += 1;

                    if(cont % 7 == 0){
                        animator.Flip();
                        isRight *= -1;
                    }

                    if(elapsed_time >= timeStunned){
                        t0 = clock();
                        isStunned = false;
                        animator["Walk"];

                        if(check_ref->getComponent<Transform2D>().position.x <= WINDOW_WIDTH/2){
                            transform.position.x = -50;
                            rigidbody.velocity.x = 100;
                            if(isRight == -1){
                                animator.Flip();
                                isRight *= -1;
                            }
                        }else{
                            transform.position.x = WINDOW_WIDTH + 50;
                            rigidbody.velocity.x = -100;
                            if(isRight == 1){
                                animator.Flip();
                                isRight *= -1;
                            }
                        }
                        transform.position.y = 150;
                    }
                }

                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;
            }
        }
    }
};