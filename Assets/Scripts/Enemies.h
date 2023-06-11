#pragma once
#include <cmath>
#include <ctime>
#include <random>

#include "Dengine.h"
#include "DengineUI.h"
#include "raylibx.h"
#include "Player.h"
#include "settings.h"

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
    float startTime;
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
        startTime  = 0.0f;
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
        startTime  = behavior.startTime;
    }
    
    Component* Clone(GameObject& gameObject) override {
        return new JosephBehavior(gameObject, *this);
    }

    
    void Start() override {
        rigidbody.velocity.x = random_sense() * rigidbody.acceleration.x;
        if (rigidbody.velocity.x < 0) {
            //transform.position.x = GetScreenWidth();
            if (isRight) {
                isRight = !isRight;
                animator.Flip();
            }
        } else {
            //transform.position.x = -(animator.GetViewDimensions().x);
        }
    }

    void OnCollision(Collision contact) override {
        if ((contact.gameObject.tag == "Floor" || contact.gameObject.tag == "Wall" || contact.gameObject.tag == "SlidingFloor")) {
            rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
            isGrounded = true;
            firstTimeAirborn = false;
            if (hasFallen && transform.position.y < GetScreenHeight()) {
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
                    rigidbody.velocity.x = last_sense*1.0f;
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
            if (!isStunned && contact.gameObject.getComponent<Script, Player>().isAttacking) {
                if (contact.contact_normal.x < 0 && !contact.gameObject.getComponent<Script, Player>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x *= -1;
                }
                if (contact.contact_normal.x > 0 && contact.gameObject.getComponent<Script, Player>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x *= -1;
                }
            }
            if (contact.gameObject.getComponent<Script, Player>().isStunned) {
                // Stop chasing
                chasePlayer = false;
                current_chase_cooldown = 0.0f;
                chase_cooldown = random_range(6.0f, 8.0f);
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

        if(startTime < 2.0) {
            startTime += deltaTime;
        } else {
            // Extra check to see if joseph is falling
            if (trackTimer > 0.2f) {
                if (transform.position.y > lastTrackY + transform.size.y/4
                    && transform.position.y < lastTrackY + transform.size.y/2) {
                    animator["Falling"];
                    last_sense = sgn(rigidbody.velocity.x);
                    rigidbody.velocity.x = last_sense*1.0f;
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
                if (transform.position.y + transform.size.y < 0) {
                    // dont intervene unless I can see youuuu
                    wantsToFall = false;
                }
                if (chasePlayer && !allplayersAbove) {
                    bool headingToPlayer = (isRight && closest_player_x > transform.position.x) ||
                                        (!isRight && closest_player_x < transform.position.x);
                    if (fabs(closest_player_x - transform.position.x) < 1.0f)
                        rigidbody.velocity.x = 0.0f;
                    else {
                        rigidbody.velocity.x = sgn(closest_player_x - transform.position.x) * rigidbody.acceleration.x * 1.5f;
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
                } else {
                    // Give back normal speed
                    rigidbody.velocity.x = sgn(rigidbody.velocity.x) * rigidbody.acceleration.x;
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
    }
};

class RedCondorBehavior : public Script {
private:
    bool turning;
    Animator& animator;
    RigidBody2D& rigidbody;
    Transform2D& transform;

    double random_val() {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> E(0, 1);
        return E(e2);
    }

    double calcularProbabilidad(double distancia, double media, double desviacion) {
        double exponente = -pow(distancia - media, 2) / (2 * pow(desviacion, 2));
        double coeficiente = 1 / (desviacion * sqrt(2 * PI));
        return coeficiente * exp(exponente);
    }

    bool desceding = false;
    float max_descending;
    int nBounces = 0;
    bool charge = false;
    bool isCharging = false;
    bool recoverCharge = false;
    bool retreat = false;
    Vector2 endPoint = {0,0};
    bool isRight = true;
    bool zigzaging = false;


public:

    float original_height;
    std::vector<GameObject*> players;

    RedCondorBehavior(GameObject& gameObject) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        turning = false;
        desceding = false;
        isRight = true;
        charge = false;
        rigidbody.velocity.x = rigidbody.acceleration.x;
        original_height = transform.position.y;
    }

    RedCondorBehavior(GameObject& gameObject, RedCondorBehavior behavior) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        turning = behavior.turning;
        desceding = behavior.desceding;
        rigidbody.velocity.x = rigidbody.acceleration.x;
        isRight = true;
        charge = false;
        original_height = behavior.original_height;
    }

    Component* Clone(GameObject& gameObject) override {
        return new RedCondorBehavior(gameObject, *this);
    }

    void OnCollision(Collision contact) {
        if (contact.gameObject.tag == "Player") {
            if (contact.gameObject.getComponent<Script, Player>().floor_level == contact.gameObject.getComponent<Script, Player>().last_level) {
                if (contact.contact_normal.y < 0) {
                    rigidbody.velocity = {0,0};
                    rigidbody.acceleration = {0,0};
                }
            } else if (contact.gameObject.getComponent<Script, Player>().isAttacking) {
                rigidbody.velocity.y *= -1;
                if ((rigidbody.velocity.x < 0 && isRight) || (rigidbody.velocity.x > 0 && !isRight)) {
                    isRight = !isRight;
                    animator.Flip();
                }
            } else {
                if (!contact.gameObject.getComponent<Script, Player>().isStunned) {
                    contact.gameObject.getComponent<Animator>()["Stunned"];
                    contact.gameObject.getComponent<Script, Player>().isStunned = true;
                    contact.gameObject.getComponent<RigidBody2D>().velocity.x = 0;
                    contact.gameObject.getComponent<Script, Player>().lifes--;
                }
            }
        }
    }

    void Update() {

        transform.position.x += rigidbody.velocity.x * GetFrameTime();
        if (!turning) {
            if (rigidbody.velocity.x > 0 &&
                ((GetScreenWidth()-(50 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF))) < (transform.position.x + animator.GetViewDimensions().x))) {
                rigidbody.velocity.x *= -1;
                animator.Flip();
                isRight = false;
            } else if (rigidbody.velocity.x < 0 && transform.position.x < (50 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF))) {
                rigidbody.velocity.x *= -1;
                animator.Flip();
                isRight = true;
            }
            turning = true;
        } else {
            if (rigidbody.velocity.x > 0 && ((transform.position.x + animator.GetViewDimensions().x) > (50 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)))) {
                turning = false;
            }
            if (rigidbody.velocity.x < 0 && ((GetScreenWidth()-(50 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF))) > transform.position.x)) {
                turning = false;
            }
        }

        if (std::get<bool>(ini["Game"]["AdvancedAI"])) {

            if (charge) {
                if (!isCharging) {
                    float less_dis = INFINITY;
                    GameObject * less_player = nullptr;
                    for (auto& p : players) {
                        if (p != nullptr) {
                            float dis = mod(p->getComponent<Transform2D>().position - transform.position);
                            if (dis < less_dis) {
                                less_dis = dis;
                                less_player = p;
                            }
                        } 
                    }
                    if (less_player != nullptr) {
                        endPoint = less_player->getComponent<Transform2D>().position;
                        Vector2 dir = endPoint - transform.position;
                        rigidbody.velocity = nor(dir, 400 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF));
                        if ((rigidbody.velocity.x < 0 && isRight) || (rigidbody.velocity.x > 0 && !isRight)) {
                            isRight = !isRight;
                            animator.Flip();
                        }
                    }
                    isCharging = true;
                } else {
                    transform.position.y += rigidbody.velocity.y * GetFrameTime();
                    if (!recoverCharge) {
                        if (transform.position.y > GetScreenHeight()+(70 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)) || transform.position.y < (-70 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF))) {
                            recoverCharge = true;
                            if (transform.position.y > original_height) {
                                rigidbody.velocity.y = -100 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                            } else if (transform.position.y < original_height) {
                                rigidbody.velocity.y =  100 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                            } else {
                                rigidbody.velocity.y =    0;
                            }
                        }
                    } else {
                        if (transform.position.y > (original_height - (10 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF))) && transform.position.y < (original_height + (10 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)))) {
                            transform.position.y = 0;
                            rigidbody.velocity.x = sign(rigidbody.velocity.x) * rigidbody.acceleration.x;
                            if ((rigidbody.velocity.x < 0 && isRight) || (rigidbody.velocity.x > 0 && !isRight)) {
                                isRight = !isRight;
                                animator.Flip();
                            }
                            rigidbody.velocity.y = 0;
                            charge = false;
                            isCharging = false;
                            recoverCharge = false;
                            retreat = false;
                        }
                    }
                }
                return;
            }

            if (desceding) {
                if (max_descending > 0) {
                    auto aux = (200 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)) * GetFrameTime();
                    transform.position.y += aux;
                    max_descending -= aux;
                } else if (transform.position.y > original_height) {
                    transform.position.y -= (50* (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)) * GetFrameTime();
                } else {
                    transform.position.y = original_height;
                    desceding = false;
                }
                return;
            }

            if (!zigzaging) {
                if (random_val() < 0.005) {
                    zigzaging = true;
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<int> dis(40 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF), 70 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF));
                    max_descending = dis(gen);
                }
            } else {
                if (max_descending > 0) {
                    auto aux = 100 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF) * GetFrameTime();
                    transform.position.y += aux;
                    max_descending -= aux;
                } else if (transform.position.y > original_height) {
                    transform.position.y -= 100 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF) * GetFrameTime();
                } else {
                    transform.position.y = original_height;
                    zigzaging = false;
                }
            }
            
            float less_dis = INFINITY;
            GameObject * less_player = nullptr;
            for (auto& p : players) {
                if (p != nullptr) {
                    float dis = mod(p->getComponent<Transform2D>().position - transform.position);
                    if (dis < less_dis) {
                        less_dis = dis;
                        less_player = p;
                    }
                } 
            }
            if (less_player != nullptr) {
                float dis = mod(transform.position - less_player->getComponent<Transform2D>().position);
                double prob = calcularProbabilidad(dis, dis/2, dis/2);
                if (transform.position.y > -3 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)) {
                    if (random_val() < prob*3) {
                        if (random_val() < 1.45/3.0) {
                            if (less_player->getComponent<Transform2D>().position.x <= transform.position.x && rigidbody.velocity.x > 0) {
                                rigidbody.velocity.x *= -1;
                                animator.Flip();
                                isRight = false;
                            } else if (less_player->getComponent<Transform2D>().position.x >= transform.position.x && rigidbody.velocity.x < 0) {
                                rigidbody.velocity.x *= -1;
                                animator.Flip();
                                isRight = true;
                            }
                        } else if (random_val() < 1.45/3.0) {
                            if (less_player->getComponent<Transform2D>().position.x <= transform.position.x && rigidbody.velocity.x > 0) {
                                rigidbody.velocity.x *= -1;
                                animator.Flip();
                                isRight = false;
                            } else if (less_player->getComponent<Transform2D>().position.x >= transform.position.x && rigidbody.velocity.x < 0) {
                                rigidbody.velocity.x *= -1;
                                animator.Flip();
                                isRight = true;
                            }
                            std::random_device rd;
                            std::mt19937 gen(rd());
                            std::uniform_int_distribution<int> dis(200 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF), 250 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF));
                            max_descending = dis(gen);
                            desceding = true;
                        } else {
                            charge = true;
                        }
                    }
                } else {
                    if (random_val() < 0.05) {
                        charge = true;
                    }
                }
                
            }
        }
    }
};

