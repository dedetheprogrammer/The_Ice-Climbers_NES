#pragma once
#ifndef ECS_H
#define ECS_H

#include <functional>
#include <memory>
#include <string>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "raylib.h"

/**
 * @brief Engine GameObject
 * 
 */
class Component;
class Script;
class Collision;
class GameObject {
private:
    //...
public:
    // Nombre del GameObject.
    std::string name;
    // Para componentes que solo puede haber uno por GameObject.
    std::unordered_map<std::type_index, Component*> components;
    // Puede haber varios Scripts en un GameObject.
    std::unordered_map<std::type_index, Script*> scripts;
    // Se puede discutir si permitir que haya varios componentes por gameObject..

    GameObject();
    GameObject(std::string name);
    GameObject(GameObject& gameObject);

    template <typename T, typename... Args> void addComponent(Args&&... args) {
        components[typeid(T)] = new T(*this, std::forward<Args>(args)...);
    }
    template<typename S, typename T, typename... Args> void addComponent(Args&&... args) {
        scripts[typeid(T)] = new T(*this, std::forward<Args>(args)...);
    }

    template <typename T> bool hasComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return true;
        }
        return false;
    }

    template <typename T> T& getComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return dynamic_cast<T&>(*it->second);
        }
        throw std::runtime_error("Componente '" + std::string(typeid(T).name()) + "' no encontrado.");
    }

    template <typename S, typename T> T& getComponent() {
        auto it = scripts.find(typeid(T));
        if (it != scripts.end()) {
            return dynamic_cast<T&>(*it->second);
        }
        throw std::runtime_error("Script '" + std::string(typeid(T).name()) + "' no encontrado.");
    }

    template <typename T> void removeComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            delete it->second;
            components.erase(it);
        }
    }

    template <typename S, typename T> void removeComponent() {
        auto it = scripts.find(typeid(T));
        if (it != scripts.end()) {
            delete it->second;
            scripts.erase(it);
        }
    }

    void Draw(float deltaTime);
    void Destroy();
    void OnCollision(Collision contact);
    void Update();
};

// ============================================================================
// ============================================================================
// Componentes
// ============================================================================
// ============================================================================
// ----------------------------------------------------------------------------
// Componente generico
// ----------------------------------------------------------------------------
/**
 * @brief A GameObject component.
 * 
 */
class Component {
private:
    // ...
public:
    GameObject& gameObject;
    //enum COMPONENT_ENUM { ANIMATOR, AUDIOSOURCE };
    // Un poco de spoiler de lo que se viene...
    Component(GameObject& gameObject);
    virtual ~Component() = default;
    virtual Component* Clone(GameObject& gameObject) = 0;
    // Unloads those components that were loaded in memory.
    virtual void Unload();
};

//-----------------------------------------------------------------------------
// Animations
//-----------------------------------------------------------------------------
/**
 * @brief An animation made of sprites.
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

    Animation();
    Animation(const char* fileName, int frame_width, int frame_height, float scale, float duration, bool loop);

    void Flip();
    Vector2 GetDimensions();
    Vector2 GetViewDimensions();
    bool HasFinished();
    void Play(Vector2 position, float deltaTime);
    void Stop();
    void Unload();

};

/**
 * @brief Animator component
 */
class Animator : public Component {
private:
    std::string entry_animation;   // (UNUSED) The entry animation of the whole stream.
    std::string current_animation; // Current animation playing.
    std::string waiting_animation; // (UNUSED) Next animation that we want to play in some cases.
    std::unordered_map<std::string, Animation> Animations; // The animations perse.
public:
    /**
     * @brief Construct a new Animator object
     * 
     * @param gameObject 
     * @param entry_animation 
     * @param Animations 
     */
    Animator(GameObject& gameObject, std::string entry_animation, std::unordered_map<std::string, Animation> Animations);
    
    /**
     * @brief Construct a new Animator object
     * 
     * @param animator 
     */
    Animator(GameObject& gameObject, Animator& animator);

