#pragma once
#ifndef COLLIDER_H
#define COLLIDER_H

#include <iostream>
#include <vector>
#include "raylib.h"
#include "raylib_ext.h"

class Collider {
private:
    // ...
public:
    enum Type { UKNOWN = -1, PLAYER, ENEMY, PROJECTILE, WALL, FLOOR };
    Vector2* pos; // Nuevo item. Coge el centro de nuestro objeto padre y se
                  // actualiza la posición actual.
    Vector2 size; // Dimensiones del collider.

    Collider() : pos(nullptr), size({0.0f,0.0f}) {}
    Collider(Vector2* pos, int width, int height) : pos(pos), size({(float)width, (float)height}) {}
    Collider(Vector2* pos, Vector2 size) : pos(pos), size(size) {}

    void Draw(Color color = {129, 242, 53, 255}) {
        DrawRectangleLinesEx({pos->x, pos->y, size.x, size.y}, 3.0f, color);
    }
};


// Esto no me iba en Grafica pero aqui si, alucinante. Teneis la teoria aqui,
// ahora no me apetece explicarla: 
// - https://www.youtube.com/watch?v=8JJ-4JgR7Dg
// - https://github.com/OneLoneCoder/Javidx9/blob/master/PixelGameEngine/SmallerProjects/OneLoneCoder_PGE_Rectangles.cpp
static bool Collides(const Vector2 ray_o, const Vector2 ray_d, const Collider& target,
    Vector2& contact_point, Vector2& contact_normal, float& contact_time)
{
    //DrawLineEx(ray_o, ray_o + 10000 * ray_d, 2.0f, PINK);
    Vector2 ray_i = 1.0f/ray_d;
    Vector2 t_near = (*target.pos - ray_o) * ray_i;
    Vector2 t_far  = (*target.pos + target.size - ray_o) * ray_i;

    if (std::isnan(t_near.x) || std::isnan(t_near.y)) return false;
    if (std::isnan(t_far.x) || std::isnan(t_far.y)) return false;

    if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
    if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

    if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

    contact_time = std::max(t_near.x, t_near.y); // contact_time = t_hit_near;
    float t_hit_far = std::min(t_far.x, t_far.y);
    if (t_hit_far < 0) return false;
    contact_point  = ray_o + (contact_time * ray_d);

    //DrawCircleV(ray_o + t_hit_far * ray_d, 10.0f, RED);
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
// De momento A es dinamico y B es estatico, 
static bool Collides(const Collider& A, const Vector2& vA, const Collider& B, 
    Vector2& contact_point, Vector2& contact_normal, float& contact_time)
{
    // Expandir el rectangulo destino con las dimensiones del rectangulo origen.
    Vector2 exp_B_pos = *B.pos - (A.size/2);
    Collider exp_B = Collider(&exp_B_pos, B.size + A.size);
    // Se coge el centro del rectangulo:
    if (Collides(*A.pos + A.size/2, vA, exp_B, contact_point, contact_normal, contact_time)) {
        return (contact_time >= 0.0f && contact_time <= 1.0f);
    }
    return false;
}

// Luego vendra el dinamismo para ambos. Va mas o menos, puede pulirse.
static bool Collides(const Collider& A, const Vector2& vA, Vector2& cpA, Vector2& cnA, float& ctA,
    const Collider& B, const Vector2& vB, Vector2& cpB, Vector2& cnB, float& ctB)
{
    // Expandir el rectangulo destino con las dimensiones del rectangulo origen.
    Vector2 exp_A_pos = *A.pos - (B.size/2), exp_B_pos = *B.pos - (A.size/2);
    Collider exp_A = Collider(&exp_A_pos, A.size + B.size), exp_B = Collider(&exp_B_pos, B.size + A.size);
    // Se coge el centro del rectangulo:
    auto cA = Collides(*A.pos + A.size/2, vA, exp_B, cpA, cnA, ctA);
    auto cB = Collides(*B.pos + B.size/2, vB, exp_A, cpB, cnB, ctB);
    if (!cA && !cB) {
        return false;
    }
    return (ctA >= 0.0f && ctA < 1.0f) || (ctB >= 0.0f && ctB < 1.0f);
}


#endif