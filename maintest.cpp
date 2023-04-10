#include <iostream>
#include <random>
#include <sstream>

#include "EngineECS.h"
#include "raylib.h"
#include "raylibx.h"
#include "Grass_block.h"

/**
 * @brief GameObject physical limits.
 * 
 */
/*
class Collider2D {
private:
    // ...
public:
    enum COLLIDER_ENUM { UKNOWN = -1, PLAYER, ENEMY, PROJECTILE, WALL, FLOOR };
    Vector2* pos; // Nuevo item. Coge el centro de nuestro objeto padre y se
                  // actualiza la posición actual.
    Vector2 size; // Dimensiones del collider.
    
    Collider2D() {}
    Collider2D(Vector2* pos, int width, int height)
        : pos(pos), size({(float)width, (float)height}) {}
    Collider2D(Vector2* pos, Vector2 size)
        : pos(pos), size(size) {}

    void Draw(Color color = {129, 242, 53, 255}) {
        DrawRectangleLinesEx({pos->x, pos->y, size.x, size.y}, 3.0f, color);
    }
};

struct Object {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    int width;
    int height;
    int sense;
    bool isDragging = false;
    Collider2D collider;

    Object(Vector2 pos, Vector2 vel, Vector2 acc, int width, int height, int sense) {
        this->pos    = pos;
        this->vel    = {0,0};
        this->acc    = acc;
        this->width  = width;
        this->height = height;
        collider = Collider2D(&this->pos, width, height);
    }
    
    void Draw(bool draw_collider, Color color) {
        DrawRectangle(pos.x, pos.y, width, height, color);
        if (draw_collider) {
            collider.Draw();
        }
    }
};
*/
/*
bool Collides(const Vector2 ray_o, const Vector2 ray_d,
    const Collider2D& target, Vector2& contact_point, Vector2& contact_normal,
    float& contact_time)
{
    DrawLineEx(ray_o, ray_o + 10000 * ray_d, 2.0f, PINK);
    Vector2 ray_i = 1.0f/ray_d;
    Vector2 t_near = (*target.pos - ray_o) * ray_i;
    Vector2 t_far  = (*target.pos + target.size - ray_o) * ray_i;

    if (std::isnan(t_near.x) || std::isnan(t_near.y)) return false;
    if (std::isnan(t_far.x)  || std::isnan(t_far.y) ) return false;

    if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
    if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

    if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

    contact_time = std::max(t_near.x, t_near.y); // contact_time = t_hit_near;
    float t_hit_far = std::min(t_far.x, t_far.y);
    if (t_hit_far < 0) {
        return false;
    }
    contact_point  = ray_o + (contact_time * ray_d);

    DrawCircleV(ray_o + t_hit_far * ray_d, 10.0f, RED);
    if (t_near.x > t_near.y) {
        if (ray_i.x < 0) {
            contact_normal = {1,0};
        } else {
            contact_normal = {-1,0};
        }
    } else if (t_near.x < t_near.y) {
        if (ray_i.y < 0) {
            contact_normal = {0,1};
        } else {
            contact_normal = {0,-1};
        }
    }
    return true;
}

// Lo que hacemos aqui es extender el Collider a nuevas dimensiones, cogemos el 
// la anchura y se extiende w/2 del collider A y la altura h/2 del collider A, 
// se extiende por ambos lados.
// De momento A es dinamico y B es estatico:
bool Collides(const Collider2D& A, const Vector2& vA, const Collider2D& B, 
    Vector2& contact_point, Vector2& contact_normal, float& contact_time)
{
    // Expandir el rectangulo destino con las dimensiones del rectangulo origen.
    Vector2 exp_B_pos = *B.pos - (A.size/2);
    Collider2D exp_B = Collider2D(B.gameObject, &exp_B_pos, B.size + A.size);
    // Se coge el centro del rectangulo:
    if (Collides(*A.pos + A.size/2, vA, exp_B, contact_point, contact_normal, contact_time)) {
        return (contact_time >= 0.0f && contact_time < 1.0f);
    }
    return false;
}
*/