    /**
     * @brief Clones the current animator into a new one.
     * 
     * @return Component* 
     */
    Component* Clone(GameObject& gameObject) override;

    /**
     * @brief Flips all the animations.
     * 
     */
    void Flip();

    /**
     * @brief Get the dimensions of the current animation.
     * 
     * @return Vector2 
     */
    Vector2 GetDimensions();

    /**
     * @brief Get the scaled dimensions of the current animation.
     * 
     * @return Vector2 
     */
    Vector2 GetViewDimensions();

    /**
     * @brief Indicates if the current animation has finished.
     * 
     * @param animation 
     * @return true 
     * @return false 
     */
    bool HasFinished(std::string animation);

    /**
     * @brief Indicates if the current state animation is the given one.
     * 
     * @param animation 
     * @return true 
     * @return false 
     */
    bool InState(std::string animation);

    /**
     * @brief Plays the current animation.
     * 
     * @param position 
     * @param deltaTime 
     */
    void Play(Vector2 position, float deltaTime);

    /**
     * @brief If the entry animation is the current one, plays it and pass to the
     * given next animation.
     * 
     * @param entry_animation 
     * @param next_animation 
     * @return true 
     * @return false 
     */
    bool Trigger(std::string entry_animation, std::string next_animation);

    /**
     * @brief Unloads all the spritesheets.
     * 
     */
    void Unload() override;

    /**
     * @brief Stops the current animation and changes to the given one.
     * 
     * @param animation 
     */
    void operator[](std::string animation);
};

//-----------------------------------------------------------------------------
// Audio
//-----------------------------------------------------------------------------´
/**
 * @brief Fahter class Audio, covers every type of audio.
 */
class AudioSource {
private:
    // ...
public:
    AudioSource();
    virtual ~AudioSource() = default;
    virtual void Play() = 0;
    virtual void Unload() = 0;
};

/**
 * @brief Audio type: sound.
 */
class SoundSource : public AudioSource {
private:
    Sound source;
public:
    SoundSource(const char* fileName);
    void Play() override;
    void Unload() override;
};

/**
 * @brief Audio type: music.
 */
class MusicSource : public AudioSource {
private:
    Music source;
public:
    MusicSource (const char *fileName, bool loop);
    void Init();
    void Play() override;
    void Unload() override;
};

/**
 * @brief Audioplayer component.
 */
class AudioPlayer : public Component {
private:
    std::unordered_map<std::string, std::shared_ptr<AudioSource>> Audios;
public:
    AudioPlayer(GameObject& gameObject, std::unordered_map<std::string, std::shared_ptr<AudioSource>> Audios);
    AudioPlayer(GameObject& gameObject, AudioPlayer& audioplayer);
    Component* Clone(GameObject& gameObject) override;
    void Unload() override;
    void operator[ ](std::string audiosource);
};

/**
 * @brief GameObject physical limits.
 * 
 */
class Collider2D : public Component {
private:
    Color color;
public:
    enum COLLIDER_ENUM { UKNOWN = -1, PLAYER, ENEMY, PROJECTILE, WALL, FLOOR };
    Vector2* pos; // Nuevo item. Coge el centro de nuestro objeto padre y se
                  // actualiza la posición actual.
    Vector2 size; // Dimensiones del collider.
    
    Collider2D(GameObject& gameObject, Vector2* pos, int width, int height, Color color = {129, 242, 53, 255});
    Collider2D(GameObject& gameObject, std::string name, Vector2* pos, int width, int height, Color color = {129, 242, 53, 255});
    Collider2D(GameObject& gameObject, Vector2* pos, Vector2 size, Color color = {129, 242, 53, 255});
    Collider2D(GameObject& gameObject, std::string name, Vector2* pos, Vector2 size, Color color = {129, 242, 53, 255});
    Collider2D(GameObject& gameObject, Collider2D& collider);
    Component* Clone(GameObject& gameObject) override;
    void Draw();
};

