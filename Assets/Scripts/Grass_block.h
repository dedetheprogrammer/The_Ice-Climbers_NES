#include "EngineECS.h"

class GrassBlockBehavior : public Script {
private:
    Sprite& sprite;
    Transform2D& transform;
    Collider2D& collider;
public:
    GrassBlockBehavior(GameObject& gameObject) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()),
        collider(gameObject.getComponent<Collider2D>()) {}
    Component* Clone(GameObject& gameObject) {
        return new GrassBlockBehavior(gameObject);
    }

    void Update() override {
        sprite.Draw();
        collider.Draw();
    }

    void OnCollision(Collision contact) override {
        // Nada
    }

};