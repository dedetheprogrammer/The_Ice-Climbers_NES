#pragma once
#ifndef ECS_H
#define ECS_H

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include "raylib.h"
#include "raylib_ext.h"

// ============================================================================
// ============================================================================
// COMPONENTS
// ============================================================================
// ============================================================================
/**
 * @brief A GameObject component.
 * 
 */
class Component {
private:
    //...
public:
    //enum COMPONENT_ENUM { ANIMATOR, AUDIOSOURCE };
    // Un poco de spoiler de lo que se viene...
    Component() {}
    virtual ~Component() = default;
    // Unloads those components that were loaded in memory.
    virtual void Unload() = 0;
};

//-----------------------------------------------------------------------------
// Animations
//-----------------------------------------------------------------------------´
/**
 * @brief An animation made of sprites.
 * 
 */
class Animation {
private:
    // Animation properties:
    Texture2D spritesheet;    // Animation spritesheet.
    int frame_width;          // Width of each the sprite.
    int frame_height;         // Height of each sprite.
    int frames_per_row;       // (NOT IMPLEMENTED) Frames per row.
    int frames_rows;          // (NOT IMPLEMENTED) Total frames rows.
    int total_frames;         // Total number of frames.
    // Animation state:
    int current_frame;        // Current frame.
    int current_row;          // (NOT IMPLEMENTED) Current frames row.
    // Animation rendering:
    float scale;              // Sprite scaling.
    Rectangle frame_src;      // Covers the area of the current frame.
    Rectangle frame_dst;      // Covers the area where to draw the current frame.
    // Animation timing:
    float duration;           // How long will the animation last?
    float frame_lapse;        // Play time of a single frame.
    float current_frame_time; // Time since the current frame started.
    float current_time;       // Time since the animation started.
    bool loop;                // Repeat the animation in loop.

public:

    Animation() {
        // Animation properties:
        frame_width = 0;
        frame_height = 0;
        frames_per_row = 0; 
        frames_rows = 0;
        total_frames = 0;
        // Animation state:
        current_frame = 0;
        current_row = 0;
        // Animation rendering:
        scale = 0.0f;
        frame_src = {};
        frame_dst = {};
        // Animation timing:
        duration = 0.0f;
        frame_lapse = 0.0f;
        current_frame_time = 0.0f;
        current_time = 0.0f;
        loop = false;
    }

    Animation(const char* fileName, int frame_width, int frame_height, float scale, float duration, bool loop) {
        // Animation properties:
        spritesheet = LoadTexture(fileName);
        this->frame_width = frame_width;
        this->frame_height = frame_height;
        frames_per_row = spritesheet.width/frame_width; 
        frames_rows = spritesheet.height/frame_height;
        total_frames = frames_rows * frames_per_row;
        // Animation state:
        current_frame = 0;
        current_row = 0;
        // Animation rendering:
        this->scale = scale;
        frame_src = {0,0,(float)frame_width,(float)frame_height};
        frame_dst = {0,0,frame_width * scale, frame_height * scale};
        // Animation timing:
        this->duration = duration;
        frame_lapse = duration/(float)total_frames;
        current_frame_time = 0.0f;
        current_time = 0.0f;
        this->loop = loop;
    }

    void Flip() {
        frame_src.width *= -1;
    }

    Vector2 GetDimensions() {
        return {(float)std::abs(frame_width), (float)std::abs(frame_height)};
    }

    Vector2 GetViewDimensions() {
        return {std::abs(frame_width)*scale, std::abs(frame_height)*scale};
    }

    bool HasFinished() {
        return (current_time >= duration);
    }

    void Play(Vector2 position, float deltaTime) {
        if (current_frame_time >= frame_lapse) {
            current_frame_time = 0;
            if (loop) {
                current_frame = (current_frame+1) % frames_per_row;
            } else {
                if (current_frame + 1 < frames_per_row) {
                    current_frame++;
                }
            }
            //if (!loop && current_frame+1 == frames_columns) 
            //if (!current_frame) {
            //    current_row = (current_row+1) % frames_rows;
            //}
            frame_src.x = (float) frame_width  * current_frame;
            frame_src.y = (float) frame_height * current_row; 
        }
        frame_dst.x = position.x; frame_dst.y = position.y;
        DrawTexturePro(spritesheet, frame_src, frame_dst, Vector2{0,0}, 0, WHITE);
        current_frame_time += deltaTime;
        current_time += deltaTime; 
    }

    void Stop() {
        current_frame = current_row = 0;
        current_frame_time = 0;
        current_time = 0;
        frame_src.x = frame_src.y = 0;
    }

    void Unload() {
        UnloadTexture(spritesheet);
    }

};

/**
 * @brief Animator component
 * 
 */
class Animator : public Component {
public:
    using animator_map = std::unordered_map<std::string, Animation>;
    Animator()
        : current_animation(""), waiting_animation(""), Animations({}) {}
    Animator(std::string entry_animation, animator_map Animations)
        : current_animation(entry_animation), Animations(Animations) {}