//-----------------------------------------------------------------------------
// Physics
//-----------------------------------------------------------------------------
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

    RigidBody2D(GameObject& gameObject, float mass, float gravity, Vector2 max_velocity, Vector2 acceleration);
    RigidBody2D(GameObject& gameObject, RigidBody2D& rigidbody);
    Component* Clone(GameObject& gameObject) override;
    void Draw(Vector2 center);
};

//-----------------------------------------------------------------------------
// Scripts
//-----------------------------------------------------------------------------
class Script : public Component {
private:
    // ...
public:
    Script(GameObject& gameObject);
    virtual Component* Clone(GameObject& gameObject) = 0;
    // virtual void Start() = 0;
    virtual void OnCollision(Collision contact) = 0;
    virtual void Update() = 0;
};

//-----------------------------------------------------------------------------
// Sprite
//-----------------------------------------------------------------------------
class Sprite : public Component {
private:
    float scale;
    Rectangle src; // What pixels of the sprite do I want to draw?
    Rectangle dst; // Where and how do I draw these pixels?
    Texture2D img; // Sprite image.

public:
    Sprite(GameObject& gameObject, const char* fileName, Vector2 src_origin = {0,0}, float dst_scale = 1.0f);
    Sprite(GameObject& gameObject, Sprite& sprite);

    float GetScale();
    Vector2 GetDimensions();
    Vector2 GetViewDimensions();
    Component* Clone(GameObject& gameObject) override;
    void Draw(Vector2 position);
    void Unload() override;

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

    Transform2D(GameObject& gameObject, Vector2 position = {0,0}, float rotation = 0.0f, Vector2 scale = {1.0f, 1.0f});
    Transform2D(GameObject& gameObject, Transform2D& transform);
    Component* Clone(GameObject& gameObject) override;
};

// Hola
int GetAxis(std::string axis);

// ============================================================================
// ============================================================================
// SYSTEMS
// ============================================================================
// ============================================================================
// ----------------------------------------------------------------------------
// Collision System
// ----------------------------------------------------------------------------
class Collision {
private:
    //...
public:
    GameObject& gameObject;
    // enum
    float contact_time;
    Vector2 contact_point;
    Vector2 contact_normal; // x=-1 (IZQ), x=1 (DCH), y=-1 (ARR), y=1 (ABJ)
    Collision(GameObject& gameObject, float contact_time, Vector2 contact_point, Vector2 contact_normal);
};

class CollisionSystem {
private:
    static std::unordered_map<std::string, Collider2D*> colliders;

    // Esto no me iba en Grafica pero aqui si, alucinante. Teneis la teoria aqui,
    // ahora no me apetece explicarla: 
    // - https://www.youtube.com/watch?v=8JJ-4JgR7Dg
    // - https://github.com/OneLoneCoder/Javidx9/blob/master/PixelGameEngine/SmallerProjects/OneLoneCoder_PGE_Rectangles.cpp
    static bool Collides(const Vector2 ray_o, const Vector2 ray_d, const Collider2D& target,
        Vector2& contact_point, Vector2& contact_normal, float& contact_time);

    // Lo que hacemos aqui es extender el Collider a nuevas dimensiones, cogemos el 
    // la anchura y se extiende w/2 del collider A y la altura h/2 del collider A, 
    // se extiende por ambos lados.
    // De momento A es dinamico y B es estatico, 
    static bool Collides(const Collider2D& A, const Collider2D& B, Vector2& contact_point,
        Vector2& contact_normal, float& contact_time);

public:
    static void addCollider(std::string name, Collider2D* collider);
    static void removeCollider(std::string name);
    static void printout();
    static void checkCollisions();
    static void checkCollisions(GameObject& gameObject);
};


class GameSystem {
private:
    static std::unordered_map<std::string, std::vector<GameObject*>> scene_instances;
public:
    static void Instantiate(GameObject& gameObject, Vector2 position);
    static void Update();
};

#endif
