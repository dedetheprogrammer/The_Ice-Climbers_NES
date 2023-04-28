#include "EngineECS.h"
#include "settings.h"

#include "Block.h"
#include "Cloud.h"
#include "Enemies.h"
#include "Popo.h"
#include "Fruit.h"

class Flicker {
private:
    float current_time; // Current time in the current state.
public:
    bool  action;       // Indicates if is the moment to perform the action or not.
    float trigger_time; // Time for triggering into the next state.

    Flicker() : current_time(0), action(false), trigger_time(0) {}
    Flicker(float trigger_time) : current_time(0), action(false), trigger_time(trigger_time) {}

    bool Trigger(float deltaTime) {
        current_time += deltaTime;
        if (current_time >= trigger_time) {
            action = !action;
            current_time = 0;
        }
        return action;
    }
};

Font NES;

void Game(int numPlayers) {

    int level_phase = 0;
    bool acabar = false;

    //MusicSource BGM("Assets/NES - Ice Climber - Sound Effects/Go Go Go - Nightcore.mp3", true);
    MusicSource BGM("Assets/Sounds/Mick Gordon - The Only Thing They Fear Is You.mp3", true);

    Canvas Mountain("Assets/Sprites/00_Mountain.png", {0,-1560}, {(float)WINDOW_WIDTH, WINDOW_HEIGHT*3.65f});
    Canvas LifePopo1("Assets/Sprites/Popo/Life.png", {30,40}, {20,20});
    Canvas LifePopo2("Assets/Sprites/Popo/Life.png", {60,40}, {20,20});
    Canvas LifePopo3("Assets/Sprites/Popo/Life.png", {90,40}, {20,20});
    Canvas LifeNana1("Assets/Sprites/Nana/Life.png", {30,70}, {20,20});
    Canvas LifeNana2("Assets/Sprites/Nana/Life.png", {60,70}, {20,20});
    Canvas LifeNana3("Assets/Sprites/Nana/Life.png", {90,70}, {20,20});

    // Rectangles = Sprites component?
    // Mountain background:
    //Rectangle Mountain_src{0, Mountain_sprite.height - Mountain_view_height - 10, (float)Mountain_sprite.width, Mountain_view_height};
    //Rectangle Mountain_dst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    // PAUSE frame:
    Texture2D Pause_frame = LoadTexture("Assets/Sprites/Small_frame.png");
    float paused_showtime = 0.75;
    bool show = true;
    Rectangle src_0{0, 0, (float)Pause_frame.width, (float)Pause_frame.height};
    Rectangle dst_1{(WINDOW_WIDTH - Pause_frame.width*3.0f)/2.0f + 4, (WINDOW_HEIGHT - Pause_frame.height)/2.0f - 3, Pause_frame.width*3.0f, Pause_frame.height*3.0f};

    GameObject BaseFloor("Base Floor", "Floor");
    BaseFloor.addComponent<Collider2D>(&BaseFloor.getComponent<Transform2D>().position, Vector2{1224,30}, PINK);
    GameSystem::Instantiate(BaseFloor, GameObjectOptions{.position{-100,560}});

    GameObject LevelFloor_0("Level Floor 0", "Floor");
    GameObject LevelFloor_1("Level Floor 1", "Floor");
    GameObject LevelFloor_2("Level Floor 2", "Floor");
    GameObject LevelFloor_3("Level Floor 3", "Floor");
    GameObject LevelFloor_4("Level Floor 4", "Floor");
    GameObject LevelWall_0("Level Wall 0", "Wall");
    GameObject LevelWall_1("Level Wall 1", "Wall");
    GameObject LevelWall_2("Level Wall 2", "Wall");
    GameObject Death("Death", "Death");

    // Scene:
    GameObject GrassBlock("Grass Block", "Floor", {"Block"});
    GrassBlock.addComponent<Sprite>("Assets/Sprites/Grass_block_large.png", 3.62f, 3.0f);
    int block_width = GrassBlock.getComponent<Sprite>().GetViewDimensions().x, block_height = GrassBlock.getComponent<Sprite>().GetViewDimensions().y;
    float collider_width  = block_width-5.0f;
    float collider_offset = (collider_width)/2;

    GameObject GrassHole("Grass Hole", "Hole");
    GrassHole.addComponent<Collider2D>(&GrassHole.getComponent<Transform2D>().position, Vector2{(float)block_width, (float)block_height}, RED);
    GrassHole.addComponent<Script, HoleBehavior>();

    float LevelFloor_0_width = block_width * 9.0f, LevelFloor_1_width = block_width * 6.0f, LevelFloor_2_width = block_width * 4.0f, LevelFloor_3_width = block_width * 3.0f,
        LevelFloor_4_width = block_width * 7.0f;
    LevelFloor_0.addComponent<Collider2D>(&LevelFloor_0.getComponent<Transform2D>().position, Vector2{LevelFloor_0_width,25}, PINK);
    LevelFloor_1.addComponent<Collider2D>(&LevelFloor_1.getComponent<Transform2D>().position, Vector2{LevelFloor_1_width,25}, PINK);
    LevelFloor_2.addComponent<Collider2D>(&LevelFloor_2.getComponent<Transform2D>().position, Vector2{LevelFloor_2_width,25}, PINK);
    LevelFloor_3.addComponent<Collider2D>(&LevelFloor_3.getComponent<Transform2D>().position, Vector2{LevelFloor_3_width,25}, PINK);
    LevelFloor_4.addComponent<Collider2D>(&LevelFloor_4.getComponent<Transform2D>().position, Vector2{LevelFloor_4_width,25}, PINK);
    LevelWall_0.addComponent<Collider2D>(&LevelWall_0.getComponent<Transform2D>().position, Vector2{25, block_height * 6.0f}, YELLOW);
    LevelWall_1.addComponent<Collider2D>(&LevelWall_1.getComponent<Transform2D>().position, Vector2{25, block_height * 7.0f}, YELLOW);
    LevelWall_2.addComponent<Collider2D>(&LevelWall_2.getComponent<Transform2D>().position, Vector2{25, block_height * 3.0f}, YELLOW);
    LevelWall_2.addComponent<Collider2D>(&LevelWall_2.getComponent<Transform2D>().position, Vector2{25, block_height * 3.0f}, YELLOW);
    Death.addComponent<Collider2D>(&Death.getComponent<Transform2D>().position, Vector2{block_height * 10.0f, 25}, RED);

    GrassBlock.addComponent<Collider2D>(&GrassBlock.getComponent<Transform2D>().position, GrassBlock.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    GrassBlock.addComponent<Script, BlockBehavior>();

    GrassBlock.getComponent<Script, BlockBehavior>().hole = &GrassHole;
    GrassHole.getComponent<Script, HoleBehavior>().original_block = &GrassBlock;
    for (int i = 0; i < 24; i++) {
        GameSystem::Instantiate(GrassBlock, GameObjectOptions{.position{113.0f + block_width * i, 423}});
    }
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f - LevelFloor_0_width, 423}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f + block_width*24, 423}});

    GameObject DirtBlock("Dirt Block", "Floor", {"Block"});
    GameObject DirtHole("Dirt Hole", "Hole");
    DirtHole.addComponent<Collider2D>(&DirtHole.getComponent<Transform2D>().position, Vector2{(float)block_width, (float)block_height}, RED);
    DirtHole.addComponent<Script, HoleBehavior>();

    DirtBlock.addComponent<Sprite>("Assets/Sprites/Dirt_block_large.png", 3.62f, 3.0f);
    DirtBlock.addComponent<Collider2D>(&DirtBlock.getComponent<Transform2D>().position, DirtBlock.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    DirtBlock.addComponent<Script, BlockBehavior>();

    DirtBlock.getComponent<Script, BlockBehavior>().hole = &DirtHole;
    DirtHole.getComponent<Script, HoleBehavior>().original_block = &DirtBlock;


    for (int i = 0; i < 22; i++) {
        GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{(113.0f+block_width) + block_width*i, 287}});
        GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{(113.0f+block_width) + block_width*i, 150}});
        GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{(113.0f+block_width) + block_width*i,  14}});
    }
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f - LevelFloor_0_width + block_width, 287}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f + block_width*23, 287}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f - LevelFloor_0_width + block_width, 150}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f + block_width*23, 150}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f - LevelFloor_0_width + block_width, 14}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f + block_width*23, 14}});

    GameObject IceBlock("Ice Block", "Floor", {"Ice","Block"});
    GameObject IceHole("Dirt Hole", "Hole");
    IceHole.addComponent<Collider2D>(&IceHole.getComponent<Transform2D>().position, Vector2{(float)block_width, (float)block_height}, RED);
    IceHole.addComponent<Script, HoleBehavior>();

    IceBlock.addComponent<Sprite>("Assets/Sprites/Ice_block_large.png", 3.62f, 3.0f);
    IceBlock.addComponent<Collider2D>(&IceBlock.getComponent<Transform2D>().position, IceBlock.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    IceBlock.addComponent<Script, BlockBehavior>();

    IceBlock.getComponent<Script, BlockBehavior>().hole = &IceHole;
    IceHole.getComponent<Script, HoleBehavior>().original_block = &IceBlock;
    for (int i = 0; i < 20; i++) {
        GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{(113.0f+(2*block_width)) + block_width*i, -123}});
        GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{(113.0f+(2*block_width)) + block_width*i, -260}});
        GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{(113.0f+(2*block_width)) + block_width*i, -397}});
    }
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f - LevelFloor_0_width + 2*block_width, -123}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f + block_width*22, -123}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f - LevelFloor_0_width + 2*block_width, -260}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f + block_width*22, -260}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f - LevelFloor_0_width + 2*block_width, -397}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{113.0f + block_width*22, -397}});

    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{114.0f + block_width*21, -533}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{114.0f - LevelFloor_0_width + 3*block_width, -533}});
    GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{114.0f + block_width*5, -533}});
    GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{114.0f + block_width*13, -533}});
    GameSystem::Instantiate(LevelWall_0, GameObjectOptions{.position{115.0f - 2*block_width, -624.0f - 2*block_height}});
    GameSystem::Instantiate(LevelWall_0, GameObjectOptions{.position{114.0f + block_width*25, -624.0f - 2*block_height}});

    GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{113.0f + block_width*2, -626}});
    GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{113.0f + block_width*9, -626}});
    GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{113.0f + block_width*18, -626}});

    GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{115.0f - block_width, -620.0f - 9*block_height}});
    GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{115.0f + block_width*24, -620.0f - 9*block_height}});
    GameObject Cloud("Cloud", "Cloud");
    Cloud.addComponent<Sprite>("Assets/Sprites/Cloud_Slow_Long.png", 3.62f, 3.0f);
    Cloud.addComponent<Collider2D>(&Cloud.getComponent<Transform2D>().position, Cloud.getComponent<Sprite>().GetViewDimensions());
    Cloud.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{100, 0});
    Cloud.addComponent<Script, CloudBehavior>();
    GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, -626.0f - block_width*5}});

    GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{116.0f, -615.0f - 16*block_height}});
    GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{115.0f + block_width*23, -615.0f - 16*block_height}});
    GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{114.0f - block_width, -612.0f - 12*block_height}});
    GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{114.0f + 14*block_width, -612.0f - 11*block_height}});

    GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{116.0f + block_width, -629.0f - 22*block_height}});
    GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{115.0f + block_width*22, -629.0f - 22*block_height}});
    GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{113.0f + 8*block_width, -606.0f - 17*block_height}});
    GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{115.0f + 17*block_width, -607.0f - 16*block_height}});

    GameObject SmallCloud("Small Cloud", "Cloud");
    SmallCloud.addComponent<Sprite>("Assets/Sprites/Cloud_Slow_Short.png", 3.62f, 3.0f);
    SmallCloud.addComponent<Collider2D>(&SmallCloud.getComponent<Transform2D>().position, SmallCloud.getComponent<Sprite>().GetViewDimensions());
    SmallCloud.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{100, 0});
    SmallCloud.addComponent<Script, CloudBehavior>();
    GameSystem::Instantiate(SmallCloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, -626.0f - block_width*18}});

    GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{116.0f + block_width*2, -620.0f - 29*block_height}});
    GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{115.0f + block_width*21, -620.0f - 29*block_height}});
    GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{113.0f + 4*block_width, -614.0f - 27*block_height}});
    GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{114.0f + 12*block_width, -617.0f - 26*block_height}});

    GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{116.0f + block_width*3, -634.0f - 35*block_height}});
    GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{115.0f + block_width*20, -634.0f - 35*block_height}});
    GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{114.0f + 8*block_width, -610.0f - 32*block_height}});
    GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{114.0f + 15*block_width, -612.0f - 30*block_height}});
    GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{114.0f + 10*block_width, -608.0f - 35*block_height}});

    GameSystem::Instantiate(LevelWall_2, GameObjectOptions{.position{116.0f + block_width*4, -629.0f - 38*block_height}});
    GameSystem::Instantiate(LevelWall_2, GameObjectOptions{.position{115.0f + block_width*19, -629.0f - 38*block_height}});
    GameSystem::Instantiate(LevelFloor_4, GameObjectOptions{.position{114.0f + 2*block_width, -624.0f - 39*block_height}});
    GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{114.0f + 16*block_width, -624.0f - 39*block_height}});
    GameSystem::Instantiate(Death, GameObjectOptions{.position{114.0f - 6*block_width, -624.0f - 38*block_height}});
    GameSystem::Instantiate(Death, GameObjectOptions{.position{114.0f + 22*block_width, -624.0f - 38*block_height}});

    GameObject Condor("Red Condor", "Goal");
    Condor.addComponent<Animator>("Fly", std::unordered_map<std::string, Animation> {
        {"Fly", Animation("Assets/Sprites/Red_condor_fly.png", 32, 16, 2.7, 0.75, true)}
    });
    Condor.addComponent<Collider2D>(&Condor.getComponent<Transform2D>().position, Condor.getComponent<Animator>().GetViewDimensions(), ORANGE);
    Condor.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{100, 0});
    Condor.addComponent<Script, RedCondorBehavior>();
    GameSystem::Instantiate(Condor, GameObjectOptions{.position={400, -624.0f - 49*block_height}});

    GameObject Icicle("Icicle", "Icicle", {}, {"Hole", "Player", "Floor"});
    Icicle.addComponent<Sprite>("Assets/Sprites/Icicle.png", 3, 3);
    Icicle.addComponent<Collider2D>(&Icicle.getComponent<Transform2D>().position, Icicle.getComponent<Sprite>().GetViewDimensions());
    Icicle.addComponent<RigidBody2D>(1, 98, Vector2{0,0}, Vector2{0,0});
    Icicle.addComponent<Script, IcicleBehavior>();

    GameObject Topi("Topi", "Enemy", {"Topi"}, {"Floor", "Hole", "Player", "Icicle"});
    GameObject Fruit("Fruit", "Fruit");
    Fruit.addComponent<Sprite>("Assets/Sprites/Fruit_Eggplant.png", 3.62f, 3.0f);
    Fruit.addComponent<Collider2D>(&Fruit.getComponent<Transform2D>().position, Fruit.getComponent<Sprite>().GetViewDimensions(), ORANGE);
    Fruit.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{100, 0});
    Fruit.addComponent<Script, FruitBehavior>();
    GameSystem::Instantiate(Fruit, GameObjectOptions{.position={400, -1055}});
    GameSystem::Instantiate(Fruit, GameObjectOptions{.position={550, -1380}});
    GameSystem::Instantiate(Fruit, GameObjectOptions{.position={350, -1430}});

    Topi.addComponent<Animator>("Walk", std::unordered_map<std::string, Animation> {
            {"Walk", Animation("Assets/Sprites/Topi/01_Walk.png", 16, 16, 3, 0.3, true)},
            {"Stunned", Animation("Assets/Sprites/Topi/02_Stunned.png", 16, 16, 3, 0.5, true)},
        }
    );
    Topi.addComponent<RigidBody2D>(1, 375, Vector2{0,0}, Vector2{70,0});
    Vector2 topi_size = Topi.getComponent<Animator>().GetViewDimensions();
    Topi.addComponent<Collider2D>(&Topi.getComponent<Transform2D>().position, Vector2{collider_width, topi_size.y}, Vector2{topi_size.x/2 - collider_offset, 0});
    Topi.addComponent<Script, TopiBehavior>(Icicle);
    GameSystem::Instantiate(Topi, GameObjectOptions{.position{0,510}});
    GameSystem::Instantiate(Topi, GameObjectOptions{.position{0,220}});

    // ¿Como construyo un GameObject para Popo?
    // 1. Creamos el GameObject. Recuerda:
    //  - El GameObject no tiene ningún componente nada más crearlo.
    //  - El GameObject solo puede tener un elemento de cada tipo. Si le vuelves
    //    a meter otro, perderá el primero.
    GameObject Popo("Popo", "Player", {}, {"Floor", "Wall", "Cloud", "Enemy", "Goal", "Hole", "Fruit"});
    GameObject Nana("Nana", "Player", {}, {"Floor", "Wall", "Cloud", "Enemy", "Goal", "Hole", "Fruit"});
    // 2.a Añadimos el componente Transform. Es muy importante este componente ya que es el que indica las propiedades
    //  del objeto, como posicion, tamaño o rotación. De momento solo usamos tamaño.
    Popo.addComponent<Transform2D>();
    Nana.addComponent<Transform2D>();
    // 2.b. Se podría haber ahorrado el addComponent<Transform2D> y crearlo en el GameObject directamente:
    // GameObject Popo(Vector2{600,500});
    // 3. Añadimos el componente de Animaciones. Como veis, hay que indicarle de que tipo es la lista {...},
    // si no, dará error.
    Popo.addComponent<Animator>("Idle", std::unordered_map<std::string, Animation> {
        {"Idle", Animation("Assets/Sprites/Popo/00_Idle.png", 16, 24, 2.5, 0.75, true)},
        {"Walk", Animation("Assets/Sprites/Popo/02_Walk.png", 16, 24, 2.5, 0.135, true)},
        {"Brake", Animation("Assets/Sprites/Popo/03_Brake.png", 16, 24, 2.5, 0.3, true)},
        {"Jump", Animation("Assets/Sprites/Popo/04_Jump.png", 20, 25, 2.5, 0.5, false)},
        {"Attack", Animation("Assets/Sprites/Popo/05_Attack.png", 21, 25, 2.5, 0.3, false)},
        {"Stunned", Animation("Assets/Sprites/Popo/06_Stunned.png", 16, 21, 2.5, 0.5, true)},
        {"Fall", Animation("Assets/Sprites/Popo/07_Fall.png", 21, 25, 2.5, 0.3, false)},
        //{"Crouch", Animation("Assets/Sprites/Popo/06_Crouch.png", 0,0,0,0, false)},
    });
    Nana.addComponent<Animator>("Idle", std::unordered_map<std::string, Animation> {
        {"Idle", Animation("Assets/Sprites/Nana/00_Idle.png", 16, 24, 2.5, 0.75, true)},
        {"Walk", Animation("Assets/Sprites/Nana/02_Walk.png", 16, 24, 2.5, 0.135, true)},
        {"Brake", Animation("Assets/Sprites/Nana/03_Brake.png", 16, 24, 2.5, 0.3, true)},
        {"Jump", Animation("Assets/Sprites/Nana/04_Jump.png", 20, 25, 2.5, 0.5, false)},
        {"Attack", Animation("Assets/Sprites/Nana/05_Attack.png", 21, 25, 2.5, 0.3, false)},
        {"Stunned", Animation("Assets/Sprites/Nana/06_Stunned.png", 16, 21, 2.5, 0.5, true)},
        {"Fall", Animation("Assets/Sprites/Nana/07_Fall.png", 21, 25, 2.5, 0.3, false)},
        //{"Crouch", Animation("Assets/Sprites/Popo/06_Crouch.png", 0,0,0,0, false)},
    });
    // 3. Añadimos el componente de Audio:
    Popo.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/09-Jump.wav"))},
    });
    Nana.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/09-Jump.wav"))},
    });
    // 4. Añadimos el Rigidbody:
    Popo.addComponent<RigidBody2D>(1, 500, Vector2{70,0}, Vector2{200,400});
    Nana.addComponent<RigidBody2D>(1, 500, Vector2{70,0}, Vector2{200,400});
    // 5. Añadimos el Collider. Este es el componente más jodido, necesitas:
    //  - El Transform2D que tiene la posición del objeto.
    //  - El Animator que tiene el tamaño del sprite según en que animación esté, en este
    //    caso, es la animación inicial.

    Vector2 popo_size = Popo.getComponent<Animator>().GetViewDimensions();
    Vector2 nana_size = Nana.getComponent<Animator>().GetViewDimensions();
    Popo.addComponent<Collider2D>(&Popo.getComponent<Transform2D>().position, Vector2{collider_width, popo_size.y}, Vector2{popo_size.x/2 - collider_offset, 0});
    Nana.addComponent<Collider2D>(&Nana.getComponent<Transform2D>().position, Vector2{collider_width, nana_size.y}, Vector2{nana_size.x/2 - collider_offset, 0});
    Popo.addComponent<Script, PopoBehavior>(Controller_0);
    Nana.addComponent<Script, PopoBehavior>(Controller_1);
    GameObject& Player = GameSystem::Instantiate(Popo, GameObjectOptions{.position = {400,450}});
    //GameObject& Player2;
    GameObject* Player_2 = nullptr;
    if (numPlayers == 2) {
        Player_2 = &GameSystem::Instantiate(Nana, GameObjectOptions{.position = {450,450}});
    }


    //GameSystem::Printout();
    float timeToShowScores = 0.0f;
    bool finished = false;
    bool play_music = false;
    bool paused = false;
    bool moving_camera = false;
    float objects_offset = 80, current_objects_offset = 0;
    BGM.Init();

    GameSystem::Start();
    while(!WindowShouldClose() && !finished) {
        BeginDrawing();
        ClearBackground(BLACK);
        if (IsKeyPressed(KEY_M)) {
            play_music = !play_music;
        }
        if (play_music) {
            BGM.Play();
        }

        Mountain.Draw();
        if (IsGamepadAvailable(0)) {
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                paused = !paused;
            }
        } else if (IsKeyPressed(KEY_BACKSPACE)){
            paused = !paused;
        }
        if (!paused) {
            if(Player.getComponent<Script, PopoBehavior>().lifes > 0) {
                LifePopo1.Draw();
            }
            if(Player.getComponent<Script, PopoBehavior>().lifes > 1) {
                LifePopo2.Draw();
            }
            if(Player.getComponent<Script, PopoBehavior>().lifes > 2) {
                LifePopo3.Draw();
            }
            if(numPlayers == 2){
                if(Player_2->getComponent<Script, PopoBehavior>().lifes > 0) {
                    LifeNana1.Draw();
                }
                if(Player_2->getComponent<Script, PopoBehavior>().lifes > 1) {
                    LifeNana2.Draw();
                }
                if(Player_2->getComponent<Script, PopoBehavior>().lifes > 2) {
                    LifeNana3.Draw();
                }
            }
            if (!moving_camera && Player.getComponent<Script, PopoBehavior>().isGrounded && Player.getComponent<Transform2D>().position.y < 150) {
                moving_camera = true;
                switch (level_phase) {
                case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                    level_phase++;
                    break;
                }
                //if (level_phase++ == 8) {
                //    objects_offset = 150;
                //} else {
                //    objects_offset = 80;
                //}
            }
            if(acabar){
                if (timeToShowScores < 2.0) timeToShowScores += GetFrameTime();
                else {
                    DrawText("WIN", 400, 100, 100, GREEN);
                    DrawText("Victoria 3000", 250, 200, 60, WHITE);
                    DrawText(("Frutas x " +  std::to_string(Player.getComponent<Script, PopoBehavior>().frutasRecogidas)).c_str(), 250, 270, 60, WHITE);
                    DrawText(("Bloques rotos x " + std::to_string(Player.getComponent<Script, PopoBehavior>().bloquesDestruidos)).c_str(), 250, 340, 60, WHITE);
                    DrawText(("Puntuación: " + std::to_string(3000 + Player.getComponent<Script, PopoBehavior>().frutasRecogidas * 300 +
                        Player.getComponent<Script, PopoBehavior>().bloquesDestruidos *10)).c_str(), 150, 420, 80, WHITE);

                    if(IsKeyPressed(KEY_ENTER)){
                        finished = true;
                    }
                }
            }
            if (!moving_camera) {
                GameSystem::Update();
                if (Player.getComponent<Script, PopoBehavior>().lifes <= 0 && !acabar) {
                    std::cout << "GAME OVER!\n";
                    finished = true;
                }
                if(Player.getComponent<Script, PopoBehavior>().victory){

                    //std::cout << "muevo camara puntuacions" << std::endl;
                    Player.getComponent<Script, PopoBehavior>().victory;
                    float shift = 300 * GetFrameTime();
                    current_objects_offset  += shift;
                    if (current_objects_offset <= objects_offset) {
                        GameSystem::Move({0,shift});
                        Mountain.Move({0,shift});
                    }else{
                        acabar = true;
                        current_objects_offset = 0;
                    }
                }
            } else {
                //std::cout << Player.getComponent<Script, PopoBehavior>().victory  << std::endl;
                float shift = 100 * GetFrameTime();
                current_objects_offset  += shift;
                if (current_objects_offset <= objects_offset) {
                    GameSystem::Move({0,shift});
                    Mountain.Move({0,shift});
                } else {
                    current_objects_offset = 0;
                    moving_camera = false;
                }
                GameSystem::Render();
            }
        } else {
            DrawTexturePro(Pause_frame, src_0, dst_1, Vector2{0,0}, 0, WHITE);
            if (show) {
                if (paused_showtime <= 0) {
                    paused_showtime = 0.75;
                    show = false;
                } else {
                    DrawTextPro(NES, "PAUSED", Vector2{WINDOW_WIDTH/2.0f-55, WINDOW_HEIGHT/2.0f}, Vector2{0,0}, 0, 30, 1.5, WHITE);
                }
            } else {
                if (paused_showtime <= 0){
                    paused_showtime = 0.75;
                    show = true;
                }
            }
            paused_showtime -= GetFrameTime();
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }
        if (IsKeyPressed(KEY_R)) {
            Player.getComponent<Transform2D>().position = Vector2{600,70};
        }
        DrawText("Press [M] to mute the music", 20, 20, 20, WHITE);
        EndDrawing();
    }
    UnloadTexture(Pause_frame);
    Popo.Destroy();
    Nana.Destroy();
    LevelFloor_0.Destroy();
    LevelFloor_1.Destroy();
    LevelFloor_2.Destroy();
    GrassBlock.Destroy();
    GameSystem::DestroyAll();
    BGM.Unload();
}

