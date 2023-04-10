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
GameObject::GameObject(GameObject& gameObject) {
    this->name = gameObject.name;
    for (auto const& [type, component] : gameObject.components) {
        components[type] = component->Clone(gameObject);
    }
    // Quedan los scripts.
    for (auto const& [type, script] : gameObject.scripts) {
        scripts[type] = dynamic_cast<Script*>(script->Clone(gameObject));
    }
}

void GameObject::Destroy() {
    for (auto& component : components) {
        component.second->Unload();
        delete component.second;
    }
}

void GameObject::Draw(float deltaTime) {
    auto it  = components.find(typeid(Animator));
    auto pos = dynamic_cast<Transform2D&>(*components.find(typeid(Transform2D))->second).position;
    if (it != components.end()) {
        dynamic_cast<Animator&>(*it->second).Play(pos, deltaTime);
    } else {
        auto it = components.find(typeid(Sprite));
        if (it != components.end()) {
            dynamic_cast<Sprite&>(*it->second).Draw(pos);
        }
    }
    
}

void GameObject::OnCollision(Collision contact) {
    for (auto & [_, Script] : scripts) {
        Script->OnCollision(contact);
    }
}

void GameObject::Update() {
    for (auto &[_, Script] : scripts) {
        Script->Update();
    }
}

// ============================================================================
// ============================================================================
// Componentes
// ============================================================================
// ============================================================================
// ----------------------------------------------------------------------------
// Componente generico
// ----------------------------------------------------------------------------
Component::Component(GameObject& gameObject) : gameObject(gameObject) {};
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

Animator::Animator(GameObject& gameObject, std::string entry_animation, std::unordered_map<std::string, Animation> Animations)
    : Component(gameObject), current_animation(entry_animation), Animations(Animations) {}
Animator::Animator(GameObject& gameObject, Animator& animator) : Component(gameObject) {
    entry_animation   = animator.entry_animation;
    current_animation = animator.current_animation; 
    waiting_animation = animator.waiting_animation;
    Animations        = animator.Animations;
}
Component* Animator::Clone(GameObject& gameObject) {
    return new Animator(gameObject, *this);
}

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
// -- Audio source
AudioSource::AudioSource() {}
// -- Sound source
SoundSource::SoundSource(const char* fileName) : source(LoadSound(fileName)) {}
void SoundSource::Play() { PlaySound(source); }
void SoundSource::Unload() { UnloadSound(source); }
// -- Music source
MusicSource::MusicSource (const char *fileName, bool loop) : source(LoadMusicStream(fileName)) {
    source.looping = loop;
}
void MusicSource::Init() { PlayMusicStream(source); }
void MusicSource::Play() { UpdateMusicStream(source); }
void MusicSource::Unload() { UnloadMusicStream(source); }
// -- The audio player
AudioPlayer::AudioPlayer(GameObject& gameObject, std::unordered_map<std::string, std::shared_ptr<AudioSource>> Audios)
    : Component(gameObject), Audios(Audios) {}
AudioPlayer::AudioPlayer(GameObject& gameObject, AudioPlayer& audioplayer) : Component(gameObject) {
    Audios = audioplayer.Audios;
}
Component* AudioPlayer::Clone(GameObject& gameObject) {
    return new AudioPlayer(gameObject, *this);
}
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
Collider2D::Collider2D(GameObject& gameObject, Vector2* pos, int width, int height, Color color)
    : Component(gameObject), color(color), pos(pos), size({(float)width, (float)height}) {}
Collider2D::Collider2D(GameObject& gameObject, std::string name, Vector2* pos, int width, int height, Color color)
    : Component(gameObject), color(color), pos(pos), size({(float)width, (float)height})
{
    CollisionSystem::addCollider(name, this);
}
Collider2D::Collider2D(GameObject& gameObject, Vector2* pos, Vector2 size, Color color)
    : Component(gameObject), color(color), pos(pos), size(size) {}
Collider2D::Collider2D(GameObject& gameObject, std::string name, Vector2* pos, Vector2 size, Color color)
    : Component(gameObject), color(color), pos(pos), size(size)
{
    CollisionSystem::addCollider(name, this);
}
Collider2D::Collider2D(GameObject& gameObject, Collider2D& collider) : Component(gameObject) {
    color = collider.color;
    pos   = &gameObject.getComponent<Transform2D>().position;
    size  = collider.size; 
}

