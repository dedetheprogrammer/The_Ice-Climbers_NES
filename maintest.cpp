#include <sstream>
#include <iostream>
#include "raylib.h"
#include <typeindex>

class PuertoRICO {
public:
    Vector2 pos;
    int width;
    int height;
    bool isDragging = false;
    //Collider2D collider;

    PuertoRICO(Vector2 pos, int width, int height) {
        this->pos    = pos;
        this->width  = width;
        this->height = height;
        //collider = Collider2D(&this->pos, width, height);
    }
    
    void Draw(bool draw_collider, Color color) {
        //DrawRectangle(pos.x, pos.y, width, height, color);
        //if (draw_collider) {
        //    collider.Draw();
        //}
    }
};


int main() {

    PuertoRICO A({365,100},70,70);

    /*
    float gravity = 98.0f;
    Vector2 v { 0.0f, 0.0f };
    float jump_force = 200.0f, weight = 1.0f;
    float brake_force = 100.0f;
    Vector2 a { 100.0f, 100.0f };
    bool isGrounded = false;

    InitWindow(800, 800, "COÑO");
    Object A({365,100},70,70);
    Object B({100,300},600,200);

    while(!WindowShouldClose()) {



        float deltaTime = GetFrameTime();
        //std::stringstream ss;

        //auto collided = Collides(A.collider, B.collider);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), {A.pos.x, A.pos.y, (float)A.width, (float)A.height})) {
                A.isDragging = true;
            }
        }
        if (A.isDragging) {
            A.pos = GetMousePosition();
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            A.isDragging = false;
        }

        //Vector2 mp = GetMousePosition(), ray_o = {400,100}, ray_d = mp - ray_o;
        if (IsKeyDown(KEY_RIGHT)) {
            v.x =  100.0f;
        } else if (IsKeyDown(KEY_LEFT)) {
            v.x = -100.0f;
        }
        if (isGrounded) {
            if (IsKeyDown(KEY_SPACE)) {
                v.y = -jump_force * weight;
                isGrounded = false;
            }
        }

        float ct;        // T_hit_near.
        Vector2 cp, cn; // Contact point and contact normal.
        //auto collided = Collides(A.collider, v * deltaTime, B.collider, cp, cn, ct);
        BeginDrawing();
        ClearBackground(BLACK); 

        A.Draw(true, RED);
        if (collided) {
            B.Draw(true, GREEN);
            //v += cn * abs(v) * (1 - ct);
            //v.x -= sgn(v.x) * brake_force * deltaTime;
            isGrounded = true;
        } else {
            A.pos += v * deltaTime;
            v.y += gravity * deltaTime;
            B.Draw(true, BLUE);
        }
        EndDrawing();
    }
    */
    std::cout << typeid(A).name() << std::endl;

}