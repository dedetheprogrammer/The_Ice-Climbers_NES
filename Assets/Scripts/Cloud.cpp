#include "Cloud.h"

int CloudBehavior::random_sense() {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> E(0, 1);
    return (E(e2) < 0.5) ? -1 : 1;
}

CloudBehavior::CloudBehavior(GameObject& gameObject) : Script(gameObject),
    rigidbody(gameObject.getComponent<RigidBody2D>()),
    sprite(gameObject.getComponent<Sprite>()),
    transform(gameObject.getComponent<Transform2D>())
{
    spawned = false;
    started = false;
    cooldown = current_cooldown = 2.0f;
}

CloudBehavior::CloudBehavior(GameObject& gameObject, CloudBehavior behavior) : Script(gameObject),
    rigidbody(gameObject.getComponent<RigidBody2D>()),
    sprite(gameObject.getComponent<Sprite>()),
    transform(gameObject.getComponent<Transform2D>())
{
    spawned = false;
    started = false;
    cooldown = current_cooldown = 2.0f;
}

Component* CloudBehavior::Clone(GameObject& gameObject) {
    return new CloudBehavior(gameObject, *this);
}

void CloudBehavior::Update() {
    if (!started) {
        if (current_cooldown >= cooldown) {
            current_cooldown = 0.0f;
            started = true;
            rigidbody.velocity.x = random_sense() * rigidbody.acceleration.x;
            if (rigidbody.velocity.x < 0) {
                transform.position.x = GetScreenWidth() + 10;
            } else {
                transform.position.x = -(sprite.GetViewDimensions().x + 10);
            }
        } else {
            current_cooldown += GetFrameTime();
        }
    } else {
        transform.position.x += rigidbody.velocity.x * GetFrameTime();
        if (!spawned){
            if (transform.position.x < (GetScreenWidth()+10) && rigidbody.velocity.x < 0) {
                spawned = true;
            }
            if ((transform.position.x + sprite.GetViewDimensions().x) > 10 && rigidbody.velocity.x > 0) {
                spawned = true;
            }
        } else {
            if (transform.position.x > GetScreenWidth() && rigidbody.velocity.x > 0) {
                started = spawned = false;
            }
            if (transform.position.x + sprite.GetViewDimensions().x <= 0 && rigidbody.velocity.x < 0) {
                started = spawned = false;
            }
        }
    }
}