Component* Collider2D::Clone(GameObject& gameObject) {
    return new Collider2D(gameObject, *this);
}
void Collider2D::Draw() {
    DrawRectangleLinesEx({pos->x, pos->y, size.x, size.y}, 3.0f, color);
}

//-----------------------------------------------------------------------------
// Physics
//-----------------------------------------------------------------------------
RigidBody2D::RigidBody2D(GameObject& gameObject, float mass, float gravity, 
    Vector2 max_velocity, Vector2 acceleration) : Component(gameObject)
{
    this->mass    = mass;
    this->gravity = gravity;
    velocity = {0,0};
    this->max_velocity = max_velocity;
    this->acceleration = acceleration;
}
RigidBody2D::RigidBody2D(GameObject& gameObject, RigidBody2D& rigidbody) : Component(gameObject) {
    mass         = rigidbody.mass;
    gravity      = rigidbody.gravity;
    velocity     = rigidbody.velocity;
    max_velocity = rigidbody.max_velocity;
    acceleration = rigidbody.acceleration;
}
Component* RigidBody2D::Clone(GameObject& gameObject) {
    return new RigidBody2D(gameObject, *this);
}
void RigidBody2D::Draw(Vector2 center) {
    DrawLineEx(center, center + velocity, 3.0f, BLUE);
    DrawLineEx(center, center + (Vector2){velocity.x, 0.0f}, 3.0f, RED);
    DrawLineEx(center, center + (Vector2){0.0f, velocity.y}, 3.0f, GREEN);
}

//-----------------------------------------------------------------------------
// Scripts
//-----------------------------------------------------------------------------
Script::Script(GameObject& gameObject) : Component(gameObject) {}

//-----------------------------------------------------------------------------
// Sprites
//-----------------------------------------------------------------------------
Sprite::Sprite(GameObject& gameObject, const char* fileName, Vector2 src_origin,
    float scale) : Component(gameObject)
{
    img = LoadTexture(fileName);
    this->scale = scale;
    src = {src_origin.x, src_origin.y, (float)img.width, (float)img.height};
    dst = {0, 0, img.width * scale, img.height * scale};
}
Sprite::Sprite(GameObject& gameObject, Sprite& sprite) : Component(gameObject) {
    scale = sprite.scale;
    src   = sprite.src;
    dst   = sprite.dst;
    img   = sprite.img;
}
Component* Sprite::Clone(GameObject& gameObject) {
    return new Sprite(gameObject, *this);
}
float Sprite::GetScale() { 
    return scale;
}
Vector2 Sprite::GetDimensions() {
    return {src.width, src.height};
}
Vector2 Sprite::GetViewDimensions() {
    return {dst.width, dst.height};
}
void Sprite::Draw(Vector2 position) {
    dst.x = position.x; dst.y = position.y;
    DrawTexturePro(img, src, dst, {0,0}, 0, WHITE);
}
void Sprite::Unload() {
    UnloadTexture(img);
}

//-----------------------------------------------------------------------------
// Transform2D
//-----------------------------------------------------------------------------
Transform2D::Transform2D(GameObject& gameObject, Vector2 position, float rotation, Vector2 scale)
        : Component(gameObject), position(position), rotation(rotation), scale(scale) {}
Transform2D::Transform2D(GameObject& gameObject, Transform2D& transform) : Component(gameObject) {
    position = transform.position;
    rotation = transform.rotation;
    scale    = transform.scale;
}
Component* Transform2D::Clone(GameObject& gameObject) {
    return new Transform2D(gameObject, *this);
}
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
Collision::Collision(GameObject& gameObject, float contact_time, Vector2 contact_point, Vector2 contact_normal)
    : gameObject(gameObject), contact_time(contact_time), contact_point(contact_point), contact_normal(contact_normal) {}

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
    if (t_hit_far < 0) return false;
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
// se extiende por ambos lados:
bool CollisionSystem::Collides(const Collider2D& A, const Collider2D& B, 
    Vector2& contact_point, Vector2& contact_normal, float& contact_time)
{
    // Expandir el rectangulo destino con las dimensiones del rectangulo origen.
    Vector2 exp_B_pos = *B.pos - (A.size/2);
    if (Collides(
        // El rayo se lanza desde el centro del collider:
        *A.pos + A.size/2,
        // La direccion del rayo se obtiene mediante la velocidad del objeto:
        A.gameObject.getComponent<RigidBody2D>().velocity * GetFrameTime(),
        // Se crea un nuevo collider ampliado a la mitad del objeto dinamico:
        Collider2D(B.gameObject, &exp_B_pos, B.size + A.size),
        // Información de la colision:
        contact_point,  // Punto del collider con el que el rayo colisiona.
        contact_normal, // Normal del lado con el que el rayo colisiona.
        contact_time    // Tiempo restante para la colision.
    )) {
        return (contact_time >= 0.0f && contact_time < 1.0f);
    }
    return false;
}

