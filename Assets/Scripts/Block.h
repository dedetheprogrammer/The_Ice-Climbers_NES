#include "EngineECS.h"
#include <iostream>

class BlockBehavior : public Script {
private:
    Sprite& sprite;
    Transform2D& transform;
    GameObject hole;
public:
    BlockBehavior(GameObject& gameObject) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>())
    {
        hole = GameObject(gameObject.name + " Hole", "Hole");
        hole.addComponent<Collider2D>(&transform.position, sprite.GetViewDimensions(), RED);
    }
    Component* Clone(GameObject& gameObject) {
        return new BlockBehavior(gameObject);
    }

    void Update() override {

    }

    void OnCollision(Collision contact) override {
        if (contact.gameObject.tag == "Player") {
            if (contact.contact_normal.y < 0) {
                std::cout << "Popo me ha violado violentamente\n";
                GameSystem::Instantiate(hole, GameObjectOptions{.position = transform.position});
                gameObject.Destroy();
            }
        }
    }

};