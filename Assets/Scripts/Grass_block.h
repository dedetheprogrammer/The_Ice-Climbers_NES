#include "EngineECS.h"

class GrassBlockBehavior : public Script {
private:
    Sprite& sprite;
    std::string texturePath;
    Transform2D& transform;
    Collider2D& collider;
    bool active = true;
public:
    GrassBlockBehavior(GameObject& gameObject, std::string texturePath) : Script(gameObject),
        sprite(gameObject.getComponent<Sprite>()),
        texturePath(texturePath),
        transform(gameObject.getComponent<Transform2D>()),
        collider(gameObject.getComponent<Collider2D>()) {}

    Component* Clone(GameObject& gameObject) {
        return new GrassBlockBehavior(gameObject, texturePath);
    }

    void Update() override {
        sprite.Draw(); 
        collider.Draw();
    }

    void OnCollision(Collision contact) override {
        if (contact.gameObject.tag == "Player") {
            /*if (contact.contact_normal.y < 0 && collider.active) {
                collider.active = false;
                //sprite.Unload();
                sprite.ChangeTexture("Assets/Sprites/block_invisible.png");
            }*/
        }
        else if (contact.gameObject.tag == "Cone") {
            if (!collider.active) {
                collider.active = true;
                //sprite.Unload();
                sprite.ChangeTexture(texturePath.c_str());
            }
        }
    }
};