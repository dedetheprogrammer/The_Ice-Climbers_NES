#include "EngineECS.h"
#include "raylibx.h"
#include "raylib.h"

std::unordered_map<std::string, int> GameSystem::nGameObjects;
std::unordered_map<std::string, std::unordered_map<std::string, GameSystem::GameObjectRef>> GameSystem::GameObjects;

// ============================================================================
// ============================================================================
// Fondo
// ============================================================================
// ============================================================================
Background::Background(Texture2D Texture, int window_width, int window_height):
    Texture(Texture), window_width(window_width), window_height(window_height),
    move(0.0f), top(false), moving(false), max(0.0f){
    background_view_height = (Texture.width * window_height)/(float)window_width;
}

void Background::Draw() {
    if(!top && !moving) {
        float lowestY = INFINITY;
        float height = 0.0f;
        for (auto player : GameSystem::getObjectsByTag("Player")) {
            auto y = player->getComponent<Transform2D>().position.y;
            height = player->getComponent<Collider2D>().size.y;
            if(y < lowestY) lowestY = y;
        }

        if(lowestY <= (288.0f - height)) {
            moving = true;
        }

        if(background_view_height + move > Texture.height) {
            move = Texture.height - background_view_height;
            top = true;
        }
    } else if(moving) {
        max += 8;
        if(max == 8.0f * 4 * 6) {
            max = 0.0f;
            moving = false;
        }else {
            move += 2;
            for (auto& [_, ref] : GameSystem::GameObjects){
                for(auto& [_, gameObject] : ref){
                    gameObject.gameObject->getComponent<Transform2D>().position.y += 8;
                    // Quitar los elementos que no se vean en pantalla por abajo
                    /*if(gameObject.gameObject->getComponent<Transform2D>().position.y > window_height){
                        gameObject.gameObject->Destroy();
                    }*/
                }
            }
        }
    }
    Rectangle Mountain_src{0, Texture.height - background_view_height - move, (float)Texture.width, background_view_height};
    Rectangle Mountain_dst{0, 0, (float)window_width, (float)window_height};
    DrawTexturePro(Texture, Mountain_src, Mountain_dst, Vector2{0,0}, 0, WHITE);

}

// ============================================================================
// ============================================================================
// Entidades
// ============================================================================
// ============================================================================
GameObject::GameObject(std::string name, std::string tag,
    std::unordered_set<std::string> second_tags,
    std::unordered_set<std::string> related_tags
) : name(name), tag(tag), second_tags(second_tags), related_tags(related_tags) {}
GameObject::GameObject(GameObject& gameObject) {
    name = gameObject.name;
    tag  = gameObject.tag;
    related_tags = gameObject.related_tags;
    Clone(gameObject);
}
GameObject::GameObject(GameObject& gameObject, const GameObjectOptions& options) {
    this->name         = options.name;
    this->tag          = options.tag;
    this->related_tags = options.related_tags;
    Clone(gameObject);
    //this->getComponent<Collider2D>().color = options.collider_color;
    this->getComponent<Transform2D>().position = options.position;
}

void GameObject::Clone(GameObject& gameObject) {
    components[typeid(Transform2D)] = gameObject.components[typeid(Transform2D)]->Clone(*this);
    for (auto [type, component] : gameObject.components) {
        if (type != typeid(Transform2D)) {
            components[type] = component->Clone(*this);
        }
    }
    for (auto [type, script] : gameObject.scripts) {
        scripts[type] = dynamic_cast<Script*>(script->Clone(*this));
    }
}

