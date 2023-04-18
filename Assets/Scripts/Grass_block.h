#include "EngineECS.h"

class GrassBlockBehavior : public Script {
private:
    Sprite& sprite;
    Transform2D& transform;
    Collider2D& collider;
    bool active = true;
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
        if (contact.gameObject.tag == "Player") {
            if (contact.contact_normal.y < 0 && collider.active) {
                collider.active = false;
                //sprite.Unload();
                sprite.ChangeTexture("Assets/Sprites/block_invisible.png");
            }
        }
    }
};