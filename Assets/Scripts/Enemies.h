#pragma once
#include "EngineECS.h"
#include "Block.h"
#include "Popo.h"
#include "settings.h"
#include <iostream>
#include <ctime>
#include "random"

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
        if (contact.gameObject.tag == "Player") {
            if (contact.gameObject.getComponent<Script, PopoBehavior>().isAttacking) {
                if (contact.contact_normal.x < 0 && !contact.gameObject.getComponent<Script, PopoBehavior>().isRight) {
                    gameObject.Destroy();
                }
                if (contact.contact_normal.x > 0 && contact.gameObject.getComponent<Script, PopoBehavior>().isRight) {
                    gameObject.Destroy();
                }
            }
        }
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        transform.position.x += rigidbody.velocity.x * deltaTime;
        //transform.position.y += rigidbody.velocity.y * deltaTime;
        rigidbody.velocity.y += rigidbody.gravity * deltaTime;
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
            transform.position.x = GetScreenWidth() + 70;
            if (isRight) {
                isRight = !isRight;
                animator.Flip();
            }
        } else {
            transform.position.x = -(animator.GetViewDimensions().x + 70);
        }
    }

    void OnCollision(Collision contact) override {

        if ((contact.gameObject.tag == "Floor" || contact.gameObject.tag == "Wall") && !ignoreFloor) {
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
            if (!hasFallen) {
                std::cout << collider.Pos().x << "," << pos_x2 << "," << isRight << "\n";
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
                    std::cout << "Ignorando" << std::endl;
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
                    std::cout << "Ignorando" << std::endl;
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
                        auto ic = GameSystem::Instantiate(Icicle, GameObjectOptions{.position{transform.position.x - 40, original_level - (original_level - icicle_size.y)}});
                        std::cout << "Topi x = " << transform.position.x << std::endl;
                        std::cout << "Icicle x = " << ic.getComponent<Transform2D>().position.y << std::endl;
                        if (isRight) {
                            isRight = !isRight;
                            animator.Flip();
                        }
                    } else {
                        transform.position.x = -(animator.GetViewDimensions().x + 70);
                        auto ic = GameSystem::Instantiate(Icicle, GameObjectOptions{.position{transform.position.x + 40, original_level - (original_level - icicle_size.y)}});
                        std::cout << "Topi x = " << transform.position.x << std::endl;
                        std::cout << "Icicle x = " << ic.getComponent<Transform2D>().position.y << std::endl;
                        if (!isRight) {
                            isRight = !isRight;
                            animator.Flip();
                        }
                    }
                } else {
                    rigidbody.velocity.x = random_sense() * rigidbody.acceleration.x;
                    if (rigidbody.velocity.x < 0) {
                        transform.position.x = GetScreenWidth() + 40;
                        if (isRight) {
                            isRight = !isRight;
                            animator.Flip();
                        }
                    } else {
                        transform.position.x = -(animator.GetViewDimensions().x + 40);
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
                    std::cout << "stun 1" << std::endl;
                }
                if (contact.contact_normal.x > 0 && contact.gameObject.getComponent<Script, PopoBehavior>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                    t0 = clock();
                    std::cout << "stun 2" << std::endl;
                }
            }
            if(!isStunned && (!contact.gameObject.getComponent<Script, PopoBehavior>().isGrounded) && (contact.contact_normal.y < 0)){
                animator["Stunned"];
                isStunned = true;
                rigidbody.velocity.x = 0;
                t0 = clock();
                std::cout << "stun 3" << std::endl;
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
                    rigidbody.velocity.y = 80;
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