/*
int main() {

    //float gravity = 98.0f;
    //float jump_force = 200.0f, weight = 1.0f;
    //float brake_force = 100.0f;
    //bool isGrounded = false;

    InitWindow(800, 800, "COÑO");
    Object A({365,100},{0,0},{100,100},70,70, 1);
    Object B({100,300},{0,0},{0,0},600,200, 1);
    Object C({100,365},{0,0},{250,250},70,70, 1);

    while(!WindowShouldClose()) {

        float deltaTime = GetFrameTime();
        if (IsKeyDown(KEY_RIGHT)) {
            A.vel.x =  A.acc.x;
        } else if (IsKeyDown(KEY_LEFT)) {
            A.vel.x = -A.acc.x;
        } else {
            A.vel.x =    0.0f;
        }
        if (IsKeyDown(KEY_UP)) {
            A.vel.y = -A.acc.y;
        } else if (IsKeyDown(KEY_DOWN)) {
            A.vel.y =  A.acc.y;
        } else {
            A.vel.y =    0.0f;
        }

        if (IsKeyDown(KEY_D)) {
            C.vel.x =  C.acc.x;
        } else if (IsKeyDown(KEY_A)) {
            C.vel.x = -C.acc.x;
        } else {
            C.vel.x =    0.0f;
        }
        if (IsKeyDown(KEY_W)) {
            C.vel.y = -C.acc.y;
        } else if (IsKeyDown(KEY_S)) {
            C.vel.y =  C.acc.y;
        } else {
            C.vel.y =    0.0f;
        }

        float ct;        // T_hit_near.
        Vector2 cp, cn;  // Contact point and contact normal.
        BeginDrawing();
        ClearBackground(BLACK); 

        A.Draw(true, RED);
        C.Draw(true, VIOLET);
        if (Collides(A.collider, A.vel * deltaTime, C.collider, cp, cn, ct)) {
            A.vel += cn * abs(A.vel) * (1 - ct);
        } else {
            A.pos.y += A.vel.y * GetFrameTime();
            if (A.pos.x + A.width > GetScreenWidth()) {
                A.vel *= -1;
            }
            if (A.pos.y <= 100) {
                A.sense =  1;
                A.vel.y = A.acc.y;
            } else if (A.pos.y >= 700) {
                A.sense = -1;
                A.vel.y = - A.acc.y;
            } else {
                A.vel.y = A.sense * A.acc.y;
            }
        }
        if (Collides(C.collider, C.vel * deltaTime, A.collider, cp, cn, ct)) {
            C.vel += cn * abs(C.vel) * (1 - ct);
        } else {
            C.pos.x += C.vel.x * GetFrameTime();
            if (C.pos.x <= 100) {
                C.sense = 1;
                C.vel.x = C.acc.x;
            } else if (C.pos.x >= 700) {
                C.sense = -1;
                C.vel.x = -C.acc.x;
            } else {
                C.vel.x = C.sense * C.acc.y;
            }
        }
        if (Collides(A.collider, A.vel * deltaTime, B.collider, cp, cn, ct)) {
            B.Draw(true, GREEN);
            DrawCircleV(cp, 10.0f, RED);
            DrawLineEx(cp, cp + cn * 40.0f, 2.0f, YELLOW);
            A.vel += cn * abs(A.vel) * (1 - ct);
            if (cn.y > 0) {
                std::cout << "COLISIONA POR ABAJO\n";
            }
            if (cn.x < 0) {
                std::cout << "COLISIONA POR LA IZQUIERDA\n";
            }
            if (cn.y < 0) {
                std::cout << "COLISIONA POR ARRIBA\n";
            }
            if (cn.x > 0) {
                std::cout << "COLISIONA POR LA DERECHA\n";
            }
        } else {
            std::cout << "NO COLISIONA...\n";
            B.Draw(true, BLUE);
        }
        EndDrawing();
    }
}
*/

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 1);
int sense() {
    return dis(gen) == 0 ? -1 : 1;
}