class IcicleBehavior : public Script {
private:
    AudioPlayer& audioplayer;
    RigidBody2D& rigidbody;
    Transform2D& transform;

    double random_val() {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> E(0, 1);
        return E(e2);
    }

    bool checkPlayers() {
        for (auto& p : players) {
            if (p != nullptr &&
                (
                     p->getComponent<Script, Player>().floor_level == floor_level || 
                    (p->getComponent<Script, Player>().floor_level == floor_level-1 && !p->getComponent<Script, Player>().isGrounded) ||
                    (p->getComponent<Script, Player>().floor_level == floor_level+1 && !p->getComponent<Script, Player>().isGrounded)
                )
            ) {
                return true;
            }
        }
        return false;
    }

public:

    int* current_blocks = nullptr;
    bool ignore_hole = false;
    int floor_level;
    std::vector<GameObject*> players;

    IcicleBehavior(GameObject& gameObject) : Script(gameObject),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>()) {}

    IcicleBehavior(GameObject& gameObject, IcicleBehavior& behavior) : Script(gameObject),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>()) {}

    Component* Clone(GameObject& gameObject) override {
        return new IcicleBehavior(gameObject, *this);
    }

    void OnCollision(Collision contact) override {
        if (contact.gameObject.tag == "Hole") {
            if (!ignore_hole) {
                audioplayer["Rebuild"];
                auto position = contact.gameObject.getComponent<Transform2D>().position;
                auto a_block = &GameSystem::Instantiate(*contact.gameObject.getComponent<Script, HoleBehavior>().original_block, GameObjectOptions{.position=position});
                if (current_blocks != nullptr) {
                    (*current_blocks)++;
                }
                if (a_block->tag == "Floor") {
                    a_block->getComponent<Script, BlockBehavior>().floor_level = floor_level;
                    a_block->getComponent<Script, BlockBehavior>().current_blocks = current_blocks;
                } else {
                    a_block->getComponent<Script, SlidingBlockBehavior>().floor_level = floor_level;
                    a_block->getComponent<Script, SlidingBlockBehavior>().current_blocks = current_blocks;
                }

                contact.gameObject.Destroy();
                gameObject.Destroy();
            }
        }
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        transform.position.x += rigidbody.velocity.x * deltaTime;
        if (std::get<bool>(ini["Game"]["AdvancedAI"]) && checkPlayers() && !ignore_hole) {
            if (random_val() > 0.9925) {
                ignore_hole = true;
                rigidbody.velocity.x = sign(rigidbody.velocity.x) * 400 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
            }
        }
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
    float startTime;

    GameObject& Icicle;

    int random_sense() {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> E(0, 1);
        return (E(e2) < 0.5) ? -1 : 1;
    }

    double random_val() {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> E(0, 1);
        return E(e2);
    }

public:
    AudioPlayer& audioplayer;
    Animator& animator;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;
    int* current_blocks;
    int total_blocks;
    int floor_level;
    std::vector<GameObject*> players;

    TopiBehavior(GameObject& gameObject, GameObject& Icicle, std::vector<GameObject*>* blocks = {}) : Script(gameObject),
        Icicle(Icicle),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
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
        hasFallen   = false;
        needIcicle  = false;
        startTime   = 0.0f;
    }

    TopiBehavior(GameObject& gameObject, TopiBehavior& behavior) : Script(gameObject),
        Icicle(behavior.Icicle),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
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
        startTime   = behavior.startTime;
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
            if (!isStunned && contact.gameObject.getComponent<Script, Player>().isAttacking) {
                if (contact.contact_normal.x < 0 && !contact.gameObject.getComponent<Script, Player>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x *= -1;
                    audioplayer["Hit"];
                }
                if (contact.contact_normal.x > 0 && contact.gameObject.getComponent<Script, Player>().isRight) {
                    animator["Stunned"];
                    isStunned = true;
                    rigidbody.velocity.x *= -1;
                    audioplayer["Hit"];
                }
            }
        }
        if (contact.gameObject.tag == "Icicle") {
            if (!contact.gameObject.getComponent<Script, IcicleBehavior>().ignore_hole) {
                contact.gameObject.getComponent<RigidBody2D>().velocity.x = rigidbody.velocity.x;
            } else {
                rigidbody.velocity.x *= -1;
                if ((rigidbody.velocity.x < 0 && isRight) || (rigidbody.velocity.x > 0 && !isRight)) {
                    isRight = !isRight;
                    animator.Flip();
                }
                isRunning = true;
            }
        }
    }

    bool checkPlayers() {
        for (auto& p : players) {
            if (p != nullptr &&(
                 p->getComponent<Script, Player>().floor_level == floor_level || 
                (p->getComponent<Script, Player>().floor_level == floor_level-1 && !p->getComponent<Script, Player>().isGrounded) ||
                (p->getComponent<Script, Player>().floor_level == floor_level+1 && !p->getComponent<Script, Player>().isGrounded)
            ) && (
                (rigidbody.velocity.x > 0 && p->getComponent<Transform2D>().position.x > transform.position.x) ||
                (rigidbody.velocity.x < 0 && p->getComponent<Transform2D>().position.x < transform.position.x)
            )) {
                return true;
            }
        }
        return false;
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        if (startTime < 2.0) {
            startTime += deltaTime;
        } else {
            if (!started) {
                if ((current_cooldown >= cooldown) || needIcicle) {
                    needIcicle = false;
                    current_cooldown = 0.0f;
                    started = true;
                    isStunned = false;
                    hasFallen = false;
                    animator["Walk"];
                    auto icicle_size = Icicle.getComponent<Sprite>().GetViewDimensions();
                    if (isRunning) {
                        isRunning = false;
                        rigidbody.velocity.x = -last_sense * rigidbody.acceleration.x;
                        if (last_sense > 0) {
                            transform.position.x = GetScreenWidth() + (70 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF));
                            if (isRight) {
                                isRight = !isRight;
                                animator.Flip();
                            }

                            // IF IA WAS ACTIVE
                            auto an_icicle = &GameSystem::Instantiate(Icicle, GameObjectOptions{.position{transform.position.x - (40 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)), original_level + (animator.GetViewDimensions().y - icicle_size.y) + 2}});
                            an_icicle->getComponent<Script, IcicleBehavior>().floor_level = floor_level;
                            if (std::get<bool>(ini["Game"]["AdvancedAI"]) && current_blocks != nullptr) {
                                an_icicle->getComponent<Script, IcicleBehavior>().players = players;
                                if (checkPlayers()) {
                                    an_icicle->getComponent<Script, IcicleBehavior>().current_blocks = current_blocks;
                                    if (random_val() > (total_blocks - *current_blocks)/(double)total_blocks) {
                                        an_icicle->getComponent<RigidBody2D>().velocity.x = -350 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                                        an_icicle->getComponent<Script, IcicleBehavior>().ignore_hole = true;
                                        rigidbody.velocity.x *= -1;
                                        if ((rigidbody.velocity.x < 0 && isRight) || (rigidbody.velocity.x > 0 && !isRight)) {
                                            isRight = !isRight;
                                            animator.Flip();
                                        }
                                    } else {
                                        an_icicle->getComponent<Script, IcicleBehavior>().ignore_hole = false;
                                    }
                                }
                            }

                        } else {
                            transform.position.x = -(animator.GetViewDimensions().x + (90 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)));
                            if (!isRight) {
                                isRight = !isRight;
                                animator.Flip();
                            }

                            // IF IA WAS ACTIVE
                            auto an_icicle = &GameSystem::Instantiate(Icicle, GameObjectOptions{.position{transform.position.x + (50 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)), original_level + (animator.GetViewDimensions().y - icicle_size.y) + 2}});
                            an_icicle->getComponent<Script, IcicleBehavior>().floor_level = floor_level;
                            if (std::get<bool>(ini["Game"]["AdvancedAI"]) && current_blocks != nullptr) {
                                an_icicle->getComponent<Script, IcicleBehavior>().players = players;
                                if (checkPlayers()) {
                                    an_icicle->getComponent<Script, IcicleBehavior>().current_blocks = current_blocks;
                                    if (random_val() > (total_blocks - *current_blocks)/(double)total_blocks) {
                                        an_icicle->getComponent<RigidBody2D>().velocity.x = 350 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                                        an_icicle->getComponent<Script, IcicleBehavior>().ignore_hole = true;
                                        rigidbody.velocity.x *= -1;
                                        if ((rigidbody.velocity.x < 0 && isRight) || (rigidbody.velocity.x > 0 && !isRight)) {
                                            isRight = !isRight;
                                            animator.Flip();
                                        }
                                    } else {
                                        an_icicle->getComponent<Script, IcicleBehavior>().ignore_hole = false;
                                    }
                                }
                            }
                        }

                    } else {
                        rigidbody.velocity.x = random_sense() * rigidbody.acceleration.x;
                        if (rigidbody.velocity.x < 0) {
                            transform.position.x = GetScreenWidth() + (70 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF));
                            if (isRight) {
                                isRight = !isRight;
                                animator.Flip();
                            }

                            // IF IA WAS ACTIVE
                            if (std::get<bool>(ini["Game"]["AdvancedAI"]) && current_blocks != nullptr) {
                                if (random_val() > 0.45) {                                
                                    auto an_icicle = &GameSystem::Instantiate(Icicle, GameObjectOptions{.position{transform.position.x + (40 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)), original_level + (animator.GetViewDimensions().y - icicle_size.y) + 2}});
                                    an_icicle->getComponent<Script, IcicleBehavior>().floor_level = floor_level;
                                    an_icicle->getComponent<Script, IcicleBehavior>().players = players;
                                    if (checkPlayers()) {
                                        an_icicle->getComponent<Script, IcicleBehavior>().current_blocks = current_blocks;
                                        if (random_val() > (total_blocks - *current_blocks)/(double)total_blocks) {
                                            an_icicle->getComponent<RigidBody2D>().velocity.x = -350 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                                            an_icicle->getComponent<Script, IcicleBehavior>().ignore_hole = true;
                                            rigidbody.velocity.x *= -1;
                                            if ((rigidbody.velocity.x < 0 && isRight) || (rigidbody.velocity.x > 0 && !isRight)) {
                                                isRight = !isRight;
                                                animator.Flip();
                                            }
                                        } else {
                                            an_icicle->getComponent<Script, IcicleBehavior>().ignore_hole = false;
                                        }
                                    }
                                }
                            }
                        } else {
                            
                            transform.position.x = -(animator.GetViewDimensions().x + (90 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)));
                            if (!isRight) {
                                isRight = !isRight;
                                animator.Flip();
                            }

                            // IF IA WAS ACTIVE
                            if (std::get<bool>(ini["Game"]["AdvancedAI"]) && current_blocks != nullptr) {
                                if (random_val() > 0.45) {
                                    auto an_icicle = &GameSystem::Instantiate(Icicle, GameObjectOptions{.position{transform.position.x - (50 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)), original_level + (animator.GetViewDimensions().y - icicle_size.y) + 2}});
                                    an_icicle->getComponent<Script, IcicleBehavior>().floor_level = floor_level;
                                    an_icicle->getComponent<Script, IcicleBehavior>().players = players;
                                    if (checkPlayers()) {
                                        an_icicle->getComponent<Script, IcicleBehavior>().current_blocks = current_blocks;
                                        if (random_val() > (total_blocks - *current_blocks)/(double)total_blocks) {
                                            an_icicle->getComponent<RigidBody2D>().velocity.x = 350 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                                            an_icicle->getComponent<Script, IcicleBehavior>().ignore_hole = true;
                                            rigidbody.velocity.x *= -1;
                                            if ((rigidbody.velocity.x < 0 && isRight) || (rigidbody.velocity.x > 0 && !isRight)) {
                                                isRight = !isRight;
                                                animator.Flip();
                                            }
                                        } else {
                                            an_icicle->getComponent<Script, IcicleBehavior>().ignore_hole = false;
                                        }
                                    }
                                }
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
    bool isMoving;
    int cont;
    unsigned t0, t1;

