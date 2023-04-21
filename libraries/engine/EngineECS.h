#pragma once
#ifndef ECS_H
#define ECS_H

#include <functional>
#include <memory>
#include <string>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "raylib.h"

// Configuración de una instancia.
struct GameObjectOptions {
    // Nombre que queremos que reciba la nueva instancia.
    std::string name      = "";
    // Tag que queremos que reciba la nueva instancia.
    std::string tag       = "";
    // Tags secundarios, el gameObject puede tener un tag principal y luego 
    // tags secundarios para colisiones, etc.
    std::unordered_set<std::string> second_tags;
    // Tags que queremos que se comprueben en las colisiones.
    std::unordered_set<std::string> related_tags = {};
    // Posición en la que queremos que se instancie la nueva instancia.
    Vector2 position      = {-1,-1};
    // Color del colider para cuando se dibuje (de momento sin implementar).
    // Color collider_color  = {-1,-1,-1,-1};
};

class Component;
class Script;
class Collision;

//-----------------------------------------------------------------------------
// Canvas
//-----------------------------------------------------------------------------
class Canvas {
private:
    Rectangle src; // What pixels of the sprite do I want to draw?
    Rectangle dst; // Where and how do I draw these pixels?
    Texture2D sprite;
public:
    Canvas(const char* fileName, Vector2 position, Vector2 size);
    void Draw();
    void Move(Vector2 translation);
};

//-----------------------------------------------------------------------------
// Un objeto del motor.
//-----------------------------------------------------------------------------
class GameObject {
private:
    // Clonar GameObject.
    void Clone(GameObject& gameObject);
public:
    // Object that is clone of.
    GameObject* prefab;
    // Nombre del GameObject (unico por GameObject).
    std::string name;
    // Tag de GameObject (puede englobar varios GameObject).
    std::string tag;
    // Tags secundarios, el gameObject puede tener un tag principal y luego 
    // tags secundarios para colisiones, etc.
    std::unordered_set<std::string> second_tags;
    // Tags a los que esta relacionado el GameObject actual. 
    std::unordered_set<std::string> related_tags;
    // Para componentes que solo puede haber uno por GameObject.
    std::unordered_map<std::type_index, Component*> components;
    // Puede haber varios Scripts en un GameObject.
    std::unordered_map<std::type_index, Script*> scripts;
    // Se puede discutir si permitir que haya varios componentes por gameObject.

    GameObject(std::string name = "GameObject", std::string tag = "",
        std::unordered_set<std::string> second_tags  = {},
        std::unordered_set<std::string> related_tags = {});
    GameObject(GameObject& gameObject);
    GameObject(GameObject& gameObject, const GameObjectOptions& options);

    template <typename T, typename... Args> void addComponent(Args&&... args) {
        components[typeid(T)] = new T(*this, std::forward<Args>(args)...);
    }
    template<typename S, typename T, typename... Args> void addComponent(Args&&... args) {
        scripts[typeid(T)] = new T(*this, std::forward<Args>(args)...);
    }

    void Destroy();

    template <typename T> bool hasComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return true;
        }
        return false;
    }

    bool hasSecondTag(std::string tag);

    template <typename T> T& getComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return dynamic_cast<T&>(*it->second);
        }
        throw std::runtime_error(name + ": Componente '" + std::string(typeid(T).name()) + "' no encontrado.");
    }

    template <typename S, typename T> T& getComponent() {
        auto it = scripts.find(typeid(T));
        if (it != scripts.end()) {
            return dynamic_cast<T&>(*it->second);
        }
        throw std::runtime_error(name + ": Script '" + std::string(typeid(T).name()) + "' no encontrado.");
    }

    void OnCollision(Collision contact);
    void Printout();

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

// A GameObject component.
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

// An animation made of sprites.
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
    Vector2 scale;            // Sprite scaling.
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
    Animation(const char* fileName, int frame_width, int frame_height, Vector2 scale, float duration, bool loop);

    void Flip();
    Vector2 GetScale();
    Vector2 GetDimensions();
    Vector2 GetViewDimensions();
    bool HasFinished();
    void Play(Vector2 position);
    void Stop();
    void Unload();
    void operator[](int current_frame);

};

// Animator component
class Animator : public Component {
private:
    std::string entry_animation;   // (UNUSED) The entry animation of the whole stream.
    std::string current_animation; // Current animation playing.
    std::string waiting_animation; // (UNUSED) Next animation that we want to play in some cases.
    std::unordered_map<std::string, Animation> Animations; // The animations perse.
public:
    // Construct a new Animator object
    Animator(GameObject& gameObject, std::string entry_animation, std::unordered_map<std::string, Animation> Animations);
    
    // Construct a new Animator object
    Animator(GameObject& gameObject, Animator& animator);

    // Clones the current animator into a new one.
    Component* Clone(GameObject& gameObject) override;

    // Flips all the animations.
    void Flip();

    // @brief Gets the scale of the current animation.
    Vector2 GetScale();

    // Gets the dimensions of the current animation.
    Vector2 GetDimensions();

    // Gets the scaled dimensions of the current animation.
    Vector2 GetViewDimensions();