void GameObject::Destroy() {
    for (auto& component : components) {
        component.second->Unload();
        delete component.second;
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
    scale = {};
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
    std::cout << "Hola: " << scale << "\n";
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
    this->scale = {scale, scale};
    frame_src = {0,0,(float)frame_width,(float)frame_height};
    frame_dst = {0,0,frame_width * scale, frame_height * scale};
    // Animation timing:
    this->duration = duration;
    frame_lapse = duration/(float)total_frames;
    current_frame_time = 0.0f;
    current_time = 0.0f;
    this->loop = loop;
}
Animation::Animation(const char* fileName, int frame_width, int frame_height, Vector2 scale, float duration, bool loop) {
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
    frame_dst = {0,0,frame_width * scale.x, frame_height * scale.y};
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
Vector2 Animation::GetScale() {
    return scale;
}
Vector2 Animation::GetDimensions() {
    return {std::abs(frame_src.x), std::abs(frame_src.y)};
}
Vector2 Animation::GetViewDimensions() {
    return {std::abs(frame_dst.width), std::abs(frame_dst.height)};
}
bool Animation::HasFinished() {
    return (current_time >= duration);
}
void Animation::Play(Vector2 position) {
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
    current_frame_time += GetFrameTime();
    current_time += GetFrameTime(); 
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
Vector2 Animator::GetScale() {
    return Animations[current_animation].GetScale();
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
void Animator::Play() {
    Animations[current_animation].Play(gameObject.getComponent<Transform2D>().position);
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
    : Component(gameObject), color(color), pos(pos), size({(float)width, (float)height}), active(true) {}
Collider2D::Collider2D(GameObject& gameObject, Vector2* pos, Vector2 size, Color color)
    : Component(gameObject), color(color), pos(pos), size(size), active(true) {}
Collider2D::Collider2D(GameObject& gameObject, Collider2D& collider) : Component(gameObject), active(true) {
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
void RigidBody2D::Draw() {
    auto center = gameObject.getComponent<Transform2D>().position + 
        (gameObject.getComponent<Collider2D>().size/2);
    DrawLineEx(center, center + velocity, 3.0f, BLUE);
    DrawLineEx(center, center + Vector2{velocity.x, 0.0f}, 3.0f, RED);
    DrawLineEx(center, center + Vector2{0.0f, velocity.y}, 3.0f, GREEN);
}

//-----------------------------------------------------------------------------
// Scripts
//-----------------------------------------------------------------------------
Script::Script(GameObject& gameObject) : Component(gameObject) {}

//-----------------------------------------------------------------------------
// Sprites
//-----------------------------------------------------------------------------
Sprite::Sprite(GameObject& gameObject, const char* fileName, float scale) : Component(gameObject) {
    img = LoadTexture(fileName);
    this->scale = {scale, scale};
    src = {0, 0, (float)img.width, (float)img.height};
    dst = {0, 0, img.width * scale, img.height * scale};
}
Sprite::Sprite(GameObject& gameObject, const char* fileName, Vector2 scale) : Component(gameObject) {
    img = LoadTexture(fileName);
    this->scale = scale;
    src = {0, 0, (float)img.width, (float)img.height};
    dst = {0, 0, img.width * scale.x, img.height * scale.y};
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
Vector2 Sprite::GetScale() { 
    return scale;
}
Vector2 Sprite::GetDimensions() {
    return {src.width, src.height};
}
Vector2 Sprite::GetViewDimensions() {
    return {dst.width, dst.height};
}
void Sprite::Draw() {
    auto& pos = gameObject.getComponent<Transform2D>().position;
    dst.x = pos.x; dst.y = pos.y;
    DrawTexturePro(img, src, dst, {0,0}, 0, WHITE);
}
void Sprite::Unload() {
    UnloadTexture(img);
}

void Sprite::ChangeTexture(const char* path){
    img = LoadTexture(path);
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
// Game System
// ----------------------------------------------------------------------------
/**
 * @brief Va a cambiar un poco la representación interna del sistema del juego:
 *  - Los objetos se van a dividir por labels/tags: una tag es un nombre que puede englobar uno o varios gameobjects.
 *  - A la hora de instanciar un GameObject, se puede decidir la configuración del mismo (tag, nombre, tags de comprobación).
 *  - Si defines los tags, a la hora de comprobar las colisiones, comprobará solo las tags que se le han indicado en la configuración.
 */

Collision::Collision(GameObject& gameObject, float contact_time, Vector2 contact_point, Vector2 contact_normal)
    : gameObject(gameObject), contact_time(contact_time), contact_point(contact_point), contact_normal(contact_normal) {}

bool GameSystem::Collides(const Vector2 ray_o, const Vector2 ray_d,
    const Collider2D& target, Vector2& contact_point, Vector2& contact_normal,
    float& contact_time)
{
    DrawLineEx(ray_o, ray_o + (10000 * ray_d), 2.0f, PINK);
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
bool GameSystem::Collides(const Collider2D& A, const Collider2D& B, Vector2& contact_point, 
    Vector2& contact_normal, float& contact_time)
{
    // Expandir el rectangulo destino con las dimensiones del rectangulo origen.
    Vector2 exp_B_pos = *B.pos - (A.size/2);
    if (Collides(
        // El rayo se lanza desde el centro del collider:
        *A.pos + A.size/2,
        // La dirección del rayo se obtiene mediante la velocidad del objeto:
        A.gameObject.getComponent<RigidBody2D>().velocity * GetFrameTime(),
        // Se crea un nuevo collider ampliado a la mitad del objeto dinámico:
        Collider2D(B.gameObject, &exp_B_pos, B.size + A.size),
        // Información de la colision:
        contact_point,  // Punto del collider con el que el rayo intersecta.
        contact_normal, // Normal del lado con el que el rayo intersecta.
        contact_time    // Tiempo restante de interseccion.
    )) {
        return (contact_time >= 0.0f && contact_time < 1.0f);
    }
    return false;
}

void GameSystem::Collisions(GameObject& gameObject) {
    if (!gameObject.tag.empty() && gameObject.hasComponent<Collider2D>() && gameObject.hasComponent<RigidBody2D>()) {
        for (auto& rel_tag : gameObject.related_tags) {
            for (auto& [check_name, check_ref] : GameObjects[rel_tag]) {
                if (gameObject.name != check_name && check_ref.gameObject->hasComponent<Collider2D>()) {
                    float ct = 0; Vector2 cp{0,0}, cn{0,0};
                    auto Collider_A = gameObject.getComponent<Collider2D>();
                    auto Collider_B = check_ref.gameObject->getComponent<Collider2D>();
                    if (Collides(Collider_A, Collider_B, cp, cn, ct)) {
                        if(check_ref.gameObject->tag != "Floor" && gameObject.tag != "Floor")
                            std::cout << gameObject.tag << " colisiona con " << check_ref.gameObject->tag << std::endl;
                        gameObject.OnCollision(Collision(*check_ref.gameObject, ct, cp, cn));
                        if (!check_ref.gameObject->hasComponent<RigidBody2D>()) {
                            check_ref.gameObject->OnCollision(Collision(gameObject, ct, cp, -cn));
                        }
                    }
                }
            }
        }
    }
}

void GameSystem::Instantiate(GameObject& gameObject, GameObjectOptions options) {
    // Configuring the name:
    if (options.name.empty()) {
        options.name = gameObject.name;
        if (nGameObjects.find(gameObject.name) != nGameObjects.end()) {
            options.name += " (" + std::to_string(nGameObjects[gameObject.name]++) + ")";
        } else {
            nGameObjects[gameObject.name] = 1;
        }
    }
    // Configuring the tag:
    if (options.tag.empty()) {
        options.tag = gameObject.tag;
    }
    // Configuring the secondary tags:
    if (options.second_tags.empty()) {
        options.second_tags = gameObject.second_tags;
    }
    // Configuring the related tags:
    if (options.tag == "All") {
        options.related_tags = {};
        for (auto& [tag, _] : GameObjects) {
            options.related_tags.insert(tag);
        }
    }
    if (options.related_tags.empty()) {
        options.related_tags = gameObject.related_tags;
    }
    // Configuring the position:
    if (options.position.x == -1 || options.position.y == -1) {
        options.position = gameObject.getComponent<Transform2D>().position;
    }
    // Configuring the color:
    //if (options.collider_color.r == -1 || options.collider_color.g == -1 || options.collider_color.b == -1 || options.collider_color.a == -1) {
    //    options.collider_color = gameObject.getComponent<Collider2D>().color;
    //}
    // Inserting the new instance:
    GameObjects[options.tag][options.name] = {
        &gameObject,
        new GameObject(gameObject, options)
    };
}

void GameSystem::Printout() {
    std::cout << ">>> Scene objects <<<\n";
    for (auto& [tag, tag_instances] : GameObjects) {
        std::cout << ">>>> Tag: " << tag << "\n";
        for (auto& [name, instance] : tag_instances) {
            std::cout << ">>>>> Name: " << name << "\n";  
        }
    }
}

void GameSystem::Update() {
    for (auto& [_, gameObjectRefs] : GameObjects) {
        for (auto& [_, ref] : gameObjectRefs) {
            ref.gameObject->Update();
            Collisions(*ref.gameObject);
            if (ref.gameObject->hasComponent<Animator>()) {
                ref.gameObject->getComponent<Animator>().Play();
            } else if (ref.gameObject->hasComponent<Sprite>()) {
                ref.gameObject->getComponent<Sprite>().Draw();
            }
            if (ref.gameObject->hasComponent<Collider2D>()) {
                ref.gameObject->getComponent<Collider2D>().Draw();
            }
            if (ref.gameObject->hasComponent<RigidBody2D>()) {
                ref.gameObject->getComponent<RigidBody2D>().Draw();
            }
        }
    }
}


std::vector<GameObject*> GameSystem::getObjectsByTag(std::string tag) {
    std::vector<GameObject*> ret;
    for (auto& [_, ref] : GameObjects[tag])
        ret.push_back(ref.gameObject);

    return ret;
}
