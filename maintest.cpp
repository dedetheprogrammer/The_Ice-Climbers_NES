#include "raylib.h"
#include <sstream>
//#include "./Colisiones/colisiones.h"
#include "collider.h"
#include <iostream>

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

struct Object {
    Vector2 pos;
    int width;
    int height;
    bool isDragging = false;
    Collider collider;

    Object(Vector2 pos, int width, int height) {
        this->pos    = pos;
        this->width  = width;
        this->height = height;
        collider = Collider(&this->pos, width, height);
    }
    
    void Draw(bool draw_collider, Color color) {
        DrawRectangle(pos.x, pos.y, width, height, color);
        if (draw_collider) {
            collider.Draw();
        }
    }
};


int main() {

    float gravity = 98.0f;
    Vector2 vA { 0.0f, 0.0f };
    Vector2 aA { 100.0f, 100.0f };
    Vector2 vC { 0.0f, 0.0f };
    Vector2 aC { 100.0f, 100.0f };

    InitWindow(800, 800, "COÑO");
    Object A({365,100},70,70);
    Object B({300,300},200,200);
    Object C({365,650},70,70);

    while(!WindowShouldClose()) {

        float deltaTime = GetFrameTime();
        //auto collided = Collides(A.collider, B.collider);
        /*
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
        */
        //Vector2 mp = GetMousePosition(), ray_o = {400,100}, ray_d = mp - ray_o;
        Vector2 cpA, cnA, cpC, cnC; // Contact point and contact normal.
        float ctA, ctC;        // T_hit_near.

        if (IsKeyDown(KEY_RIGHT)) {
            vA.x =  aA.x;
        } else if (IsKeyDown(KEY_LEFT)) {
            vA.x = -aA.x;
        } else {
            vA.x =  0;            
        }
        if (IsKeyDown(KEY_UP)) {
            vA.y = -aA.y;
        } else if (IsKeyDown(KEY_DOWN)) {
            vA.y =  aA.y;
        } else {
            vA.y =  0;
        }

        if (IsKeyDown(KEY_D)) {
            vC.x =  aC.x;
        } else if (IsKeyDown(KEY_A)) {
            vC.x = -aC.x;
        } else {
            vC.x =  0;            
        }
        if (IsKeyDown(KEY_W)) {
            vC.y = -aC.y;
        } else if (IsKeyDown(KEY_S)) {
            vC.y =  aC.y;
        } else {
            vC.y =  0;
        }

        auto collided_A = Collides(A.collider, vA * GetFrameTime(), B.collider, cpA, cnA, ctA);
        //auto collided_C = Collides(A.collider, vA * deltaTime, cpA, cnA, ctA, C.collider, vC * deltaTime, cpC, cnC, ctC);
        BeginDrawing();
        ClearBackground(BLACK); 
        std::stringstream ss;

        A.Draw(true, RED);
        if (collided_A) {
            B.Draw(true, GREEN);
            //DrawCircleV(cp, 10.0f, RED);
            //DrawLineEx(cp, cp + cn * 40.0f, 2.0f, YELLOW);
            vA += cnA * abs(vA) * (1 - ctA);
            if (cnA.y > 0) {
                ss << "COLISIONA POR ABAJO";
            }
            if (cnA.x < 0) {
                ss << "COLISIONA POR LA IZQUIERDA";
            }
            if (cnA.y < 0) {
                ss << "COLISIONA POR ARRIBA";
            }
            if (cnA.x > 0) {
                ss << "COLISIONA POR LA DERECHA";
            }
        } else {
            ss << "NO COLISIONA...";
            A.pos += vA * GetFrameTime();
            B.Draw(true, BLUE);
        }

        C.Draw(true, VIOLET);
        /*
        if (collided_C) {
            A.Draw(true, YELLOW);
            C.Draw(true, YELLOW);
            //DrawCircleV(cp, 10.0f, RED);
            //DrawLineEx(cp, cp + cn * 40.0f, 2.0f, YELLOW);
            vA += cnA * abs(vA) * (1 - ctA);
            vC += cnC * abs(vA) * (1 - ctC);
            if (cnA.y > 0) {
                ss << "A(ABAJO)";
            }
            if (cnA.x < 0) {
                ss << "A(IZQUIERDA)";
            }
            if (cnA.y < 0) {
                ss << "A(ARRIBA)";
            }
            if (cnA.x > 0) {
                ss << "A(DERECHA)";
            }
            
            if (cnC.y > 0) {
                ss << ", C(ABAJO)";
            }
            if (cnC.x < 0) {
                ss << ", C(IZQUIERDA)";
            }
            if (cnC.y < 0) {
                ss << ", C(ARRIBA)";
            }
            if (cnC.x > 0) {
                ss << ", C(DERECHA)";
            }
        } else {
            ss << "NO COLISIONAN...";
            A.pos += vA * deltaTime;
            C.pos += vC * deltaTime;
        }
        */

        ss << "\nCONTACT POINT: " << cpA
           << "\nCONTACT NORMAL: " << cnA
           << "\nNEAR HIT: " << ctA
           << "\nPOSITION: " << *A.collider.pos
           << "\nVELOCITY: " << vA;
        DrawText(ss.str().c_str(), 100, 100, 20, YELLOW);
        //DrawLineV(ray_o, mp, PINK);
        EndDrawing();
    }

    /*

    Object ch({350, 650, 70, 70}, Rectangulo({350,650},{70,70}));
    Collider c(Rectangulo({0,720}, {800,80}));
    
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        Rectangle r {ch.box_collider.shape.x-1, ch.box_collider.shape.y-1, ch.box_collider.shape.width+1, ch.box_collider.shape.height+1};
        DrawRectanglePro(ch.sprite, {0,0}, 0, RED);
        DrawRectangleLinesEx(r, 2.0f, (Color){129, 242, 53, 255});
        c.shape.Draw(BLUE);
        if (IsKeyDown(KEY_RIGHT)) {
            curr_speed.x = accel.x;
        } else if (IsKeyDown(KEY_LEFT)) {
            curr_speed.x = -accel.x;
        } else {
            curr_speed.x = 0;            
        }

        if (!ch.is_jumping && IsKeyPressed(KEY_SPACE)) {
            curr_speed.y = accel.y;
            ch.is_jumping = 1;
        }
        
        ch.sprite.x += curr_speed.x * GetFrameTime();
        auto curr_coll = CollisionHelper::CollidesV(c.shape, {0,0}, ch.box_collider.shape, curr_speed);
		std::stringstream t;
        t << "Is valid? " << curr_coll.valid
          << "\nPosition: " << c.shape.x << "," << c.shape.y
          << "\nNormal: " << curr_coll.colNormal.x << "," << curr_coll.colNormal.y ;
		DrawText(t.str().c_str(), 100, 100, 20, YELLOW);
        if (curr_speed.y <= 0 && curr_coll.valid) {
            curr_speed.y = 0;
            ch.is_jumping = 0;
        }
        if (ch.is_jumping) {
            ch.sprite.y -= curr_speed.y * GetFrameTime();
            ch.box_collider.shape.y = ch.sprite.y;
            curr_speed.y -= gravity * GetFrameTime();
        }
        EndDrawing();
    }
    */

}