    // Indicates if the current animation has finished.
    bool HasFinished(std::string animation);

    // Indicates if the current state animation is the given one.
    bool InState(std::string animation);

    // Plays the current animation.
    void Play();

    // If the entry animation is the current one, plays it and pass to the
    bool Trigger(std::string entry_animation, std::string next_animation);

    // Unloads all the spritesheets.
    void Unload() override;

    // Stops the current animation and changes to the given one.
    Animation& operator[](std::string animation);
};

//-----------------------------------------------------------------------------
// Audio
//-----------------------------------------------------------------------------

// Fahter class Audio, covers every type of audio.
class AudioSource {
private:
    // ...
public:
    AudioSource();
    virtual ~AudioSource() = default;
    virtual void Play() = 0;
    virtual void Unload() = 0;
};

// Audio type: sound.
class SoundSource : public AudioSource {
private:
    Sound source;
public:
    SoundSource(const char* fileName);
    void Play() override;
    void Unload() override;
};

// Audio type: music.
class MusicSource : public AudioSource {
private:
    Music source;
public:
    MusicSource (const char *fileName, bool loop);
    void Init();
    void Play() override;
    void Unload() override;
};


// Audioplayer component.
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

// GameObject physical limits.
class Collider2D : public Component {
private:
    //...
public:
    // enum COLLIDER_ENUM { UKNOWN = -1, PLAYER, ENEMY, PROJECTILE, WALL, FLOOR };
    Color color;  // Color del collider.
    Vector2* pos; // Nuevo item. Coge el centro de nuestro objeto padre y se
                  // actualiza la posición actual.
    Vector2 offset; 
    Vector2 size; // Dimensiones del collider.
    
    Collider2D(GameObject& gameObject, Vector2* pos, int width, int height, Color color = {129, 242, 53, 255});
    Collider2D(GameObject& gameObject, Vector2* pos, Vector2 size, Color color = {129, 242, 53, 255});
    Collider2D(GameObject& gameObject, Vector2* pos, Vector2 size, Vector2 offset, Color color = {129, 242, 53, 255});
    Collider2D(GameObject& gameObject, Collider2D& collider);
    Component* Clone(GameObject& gameObject) override;
    void Draw();
    Vector2 Pos() const;
};

//-----------------------------------------------------------------------------
// Physics
//-----------------------------------------------------------------------------
// GameObject physics behaviour.
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
    void Draw();
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
    Vector2 scale;
    Rectangle src; // What pixels of the sprite do I want to draw?
    Rectangle dst; // Where and how do I draw these pixels?
    Texture2D img; // Sprite image.
public:
    Sprite(GameObject& gameObject, const char* fileName, float scale);
    Sprite(GameObject& gameObject, const char* fileName, float scale_x, float scale_y);
    Sprite(GameObject& gameObject, const char* fileName, Vector2 view_size);
    Sprite(GameObject& gameObject, Sprite& sprite);

    Vector2 GetScale();
    Vector2 GetDimensions();
    Vector2 GetViewDimensions();
    Component* Clone(GameObject& gameObject) override;
    void Draw();
    void Unload() override;

};

//-----------------------------------------------------------------------------
// Status
//-----------------------------------------------------------------------------
// GameObject status properties
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

// Devuelve si existe movimiento en alguno de los ejes.
int GetAxis(std::string axis);

// ============================================================================
// ============================================================================
// SYSTEMS
// ============================================================================
// ============================================================================
// ----------------------------------------------------------------------------
// Game System
// /!\ Aunque no lo creais y no lo parezca, todo esto lo hago para encapsular 
// el tratamiento a pelo de los colliders y de los objetos en escena, de manera
// que con llamar a un par de funciones lo haga solo sin que vosotros tengais 
// que controlarlo (+ reutilizacion de codigo + mantenimiento + generalizacion).
// ----------------------------------------------------------------------------

// Información de una colision resultante.
struct Collision {
    // GameObject con el que ha colisionado.
    GameObject& gameObject;
    // Tiempo de colision
    float contact_time;
    // Punto de intersección del rayo del objeto dinámico al estático
    Vector2 contact_point;
    // Normal del punto de intersección (lado desde el que van a chocar).
    Vector2 contact_normal; // x=-1 (IZQ), x=1 (DCH), y=-1 (ARR), y=1 (ABJ)
    // Constructor
    Collision(GameObject& gameObject, float contact_time, Vector2 contact_point, Vector2 contact_normal);
};

class GameSystem {
private:
    // Spoiler, but not for now:
    // - La idea es dividir el juego en escenas, por lo que cada escena tendrá
    //   sus objetos, y ya, opcional, puede plantearse pero se deja en el aire.
    // std::unordered_map<std::string, Scene> scenes;
    // std::string current scene;
    // Instances
    static std::unordered_map<std::string, int> nGameObjects;
    static std::unordered_map<std::string, std::unordered_map<std::string, GameObject*>> GameObjects;

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
    static void Collisions(GameObject& gameObject);
    static void Destroy(GameObject& gameObject);
    static GameObject& Instantiate(GameObject& gameObject, GameObjectOptions options);
    static void Move(Vector2 translation);
    static void Printout();
    static void Render();
    static void Update();
};

#endif