    void Flip() {
        for (auto& animation : Animations) {
            animation.second.Flip();
        }
    }

    Vector2 GetDimensions() {
        return Animations[current_animation].GetDimensions();
    }

    Vector2 GetViewDimensions() {
        return Animations[current_animation].GetViewDimensions();
    }

    bool HasFinished(std::string animation) {
        return (current_animation == animation) ? Animations[current_animation].HasFinished() : true;
    }

    bool InState(std::string animation) {
        return current_animation == animation;
    }

    void Play(Vector2 position, float deltaTime) {
        Animations[current_animation].Play(position, deltaTime);
    }

    bool Trigger(std::string entry_animation, std::string next_animation) {
        if (current_animation == entry_animation) {
            current_animation = next_animation;
            return true;
        }
        return false;
    }

    void Unload() override {
        for (auto& animation : Animations) {
            animation.second.Unload();
        }
    }

    void operator[](std::string animation) {
        if (animation != current_animation) Animations[animation].Stop();
        current_animation = animation;
    }
private:
    std::string entry_animation;   // (UNUSED) The entry animation of the whole stream.
    std::string current_animation; // Current animation playing.
    std::string waiting_animation; // (UNUSED) Next animation that we want to play in some cases.
    std::unordered_map<std::string, Animation> Animations; // The animations perse.
};

//-----------------------------------------------------------------------------
// Audio
//-----------------------------------------------------------------------------´
/**
 * @brief Fahter class Audio, covers every type of audio.
 * 
 */
class AudioSource {
private:
    // ...
public:
    AudioSource() {}
    virtual ~AudioSource() = default;
    virtual void Play() = 0;
    virtual void Unload() = 0;
};

/**
 * @brief Audio type: sound.
 * 
 */
class SoundSource : public AudioSource {
private:
    Sound source;
public:
    SoundSource(const char* fileName) : source(LoadSound(fileName)) {}

    void Play() override {
        PlaySound(source);
    }

    void Unload() override {
        UnloadSound(source);
    }
};

/**
 * @brief Audio type: music.
 * 
 */
class MusicSource : public AudioSource {
private:
    Music source;
public:
    MusicSource (const char *fileName, bool loop) : source(LoadMusicStream(fileName)) {
        source.looping = loop;
    }

    void Init() {
        PlayMusicStream(source);
    }

    void Play() override {
        UpdateMusicStream(source);
    }

    void Unload() override {
        UnloadMusicStream(source);
    }
};

/**
 * @brief Audioplayer component.
 * 
 */
class AudioPlayer : public Component {
public:
    using audioplayer_map = std::unordered_map<std::string, std::shared_ptr<AudioSource>>;
    AudioPlayer() {}
    AudioPlayer(audioplayer_map Audios) : Audios(Audios) {}

    void Unload() override {
        for (auto& audiosource : Audios) {
            audiosource.second->Unload();
        }
    }

    void operator[ ](std::string audiosource) {
        Audios[audiosource]->Play();
    }
private:
    audioplayer_map Audios;
};

//-----------------------------------------------------------------------------
// Physics
//-----------------------------------------------------------------------------´
/**
 * @brief GameObject physical limits.
 * 
 */
class Collider2D : public Component {
private:
    // ...
public:
    enum COLLIDER_ENUM { UKNOWN = -1, PLAYER, ENEMY, PROJECTILE, WALL, FLOOR };
    Vector2* pos; // Nuevo item. Coge el centro de nuestro objeto padre y se
                  // actualiza la posición actual.
    Vector2 size; // Dimensiones del collider.

    Collider2D() : pos(nullptr), size({0.0f,0.0f}) {}
    Collider2D(Vector2* pos, int width, int height) : pos(pos), size({(float)width, (float)height}) {}
    Collider2D(Vector2* pos, Vector2 size) : pos(pos), size(size) {}

    void Draw(Color color = {129, 242, 53, 255}) {
        DrawRectangleLinesEx({pos->x, pos->y, size.x, size.y}, 3.0f, color);
    }

    void Unload() override {
        // No carga nada, pero asi C++ no se queja.
    }
};