public:
    AudioPlayer& audioplayer;
    Animator& animator;
    Collider2D& collider;
    RigidBody2D& rigidbody;
    Transform2D& transform;
    float isRight;
    int floor_level;
    std::vector<GameObject*> players;
    float vertical_scale;
    std::vector<float> floor_levels;

    NutpickerBehavior(GameObject& gameObject, GameObject& Icicle) : Script(gameObject),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        isStunned = false;
        time = rand() % 10 + 15;
        timeStunned = rand() % 25 + 20;
        t = 0;
        tStunned = 0;
        isRight = 1;
        cont = 0;
        isMoving = false;
    }

    NutpickerBehavior(GameObject& gameObject, NutpickerBehavior& behavior) : Script(gameObject),
        audioplayer(gameObject.getComponent<AudioPlayer>()),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        isStunned = false;
        time = behavior.time;
        timeStunned = behavior.timeStunned;
        t = behavior.t;
        tStunned = behavior.tStunned;
        t0 = behavior.t0;
        t1 = behavior.t1;
        rigidbody.velocity.x = 80;
        isRight = behavior.isRight;
        cont = behavior.cont;
        isMoving = behavior.isMoving;
    }

    Component* Clone(GameObject& gameObject) override {
        return new NutpickerBehavior(gameObject, *this);
    }

    void OnCollision(Collision contact) override {

        if (contact.gameObject.tag == "Player") {
            if (!isStunned && contact.gameObject.getComponent<Script, Player>().isAttacking) {
                if (contact.contact_normal.x < 0 && !contact.gameObject.getComponent<Script, Player>().isRight) {
                    animator["Stunned"];
                    audioplayer["Hit"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                    rigidbody.velocity.y = 300 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                    t0 = clock();
                }
                if (contact.contact_normal.x > 0 && contact.gameObject.getComponent<Script, Player>().isRight) {
                    animator["Stunned"];
                    audioplayer["Hit"];
                    isStunned = true;
                    rigidbody.velocity.x = 0;
                    rigidbody.velocity.y = 300 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                    t0 = clock();
                }
            }
            if(!isStunned && (!contact.gameObject.getComponent<Script, Player>().isGrounded) && (contact.contact_normal.y < 0)){
                animator["Stunned"];
                audioplayer["Hit"];
                isStunned = true;
                rigidbody.velocity.x = 0;
                rigidbody.velocity.y = 300 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                t0 = clock();
            }
        }
        if (std::get<bool>(ini["Game"]["AdvancedAI"])) {
            if (!isStunned) {
                if ((contact.gameObject.tag == "Floor" || contact.gameObject.tag == "Wall") && (contact.contact_normal.y < 0)) {
                    if (contact.gameObject.tag == "Floor") {
                        if (contact.gameObject.hasSecondTag("Block")) {
                            floor_level = contact.gameObject.getComponent<Script, BlockBehavior>().floor_level;
                        } else {
                            floor_level = contact.gameObject.getComponent<Script, FloorBehavior>().floor_level;
                        }
                    }
                    if (random_val() > 0.95) {
                        isMoving = false;
                        rigidbody.velocity.x = 0;
                        rigidbody.velocity.y = 0;
                        animator["Idle"];
                    }
                }
            }
        }
    }
    
    double random_val() {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> E(0, 1);
        return E(e2);
    }

    void move(float deltaTime, std::string check_name, GameObject* check_ref) {
        if(!isStunned){
            t1 = clock();
            auto elapsed_time = (double(t1-t0)/CLOCKS_PER_SEC);

            if(elapsed_time < time) {
                if(transform.position.y > check_ref->getComponent<Transform2D>().position.y + check_ref->getComponent<Animator>().GetViewDimensions().y + (50 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF))){
                    rigidbody.velocity.y = -200 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                }else if(transform.position.y + animator.GetViewDimensions().y + (50 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)) < check_ref->getComponent<Transform2D>().position.y){
                    rigidbody.velocity.y = 200 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
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
            rigidbody.velocity.y = 200 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
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
                    transform.position.x = -50 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                    rigidbody.velocity.x = 100 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                    if(isRight == -1){
                        animator.Flip();
                        isRight *= -1;
                    }
                }else{
                    transform.position.x = WINDOW_WIDTH + (50 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF));
                    rigidbody.velocity.x = -100 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
                    if(isRight == 1){
                        animator.Flip();
                        isRight *= -1;
                    }
                }
                transform.position.y = 150 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
            }
        }

        transform.position.x += rigidbody.velocity.x * deltaTime;
        transform.position.y += rigidbody.velocity.y * deltaTime;
    }

    void move_ai() {
        if (!isStunned) {
            float less_distance = INFINITY;
            GameObject* less_player = nullptr;
            for (auto& p : players) {
                if (p != nullptr) {
                    Vector2 pos  = p->getComponent<Transform2D>().position;
                    float dis  = mod(transform.position - pos);
                    if (dis < less_distance) {
                        less_distance = dis;
                        less_player = p;
                    }
                }
            }
            Vector2 dif = nor(less_player->getComponent<Transform2D>().position - transform.position, 100 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF));
            rigidbody.velocity.x = dif.x;
            if ((rigidbody.velocity.x > 0 && isRight < 0) || (rigidbody.velocity.x < 0 && isRight > 0)) {
                isRight *= -1;
                animator.Flip();
            }  
            rigidbody.velocity.y = dif.y;
        } else {
            if (transform.position.y > GetScreenHeight()) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<int> dis(0, floor_levels.size()-1);
                int val = dis(gen);
                if (random_val() > 0.5) {
                    transform.position = {(float)GetScreenWidth()-GetRandomValue(40 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF), 200 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)), floor_levels[val] - GetRandomValue(30 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF), 100 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF))};
                    if (isRight > 0) {
                        isRight *= -1;
                        animator.Flip();
                    }
                } else {
                    transform.position = {(float)GetRandomValue(40 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF), 200 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)), floor_levels[val] - GetRandomValue(30 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF), 100 * (GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF))};
                    if (isRight < 0) {
                        isRight *= -1;
                        animator.Flip();
                    }
                }
                isMoving = false;
                animator["Idle"];
                floor_level = val+1;
                isStunned = false;
            }
        }

    }

    void Update() override {
        float deltaTime = GetFrameTime();
        if(t0 == 0) t0 = clock();
        bool primer = true;

        if (!std::get<bool>(ini["Game"]["AdvancedAI"])) {
            for (auto& [check_name, check_ref] : GameSystem::GameObjects["Player"]) {
                if(primer){ 
                    primer = false;
                    move(deltaTime, check_name, check_ref);
                    
                }
            }
        } else {
            for (auto& p : players) {
                if (p != nullptr) {
                    if (p->getComponent<Script, Player>().floor_level <= 8) {
                        if (!isMoving) {
                            if (random_val() > 0.00025) {
                                if (p->getComponent<Script, Player>().floor_level == floor_level) {
                                    move_ai();
                                    isMoving = true;
                                    animator["Walk"];
                                }
                            } else {
                                move_ai();
                                isMoving = true;
                                animator["Walk"];
                            }
                        } else {
                            move_ai();
                            transform.position.x += rigidbody.velocity.x * deltaTime;
                            transform.position.y += rigidbody.velocity.y * deltaTime;
                        }
                    }
                }
            }
        }
    }
};