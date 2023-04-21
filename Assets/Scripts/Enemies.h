#include "EngineECS.h"
#include "Popo.h"

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

    void OnCollision(Collision contact) {}
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

class TopiBehavior : public Script {
private:
    // Variables para Popo:
    float current_cooldown;
    float cooldown;
    bool started;
    bool spawned;
    bool isGrounded;  // Telling us if the object is on the ground.
    bool isRunning;
    bool isRight;     // Telling us if the objsdect is facing to the right.
    bool isStunned;

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

    TopiBehavior(GameObject& gameObject) : Script(gameObject), 
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        cooldown = current_cooldown = 3.0f;
        started     = false;
        spawned     = false;
        isGrounded  = false;
        isRunning   = false;
        isRight     = true;
        isStunned   = false;
    }

    TopiBehavior(GameObject& gameObject, TopiBehavior& behavior) : Script(gameObject),
        animator(gameObject.getComponent<Animator>()),
        collider(gameObject.getComponent<Collider2D>()),
        rigidbody(gameObject.getComponent<RigidBody2D>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        cooldown = current_cooldown = 3.0f;
        started     = false;
        spawned     = false;
        isGrounded  = behavior.isGrounded;
        isRight     = behavior.isRight;
        isRunning   = behavior.isRunning;
        isStunned   = behavior.isStunned;
    }

    Component* Clone(GameObject& gameObject) override {
        return new TopiBehavior(gameObject, *this);
    }

    void OnCollision(Collision contact) override {

        if (contact.gameObject.tag == "Floor") {
            rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
            isGrounded = true;
        }
        if (contact.gameObject.tag == "Hole") {
            int pos_x1 = contact.gameObject.getComponent<Collider2D>().Pos().x, pos_x2 = pos_x1 + contact.gameObject.getComponent<Collider2D>().size.x,
                pos_p2 = transform.position.x + collider.size.x;
            if (pos_p2 <= pos_x1 || transform.position.x >= pos_x2) {
                rigidbody.velocity.x *= -2;
                if (rigidbody.velocity.x > 0 && !isRight) {
                    isRight = !isRight;
                    animator.Flip();
                }
                if (rigidbody.velocity.x < 0 && isRight) {
                    isRight = !isRight;
                    animator.Flip();
                }
                if (isGrounded && animator.GetViewDimensions().x > contact.gameObject.getComponent<Transform2D>().position.x) {
                    animator["Fall"];
                    rigidbody.velocity.x = 0;
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
        /*
        if (contact.gameObject.tag != "Floor") std::cout << "Soy Topi, me choco con: " << contact.gameObject.tag << std::endl;

        if (contact.gameObject.tag == "Floor") {
            if (contact.contact_normal.y < 0) {
                if(!contact.gameObject.getComponent<Collider2D>().active) {
                    if(isRunning) {
                        animator["Stunned"];
                    } else {
                        rigidbody.velocity.x = rigidbody.max_velocity.x * 2;
                        isRunning = true;
                        animator.Flip();
                        move *= -1;
                    }
                } else {
                    rigidbody.velocity.y += contact.contact_normal.y * std::abs(rigidbody.velocity.y) * (1 - contact.contact_time) * 1.05;
                }
            }
        } else if (contact.gameObject.tag == "Player") { // Se ocupa el player
            
        }
        */
    }

    void Update() override {
        float deltaTime = GetFrameTime();
        if (!started) {
            if (current_cooldown >= cooldown) {
                current_cooldown = 0.0f;
                started = true;
                isStunned = false;
                animator["Walk"];
                rigidbody.velocity.x = random_sense() * rigidbody.acceleration.x;
                if (rigidbody.velocity.x < 0) {
                    transform.position.x = GetScreenWidth() + 10;
                    if (isRight) {
                        isRight = !isRight;
                        animator.Flip();
                    }
                } else {
                    transform.position.x = -(animator.GetViewDimensions().x + 10);
                    if (!isRight) {
                        isRight = !isRight;
                        animator.Flip();
                    }
                }
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
    }
};