// Esto no me iba en Grafica pero aqui si, alucinante. Teneis la teoria aqui,
// ahora no me apetece explicarla: 
// - https://www.youtube.com/watch?v=8JJ-4JgR7Dg
// - https://github.com/OneLoneCoder/Javidx9/blob/master/PixelGameEngine/SmallerProjects/OneLoneCoder_PGE_Rectangles.cpp
bool Collides(const Vector2 ray_o, const Vector2 ray_d, const Collider2D& target,
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
bool Collides(const Collider2D& A, const Vector2& vA, const Collider2D& B, 
    Vector2& contact_point, Vector2& contact_normal, float& contact_time)
{
    // Expandir el rectangulo destino con las dimensiones del rectangulo origen.
    Vector2 exp_B_pos = *B.pos - (A.size/2);
    Collider2D exp_B = Collider2D(&exp_B_pos, B.size + A.size);
    // Se coge el centro del rectangulo:
    if (Collides(*A.pos + A.size/2, vA, exp_B, contact_point, contact_normal, contact_time)) {
        return (contact_time >= 0.0f && contact_time <= 1.0f);
    }
    return false;
}

// Luego vendra el dinamismo para ambos. Va mas o menos, puede (debe) pulirse.
bool Collides(const Collider2D& A, const Vector2& vA, Vector2& cpA, Vector2& cnA, float& ctA,
    const Collider2D& B, const Vector2& vB, Vector2& cpB, Vector2& cnB, float& ctB)
{
    // Expandir el rectangulo destino con las dimensiones del rectangulo origen.
    Vector2 exp_A_pos = *A.pos - (B.size/2), exp_B_pos = *B.pos - (A.size/2);
    Collider2D exp_A = Collider2D(&exp_A_pos, A.size + B.size), exp_B = Collider2D(&exp_B_pos, B.size + A.size);
    // Se coge el centro del rectangulo:
    auto cA = Collides(*A.pos + A.size/2, vA, exp_B, cpA, cnA, ctA);
    auto cB = Collides(*B.pos + B.size/2, vB, exp_A, cpB, cnB, ctB);
    if (!cA && !cB) {
        return false;
    }
    return (ctA >= 0.0f && ctA < 1.0f) || (ctB >= 0.0f && ctB < 1.0f);
}

/**
 * @brief GameObject physics behaviour.
 * 
 */
class RigidBody2D : public Component{
private:
    // ...
public:
    float mass;           // Mass of the object.
    float gravity;        // Gravity acceleration.
    Vector2 velocity;     // Current speed of the object.
    Vector2 max_velocity; // Max velocity that the object can reach.
    Vector2 acceleration; // Object accelerations.

    RigidBody2D() : mass(0), gravity(0), velocity({}) {}
    RigidBody2D(float mass, float gravity, Vector2 max_velocity, Vector2 acceleration) {
        this->mass    = mass;
        this->gravity = gravity;
        velocity = {0,0};
        this->max_velocity = max_velocity;
        this->acceleration = acceleration;
    }

    void Draw(Vector2 center) {
        DrawLineEx(center, center + velocity, 3.0f, BLUE);
        DrawLineEx(center, center + (Vector2){velocity.x, 0.0f}, 3.0f, RED);
        DrawLineEx(center, center + (Vector2){0.0f, velocity.y}, 3.0f, GREEN);
    }

    void Unload() override {
        // Tampoco hace nada, pero C++ no se queja.
    }
};


//-----------------------------------------------------------------------------
// Status
//-----------------------------------------------------------------------------
/**
 * @brief GameObject status properties
 * 
 */
class Transform2D : public Component{
private:
    // ...
public:
    Vector2 position;
    float rotation;
    Vector2 scale;

    Transform2D()
        : position({0,0}), rotation(0.0f), scale({1,1}) {}
    Transform2D(Vector2 position)
        : position(position), rotation(0.0f), scale({1.0f, 1.0f}) {}
    Transform2D(Vector2 position, float rotation, Vector2 scale)
        : position(position), rotation(rotation), scale(scale) {}

    void Unload() override {
        // No hace nada, pero no se queja.
    }
};

int GetAxis(std::string axis) {
    if (axis == "Horizontal") {
        bool left_key  = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        bool right_key = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT); 
        if (left_key && right_key) return 0;
        else if (left_key) return -1;
        else if (right_key) return 1;
    } else if (axis == "Vertical") {
        bool down_key = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
        bool up_key   = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP); 
        if (down_key && up_key) return 0;
        if (down_key) return -1;
        else if (up_key) return 1;
    }
    return 0;
}

// ============================================================================
// ============================================================================
// ENTITIES
// ============================================================================
// ============================================================================
/**
 * @brief Engine GameObject
 * 
 */
class GameObject {
private:
    //...
public:
    std::unordered_map<std::type_index, Component*> components;

    /*
    template <typename... Args>
    GameObject(Args&&... args) {
        components[typeid(Transform2D)] = new Transform2D(std::forward<Args>(args)...);
    }
    */

    template <typename T>
    T& getComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return dynamic_cast<T&>(*it->second);
        }
        throw std::runtime_error("Componente no encontrado.");
    }

    //template <typename T, typename... Args>
    //void addComponent(Args... args) {
    //    components[typeid(T)] = new T(std::forward<Args>(args)...);
    //}

    template <typename T, typename... Args>
    void addComponent(Args&&... args) {
        components[typeid(T)] = new T(std::forward<Args>(args)...);
    }

    //template <typename T>
    //void addComponent(T component) {
    //    components[typeid(T)] = component;
    //}

    void destroy() {
        for (auto& component : components) {
            component.second->Unload();
            delete component.second;
        }
    }

    template <typename T>
    void removeComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            delete it->second;
            components.erase(it);
        }
    }
};

// ============================================================================
// ============================================================================
// SYSTEMS
// ============================================================================
// ============================================================================
// Nothing to see here.

#endif
