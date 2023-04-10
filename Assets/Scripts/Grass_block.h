#include "EngineECS.h"

class GrassBlockBehavior : public Script {
private:
    //Sprite& sprite;
    Transform2D& transform;
public:
    std::string tu_vieja;

    GrassBlockBehavior(GameObject& gameObject) : Script(gameObject),
        //sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()) {
            tu_vieja = "TU PUTA MADRE";
        }
    GrassBlockBehavior(GameObject& gameObject, GrassBlockBehavior& gbb) : Script(gameObject),
        //sprite(gameObject.getComponent<Sprite>()),
        transform(gameObject.getComponent<Transform2D>()) {
            tu_vieja = gbb.tu_vieja;
        }
    Component* Clone(GameObject& gameObject) {
        return new GrassBlockBehavior(gameObject);
    }

    void Update() override {
        //sprite.Draw(transform.position);
    }

    void OnCollision(Collision contact) override {
        // Nada
    }
};