int main() {

    GameObject A("A"), B("B");
    A.addComponent<Transform2D>(Vector2{365,200});
    A.addComponent<Collider2D>(&A.getComponent<Transform2D>().position, Vector2{70,70});
    A.addComponent<RigidBody2D>(1, 98, Vector2{100,100}, Vector2{250.0f,250.0f});
    A.getComponent<RigidBody2D>().velocity = {250.0f * sense(), 250.0f * sense()};
    A.addComponent<Script, GrassBlockBehavior>();

    B.addComponent<Transform2D>(Vector2{300,365});
    B.addComponent<Collider2D>(&B.getComponent<Transform2D>().position, Vector2{100,100});
    B.addComponent<RigidBody2D>(1, 98, Vector2{100,100}, Vector2{100,100});
    B.getComponent<RigidBody2D>().velocity = {100.0f * sense(), 100.0f * sense()};

    GameObject C(A);
    std::cout << C.name << "\n";
    std::cout << A.getComponent<Transform2D>().position << "\n";
    std::cout << C.getComponent<Transform2D>().position << "\n";
    A.getComponent<Transform2D>().position = Vector2{100,100};
    std::cout << A.getComponent<Transform2D>().position << "\n";
    std::cout << C.getComponent<Transform2D>().position << "\n";
    
    GameSystem::Instantiate(A, Vector2{0,0});
    GameSystem::Instantiate(A, Vector2{10,10});
    GameSystem::Update();

    std::cout << A.getComponent<Script, GrassBlockBehavior>().tu_vieja << "\n";
    std::cout << C.getComponent<Script, GrassBlockBehavior>().tu_vieja << "\n";
    A.getComponent<Script, GrassBlockBehavior>().tu_vieja = "ME VOY A CORTAR LOS COJONES";
    std::cout << A.getComponent<Script, GrassBlockBehavior>().tu_vieja << "\n";
    std::cout << C.getComponent<Script, GrassBlockBehavior>().tu_vieja << "\n";


    /*
    InitWindow(800, 800, "COÑO");
    while (!WindowShouldClose()) {

        float deltaTime = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        float ct; Vector2 cp, cn;
        if (CollisionSystem::Collides(A.getComponent<Collider2D>(), B.getComponent<Collider2D>(), cp, cn, ct)) {
            if (cn.x != 0) {
                A.getComponent<RigidBody2D>().velocity.x *= -1;
            } else if (cn.y != 0) {
                A.getComponent<RigidBody2D>().velocity.y *= -1;
            }
            //A.getComponent<RigidBody2D>().velocity = cn * abs(A.getComponent<RigidBody2D>().velocity) * (1-ct);
            DrawRectangleV(A.getComponent<Transform2D>().position, {70,70}, YELLOW);
        } else {
            A.getComponent<Transform2D>().position += A.getComponent<RigidBody2D>().velocity * deltaTime;
            if (A.getComponent<Transform2D>().position.x <= 0 || A.getComponent<Transform2D>().position.x + 70 >= GetScreenWidth()) {
                A.getComponent<RigidBody2D>().velocity.x *= -1;
            } else if (A.getComponent<Transform2D>().position.y <= 0 || A.getComponent<Transform2D>().position.y + 70 >= GetScreenHeight()) {
                A.getComponent<RigidBody2D>().velocity.y *= -1;
            }
            DrawRectangleV(A.getComponent<Transform2D>().position, {70,70}, RED);
        }
        A.getComponent<Collider2D>().Draw();

        if (CollisionSystem::Collides(B.getComponent<Collider2D>(), A.getComponent<Collider2D>(), cp, cn, ct)) {
            if (cn.x != 0) {
                B.getComponent<RigidBody2D>().velocity.x *= -1;
            } else if (cn.y != 0) {
                B.getComponent<RigidBody2D>().velocity.y *= -1;
            }
            //B.getComponent<RigidBody2D>().velocity += cn * abs(B.getComponent<RigidBody2D>().velocity) * (1-ct);
            DrawRectangleV(B.getComponent<Transform2D>().position, {100,100}, YELLOW);
        } else {
            B.getComponent<Transform2D>().position += B.getComponent<RigidBody2D>().velocity * deltaTime;
            if (B.getComponent<Transform2D>().position.x <= 0 || B.getComponent<Transform2D>().position.x + 100 >= GetScreenWidth()) {
                B.getComponent<RigidBody2D>().velocity.x *= -1;
            } else if (B.getComponent<Transform2D>().position.y <= 0 || B.getComponent<Transform2D>().position.y + 100 >= GetScreenHeight()) {
                B.getComponent<RigidBody2D>().velocity.y *= -1;
            }
            DrawRectangleV(B.getComponent<Transform2D>().position, {100,100}, VIOLET);
        }
        B.getComponent<Collider2D>().Draw();
        EndDrawing();
    }
    */
}