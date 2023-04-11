#include "EngineECS.h"
class test : public Script {
private:
    // ...
    Transform2D& transform;
public:
    test(GameObject& gameObject) : Script(gameObject),
        transform(gameObject.getComponent<Transform2D>()) {}
    void Update() override {
        transform.position.x += GetAxis("Horizontal") * 100 * GetFrameTime();
        transform.position.y += -GetAxis("Vertical")  * 100 * GetFrameTime();
    }
    void OnCollision(Collision contact) override {

    }
    Component* Clone(GameObject& gameObject) {
        return new test(gameObject);
    }
};