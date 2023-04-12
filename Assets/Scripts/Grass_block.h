#include "EngineECS.h"

class GrassBlockBehavior : public Script {
private:
    Sprite& sprite;
    Transform2D& transform;
public:
    GrassBlockBehavior(GameObject& gameObject) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()) {}
    Component* Clone(GameObject& gameObject) {
        return new GrassBlockBehavior(gameObject);
    }

    void Update() override {
        sprite.Draw();
    }

    void OnCollision(Collision contact) override {
        if (contact.gameObject.name == "Popo") {
            std::cout << "My name is " << gameObject.name << " White, I live in your mom.\n";
        }
    }

};