void CollisionSystem::addCollider(std::string name, Collider2D* collider) {
    colliders[name] = collider;
}
void CollisionSystem::removeCollider(std::string name) {
    colliders.erase(name);
}
void CollisionSystem::checkCollisions() {
    // Iteramos sobre el hashmap de colliders
    for (auto const& [Collider_A_name, Collider_A] : colliders) {
        if (Collider_A->gameObject.hasComponent<RigidBody2D>()) {
            for (auto const& [Collider_B_name, Collider_B] : colliders) {
                // Comprobamos que no sea el mismo collider
                if (Collider_A_name != Collider_B_name) {
                    // Comprobamos la colisión
                    float ct = 0; Vector2 cp{0,0}, cn{0,0};
                    if (CollisionSystem::Collides(*Collider_A, *Collider_B, cp, cn, ct)) {
                        Collider_A->gameObject.OnCollision(Collision(Collider_B->gameObject, ct, cp, cn));
                        if (!Collider_B->gameObject.hasComponent<RigidBody2D>()) {
                            Collider_B->gameObject.OnCollision(Collision(Collider_A->gameObject, 0, {0,0}, {0,0}));
                        }
                    }
                }
            }
        }
    }
}
void CollisionSystem::checkCollisions(GameObject& gameObject) {
    // Iteramos sobre el hashmap de colliders
    if (gameObject.hasComponent<RigidBody2D>()) {
        for (auto const& [Collider_name, Collider] : colliders) {
            if (Collider_name != gameObject.name) {
                float ct = 0; Vector2 cp{0,0}, cn{0,0};
                if (CollisionSystem::Collides(gameObject.getComponent<Collider2D>(), *Collider, cp, cn, ct)) {
                    gameObject.OnCollision(Collision(Collider->gameObject, ct, cp, cn));
                    if (!Collider->gameObject.hasComponent<RigidBody2D>()) {
                        Collider->gameObject.OnCollision(Collision(gameObject, ct, cp, cn));
                    }
                }
            }
        }
    }
}

void CollisionSystem::printout() {
    for (auto& c : colliders) {
        std::cout << c.first << "\n";
    }
}

// ----------------------------------------------------------------------------
// Game System
// ----------------------------------------------------------------------------
std::unordered_map<std::string, std::vector<GameObject*>> GameSystem::scene_instances;

void GameSystem::Instantiate(GameObject& gameObject, Vector2 position) {

    GameObject* instance = new GameObject(gameObject);
    instance->getComponent<Transform2D>().position = position;
    auto it = scene_instances.find(gameObject.name);
    if (it != scene_instances.end()) {
        instance->name += "_" + std::to_string(it->second.size());
        it->second.push_back(instance);
    } else {
        scene_instances[gameObject.name].push_back(instance);
    }
}

void GameSystem::Update() {
    std::cout << ">>> Scene objects <<<\n";
    for (auto const& [name, instances] : scene_instances) {
        std::cout << name << ":\n";
        for (auto const& instance : instances) {
            std::cout << "  " << instance->name << "\n";
            for (auto const& [type, _] : instance->scripts) {
                std::cout << "    " << type.name() << "\n";
            }
        }
    }
    //for (auto const& [_, instances] : scene_instances) {
    //    for (auto const& instance : instances) {
    //        instance->Update();
    //        CollisionSystem::checkCollisions(*instance);
    //        if (instance->hasComponent<Collider2D>()) {
    //            instance->getComponent<Collider2D>().Draw();
    //        }
    //        instance->Draw(GetFrameTime());
    //    }
    //}
}