//-----------------------------------------------------------------------------
// Menus
// ----------------------------------------------------------------------------
enum MENU_ENUM { MAIN_MENU, NEW_GAME, NORMAL_GAME, SETTINGS, VIDEO_SETTINGS, AUDIO_SETTINGS, CONTROL_SETTINGS };

int main() {
    init_config();
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<float> D(0, (float)WINDOW_WIDTH);
    NES = LoadFont("Assets/Fonts/Pixel_NES.otf");

    // ---- Music
    /*Music ts_music = LoadMusicStream("Assets/NES - Ice Climber - Sound Effects/01-Main-Title.mp3");
    ts_music.looping = true;
    bool play_music = false;*/

    // Initial trailer --------------------------------------------------------
     int state = 0, shown = 0;
    
    Texture2D NintendoLogo = LoadTexture("Assets/SPRITES/Nintendo_logo.png");
    float nintendologo_fade = 0;
    float nintendologo_fade_add = 0.4;
    Rectangle NintendoLogoSrc{0, 0, (float)NintendoLogo.width, (float)NintendoLogo.height};
    Rectangle NintendoLogoDst{(WINDOW_WIDTH - NintendoLogo.width*0.3f)/2.0f, (WINDOW_HEIGHT - NintendoLogo.height*0.3f)/2.0f, NintendoLogo.width*0.3f, NintendoLogo.height*0.3f};
    Texture2D TeamLogo = LoadTexture("Assets/SPRITES/Team_logo.png");
    float teamlogo_fade = 0;
    float teamlogo_fade_add = 0.3;
    Rectangle TeamLogoSrc{0, 0, (float)TeamLogo.width, (float)TeamLogo.height};
    Rectangle TeamLogoDst{(WINDOW_WIDTH - TeamLogo.width*2.8f)/2.0f, (WINDOW_HEIGHT - TeamLogo.height*2.8f)/2.0f, TeamLogo.width*2.8f, TeamLogo.height*2.8f};
    float full_black_fade = 1.0f;
    float full_black_fade_add = 0.3;
    Vector2 text_measures = MeasureTextEx(NES, "PRESS <ENTER> TO START", 35, 2);
    Flicker pstart(1);
    bool first_enter   = false;
    float current_key_cooldown = 0, key_cooldown_add = 0.2, key_cooldown = 0.1;

    // Titlescreen ------------------------------------------------------------
    // - Fore pines
    Texture2D ForePines = LoadTexture("Assets/SPRITES/Titlescreen_01_Fore_Pines.png");
    float ForePinesSpeed = 0.6;
    float ForePinesHeight = (WINDOW_WIDTH * ForePines.height)/(float)(ForePines.width);
    Rectangle ForePinesSrc{0, 0, (float)ForePines.width, (float)ForePines.height};
    Rectangle ForePinesDst{0, WINDOW_HEIGHT - ForePinesHeight, (float)WINDOW_WIDTH, ForePinesHeight};
    // - Mid pines
    Texture2D MidPines = LoadTexture("Assets/SPRITES/Titlescreen_02_Mid_Pines.png");
    float MidPinesSpeed = 0.3;
    float MidPinesHeight = (WINDOW_WIDTH * MidPines.height)/(float)(MidPines.width);
    Rectangle MidPinesSrc{0, 0, (float)MidPines.width, (float)MidPines.height};
    Rectangle MidPinesDst{0, WINDOW_HEIGHT - MidPinesHeight + 5, (float)WINDOW_WIDTH, MidPinesHeight};
    // - Mountain
    Texture2D Mountain = LoadTexture("Assets/SPRITES/Titlescreen_03_Mountain.png");
    float MountainSpeed = 0.1;
    float MountainHeight = (WINDOW_WIDTH * Mountain.height)/(float)(Mountain.width);
    Rectangle MountainSrc{0, 0, (float)Mountain.width, (float)Mountain.height};
    Rectangle MountainDst{D(e2), WINDOW_HEIGHT - (MountainHeight * 0.75f), WINDOW_WIDTH * 0.75f, MountainHeight * 0.75f};
    // - Background fields
    Texture2D Fields = LoadTexture("Assets/SPRITES/Titlescreen_04_Fields.png");
    float FieldsHeight = (WINDOW_WIDTH * Fields.height)/(float)(Fields.width);
    Rectangle FieldsSrc{0, 0, (float)Fields.width, (float)Fields.height};
    Rectangle FieldsDst{0, WINDOW_HEIGHT - FieldsHeight, (float)WINDOW_WIDTH, FieldsHeight};
    // - Falling snow
    float SnowSpeed = 0.1;
    Texture2D Snow = LoadTexture("Assets/SPRITES/Titlescreen_05_Snow.png");
    Rectangle SnowSrc{0, 0, (float)Snow.width,  (float)Snow.height};
    Rectangle SnowDst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    // - Sign
    float SignSpeed = 105, height_dst = 30;
    Texture2D Sign = LoadTexture("Assets/SPRITES/Titlescreen_06_Sign.png");
    Rectangle SignSrc{0, 0, (float)Sign.width,  (float)Sign.height};
    Rectangle SignDst{(WINDOW_WIDTH - Sign.width*2.5f)/2.0f, -Sign.height * 2.5f, Sign.width * 2.5f, Sign.height * 2.5f};

    Texture2D OldBackground = LoadTexture("Assets/SPRITES/Titlescreen_Old_01_Background.png");
    Rectangle OldBackgroundSrc{0, 0, (float)OldBackground.width,  (float)OldBackground.height};
    Rectangle OldBackgroundDst{0, 0, (float)WINDOW_WIDTH, WINDOW_HEIGHT - 90.0f};
    float water_speed = 5;
    Texture2D OldWater = LoadTexture("Assets/SPRITES/Titlescreen_Old_02_Water.png");
    Rectangle OldWaterSrc{0, 0, (float)OldWater.width,  (float)OldWater.height};
    Rectangle OldWaterDst{0,  WINDOW_HEIGHT - 90.0f, (float)WINDOW_WIDTH, 90.0f};
    Texture2D OldSign = LoadTexture("Assets/SPRITES/Titlescreen_Old_03_Sign.png");
    Rectangle OldSignSrc{0, 0, (float)OldSign.width,  (float)OldSign.height};
    Rectangle OldSignDst{(WINDOW_WIDTH - OldSign.width*2.5f)/2.0f, -OldSign.height * 2.5f, OldSign.width * 2.5f, OldSign.height * 2.5f};
    bool show_background = false;
    // - Copyright
    float CopySpeed = 30, copy_height_dst = WINDOW_HEIGHT - 50;
    Texture2D Copy = LoadTexture("Assets/SPRITES/Titlescreen_07_Copyright.png");
    Rectangle CopySrc{0, 0, (float)Copy.width, (float)Copy.height};
    Rectangle CopyDst{(WINDOW_WIDTH - Copy.width*3.0f)/2.0f, WINDOW_HEIGHT + (float)Copy.height*3.0f, Copy.width * 3.0f, Copy.height * 3.0f};

    // UI
    Texture2D Cross = LoadTexture("Assets/SPRITES/UI_Cross.png");
    Rectangle CrossSrc{0, 0, (float)Cross.width,  (float)Cross.height};
    Texture2D Transparent = LoadTexture("Assets/SPRITES/UI_Transparent.png");
    Rectangle TransparentSrc{0, 0, (float)Transparent.width,  (float)Transparent.height};
    Rectangle TransparentDst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    Texture2D Arrow = LoadTexture("Assets/SPRITES/UI_Arrow.png");
    Rectangle ArrowSrc{0, 0, (float)Arrow.width,  (float)Arrow.height};
    Rectangle ArrowSrcInv{0, 0, -(float)Arrow.width,  (float)Arrow.height};
    Animation OptionHammer("Assets/SPRITES/UI_Hammer_Spritesheet.png", 40, 24, 1.5, 0.5, true);
    Texture2D OldOptionHammer = LoadTexture("Assets/SPRITES/UI_Old_Hammer.png");
    Rectangle OldOptionHammerSrc{0, 0, (float)OldOptionHammer.width, (float)OldOptionHammer.height};
    Texture2D Enterkey = LoadTexture("Assets/SPRITES/Keys/enter.png");
    Rectangle EnterkeySrc{0, 0, (float)Enterkey.width, (float)Enterkey.height};
    Texture2D Spacekey  = LoadTexture("Assets/SPRITES/Keys/space.png");
    Rectangle SpacekeySrc{0, 0, (float)Spacekey.width, (float)Spacekey.height};

    // Efectos?
    std::vector<Flicker> parpadeos(2, Flicker(0.75));

    int menu_start = 224, menu_height = 290;
    bool fst_player = false, snd_player = false;
    bool close_window = false;
    bool speed_run = false;
    int current_option = 0;
    int OPTIONS = 4;
    int option_offset = menu_height/(OPTIONS+1);
    int option_drift  = 0;
    MENU_ENUM CURRENT_MENU = MAIN_MENU;
    
    //Game();
    
    while(!WindowShouldClose() && !close_window) {

        // Delta time:
        float deltaTime = GetFrameTime();

        // Background:
        if (show_background) {
            if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                DrawTexturePro(Fields, FieldsSrc, FieldsDst, {0,0}, 0, WHITE);
                MountainDst.x -= MountainSpeed;
                if (MountainDst.x + MountainDst.width < 0) MountainDst.x = GetScreenWidth();
                DrawTexturePro(Mountain, MountainSrc, MountainDst, {0,0}, 0, WHITE);
                SnowSrc.x += SnowSpeed;
                SnowSrc.y -= SnowSpeed;
                DrawTexturePro(Snow, SnowSrc, SnowDst, {0,0}, 0, WHITE);
                MidPinesSrc.x += MidPinesSpeed;
                DrawTexturePro(MidPines, MidPinesSrc, MidPinesDst, {0,0}, 0, WHITE);
                ForePinesSrc.x += ForePinesSpeed;
                DrawTexturePro(ForePines, ForePinesSrc, ForePinesDst, {0,0}, 0, WHITE);
            } else {
                DrawTexturePro(OldBackground, OldBackgroundSrc, OldBackgroundDst, {0,0}, 0, WHITE);
                OldWaterSrc.x += water_speed * deltaTime;
                DrawTexturePro(OldWater, OldWaterSrc, OldWaterDst, {0,0}, 0, WHITE);
            }
        }

        /*if (play_music) {
            UpdateMusicStream(ts_music);
        }*/

        // Begin drawing:
        BeginDrawing();
        ClearBackground(BLACK);
        if (state == 0) {
            DrawTexturePro(NintendoLogo, NintendoLogoSrc, NintendoLogoDst, {0,0}, 0, Fade(WHITE, nintendologo_fade));
            if (!shown) {
                if (nintendologo_fade <= 1.0f) {
                    nintendologo_fade += nintendologo_fade_add * deltaTime;
                } else {
                    shown = 1;
                }
            } else if (nintendologo_fade >= 0.0f) {
                nintendologo_fade -= nintendologo_fade_add * deltaTime;
            } else {
                state = 1;
                shown = 0;
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                state = 1;
                shown = 0;
            }
        } else if (state == 1) {
            DrawTexturePro(TeamLogo, TeamLogoSrc, TeamLogoDst, {0,0}, 0, Fade(WHITE, teamlogo_fade));
            if (!shown) {
                if (teamlogo_fade <= 1.0f) {
                    teamlogo_fade += teamlogo_fade_add * deltaTime;
                } else {
                    shown = 1;
                }
            } else if (teamlogo_fade >= 0.0f) {
                teamlogo_fade -= teamlogo_fade_add * deltaTime;
            } else {
                state = 2;
                shown = 0;
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                state = 2;
                shown = 0;
            }
        } else if (state == 2) {
            if (full_black_fade <= 0.9) {
                if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                    DrawTexturePro(Sign, SignSrc, SignDst, {0,0}, 0, WHITE);
                    if (!show_background) {
                        if (SignDst.y < height_dst) {
                            SignDst.y += SignSpeed * deltaTime;
                        } else {
                            show_background = true;
                            //PlayMusicStream(ts_music);
                        }
                        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                            SignDst.y = height_dst;
                            CopyDst.y = copy_height_dst;
                            show_background = true;
                            full_black_fade = 0;
                            first_enter = true;
                            //PlayMusicStream(ts_music);
                            //play_music = true;
                        }
                    }
                } else {
                    DrawTexturePro(OldSign, OldSignSrc, OldSignDst, {0,0}, 0, WHITE);
                    if (!show_background) {
                        if (OldSignDst.y < height_dst) {
                            OldSignDst.y += SignSpeed * deltaTime;
                        } else {
                            show_background = true;
                            //PlayMusicStream(ts_music);
                            //play_music = true;
                        }
                        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                            OldSignDst.y = height_dst;
                            CopyDst.y = copy_height_dst;
                            show_background = true;
                            full_black_fade = 0;
                            first_enter = true;
                            //PlayMusicStream(ts_music);
                            //play_music = true;
                        }
                    }
                }
                DrawTexturePro(Copy, CopySrc, CopyDst, {0,0}, 0, WHITE);
                if (CopyDst.y > copy_height_dst) {
                    CopyDst.y -= CopySpeed * deltaTime;
                }
            }
            if (!show_background) {
                DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, full_black_fade));
                full_black_fade -= full_black_fade_add * deltaTime;
            } else {
                if (pstart.Trigger(deltaTime)) {
                    if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                        auto aux = MeasureTextEx(NES, "PRESS", 35, 5);
                        DrawTextEx(NES, "PRESS", {WINDOW_WIDTH/2.0f - aux.x - (Enterkey.width * 1.7f) + 10, WINDOW_HEIGHT - 120.f}, 35,2, BLUE);
                        DrawTexturePro(Enterkey, EnterkeySrc, {WINDOW_WIDTH/2.0f - (Enterkey.width * 1.7f) + 10, WINDOW_HEIGHT - 120.0f, Enterkey.width * 1.7f, Enterkey.height * 1.7f}, {0,0}, 0, WHITE);
                        DrawTextEx(NES, "TO START", { WINDOW_WIDTH/2.0f + 30, WINDOW_HEIGHT - 120.f}, 35, 2, BLUE);
                    } else {
                        DrawTextEx(NES, "PRESS <ENTER> TO START", {(WINDOW_WIDTH - text_measures.x)/2.0f, WINDOW_HEIGHT - 200.0f}, 35, 2, BLUE);
                    }
                }
                if (!first_enter && IsKeyPressed(KEY_ENTER)) {
                    state = -1;
                } else {
                    if (current_key_cooldown < key_cooldown) {
                        current_key_cooldown += key_cooldown_add * deltaTime;
                    } else {
                        first_enter = false;
                    }
                }
            }
        } else {
            DrawTexturePro(Transparent, TransparentSrc, {0, WINDOW_HEIGHT - 45.0f, (float)WINDOW_WIDTH, 45.0f}, {0,0}, 0, WHITE);
            if (!std::get<bool>(ini["Graphics"]["OldFashioned"]) && show_background) {
                DrawTexturePro(Transparent, TransparentSrc, TransparentDst, {0,0}, 0, WHITE);
                DrawRectangle(464, menu_start, 1, menu_height - 20, LIGHTGRAY);
                DrawTexturePro(Enterkey, EnterkeySrc, {WINDOW_WIDTH - 180.0f, WINDOW_HEIGHT - 32.0f, Enterkey.width * 0.9f, Enterkey.height * 0.9f}, {0,0}, 0, WHITE);
            } else {
                DrawTextEx(NES, "<ENTER>", {WINDOW_WIDTH - 195.0f, WINDOW_HEIGHT - 28.0f}, 13, 1, WHITE);
            }
            DrawTextEx(NES, "Select option", {WINDOW_WIDTH - 127.0f, WINDOW_HEIGHT - 28.0f}, 13, 1, LIGHTGRAY);


            switch (CURRENT_MENU) {
            case MAIN_MENU:
                DrawTextEx(NES, "ICE CLIMBER", {500, (float)menu_start}, 35, 5, BLUE);
                DrawTextEx(NES, "CONTINUE", {500, menu_start + (float)option_offset}, 35, 2, GRAY);
                DrawTextEx(NES, "NEW GAME", {500, menu_start + (option_offset * 2.0f)}, 35, 2, WHITE);
                DrawTextEx(NES, "SETTINGS", {500, menu_start + (option_offset * 3.0f)}, 35, 2, WHITE);
                DrawTextEx(NES, "EXIT",     {500, menu_start + (option_offset * 4.0f)}, 35, 2, WHITE);
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (current_option) {
                    case 0:
                        std::cout << "Hola\n";
                        break;
                    case 1:
                        CURRENT_MENU   = NEW_GAME;
                        current_option = 0;
                        break;
                    case 2:
                        CURRENT_MENU   = SETTINGS;
                        current_option = 0;
                        break;
                    case 3:
                        close_window = true;
                    }
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    close_window = true;
                }
                break;
            case NEW_GAME:
                DrawTextEx(NES, "NEW GAME", {500, (float)menu_start}, 35, 5, BLUE);
                DrawTextEx(NES, "NORMAL MODE",  {500, menu_start + (float)option_offset}, 35, 2, WHITE);
                DrawTextEx(NES, "BRAWL!",       {500, menu_start + (option_offset * 2.0f)}, 35, 2, GRAY);
                DrawTextEx(NES, "ENDLESS MODE", {500, menu_start + (option_offset * 3.0f)}, 35, 2, GRAY);
                DrawTextEx(NES, "RETURN",       {500, menu_start + (option_offset * 4.0f)}, 35, 2, WHITE);
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (current_option) {
                    case 0:
                        CURRENT_MENU = NORMAL_GAME;
                        OPTIONS = 3;
                        current_option = 0;
                        option_offset = menu_height/(OPTIONS+1);
                        break;
                    case 1: case 2:
                        break;
                    case 3:
                        CURRENT_MENU   = MAIN_MENU;
                        current_option = 1;
                        break;
                    }
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    CURRENT_MENU   = MAIN_MENU;
                    current_option = 1;
                }
                break;
            case NORMAL_GAME:
                DrawTextEx(NES, "NORMAL MODE", {500, (float)menu_start}, 35, 5, BLUE);
                DrawTextEx(NES, "START!", {500, menu_start + (float) option_offset}, 35, 2, fst_player ? WHITE : GRAY);
                DrawTextEx(NES, "SPEED RUN?", {500, menu_start + (option_offset * 2.0f)}, 35, 2, WHITE);
                DrawRectangleV({755, 349}, {20, 20}, WHITE);
                DrawRectangleV({756, 350}, {15, 15}, BLACK);
                if (speed_run) {
                    DrawTexturePro(Cross, CrossSrc, {751, 345, CrossSrc.width * 2.0f, CrossSrc.height * 2.0f}, {0,0}, 0, WHITE);
                }
                DrawTextEx(NES, "RETURN", {500, menu_start + (option_offset * 3.0f)}, 35, 2, WHITE);
                DrawRectangleV({100, 250}, {120, 140}, GRAY);
                if (!fst_player && parpadeos[0].Trigger(deltaTime)) {
                    DrawTextEx(NES, "PRESS", {126, 282}, 20, 2, WHITE);
                    DrawTexturePro(Spacekey, SpacekeySrc, {118, 312, SpacekeySrc.width * 1.75f, SpacekeySrc.height * 1.75f}, {0,0}, 0, WHITE);
                }
                DrawTextEx(NES, "1P", {140, 400}, 35, 2, fst_player ? WHITE : GRAY);
                DrawRectangleV({250, 250}, {120, 140}, GRAY);
                if (!snd_player && parpadeos[1].Trigger(deltaTime)) {
                    DrawTextEx(NES, "PRESS", {276, 282}, 20, 2, WHITE);
                    DrawTexturePro(Spacekey, SpacekeySrc, {268, 312, SpacekeySrc.width * 1.75f, SpacekeySrc.height * 1.75f}, {0,0}, 0, WHITE);
                }
                DrawTextEx(NES, "2P", {290, 400}, 35, 2, snd_player ? WHITE : GRAY);
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (current_option) {
                    case 0:
                        if (fst_player && !snd_player) {
                            //StopMusicStream(ts_music);
                            Game(1);
                            //PlayMusicStream(ts_music);
                        } else if(snd_player) {
                            Game(2);
                        }
                        break;
                    case 1:
                        speed_run = !speed_run;
                        break;
                    case 2:
                        CURRENT_MENU   = NEW_GAME;
                        OPTIONS = 4;
                        option_offset  = menu_height/(OPTIONS+1);
                        current_option = 0;
                    }
                } else if (IsKeyPressed(KEY_SPACE)) {
                    if (!fst_player) {
                        fst_player = true;
                    } else if (!snd_player) {
                        snd_player = true;
                    }
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    CURRENT_MENU   = NEW_GAME;
                    OPTIONS = 4;
                    option_offset  = menu_height/(OPTIONS+1);
                    current_option = 0;
                }
                break;
            case SETTINGS:
                DrawTextEx(NES, "SETTINGS", {500, (float)menu_start}, 35, 5, BLUE);
                DrawTextEx(NES, "VIDEO",    {500, menu_start + (float)option_offset}, 35, 2, WHITE);
                DrawTextEx(NES, "AUDIO",    {500, menu_start + (option_offset * 2.0f)}, 35, 2, GRAY);
                DrawTextEx(NES, "CONTROLS", {500, menu_start + (option_offset * 3.0f)}, 35, 2, WHITE);
                DrawTextEx(NES, "RETURN",   {500, menu_start + (option_offset * 4.0f)}, 35, 2, WHITE);
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (current_option) {
                    case 0:
                        CURRENT_MENU   = VIDEO_SETTINGS;
                        OPTIONS        = 6;
                        current_option = 0;
                        option_offset  = menu_height/(OPTIONS+1);
                        option_drift   = 3;
                        break;
                    case 1:
                        std::cout << "Hola\n";
                        break;
                    case 2:
                        CURRENT_MENU   = CONTROL_SETTINGS;
                        OPTIONS = 4;
                        current_option = 0;
                        option_offset  = menu_height/(OPTIONS+1);
                        option_drift   = 3;
                        break;
                    case 3:
                        CURRENT_MENU   = MAIN_MENU;
                        current_option = 2;
                        break;
                    }
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    CURRENT_MENU   = MAIN_MENU;
                    current_option = 2;
                }
                break;
            case VIDEO_SETTINGS:
                // MENU TITLE:
                DrawTextEx(NES, "VIDEO SETTINGS", {500, (float)menu_start}, 35, 5, BLUE);

                // OLD STYLE OPTION:
                DrawTextEx(NES, "OLD STYLE?", {500, menu_start + (float)option_offset}, 30, 1, WHITE);
                DrawRectangleV({805, menu_start + (float)option_offset + 6}, {20, 20}, WHITE);
                DrawRectangleV({806, menu_start + (float)option_offset + 8}, {15, 15}, BLACK);
                if (std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                    DrawTexturePro(Cross, CrossSrc, {800, menu_start + (float)option_offset + 3, CrossSrc.width * 2.0f, CrossSrc.height * 2.0f}, {0,0}, 0, WHITE);
                }

                // DISPLAY MODE OPTION: 0, 1, 2. Look up for the options explanation.
                DISPLAY_MODE_OPTION = std::get<int>(ini["Graphics"]["DisplayMode"]);
                DrawTextEx(NES, "DISPLAY MODE", {500, menu_start + (option_offset * 2.0f)}, 30, 1, WHITE);
                DrawTexturePro(Arrow, ArrowSrc, {750, menu_start + (option_offset * 2.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTexturePro(Arrow, ArrowSrcInv, {870, menu_start + (option_offset * 2.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTextEx(NES, to_string((DISPLAY_MODE_ENUM)DISPLAY_MODE_OPTION).c_str(),
                    {770, menu_start + (option_offset * 2.0f)}, 17, 1, WHITE);

                // RESOLUTION. 640x480, 800x600, 900x600, 1024x768, 1280x720, 1920x1080:
                DrawTextEx(NES, "RESOLUTION", {500, menu_start + (option_offset * 3.0f)}, 30, 1, WHITE);
                DrawTexturePro(Arrow, ArrowSrc, {750, menu_start + (option_offset * 3.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTexturePro(Arrow, ArrowSrcInv, {870, menu_start + (option_offset * 3.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTextEx(NES, (std::to_string(std::get<int>(ini["Graphics"]["ScreenWidth"])) + "x" + std::to_string(std::get<int>(ini["Graphics"]["ScreenHeight"]))).c_str(),
                    {770, menu_start + (option_offset * 3.0f) + 5}, 17, 1, WHITE);

                // VSYNC:
                DrawTextEx(NES, "VSYNC", {500, menu_start + (option_offset * 4.0f)}, 30, 1, GRAY);
                DrawRectangleV({805, menu_start + (option_offset * 4.0f) + 6}, {20, 20}, WHITE);
                DrawRectangleV({806, menu_start + (option_offset * 4.0f) + 8}, {15, 15}, BLACK);
                if (std::get<bool>(ini["Graphics"]["Vsync"])) {
                    DrawTexturePro(Cross, CrossSrc, {800, menu_start + (option_offset * 4.0f) + 3, CrossSrc.width * 2.0f, CrossSrc.height * 2.0f}, {0,0}, 0, WHITE);
                }

                // FPS LIMIT
                DrawTextEx(NES, "FPS LIMIT", {500, menu_start + (option_offset * 5.0f)}, 30, 1, WHITE);
                DrawTextEx(NES, std::to_string(std::get<int>(ini["Graphics"]["FPSLimit"])).c_str(), {803, menu_start + (option_offset * 5.0f) + 3}, 20, 1, WHITE);
                DrawTexturePro(Arrow, ArrowSrc, {760, menu_start + (option_offset * 5.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTexturePro(Arrow, ArrowSrcInv, {860, menu_start + (option_offset * 5.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);

                DrawTextEx(NES, "RETURN", {500, menu_start + (option_offset * 6.0f)}, 30, 1, WHITE);
                switch (current_option) {
                case 0:
                    if (IsKeyPressed(KEY_ENTER)) {
                        ini["Graphics"]["OldFashioned"] = !std::get<bool>(ini["Graphics"]["OldFashioned"]);
                    }
                    break;
                case 1:
                    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                        DISPLAY_MODE_OPTION = (DISPLAY_MODE_OPTION+1)%3;
                        ini["Graphics"]["DisplayMode"] = DISPLAY_MODE_OPTION;
                        if (DISPLAY_MODE_OPTION == WINDOWED) {
                            if (IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                            SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                        } else if (DISPLAY_MODE_OPTION == WINDOWED_FULLSCREEN) {
                            if (IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                            SetWindowPosition(0, 30);
                            SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                        } else if (DISPLAY_MODE_OPTION == FULLSCREEN) {
                            if (!IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                        }
                    }
                    if (IsKeyPressed(KEY_LEFT)) {
                        DISPLAY_MODE_OPTION = (DISPLAY_MODE_OPTION+2)%3;
                        ini["Graphics"]["DisplayMode"] = DISPLAY_MODE_OPTION;
                        if (DISPLAY_MODE_OPTION == WINDOWED) {
                            if (IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                            SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                        } else if (DISPLAY_MODE_OPTION == WINDOWED_FULLSCREEN) {
                            if (IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                            SetWindowPosition(0, 30);
                            SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                        } else if (DISPLAY_MODE_OPTION == FULLSCREEN) {
                            if (!IsWindowFullscreen()) {
                                ToggleFullscreen();
                            }
                        }
                    }
                    break;
                case 2:
                    if (DISPLAY_MODE_OPTION != WINDOWED_FULLSCREEN) {
                        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                            RESOLUTION_OPTION = (RESOLUTION_OPTION+1)%RESOLUTION_OPTIONS.size();
                            ini["Graphics"]["ScreenWidth"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].first;
                            ini["Graphics"]["ScreenHeight"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].second;
                            SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                        }
                        if (IsKeyPressed(KEY_LEFT)) {
                            RESOLUTION_OPTION = (RESOLUTION_OPTION+RESOLUTION_OPTIONS.size()-1)%RESOLUTION_OPTIONS.size();
                            ini["Graphics"]["ScreenWidth"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].first;
                            ini["Graphics"]["ScreenHeight"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].second;
                            SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                        }
                    }
                    break;
                case 3:
                    if (IsKeyPressed(KEY_ENTER)) {
                        ini["Graphics"]["Vsync"] = !std::get<bool>(ini["Graphics"]["Vsync"]);
                    }
                    break;
                case 4:
                    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                        FPS_LIMIT_OPTION = ((FPS_LIMIT_OPTION+1)%FPS_LIMIT_OPTIONS.size());
                        ini["Graphics"]["FPSLimit"] = FPS_LIMIT_OPTIONS[FPS_LIMIT_OPTION];
                        SetTargetFPS(std::get<int>(ini["Graphics"]["FPSLimit"]));
                    }
                    if (IsKeyPressed(KEY_LEFT)) {
                        FPS_LIMIT_OPTION = ((FPS_LIMIT_OPTION + FPS_LIMIT_OPTIONS.size() - 1) % FPS_LIMIT_OPTIONS.size());
                        ini["Graphics"]["FPSLimit"] = FPS_LIMIT_OPTIONS[FPS_LIMIT_OPTION];
                        SetTargetFPS(std::get<int>(ini["Graphics"]["FPSLimit"]));
                    }
                    break;
                case 5:
                    if (IsKeyPressed(KEY_ENTER)) {
                        CURRENT_MENU   = SETTINGS;
                        OPTIONS        = 4;
                        current_option = 0;
                        option_offset  = menu_height/(OPTIONS+1);
                        option_drift   = 0;
                    }
                    break;
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    CURRENT_MENU   = SETTINGS;
                    OPTIONS        = 4;
                    current_option = 0;
                    option_offset  = menu_height/(OPTIONS+1);
                    option_drift   = 0;
                }
                break;
            case AUDIO_SETTINGS:
                break;
            case CONTROL_SETTINGS:
                // MENU TITLE:
                DrawTextEx(NES, "CONTROL SETTINGS", {500, (float)menu_start}, 35, 5, BLUE);
                
                // CURRENT PLAYER: 0, 1, 2, 3
                static int currPlyr = 0;
                DrawTextEx(NES, "PLAYER:", {500, menu_start + (option_offset * 1.0f)}, 30, 1, WHITE);
                DrawTexturePro(Arrow, ArrowSrc, {750, menu_start + (option_offset * 1.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTexturePro(Arrow, ArrowSrcInv, {870, menu_start + (option_offset * 1.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTextEx(NES, std::to_string(currPlyr).c_str(), {770, menu_start + (option_offset * 1.0f) + 5}, 17, 1, WHITE);

                // CURRENT CONTROLLER: 0, 1, 2, 3
                static int currCont = Controller::KEYBOARD;
                std::string currContStr;
                switch (currCont) {
                    case Controller::CONTROLLER_0: currContStr = "JOY 0"; break;
                    case Controller::CONTROLLER_1: currContStr = "JOY 1"; break;
                    case Controller::CONTROLLER_2: currContStr = "JOY 2"; break;
                    case Controller::CONTROLLER_3: currContStr = "JOY 3"; break;
                    case Controller::KEYBOARD: currContStr = "KEYBOARD"; break;
                    default: currContStr = "NO JOY"; break;
                }
                DrawTextEx(NES, "CONTROLLER:", {500, menu_start + (option_offset * 2.0f)}, 30, 1, WHITE);
                DrawTexturePro(Arrow, ArrowSrc, {750, menu_start + (option_offset * 2.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTexturePro(Arrow, ArrowSrcInv, {870, menu_start + (option_offset * 2.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTextEx(NES, currContStr.c_str(), {770, menu_start + (option_offset * 2.0f) + 5}, 17, 1, WHITE);

                // CURRENT ACTION
                static int currAction = Controller::LEFT;
                std::string currActionStr;
                switch (currAction) {
                    case Controller::LEFT: currActionStr = "GO LEFT"; break;
                    case Controller::RIGHT: currActionStr = "GO RIGHT"; break;
                    case Controller::DOWN: currActionStr = "GO DOWN"; break;
                    case Controller::UP: currActionStr = "GO UP"; break;
                    case Controller::JUMP: currActionStr = "JUMP"; break;
                    case Controller::ATTACK: currActionStr = "ATTACK"; break;
                    default: currActionStr = "No Action"; break;
                }
                DrawTextEx(NES, "ACTION:", {500, menu_start + (option_offset * 3.0f)}, 30, 1, WHITE);
                DrawTexturePro(Arrow, ArrowSrc, {750, menu_start + (option_offset * 3.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTexturePro(Arrow, ArrowSrcInv, {870, menu_start + (option_offset * 3.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                DrawTextEx(NES, currActionStr.c_str(), {770, menu_start + (option_offset * 3.0f) + 5}, 17, 1, WHITE);
                
                // CURRENT ACTION BINDING
                static bool selected = false;
                DrawTextEx(NES, "KEYBINDING:", {500, menu_start + (option_offset * 4.0f)}, 30, 1, WHITE);
                if (selected) {
                    DrawTexturePro(Arrow, ArrowSrcInv, {750, menu_start + (option_offset * 4.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                    DrawTexturePro(Arrow, ArrowSrc, {870, menu_start + (option_offset * 4.0f) + 2, (float)Arrow.width, (float)Arrow.height}, {0,0}, 0, WHITE);
                }
                std::string actionBinding = controllers[currPlyr]->getActionBind((Controller::Control)currAction);
                DrawTextEx(NES, actionBinding.c_str(), {770, menu_start + (option_offset * 4.0f) + 5}, 17, 1, WHITE);
                switch (current_option) {
                    case 0:
                        if (IsKeyPressed(KEY_LEFT)) currPlyr = mod(currPlyr-1, 4);
                        else if (IsKeyPressed(KEY_RIGHT)) currPlyr = mod(currPlyr+1, 4);
                        break;
                    case 1:
                        if (IsKeyPressed(KEY_LEFT)) { currCont = mod(currCont-1, 5); controllers[currPlyr]->type = (Controller::Type)currCont; }
                        else if (IsKeyPressed(KEY_RIGHT)) { currCont = mod(currCont+1, 5); controllers[currPlyr]->type = (Controller::Type)currCont; }
                        break;
                    case 2:
                        if (IsKeyPressed(KEY_LEFT)) currAction = mod(currAction-1, 6);
                        else if (IsKeyPressed(KEY_RIGHT)) currAction = mod(currAction+1, 6);
                        break;
                    case 3:
                        if (!selected && IsKeyPressed(KEY_ENTER)) {
                            selected = true;
                        } else if (selected) {
                            bool keyboard = (controllers[currPlyr]->type == Controller::Type::KEYBOARD);
                            int binding = 0;
                            int axisOffset = 0;
                            //Keyboard
                            if (keyboard) binding = GetKeyPressed();
                            //Gamepad Trigger
                            else if (GetGamepadAxisMovement(currCont, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.5)
                            { binding = GAMEPAD_AXIS_LEFT_TRIGGER; axisOffset = (currAction < 4)? ((currAction%2 == 0)? 1 : -1) : 0; }
                            else if (GetGamepadAxisMovement(currCont, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.5)
                            { binding = GAMEPAD_AXIS_RIGHT_TRIGGER; axisOffset = (currAction < 4)? ((currAction%2 == 0)? 1 : -1) : 0; }
                            //Gamepad Axis
                            else if (GetGamepadAxisMovement(currCont, GAMEPAD_AXIS_LEFT_X) > 0.5 || GetGamepadAxisMovement(currCont, GAMEPAD_AXIS_LEFT_X) < -0.5)
                            { binding = GAMEPAD_AXIS_LEFT_X; axisOffset = (currAction < 4)? ((currAction%2 == 0)? 1 : -1) : 0; }
                            else if (GetGamepadAxisMovement(currCont, GAMEPAD_AXIS_LEFT_Y) > 0.5 || GetGamepadAxisMovement(currCont, GAMEPAD_AXIS_LEFT_Y) < -0.5)
                            { binding = GAMEPAD_AXIS_LEFT_Y; axisOffset = (currAction < 4)? ((currAction%2 == 0)? 1 : -1) : 0; }
                            else if (GetGamepadAxisMovement(currCont, GAMEPAD_AXIS_RIGHT_X) > 0.5 || GetGamepadAxisMovement(currCont, GAMEPAD_AXIS_RIGHT_X) < -0.5)
                            { binding = GAMEPAD_AXIS_RIGHT_X; axisOffset = (currAction < 4)? ((currAction%2 == 0)? 1 : -1) : 0; }
                            else if (GetGamepadAxisMovement(currCont, GAMEPAD_AXIS_RIGHT_Y) > 0.5 || GetGamepadAxisMovement(currCont, GAMEPAD_AXIS_RIGHT_Y) < -0.5)
                            { binding = GAMEPAD_AXIS_RIGHT_Y; axisOffset = (currAction < 4)? ((currAction%2 == 0)? 1 : -1) : 0; }
                            //Gamepad Button
                            else binding = GetGamepadButtonPressed();

                            if (binding != 0) {
                                controllers[currPlyr]->controls[(Controller::Control)currAction] = binding;
                                if (axisOffset != 0) controllers[currPlyr]->controls[(Controller::Control)(currAction + axisOffset)] = binding;
                                save_controls(currPlyr);
                                selected = false;
                            }
                        }
                        break;
                    default:
                        break;
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    CURRENT_MENU   = SETTINGS;
                    OPTIONS        = 4;
                    current_option = 0;
                    option_offset  = menu_height/(OPTIONS+1);
                    option_drift   = 0;
                }
                break;

            }

            if (IsKeyPressed(KEY_DOWN)) {
                current_option = mod(current_option+1, OPTIONS);
            }
            if (IsKeyPressed(KEY_UP)) {
                current_option = mod(current_option-1, OPTIONS);
            }

            if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                OptionHammer.Play({420, (float)menu_start + (option_offset * (current_option+1) - option_drift)});
            } else {
                DrawTexturePro(OldOptionHammer, OldOptionHammerSrc,
                    {450, (float)menu_start + (option_offset * (current_option+1) - option_drift), OldOptionHammer.width * 4.0f, OldOptionHammer.height * 4.0f}, {0,0}, 0, WHITE);
            }
            DrawText("Elements in gray are not available yet.", 20, 20, 25, WHITE);
        }
        EndDrawing();
    }
    OptionHammer.Unload();
    UnloadTexture(NintendoLogo);
    UnloadTexture(TeamLogo);
    UnloadTexture(Sign);
    UnloadTexture(OldSign);
    UnloadTexture(Copy);
    UnloadTexture(Arrow);
    UnloadTexture(Mountain);
    UnloadTexture(OldOptionHammer);
    UnloadTexture(Fields);
    UnloadTexture(ForePines);
    UnloadTexture(MidPines);
    UnloadTexture(Transparent);
    UnloadTexture(Cross);
    UnloadTexture(Snow);
    UnloadTexture(Spacekey);
    UnloadTexture(Enterkey);
    
    UnloadFont(NES);
    //UnloadMusicStream(ts_music);
    CloseAudioDevice();
    save_config();
}