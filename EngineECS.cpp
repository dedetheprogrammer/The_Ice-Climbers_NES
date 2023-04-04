#include "EngineECS.h"
#include "raylibx.h"
#include "raylib.h"

// ============================================================================
// ============================================================================
// Entidades
// ============================================================================
// ============================================================================
GameObject::GameObject() : name("GameObject") {}
GameObject::GameObject(std::string name) : name(name) {}

void GameObject::destroy() {
    for (auto& component : components) {
        component.second->Unload();
        delete component.second;
    }
}

void GameObject::printout() {
    std::cout << name << "\n";
}

// ============================================================================
// ============================================================================
// Componentes
// ============================================================================
// ============================================================================
// ----------------------------------------------------------------------------
// Componente generico
// ----------------------------------------------------------------------------
Component::Component() {};
void Component::Unload() {};

//-----------------------------------------------------------------------------
// Animations
//-----------------------------------------------------------------------------
Animation::Animation() {
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

Animation::Animation(const char* fileName, int frame_width, int frame_height, float scale, float duration, bool loop) {
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
void Animation::Flip() {
    frame_src.width *= -1;
}
Vector2 Animation::GetDimensions() {
    return {(float)std::abs(frame_width), (float)std::abs(frame_height)};
}
Vector2 Animation::GetViewDimensions() {
    return {std::abs(frame_width)*scale, std::abs(frame_height)*scale};
}
bool Animation::HasFinished() {
    return (current_time >= duration);
}
void Animation::Play(Vector2 position, float deltaTime) {
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
void Animation::Stop() {
    current_frame = current_row = 0;
    current_frame_time = 0;
    current_time = 0;
    frame_src.x = frame_src.y = 0;
}
void Animation::Unload() {
    UnloadTexture(spritesheet);
}

Animator::Animator()
    : current_animation(""), waiting_animation(""), Animations({}) {}
Animator::Animator(std::string entry_animation, std::unordered_map<std::string, Animation> Animations)
    : current_animation(entry_animation), Animations(Animations) {}
void Animator::Flip() {
    for (auto& animation : Animations) {
        animation.second.Flip();
    }
}
Vector2 Animator::GetDimensions() {
    return Animations[current_animation].GetDimensions();
}
Vector2 Animator::GetViewDimensions() {
    return Animations[current_animation].GetViewDimensions();
}
bool Animator::HasFinished(std::string animation) {
    return (current_animation == animation) ? Animations[current_animation].HasFinished() : true;
}
bool Animator::InState(std::string animation) {
    return current_animation == animation;
}
void Animator::Play(Vector2 position, float deltaTime) {
    Animations[current_animation].Play(position, deltaTime);
}
bool Animator::Trigger(std::string entry_animation, std::string next_animation) {
    if (current_animation == entry_animation) {
        current_animation = next_animation;
        return true;
    }
    return false;
}
void Animator::Unload() {
    for (auto& animation : Animations) {
        animation.second.Unload();
    }
}
void Animator::operator[](std::string animation) {
    if (animation != current_animation) Animations[animation].Stop();
    current_animation = animation;
}

// ----------------------------------------------------------------------------
// Audio
// ----------------------------------------------------------------------------
AudioSource::AudioSource() {}

SoundSource::SoundSource(const char* fileName) : source(LoadSound(fileName)) {}
void SoundSource::Play() { PlaySound(source); }
void SoundSource::Unload() { UnloadSound(source); }

MusicSource::MusicSource (const char *fileName, bool loop) : source(LoadMusicStream(fileName)) {
    source.looping = loop;
}
void MusicSource::Init() { PlayMusicStream(source); }
void MusicSource::Play() { UpdateMusicStream(source); }
void MusicSource::Unload() { UnloadMusicStream(source); }

AudioPlayer::AudioPlayer() {}
AudioPlayer::AudioPlayer(std::unordered_map<std::string, std::shared_ptr<AudioSource>> Audios) : Audios(Audios) {}

void AudioPlayer::Unload() {
    for (auto& audiosource : Audios) {
        audiosource.second->Unload();
    }
}

void AudioPlayer::operator[ ](std::string audiosource) {
    Audios[audiosource]->Play();
}

// ----------------------------------------------------------------------------
// Collider
// ----------------------------------------------------------------------------
Collider2D::Collider2D()
    : pos(nullptr), size({0.0f, 0.0f}) {}
Collider2D::Collider2D(Vector2* pos, int width, int height)
    : pos(pos), size({(float)width, (float)height}) {}
Collider2D::Collider2D(std::string name, Vector2* pos, int width, int height)
    : pos(pos), size({(float)width, (float)height})
{
    CollisionSystem::addCollider(name, this);
}
Collider2D::Collider2D(Vector2* pos, Vector2 size)
    : pos(pos), size(size) {}
Collider2D::Collider2D(std::string name, Vector2* pos, Vector2 size)
    : pos(pos), size(size)
{
    CollisionSystem::addCollider(name, this);
}

void Collider2D::Draw(Color color) {
    DrawRectangleLinesEx({pos->x, pos->y, size.x, size.y}, 3.0f, color);
}

//-----------------------------------------------------------------------------
// Physics
//-----------------------------------------------------------------------------
RigidBody2D::RigidBody2D() : mass(0), gravity(0), velocity({}) {}
RigidBody2D::RigidBody2D(float mass, float gravity, Vector2 max_velocity, Vector2 acceleration) {
    this->mass    = mass;
    this->gravity = gravity;
    velocity = {0,0};
    this->max_velocity = max_velocity;
    this->acceleration = acceleration;
}

void RigidBody2D::Draw(Vector2 center) {
    DrawLineEx(center, center + velocity, 3.0f, BLUE);
    DrawLineEx(center, center + (Vector2){velocity.x, 0.0f}, 3.0f, RED);
    DrawLineEx(center, center + (Vector2){0.0f, velocity.y}, 3.0f, GREEN);
}

//-----------------------------------------------------------------------------
// Scripts
//-----------------------------------------------------------------------------
Script::Script(GameObject& gameObject) : gameObject(gameObject) {}

//-----------------------------------------------------------------------------
// Transform2D
//-----------------------------------------------------------------------------
Transform2D::Transform2D()
        : position({0,0}), rotation(0.0f), scale({1,1}) {}
Transform2D::Transform2D(Vector2 position)
        : position(position), rotation(0.0f), scale({1.0f, 1.0f}) {}
Transform2D::Transform2D(Vector2 position, float rotation, Vector2 scale)
        : position(position), rotation(rotation), scale(scale) {}

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
};

// ============================================================================
// ============================================================================
// Sistemas
// ============================================================================
// ============================================================================

// ----------------------------------------------------------------------------
// Collision System
// ----------------------------------------------------------------------------
std::unordered_map<std::string, Collider2D*> CollisionSystem::colliders;

/**
 * @brief Esto no me iba en Grafica pero aqui si, alucinante. Teneis la teoria aqui, 
 * ahora no me apetece explicarla:
 *  - https://www.youtube.com/watch?v=8JJ-4JgR7Dg
 *  - https://github.com/OneLoneCoder/Javidx9/blob/master/PixelGameEngine/SmallerProjects/OneLoneCoder_PGE_Rectangles.cpp
 */
bool CollisionSystem::Collides(const Vector2 ray_o, const Vector2 ray_d,
    const Collider2D& target, Vector2& contact_point, Vector2& contact_normal,
    float& contact_time)
{
    //DrawLineEx(ray_o, ray_o + 10000 * ray_d, 2.0f, PINK);
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
// De momento A es dinamico y B es estatico:
bool CollisionSystem::Collides(const Collider2D& A, const Vector2& vA, const Collider2D& B, 
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

/*
// Luego vendra el dinamismo para ambos. Va mas o menos, puede (debe) pulirse.
bool CollisionSystem::Collides(const Collider2D& A, const Vector2& vA, Vector2& cpA,
    Vector2& cnA, float& ctA, const Collider2D& B, const Vector2& vB, Vector2& cpB,
    Vector2& cnB, float& ctB)
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
*/

void CollisionSystem::addCollider(std::string name, Collider2D* collider) {
    colliders[name] = collider;
}
void CollisionSystem::removeCollider(std::string name) {
    colliders.erase(name);
}
void CollisionSystem::CheckCollisions() {
    // No hace nada de momento.
}