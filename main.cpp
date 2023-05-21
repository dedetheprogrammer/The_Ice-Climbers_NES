#include "EngineECS.h"
#include "EngineUI.h"
#include "settings.h"
#include "raylibx.h"

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

void Game(int numPlayers, int level, bool speed_run) {

    SetExitKey(KEY_NULL);
    Texture2D mountain_sprite = LoadTexture("Assets/Sprites/00_Mountain.png");
    Texture2D arena_sprite = LoadTexture("Assets/Sprites/02_Brawl.png");
    float GAME_WIDTH = WINDOW_WIDTH;
    float GAME_HEIGHT = WINDOW_HEIGHT;
    if(DISPLAY_MODE_OPTION == FULLSCREEN) {
        GAME_WIDTH = GetScreenWidth();
        GAME_HEIGHT = GetScreenHeight();
    }
    
    float vertical_scale = 0.0f, horizontal_scale = 0.0f;
    if(level < 2) {
        horizontal_scale = GAME_WIDTH / (float)mountain_sprite.width;
        vertical_scale = GAME_WIDTH * 2.3f / (float)mountain_sprite.height;
    } else {
        horizontal_scale = GAME_WIDTH / (float)arena_sprite.width;
        vertical_scale = GAME_WIDTH * 0.7f / (float)arena_sprite.height;
    }
    Vector2 scale = {horizontal_scale, vertical_scale};

    int level_phase = 0;
    bool acabar = false;

    //MusicSource BGM("Assets/NES - Ice Climber - Sound Effects/Go Go Go - Nightcore.mp3", true);
    MusicSource BGM("Assets/Sounds/Mick Gordon - The Only Thing They Fear Is You.mp3", true);
    MusicSource BGM2("Assets/Sounds/03-Play-BGM.mp3",true);

    auto BackGrounds = std::vector<Canvas>{
        Canvas("Assets/Sprites/00_Mountain.png", {0.0f, GAME_HEIGHT - GAME_WIDTH*2.3f}, {GAME_WIDTH, GAME_WIDTH*2.3f}),
        Canvas("Assets/Sprites/01_Mountain.png", {0.0f, GAME_HEIGHT - GAME_WIDTH*2.3f}, {GAME_WIDTH, GAME_WIDTH*2.3f}),
        Canvas("Assets/Sprites/02_Brawl.png", {0.0f, GAME_HEIGHT - GAME_WIDTH}, {GAME_WIDTH, GAME_HEIGHT})
    };

    Canvas LifePopo1("Assets/Sprites/Popo/Life.png", {8.0f * horizontal_scale, 16.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifePopo2("Assets/Sprites/Popo/Life.png", {16.0f * horizontal_scale, 16.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifePopo3("Assets/Sprites/Popo/Life.png", {24.0f * horizontal_scale, 16.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeNana1("Assets/Sprites/Nana/Life.png", {8.0f * horizontal_scale, 25.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeNana2("Assets/Sprites/Nana/Life.png", {16.0f * horizontal_scale, 25.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeNana3("Assets/Sprites/Nana/Life.png", {24.0f * horizontal_scale, 25.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeAmam1("Assets/Sprites/Amam/Life.png", {8.0f * horizontal_scale, 34.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeAmam2("Assets/Sprites/Amam/Life.png", {16.0f * horizontal_scale, 34.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeAmam3("Assets/Sprites/Amam/Life.png", {24.0f * horizontal_scale, 34.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeLili1("Assets/Sprites/Lili/Life.png", {8.0f * horizontal_scale, 43.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeLili2("Assets/Sprites/Lili/Life.png", {16.0f * horizontal_scale, 43.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeLili3("Assets/Sprites/Lili/Life.png", {24.0f * horizontal_scale, 43.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});

    // Rectangles = Sprites component?
    // Mountain background:
    //Rectangle Mountain_src{0, Mountain_sprite.height - Mountain_view_height - 10, (float)Mountain_sprite.width, Mountain_view_height};
    //Rectangle Mountain_dst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    // PAUSE frame:
    Texture2D Pause_frame = LoadTexture("Assets/Sprites/Small_frame.png");
    float paused_showtime = 0.75;
    bool show = true;
    Rectangle src_0{0, 0, (float)Pause_frame.width, (float)Pause_frame.height};
    Rectangle dst_1{(GAME_WIDTH - Pause_frame.width*3.0f)/2.0f + 4, (GAME_HEIGHT - Pause_frame.height)/2.0f - 3, Pause_frame.width*3.0f, Pause_frame.height*3.0f};

    // Suelo base
    GameObject BaseFloor("Base Floor", "Floor");
    BaseFloor.addComponent<Collider2D>(&BaseFloor.getComponent<Transform2D>().position, Vector2{GAME_WIDTH * 1.5f, 8.0f*vertical_scale*2.0f}, PINK);

    // Bloque de hierba
    GameObject GrassBlock("Grass Block", "Floor", {"Block"});
    GameObject GrassBlockThin("Thin Grass Block", "Floor", {"Block"});
    GameObject GrassHole("Grass Hole", "Hole");

    GrassBlock.addComponent<Sprite>("Assets/Sprites/Blocks/Grass_block_large.png", horizontal_scale, vertical_scale);
    GrassBlock.addComponent<Collider2D>(&GrassBlock.getComponent<Transform2D>().position, GrassBlock.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    GrassBlock.addComponent<Script, BlockBehavior>();
    GrassBlock.getComponent<Script, BlockBehavior>().hole = &GrassHole;
    float block_width = GrassBlock.getComponent<Sprite>().GetViewDimensions().x;
    float block_height = GrassBlock.getComponent<Sprite>().GetViewDimensions().y;
    float collider_width  = block_width-6.0f;
    float collider_offset = (collider_width)/2;

    GrassBlockThin.addComponent<Sprite>("Assets/Sprites/Blocks/Grass_block_thin.png", horizontal_scale, vertical_scale);
    GrassBlockThin.addComponent<Collider2D>(&GrassBlockThin.getComponent<Transform2D>().position, GrassBlockThin.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    GrassBlockThin.addComponent<Script, BlockBehavior>();
    GrassBlockThin.getComponent<Script, BlockBehavior>().hole = &GrassHole;

    GrassHole.addComponent<Collider2D>(&GrassHole.getComponent<Transform2D>().position, Vector2{block_width, block_height}, RED);
    GrassHole.addComponent<Script, HoleBehavior>();
    GrassHole.getComponent<Script, HoleBehavior>().original_block = &GrassBlock;

    // Bloque de tierra
    GameObject DirtBlock("Dirt Block", "Floor", {"Block"});
    GameObject DirtBlockThin("Thin Dirt Block", "Floor", {"Block"});
    GameObject DirtHole("Dirt Hole", "Hole");

    DirtBlock.addComponent<Sprite>("Assets/Sprites/Blocks/Dirt_block_large.png", horizontal_scale, vertical_scale);
    DirtBlock.addComponent<Collider2D>(&DirtBlock.getComponent<Transform2D>().position, DirtBlock.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    DirtBlock.addComponent<Script, BlockBehavior>();
    DirtBlock.getComponent<Script, BlockBehavior>().hole = &DirtHole;

    DirtBlockThin.addComponent<Sprite>("Assets/Sprites/Blocks/Dirt_block_thin.png", horizontal_scale, vertical_scale);
    DirtBlockThin.addComponent<Collider2D>(&DirtBlockThin.getComponent<Transform2D>().position, DirtBlockThin.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    DirtBlockThin.addComponent<Script, BlockBehavior>();
    DirtBlockThin.getComponent<Script, BlockBehavior>().hole = &DirtHole;

    DirtHole.addComponent<Collider2D>(&DirtHole.getComponent<Transform2D>().position, Vector2{block_width, block_height}, RED);
    DirtHole.addComponent<Script, HoleBehavior>();
    DirtHole.getComponent<Script, HoleBehavior>().original_block = &DirtBlock;

    // Bloque de hielo
    GameObject IceBlock("Ice Block", "Floor", {"Ice","Block"});
    GameObject IceBlockThin("Thin Ice Block", "Floor", {"Ice","Block"});
    GameObject IceSlidingBlock("Ice Sliding Block", "SlidingFloor", {"Ice","Block"});
    GameObject IceHole("Ice Hole", "Hole");
    GameObject IceSlidingHole("Ice Sliding Hole", "Hole");

    IceBlock.addComponent<Sprite>("Assets/Sprites/Blocks/Ice_block_large.png", horizontal_scale, vertical_scale);
    IceBlock.addComponent<Collider2D>(&IceBlock.getComponent<Transform2D>().position, IceBlock.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    IceBlock.addComponent<Script, BlockBehavior>();
    IceBlock.getComponent<Script, BlockBehavior>().hole = &IceHole;

    IceBlockThin.addComponent<Sprite>("Assets/Sprites/Blocks/Ice_block_thin.png", horizontal_scale, vertical_scale);
    IceBlockThin.addComponent<Collider2D>(&IceBlockThin.getComponent<Transform2D>().position, IceBlockThin.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    IceBlockThin.addComponent<Script, BlockBehavior>();
    IceBlockThin.getComponent<Script, BlockBehavior>().hole = &IceHole;

    IceSlidingBlock.addComponent<Sprite>("Assets/Sprites/Blocks/Ice_sliding_block.png", horizontal_scale, vertical_scale);
    IceSlidingBlock.addComponent<Collider2D>(&IceSlidingBlock.getComponent<Transform2D>().position, IceSlidingBlock  .getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    IceSlidingBlock.addComponent<RigidBody2D>(1, 0, Vector2{100,0}, Vector2{0, 0});
    IceSlidingBlock.addComponent<Script, SlidingBlockBehavior>();
    IceSlidingBlock.getComponent<Script, SlidingBlockBehavior>().hole = &IceSlidingHole;

    IceHole.addComponent<Collider2D>(&IceHole.getComponent<Transform2D>().position, Vector2{(float)block_width, (float)block_height}, RED);
    IceHole.addComponent<Script, HoleBehavior>();
    IceHole.getComponent<Script, HoleBehavior>().original_block = &IceBlock;

    IceSlidingHole.addComponent<Collider2D>(&IceSlidingHole.getComponent<Transform2D>().position, Vector2{(float)block_width, (float)block_height}, RED);
    IceSlidingHole.addComponent<Script, HoleBehavior>();
    IceSlidingHole.getComponent<Script, HoleBehavior>().original_block = &IceSlidingBlock;

    // Muros fino
    GameObject Wall("Wall", "Wall");
    Wall.addComponent<Collider2D>(&Wall.getComponent<Transform2D>().position, Vector2{block_width, block_height * 6.0f}, YELLOW);

    // Muros laterales
    GameObject GrassWall("Wall", "Wall");
    GrassWall.addComponent<Collider2D>(&GrassWall.getComponent<Transform2D>().position, Vector2{block_width * 4.0f, block_height * 5.0f}, YELLOW);
    GameObject DirtWall("Wall", "Wall");
    DirtWall.addComponent<Collider2D>(&DirtWall.getComponent<Transform2D>().position, Vector2{block_width * 5.0f, block_height * 5.0f}, YELLOW);
    GameObject IceWall("Wall", "Wall");
    IceWall.addComponent<Collider2D>(&IceWall.getComponent<Transform2D>().position, Vector2{block_width * 6.0f, block_height * 5.0f}, YELLOW);

    // Suelos y muros de la zona bonus
    GameObject LevelFloor_0("Level Floor 0", "Floor");
    GameObject LevelFloor_0_Ice("Level Floor 0 Ice", "Floor");
    GameObject LevelFloor_1("Level Floor 1", "Floor");
    GameObject LevelFloor_2("Level Floor 2", "Floor");
    GameObject LevelFloor_3("Level Floor 3", "Floor");
    GameObject LevelFloor_4("Level Floor 4", "Floor");
    GameObject BonusLevelFloor_0("Bonus 0", "Floor");
    GameObject BonusLevelFloor_1("Bonus 1", "Floor");
    GameObject LevelWall_0("Level Wall 0", "Wall");
    GameObject LevelWall_1("Level Wall 1", "Wall");
    GameObject LevelWall_2("Level Wall 2", "Wall");

    float LevelFloor_0_width = block_width * 9.0f, LevelFloor_1_width = block_width * 6.0f, LevelFloor_2_width = block_width * 4.0f, LevelFloor_3_width = block_width * 3.0f,
        LevelFloor_4_width = block_width * 7.0f;
    LevelFloor_0.addComponent<Collider2D>(&LevelFloor_0.getComponent<Transform2D>().position, Vector2{LevelFloor_0_width, block_height}, PINK);
    LevelFloor_0_Ice.addComponent<Collider2D>(&LevelFloor_0_Ice.getComponent<Transform2D>().position, Vector2{LevelFloor_0_width, block_height}, PINK);
    LevelFloor_1.addComponent<Collider2D>(&LevelFloor_1.getComponent<Transform2D>().position, Vector2{LevelFloor_1_width, block_height}, PINK);
    LevelFloor_2.addComponent<Collider2D>(&LevelFloor_2.getComponent<Transform2D>().position, Vector2{LevelFloor_2_width, block_height}, PINK);
    LevelFloor_3.addComponent<Collider2D>(&LevelFloor_3.getComponent<Transform2D>().position, Vector2{LevelFloor_3_width, block_height}, PINK);
    LevelFloor_4.addComponent<Collider2D>(&LevelFloor_4.getComponent<Transform2D>().position, Vector2{LevelFloor_4_width, block_height}, PINK);
    BonusLevelFloor_0.addComponent<Collider2D>(&BonusLevelFloor_0.getComponent<Transform2D>().position, Vector2{LevelFloor_2_width, block_height}, PINK);
    BonusLevelFloor_1.addComponent<Collider2D>(&BonusLevelFloor_1.getComponent<Transform2D>().position, Vector2{LevelFloor_1_width, block_height}, PINK);
    LevelWall_0.addComponent<Collider2D>(&LevelWall_0.getComponent<Transform2D>().position, Vector2{block_width * 3.0f, block_height * 6.0f}, YELLOW);
    LevelWall_1.addComponent<Collider2D>(&LevelWall_1.getComponent<Transform2D>().position, Vector2{block_width * 3.0f, block_height * 7.0f}, YELLOW);
    LevelWall_2.addComponent<Collider2D>(&LevelWall_2.getComponent<Transform2D>().position, Vector2{block_width * 3.0f, block_height * 3.0f}, YELLOW);

    // Zona de muerte en la cima
    GameObject Death("Death", "Death");
    Death.addComponent<Collider2D>(&Death.getComponent<Transform2D>().position, Vector2{block_width * 6.0f, block_height}, RED);

    // ¿Como construyo un GameObject para Popo?
    // 1. Creamos el GameObject. Recuerda:
    //  - El GameObject no tiene ningún componente nada más crearlo.
    //  - El GameObject solo puede tener un elemento de cada tipo. Si le vuelves
    //    a meter otro, perderá el primero.
    GameObject Popo("Popo", "Player", {}, {"Floor", "Wall", "Cloud", "Enemy", "Goal", "Hole", "Fruit", "SlidingFloor"});
    GameObject Nana("Nana", "Player", {}, {"Floor", "Wall", "Cloud", "Enemy", "Goal", "Hole", "Fruit", "SlidingFloor"});
    GameObject Amam("Amam", "Player", {}, {"Floor", "Wall", "Cloud", "Enemy", "Goal", "Hole", "Fruit", "SlidingFloor"});
    GameObject Lili("Lili", "Player", {}, {"Floor", "Wall", "Cloud", "Enemy", "Goal", "Hole", "Fruit", "SlidingFloor"});
    // 2.a Añadimos el componente Transform. Es muy importante este componente ya que es el que indica las propiedades
    //  del objeto, como posicion, tamaño o rotación. De momento solo usamos tamaño.
    Popo.addComponent<Transform2D>();
    Nana.addComponent<Transform2D>();
    Amam.addComponent<Transform2D>();
    Lili.addComponent<Transform2D>();
    // 2.b. Se podría haber ahorrado el addComponent<Transform2D> y crearlo en el GameObject directamente:
    // GameObject Popo(Vector2{600,500});
    // 3. Añadimos el componente de Animaciones. Como veis, hay que indicarle de que tipo es la lista {...},
    // si no, dará error.
    Popo.addComponent<Animator>("Idle", std::unordered_map<std::string, Animation> {
        {"Idle", Animation("Assets/Sprites/Popo/00_Idle.png", 16, 24, scale, 0.75, true)},
        {"Walk", Animation("Assets/Sprites/Popo/02_Walk.png", 16, 24, scale, 0.135, true)},
        {"Brake", Animation("Assets/Sprites/Popo/03_Brake.png", 16, 24, scale, 0.3, true)},
        {"Jump", Animation("Assets/Sprites/Popo/04_Jump.png", 20, 25, scale, 0.5, false)},
        {"Attack", Animation("Assets/Sprites/Popo/05_Attack.png", 21, 25, scale, 0.3, false)},
        {"Stunned", Animation("Assets/Sprites/Popo/06_Stunned.png", 16, 21, scale, 0.5, true)},
        {"Fall", Animation("Assets/Sprites/Popo/07_Fall.png", 21, 25, scale, 0.3, false)},
        {"Crouch", Animation("Assets/Sprites/Popo/08_Crouch.png", 16, 24, scale, 0.75, true)},
    });
    auto PopoBonusAnimator = std::unordered_map<std::string, Animation> {
        {"Idle", Animation("Assets/Sprites/Popo/00_Idle_Bonus.png", 16, 24, scale, 0.75, true)},
        {"Walk", Animation("Assets/Sprites/Popo/02_Walk_Bonus.png", 16, 24, scale, 0.135, true)},
        {"Brake", Animation("Assets/Sprites/Popo/03_Brake_Bonus.png", 16, 24, scale, 0.3, true)},
        {"Jump", Animation("Assets/Sprites/Popo/04_Jump_Bonus.png", 20, 25, scale, 0.5, false)},
        {"Attack", Animation("Assets/Sprites/Popo/05_Attack_Bonus.png", 21, 25, scale, 0.3, false)},
        {"Stunned", Animation("Assets/Sprites/Popo/06_Stunned.png", 16, 24, scale, 0.5, true)},
        {"Fall", Animation("Assets/Sprites/Popo/07_Fall_Bonus.png", 21, 25, scale, 0.3, false)},
        {"Crouch", Animation("Assets/Sprites/Popo/08_Crouch_Bonus.png", 16, 24, scale, 0.75, true)},
    };

    Nana.addComponent<Animator>("Idle", std::unordered_map<std::string, Animation> {
        {"Idle", Animation("Assets/Sprites/Nana/00_Idle.png", 16, 24, scale, 0.75, true)},
        {"Walk", Animation("Assets/Sprites/Nana/02_Walk.png", 16, 24, scale, 0.135, true)},
        {"Brake", Animation("Assets/Sprites/Nana/03_Brake.png", 16, 24, scale, 0.3, true)},
        {"Jump", Animation("Assets/Sprites/Nana/04_Jump.png", 20, 25, scale, 0.5, false)},
        {"Attack", Animation("Assets/Sprites/Nana/05_Attack.png", 21, 25, scale, 0.3, false)},
        {"Stunned", Animation("Assets/Sprites/Nana/06_Stunned.png", 16, 24, scale, 0.5, true)},
        {"Fall", Animation("Assets/Sprites/Nana/07_Fall.png", 21, 25, scale, 0.3, false)},
        {"Crouch", Animation("Assets/Sprites/Nana/08_Crouch.png", 16, 24, scale, 0.75, true)},
    });

    Amam.addComponent<Animator>("Idle", std::unordered_map<std::string, Animation> {
        {"Idle", Animation("Assets/Sprites/Amam/00_Idle.png", 16, 24, scale, 0.75, true)},
        {"Walk", Animation("Assets/Sprites/Amam/02_Walk.png", 16, 24, scale, 0.135, true)},
        {"Brake", Animation("Assets/Sprites/Amam/03_Brake.png", 16, 24, scale, 0.3, true)},
        {"Jump", Animation("Assets/Sprites/Amam/04_Jump.png", 20, 25, scale, 0.5, false)},
        {"Attack", Animation("Assets/Sprites/Amam/05_Attack.png", 21, 25, scale, 0.3, false)},
        {"Stunned", Animation("Assets/Sprites/Amam/06_Stunned.png", 16, 24, scale, 0.5, true)},
        {"Fall", Animation("Assets/Sprites/Amam/07_Fall.png", 21, 25, scale, 0.3, false)},
        {"Crouch", Animation("Assets/Sprites/Amam/08_Crouch.png", 16, 24, scale, 0.75, true)},
    });

    Lili.addComponent<Animator>("Idle", std::unordered_map<std::string, Animation> {
        {"Idle", Animation("Assets/Sprites/Lili/00_Idle.png", 16, 24, scale, 0.75, true)},
        {"Walk", Animation("Assets/Sprites/Lili/02_Walk.png", 16, 24, scale, 0.135, true)},
        {"Brake", Animation("Assets/Sprites/Lili/03_Brake.png", 16, 24, scale, 0.3, true)},
        {"Jump", Animation("Assets/Sprites/Lili/04_Jump.png", 20, 25, scale, 0.5, false)},
        {"Attack", Animation("Assets/Sprites/Lili/05_Attack.png", 21, 25, scale, 0.3, false)},
        {"Stunned", Animation("Assets/Sprites/Lili/06_Stunned.png", 16, 24, scale, 0.5, true)},
        {"Fall", Animation("Assets/Sprites/Lili/07_Fall.png", 21, 25, scale, 0.3, false)},
        {"Crouch", Animation("Assets/Sprites/Lili/08_Crouch.png", 16, 24, scale, 0.75, true)},
    });
    // 3. Añadimos el componente de Audio:
    Popo.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/09-Jump.wav"))},
    });
    Nana.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/09-Jump.wav"))},
    });
    Amam.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/09-Jump.wav"))},
    });
    Lili.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/09-Jump.wav"))},
    });
    // 4. Añadimos el Rigidbody: 13
    Popo.addComponent<RigidBody2D>(1, block_height * 21.2f, Vector2{block_height * 6.0f,0}, Vector2{block_height * 13.0f, block_height * 17.0f});
    Nana.addComponent<RigidBody2D>(1, block_height * 21.2f, Vector2{block_height * 6.0f,0}, Vector2{block_height * 13.0f, block_height * 17.0f});
    Amam.addComponent<RigidBody2D>(1, block_height * 21.2f, Vector2{block_height * 6.0f,0}, Vector2{block_height * 13.0f, block_height * 17.0f});
    Lili.addComponent<RigidBody2D>(1, block_height * 21.2f, Vector2{block_height * 6.0f,0}, Vector2{block_height * 13.0f, block_height * 17.0f});
    // 5. Añadimos el Collider. Este es el componente más jodido, necesitas:
    //  - El Transform2D que tiene la posición del objeto.
    //  - El Animator que tiene el tamaño del sprite según en que animación esté, en este
    //    caso, es la animación inicial.

    Vector2 player_size = Popo.getComponent<Animator>().GetViewDimensions();
    Popo.addComponent<Collider2D>(&Popo.getComponent<Transform2D>().position, Vector2{collider_width, player_size.y}, Vector2{player_size.x/2 - collider_offset, 0});
    Nana.addComponent<Collider2D>(&Nana.getComponent<Transform2D>().position, Vector2{collider_width, player_size.y}, Vector2{player_size.x/2 - collider_offset, 0});
    Amam.addComponent<Collider2D>(&Amam.getComponent<Transform2D>().position, Vector2{collider_width, player_size.y}, Vector2{player_size.x/2 - collider_offset, 0});
    Lili.addComponent<Collider2D>(&Lili.getComponent<Transform2D>().position, Vector2{collider_width, player_size.y}, Vector2{player_size.x/2 - collider_offset, 0});
    Popo.addComponent<Script, PopoBehavior>(Controller_0);
    Nana.addComponent<Script, PopoBehavior>(Controller_1);
    Amam.addComponent<Script, PopoBehavior>(Controller_2);
    Lili.addComponent<Script, PopoBehavior>(Controller_3);

    GameObject* Player_1 = nullptr;
    GameObject* Player_2 = nullptr;
    GameObject* Player_3 = nullptr;
    GameObject* Player_4 = nullptr;

    GameObject* JosephInstance = nullptr;

    // Nubes
    GameObject Cloud("Cloud", "Cloud");
    Cloud.addComponent<Sprite>("Assets/Sprites/Cloud_Slow_Long.png", horizontal_scale, vertical_scale);
    Cloud.addComponent<Collider2D>(&Cloud.getComponent<Transform2D>().position, Cloud.getComponent<Sprite>().GetViewDimensions());
    Cloud.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{block_width * 4.0f, 0});
    Cloud.addComponent<Script, CloudBehavior>();

    GameObject SmallCloud("Small Cloud", "Cloud");
    SmallCloud.addComponent<Sprite>("Assets/Sprites/Cloud_Slow_Short.png", horizontal_scale, vertical_scale);
    SmallCloud.addComponent<Collider2D>(&SmallCloud.getComponent<Transform2D>().position, SmallCloud.getComponent<Sprite>().GetViewDimensions());
    SmallCloud.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{block_width * 4.0f, 0});
    SmallCloud.addComponent<Script, CloudBehavior>();

    // Condor
    GameObject Condor("Red Condor", "Goal");
    Condor.addComponent<Animator>("Fly", std::unordered_map<std::string, Animation> {
        {"Fly", Animation("Assets/Sprites/Red_condor_fly.png", 32, 16, scale, 0.75, true)}
    });
    Condor.addComponent<Collider2D>(&Condor.getComponent<Transform2D>().position, Condor.getComponent<Animator>().GetViewDimensions(), ORANGE);
    Condor.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{100, 0});
    Condor.addComponent<Script, RedCondorBehavior>();

    // Icicle
    GameObject Icicle("Icicle", "Icicle", {}, {"Hole", "Player", "Floor"});
    Icicle.addComponent<Sprite>("Assets/Sprites/Icicle.png", horizontal_scale, vertical_scale);
    Icicle.addComponent<Collider2D>(&Icicle.getComponent<Transform2D>().position, Icicle.getComponent<Sprite>().GetViewDimensions());
    Icicle.addComponent<RigidBody2D>(1, 98, Vector2{0,0}, Vector2{0,0});
    Icicle.addComponent<Script, IcicleBehavior>();

    // Frutas
    GameObject Eggplant("Fruit", "Fruit");
    Eggplant.addComponent<Sprite>("Assets/Sprites/Fruit_Eggplant.png", horizontal_scale, vertical_scale);
    Eggplant.addComponent<Collider2D>(&Eggplant.getComponent<Transform2D>().position, Eggplant.getComponent<Sprite>().GetViewDimensions(), ORANGE);
    Eggplant.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{100, 0});
    Eggplant.addComponent<Script, FruitBehavior>();
    float eggplant_height = Eggplant.getComponent<Sprite>().GetViewDimensions().y;

    GameObject Lettuce("Fruit", "Fruit");
    Lettuce.addComponent<Sprite>("Assets/Sprites/Fruit_Eggplant.png", horizontal_scale, vertical_scale);
    Lettuce.addComponent<Collider2D>(&Lettuce.getComponent<Transform2D>().position, Lettuce.getComponent<Sprite>().GetViewDimensions(), ORANGE);
    Lettuce.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{100, 0});
    Lettuce.addComponent<Script, FruitBehavior>();
    float lettuce_height = Lettuce.getComponent<Sprite>().GetViewDimensions().y;

    // Enemies
    GameObject Topi("Topi", "Enemy", {"Topi"}, {"Floor", "SlidingFloor", "Hole", "Player", "Icicle", "Wall"});
    Topi.addComponent<Animator>("Walk", std::unordered_map<std::string, Animation> {
            {"Walk", Animation("Assets/Sprites/Topi/01_Walk.png", 16, 16, scale, 0.3, true)},
            {"Stunned", Animation("Assets/Sprites/Topi/02_Stunned.png", 16, 16, scale, 0.5, true)},
        }
    );
    Topi.addComponent<RigidBody2D>(1, block_height * 23.0f, Vector2{0,0}, Vector2{block_width * 3.0f,0});
    Vector2 topi_size = Topi.getComponent<Animator>().GetViewDimensions();
    Topi.addComponent<Collider2D>(&Topi.getComponent<Transform2D>().position, Vector2{collider_width, topi_size.y}, Vector2{topi_size.x/2 - collider_offset, 0});
    Topi.addComponent<Script, TopiBehavior>(Icicle);

    GameObject Joseph("Joseph", "Enemy", {"Joseph"}, {"Floor", "SlidingFloor", "Hole", "Player", "Wall"});
    Joseph.addComponent<Animator>("Walk", std::unordered_map<std::string, Animation> {
            {"Walk", Animation("Assets/Sprites/Joseph/01_Walk.png", 16, 31, scale, 0.3, true)},
            {"Stunned", Animation("Assets/Sprites/Joseph/02_Stunned.png", 16, 31, scale, 0.5, true)},
            {"Falling", Animation("Assets/Sprites/Joseph/03_Basic.png", 16, 31, scale, 0.5, true)},
        }
    );
    Joseph.addComponent<RigidBody2D>(1, block_height * 23.0f, Vector2{0,0}, Vector2{block_width * 3.0f, 0});
    Vector2 joseph_size = Joseph.getComponent<Animator>().GetViewDimensions();
    Joseph.addComponent<Collider2D>(&Joseph.getComponent<Transform2D>().position, Vector2{collider_width, joseph_size.y}, Vector2{joseph_size.x/2 - collider_offset, 0});
    Joseph.addComponent<Script, JosephBehavior>();

    GameObject Nutpicker("Nutpicker", "Enemy", {}, {"Player"});
    Nutpicker.addComponent<Animator>("Walk", std::unordered_map<std::string, Animation> {
            {"Walk", Animation("Assets/Sprites/Nutpicker - Spritesheet 01 - Fly.png", 16, 16, scale, 0.3, true)},
            {"Stunned", Animation("Assets/Sprites/Nutpicker - Spritesheet 02 - Stunned.png", 16, 16, scale, 0.5, true)},
        }
    );
    Nutpicker.addComponent<RigidBody2D>(1, block_height * 23.0f, Vector2{0,0}, Vector2{block_width * 6.0f,0});
    Nutpicker.addComponent<Collider2D>(&Nutpicker.getComponent<Transform2D>().position, Vector2{collider_width, topi_size.y}, Vector2{topi_size.x/2 - collider_offset, 0});
    Nutpicker.addComponent<Script, NutpickerBehavior>(Icicle);

    std::vector<GameObject*> Enemies{};
    GameObject* bonusLevel = nullptr;

    // Exclusivos del modo arena
    GameObject ArenaFloor("Base Floor", "Floor");
    ArenaFloor.addComponent<Collider2D>(&ArenaFloor.getComponent<Transform2D>().position, Vector2{GAME_WIDTH * 1.5f, block_height*1.0f}, PINK);
    GameObject ArenaGrassWall("Wall", "Floor");
    ArenaGrassWall.addComponent<Collider2D>(&ArenaGrassWall.getComponent<Transform2D>().position, Vector2{block_width * 4.0f, block_height * 5.0f}, YELLOW);
    GameObject DirtWallThin("Wall", "Floor");
    DirtWallThin.addComponent<Collider2D>(&DirtWallThin.getComponent<Transform2D>().position, Vector2{block_width * 2.0f, block_height * 5.0f}, YELLOW);
    GameObject IceWallSuperThin("Wall", "Floor");
    IceWallSuperThin.addComponent<Collider2D>(&IceWallSuperThin.getComponent<Transform2D>().position, Vector2{block_width * 1.0f, block_height * 21.0f}, YELLOW);
    
    if(level < 2) {
        std::vector<float> floor_levels = {
            GAME_HEIGHT - 8.0f*vertical_scale*2.0f,  // 0
            GAME_HEIGHT - 8.0f*vertical_scale*8.0f,  // 1
            GAME_HEIGHT - 8.0f*vertical_scale*14.0f, // 2
            GAME_HEIGHT - 8.0f*vertical_scale*20.0f, // 3
            GAME_HEIGHT - 8.0f*vertical_scale*26.0f, // 4
            GAME_HEIGHT - 8.0f*vertical_scale*32.0f, // 5
            GAME_HEIGHT - 8.0f*vertical_scale*38.0f, // 6
            GAME_HEIGHT - 8.0f*vertical_scale*44.0f, // 7
            GAME_HEIGHT - 8.0f*vertical_scale*50.0f  // 8
        };

        std::vector<float> wall_levels = {
            GAME_HEIGHT - 8.0f*vertical_scale*56.0f, // 0
            GAME_HEIGHT - 8.0f*vertical_scale*63.0f, // 1
            GAME_HEIGHT - 8.0f*vertical_scale*70.0f, // 2
            GAME_HEIGHT - 8.0f*vertical_scale*77.0f, // 3
            GAME_HEIGHT - 8.0f*vertical_scale*84.0f, // 4
            GAME_HEIGHT - 8.0f*vertical_scale*91.0f, // 5
            GAME_HEIGHT - 8.0f*vertical_scale*94.0f, // 6
        };

        /***** Elementos comunes a las montañas *****/
        // Suelo base
        GameSystem::Instantiate(BaseFloor, GameObjectOptions{.position{-100,floor_levels[0]}});

        // Plataformas laterales
        GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 4.0f - LevelFloor_0_width, floor_levels[1]}});
        GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*4.0f, floor_levels[1]}});
        GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 5.0f - LevelFloor_0_width, floor_levels[2]}});
        GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*5.0f, floor_levels[2]}});
        GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 5.0f - LevelFloor_0_width, floor_levels[3]}});
        GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*5.0f, floor_levels[3]}});
        GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 5.0f - LevelFloor_0_width, floor_levels[4]}});
        GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*5.0f, floor_levels[4]}});
        GameSystem::Instantiate(LevelFloor_0_Ice, GameObjectOptions{.position{block_width * 6.0f - LevelFloor_0_width, floor_levels[5]}});
        GameSystem::Instantiate(LevelFloor_0_Ice, GameObjectOptions{.position{GAME_WIDTH - block_width*6.0f, floor_levels[5]}});
        GameSystem::Instantiate(LevelFloor_0_Ice, GameObjectOptions{.position{block_width * 6.0f - LevelFloor_0_width, floor_levels[6]}});
        GameSystem::Instantiate(LevelFloor_0_Ice, GameObjectOptions{.position{GAME_WIDTH - block_width*6.0f, floor_levels[6]}});
        GameSystem::Instantiate(LevelFloor_0_Ice, GameObjectOptions{.position{block_width * 6.0f - LevelFloor_0_width, floor_levels[7]}});
        GameSystem::Instantiate(LevelFloor_0_Ice, GameObjectOptions{.position{GAME_WIDTH - block_width*6.0f, floor_levels[7]}});
        GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 25.0f, floor_levels[8]}});
        GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 7.0f - LevelFloor_0_width, floor_levels[8]}});

        // Muros de la fase bonus
        GameSystem::Instantiate(LevelWall_0, GameObjectOptions{.position{0, wall_levels[0]}});
        GameSystem::Instantiate(LevelWall_0, GameObjectOptions{.position{block_width * 29.0f, wall_levels[0]}});
        GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{block_width, wall_levels[1]}});
        GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{block_width * 28.0f, wall_levels[1]}});
        GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{block_width * 2.0f, wall_levels[2]}});
        GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{block_width * 27.0f, wall_levels[2]}});
        GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{block_width * 3.0f, wall_levels[3]}});
        GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{block_width * 26.0f, wall_levels[3]}});
        GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{block_width * 4.0f, wall_levels[4]}});
        GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{block_width * 25.0f, wall_levels[4]}});
        GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{block_width * 5.0f, wall_levels[5]}});
        GameSystem::Instantiate(LevelWall_1, GameObjectOptions{.position{block_width * 24.0f, wall_levels[5]}});

        // Players
        auto playerTransforms = std::vector<Transform2D*>();
        Player_1 = &GameSystem::Instantiate(Popo, GameObjectOptions{.position = {block_width * 8.0f, floor_levels[0] - player_size.y}});
        playerTransforms.push_back(&Player_1->getComponent<Script, PopoBehavior>().transform);
        if (numPlayers > 1) {
            Player_2 = &GameSystem::Instantiate(Nana, GameObjectOptions{.position = {block_width * 13.0f, floor_levels[0] - player_size.y}});
            playerTransforms.push_back(&Player_2->getComponent<Script, PopoBehavior>().transform);
        }
        if (numPlayers > 2) {
            Player_3 = &GameSystem::Instantiate(Amam, GameObjectOptions{.position = {block_width * 18.0f,floor_levels[0] - player_size.y}});
            playerTransforms.push_back(&Player_3->getComponent<Script, PopoBehavior>().transform);
        }
        if (numPlayers > 3) {
            Player_4 = &GameSystem::Instantiate(Lili, GameObjectOptions{.position = {block_width * 23.0f,floor_levels[0] - player_size.y}});
            playerTransforms.push_back(&Player_4->getComponent<Script, PopoBehavior>().transform);
        }

        // Zona de muerte y Condor en la cima de la montaña
        GameSystem::Instantiate(Death, GameObjectOptions{.position{0, GAME_HEIGHT - block_height * 95.0f}});
        GameSystem::Instantiate(Death, GameObjectOptions{.position{block_width * 26.0f, GAME_HEIGHT - block_height * 95.0f}});
        GameSystem::Instantiate(Condor, GameObjectOptions{.position={400, GAME_HEIGHT - block_height * 104.0f}});

        if(level == 0) {

            std::vector<float> bonus_floor_levels = {
                GAME_HEIGHT - block_height * 55.0f, // 0
                GAME_HEIGHT - block_height * 58.0f, // 1
                GAME_HEIGHT - block_height * 60.0f, // 2  Cloud
                GAME_HEIGHT - block_height * 65.0f, // 3
                GAME_HEIGHT - block_height * 66.0f, // 4
                GAME_HEIGHT - block_height * 70.0f, // 5
                GAME_HEIGHT - block_height * 71.0f, // 6
                GAME_HEIGHT - block_height * 76.0f, // 7  Cloud
                GAME_HEIGHT - block_height * 81.0f, // 8
                GAME_HEIGHT - block_height * 82.0f, // 9
                GAME_HEIGHT - block_height * 85.0f, // 10
                GAME_HEIGHT - block_height * 87.0f, // 11
                GAME_HEIGHT - block_height * 90.0f, // 12
                GAME_HEIGHT - block_height * 95.0f  // 13
            };

            for (float i = 0.0f; i < 24; i++) {
                GameSystem::Instantiate(GrassBlock, GameObjectOptions{.position{block_width * (4.0f + i), floor_levels[1]}});
                if(i == 0 || i == 1 || i == 5 || i == 6 || (i > 11 && i < 21))
                    GameSystem::Instantiate(GrassBlock, GameObjectOptions{.position{block_width * (4.0f + i), floor_levels[1] + block_height}});
                if(i == 2 || i == 4 || i == 7 || i == 11 || i == 21)
                    GameSystem::Instantiate(GrassBlockThin, GameObjectOptions{.position{block_width * (4.0f + i), floor_levels[1] + block_height}});
            }

            for (int i = 0; i < 22; i++) {
                GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2]}});
                if(i < 6 || (i > 8 && i < 12) || i > 18)            
                    GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2] + block_height}});
                if(i == 6 || i == 8 || i == 12 || i == 18)            
                    GameSystem::Instantiate(DirtBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2] + block_height}});
                GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3]}});
                if(i == 0 || (i > 3 && i < 7) || (i > 9 && i < 13) || (i > 15 && i < 19))            
                    GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3] + block_height}});
                if(i == 1 || i == 3 || i == 7 || i == 9 || i == 13 || i == 15 || i == 19 )            
                    GameSystem::Instantiate(DirtBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3] + block_height}});
                GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[4]}});
                if(i < 12 || i == 15 || i > 18)            
                    GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[4] + block_height}});
                if(i == 12 || i == 14 || i == 16 || i == 18)            
                    GameSystem::Instantiate(DirtBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[4] + block_height}});
                
            }   

            for (int i = 0; i < 20; i++) {
                GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[5]}});
                if(i == 0 || (i > 2 && i < 9) || i > 15)            
                    GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[5] + block_height}});
                if(i == 1 || i == 2 || i == 9 || i == 15)            
                    GameSystem::Instantiate(IceBlockThin, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[5] + block_height}});
                GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[6]}});
                if(i < 5 || (i > 9 && i < 13) || (i > 15 && i < 19))            
                    GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[6] + block_height}});
                if(i == 5 || i == 9 || i == 13 || i == 15 || i == 19)            
                    GameSystem::Instantiate(IceBlockThin, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[6] + block_height}});
                GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[7]}});
                if((i > 2 && i < 5) || (i > 7 && i < 11) || (i > 13 && i < 16))            
                    GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[7] + block_height}});
                if(i == 2 || i == 5 || i == 7 || i == 11 || i == 13 || i == 16)            
                    GameSystem::Instantiate(IceBlockThin, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[7] + block_height}});
            } 

            GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 9.0f, floor_levels[8]}});
            GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 17.0f, floor_levels[8]}});

            bonusLevel = &GameSystem::Instantiate(BonusLevelFloor_0, GameObjectOptions{.position{block_width * 6.0f, bonus_floor_levels[0]}});
            GameSystem::Instantiate(BonusLevelFloor_1, GameObjectOptions{.position{block_width * 13.0f, bonus_floor_levels[0]}});
            GameSystem::Instantiate(BonusLevelFloor_0, GameObjectOptions{.position{block_width * 22.0f, bonus_floor_levels[0]}});

            GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 10.0f, bonus_floor_levels[2]}});

            GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 18.0f, bonus_floor_levels[3]}});
            GameSystem::Instantiate(LevelFloor_4, GameObjectOptions{.position{block_width * 5.0f, bonus_floor_levels[4]}});

            GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 21.0f, bonus_floor_levels[5]}});
            GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 12.0f, bonus_floor_levels[6]}});

            GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, bonus_floor_levels[7]}});

            GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 16.0f, bonus_floor_levels[8]}});
            GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 8.0f, bonus_floor_levels[9]}});

            GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 19.0f, bonus_floor_levels[10]}});
            GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 12.0f, bonus_floor_levels[11]}});
            GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 14.0f, bonus_floor_levels[12]}});

            GameSystem::Instantiate(LevelWall_2, GameObjectOptions{.position{block_width * 6.0f, wall_levels[6]}});
            GameSystem::Instantiate(LevelWall_2, GameObjectOptions{.position{block_width * 23.0f, wall_levels[6]}});
            GameSystem::Instantiate(LevelFloor_4, GameObjectOptions{.position{block_width * 6.0f, bonus_floor_levels[13]}});
            GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 20.0f, bonus_floor_levels[13]}});

            GameSystem::Instantiate(Eggplant, GameObjectOptions{.position={block_width * 6.0f, bonus_floor_levels[0] - eggplant_height}});
            GameSystem::Instantiate(Eggplant, GameObjectOptions{.position={block_width * 24.0f, bonus_floor_levels[0] - eggplant_height}});
            GameSystem::Instantiate(Eggplant, GameObjectOptions{.position={block_width * 20.0f, bonus_floor_levels[3] - eggplant_height}});
            GameSystem::Instantiate(Eggplant, GameObjectOptions{.position={block_width * 12.0f, bonus_floor_levels[6] - eggplant_height}});

            Enemies.push_back(&GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[0] - (topi_size.y + 1)}}));
            Enemies.push_back(&GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[2] - (topi_size.y + 1)}}));
            Enemies.push_back(&GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[4] - (topi_size.y + 1)}}));
            Enemies.push_back(&GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[6] - (topi_size.y + 1)}}));

            JosephInstance = &GameSystem::Instantiate(Joseph, GameObjectOptions{.position{-(joseph_size.x + block_width),floor_levels[4] - (joseph_size.y + 1)}});
            JosephInstance->getComponent<Script, JosephBehavior>().playerTransforms = playerTransforms;
            Enemies.push_back(JosephInstance);
            
            GameSystem::Instantiate(Nutpicker, GameObjectOptions{.position{100000,90000}});
        } else if(level == 1) {

            std::vector<float> bonus_floor_levels = {
                GAME_HEIGHT - block_height * 55.0f, // 0
                GAME_HEIGHT - block_height * 60.0f, // 1 Cloud
                GAME_HEIGHT - block_height * 65.0f, // 2
                GAME_HEIGHT - block_height * 66.0f, // 3
                GAME_HEIGHT - block_height * 70.0f, // 4
                GAME_HEIGHT - block_height * 71.0f, // 5
                GAME_HEIGHT - block_height * 76.0f, // 6 Cloud
                GAME_HEIGHT - block_height * 79.0f, // 7 Cloud
                GAME_HEIGHT - block_height * 85.0f, // 8
                GAME_HEIGHT - block_height * 90.0f, // 9
                GAME_HEIGHT - block_height * 95.0f  // 10
            };

            GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, floor_levels[1]}});

            GameSystem::Instantiate(GrassWall, GameObjectOptions{.position{0, floor_levels[2] + block_height}});
            GameSystem::Instantiate(GrassWall, GameObjectOptions{.position{GetScreenWidth() - block_width * 4.0f, floor_levels[2] + block_height}});
            for (int i = 0; i < 24; i++)
                if(i != 7)
                    GameSystem::Instantiate(GrassHole, GameObjectOptions{.position{block_width * (4.0f + i), floor_levels[1]}});
            for (int i = 0; i < 22; i++) {
                if(i < 5 || i > 12)
                    GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2]}});
                else if(i == 5 || i == 10) 
                    GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2]}});
                else if(i == 8 || i == 9)
                    GameSystem::Instantiate(DirtHole, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2]}});
                if((i > 2 && i < 7) || i == 11 || (i > 14 && i < 18))
                    GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2] + block_height}});
                if(i == 0 || i == 2 || i == 12 || i == 14 || i == 18)
                    GameSystem::Instantiate(DirtBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2] + block_height}});
                if(i != 6 && i != 11)
                    GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3]}});
                if(i < 4 || (i > 7 && i < 10) || (i > 14 && i < 18))
                    GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3] + block_height}});
                if(i == 5 || i == 10 || i == 14 || i == 18)
                    GameSystem::Instantiate(DirtBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3] + block_height}});
            }
            GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, floor_levels[4]}});

            GameSystem::Instantiate(DirtWall, GameObjectOptions{.position{0, floor_levels[5] + block_height}});
            GameSystem::Instantiate(DirtWall, GameObjectOptions{.position{GetScreenWidth() - block_width * 5.0f, floor_levels[5] + block_height}});

            for (int i = 0; i < 20; i++) {
                GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[5]}});
                if(i == 0 || (i > 9 && i < 17))
                    GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[5] + block_height}});
                if(i == 1 || i == 9 || i == 17)
                    GameSystem::Instantiate(IceBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[5] + block_height}});
                GameSystem::Instantiate(IceHole, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[6]}});
                if((i > 1 && i < 6 ) || (i > 9 && i < 14 ) || (i > 17))
                    GameSystem::Instantiate(IceSlidingBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[7]}});
                else
                    GameSystem::Instantiate(IceSlidingHole, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[7]}});          
                if((i > 2 && i < 5) || (i > 9 && i < 14 ))
                    GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[7] + block_height}});
                if(i == 2 || i == 5)
                    GameSystem::Instantiate(IceBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[7] + block_height}});
            }
            GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, floor_levels[6]}});

            GameSystem::Instantiate(IceWall, GameObjectOptions{.position{0, floor_levels[7] + block_height}});
            GameSystem::Instantiate(IceWall, GameObjectOptions{.position{GetScreenWidth() - block_width * 6.0f, floor_levels[7] + block_height}});

            GameSystem::Instantiate(Wall, GameObjectOptions{.position{block_width * 11.0f, floor_levels[1]}});
            GameSystem::Instantiate(Wall, GameObjectOptions{.position{block_width * 11.0f, floor_levels[3]}});
            GameSystem::Instantiate(Wall, GameObjectOptions{.position{block_width * 16.0f, floor_levels[3]}});

            GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 9.0f, floor_levels[8]}});
            GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 17.0f, floor_levels[8]}});

            // Fase bonus
            GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 6.0f, bonus_floor_levels[0]}});
            GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 13.0f, bonus_floor_levels[0]}});
            GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 22.0f, bonus_floor_levels[0]}});

            GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, bonus_floor_levels[1]}});

            GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 18.0f, bonus_floor_levels[2]}});
            GameSystem::Instantiate(LevelFloor_4, GameObjectOptions{.position{block_width * 5.0f, bonus_floor_levels[3]}});

            GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 21.0f, bonus_floor_levels[4]}});
            GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 12.0f, bonus_floor_levels[5]}});

            GameSystem::Instantiate(SmallCloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, bonus_floor_levels[6]}});
            GameSystem::Instantiate(SmallCloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, bonus_floor_levels[7]}});

            GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 10.0f, bonus_floor_levels[8]}});
            GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 17.0f, bonus_floor_levels[8]}});
            GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 14.0f, bonus_floor_levels[9]}});

            GameSystem::Instantiate(LevelWall_2, GameObjectOptions{.position{block_width * 6.0f, wall_levels[6]}});
            GameSystem::Instantiate(LevelWall_2, GameObjectOptions{.position{block_width * 23.0f, wall_levels[6]}});
            GameSystem::Instantiate(LevelFloor_4, GameObjectOptions{.position{block_width * 6.0f, bonus_floor_levels[10]}});
            GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 20.0f, bonus_floor_levels[10]}});

            GameSystem::Instantiate(Lettuce, GameObjectOptions{.position={block_width * 6.0f, bonus_floor_levels[0] - lettuce_height}});
            GameSystem::Instantiate(Lettuce, GameObjectOptions{.position={block_width * 24.0f, bonus_floor_levels[0] - lettuce_height}});
            GameSystem::Instantiate(Lettuce, GameObjectOptions{.position={block_width * 20.0f, bonus_floor_levels[2] - lettuce_height}});
            GameSystem::Instantiate(Lettuce, GameObjectOptions{.position={block_width * 12.0f, bonus_floor_levels[5] - lettuce_height}});

            Enemies.push_back(&GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[0] - (topi_size.y + 1)}}));
            Enemies.push_back(&GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[2] - (topi_size.y + 1)}}));
            Enemies.push_back(&GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[3] - (topi_size.y + 1)}}));
            Enemies.push_back(&GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[5] - (topi_size.y + 1)}}));
            Enemies.push_back(&GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[7] - (topi_size.y + 1)}}));
            GameSystem::Instantiate(Nutpicker, GameObjectOptions{.position{100000,90000}});
        }
    } else if (level == 2) {
        std::vector<float> levels = {
            GAME_HEIGHT - block_height,  // 0
            GAME_HEIGHT - block_height*6.0f,  // 1
            GAME_HEIGHT - block_height*11.0f, // 2
            GAME_HEIGHT - block_height*16.0f, // 3
            GAME_HEIGHT - block_height*32.0f, // 3
        };

        // Floor
        GameSystem::Instantiate(ArenaFloor, GameObjectOptions{.position{-(GAME_WIDTH/4.0f),levels[0]}});

        // Players
        Player_1 = &GameSystem::Instantiate(Popo, GameObjectOptions{.position = {block_width * 8.0f, levels[0] - player_size.y}});
        if (numPlayers > 1)
            Player_2 = &GameSystem::Instantiate(Nana, GameObjectOptions{.position = {block_width * 13.0f, levels[0] - player_size.y}});
        if (numPlayers > 2)
            Player_3 = &GameSystem::Instantiate(Amam, GameObjectOptions{.position = {block_width * 18.0f,levels[0] - player_size.y}});
        if (numPlayers > 3)
            Player_4 = &GameSystem::Instantiate(Lili, GameObjectOptions{.position = {block_width * 23.0f,levels[0] - player_size.y}});

        GameSystem::Instantiate(ArenaGrassWall, GameObjectOptions{.position{0, levels[1]}});
        GameSystem::Instantiate(ArenaGrassWall, GameObjectOptions{.position{GAME_WIDTH - 4.0f * block_width, levels[1]}});
        GameSystem::Instantiate(DirtWallThin, GameObjectOptions{.position{0, levels[2]}});
        GameSystem::Instantiate(DirtWallThin, GameObjectOptions{.position{GAME_WIDTH - 2.0f * block_width, levels[2]}});
        GameSystem::Instantiate(IceWallSuperThin, GameObjectOptions{.position{0, levels[4]}});
        GameSystem::Instantiate(IceWallSuperThin, GameObjectOptions{.position{GAME_WIDTH - block_width, levels[4]}});

        GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 9.0f, levels[1]}});
        GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 19.0f, levels[1]}});
        GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 4.0f, levels[2]}});
        GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 14.0f, levels[2]}});
        GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 24.0f, levels[2]}});
        GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 9.0f, levels[3]}});
        GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 19.0f, levels[3]}});
    }

    //GameSystem::Printout();
    float timeToShowScores = 0.0f;
    bool finished = false;
    bool play_music = false;
    bool paused = false;
    bool moving_camera = false;
    float objects_offset = 6.0f * block_height, current_objects_offset = 0;
    float old_offset = objects_offset;
    float chrono_time = 0.0f;
    float time_limit = 120.0f;
    bool onBonus = false;
    bool game_over = false;
    int fruits_harvested1 = 0, blocks_destroyed1 = 0;
    int fruits_harvested2 = 0, blocks_destroyed2 = 0;
    bool win1 = false, win2 = false;
    BGM.Init();
    BGM2.Init();

    UIText ContinueText(NES, "CONTINUE", 40, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f - 30}, UIObject::CENTER);
    UIText ExitText(NES, "EXIT", 40, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 30}, UIObject::CENTER);
    UISprite Hammer("Assets/Sprites/UI_Old_Hammer.png", {ContinueText.pos.x - 50, ContinueText.pos.y}, 4.0f, 4.0f, UIObject::UP_RIGHT);

    UISprite BigFrame1("Assets/Sprites/Record-frame1.png", {GetScreenWidth()/4.0f, GetScreenHeight()/2.0f}, {GetScreenWidth()/2.0f - 100, GetScreenHeight() - 40.0f}, UIObject::CENTER);
    UIText Player1Text(NES, "Player 1", 40, 1, {GetScreenWidth()/4.0f, 100}, UIObject::CENTER);
    UIText Player1Status(NES, "YOU LOOSE!", 40, 1, {GetScreenWidth()/4.0f, Player1Text.pos.y + 60}, UIObject::CENTER);
    UISprite Icicler1("Assets/Sprites/Ranking-icicle.png", {GetScreenWidth()/8.0f, Player1Text.pos.y + 150}, 4.0f, 4.0f, UIObject::CENTER);
    UISprite Monster1("Assets/Sprites/Ranking-nutpicker.png", {GetScreenWidth()/8.0f, Icicler1.dst.y + Icicler1.dst.height + 60}, 4.0f, 4.0f, UIObject::CENTER);
    UISprite Vegetabler1("Assets/Sprites/Fruit_Lettuce.png", {GetScreenWidth()/8.0f, Monster1.dst.y + Monster1.dst.height + 60}, 4.0f, 4.0f, UIObject::CENTER);
    UISprite Block1("Assets/Sprites/Ranking-block.png", {GetScreenWidth()/8.0f, Vegetabler1.dst.y + Vegetabler1.dst.height + 60}, 4.0f, 4.0f, UIObject::CENTER);
    UISprite SmallFrame1("Assets/Sprites/Record-frames1.png", {GetScreenWidth()/4.0f, Block1.dst.y + Block1.dst.height + 80}, 3.5f, 3.5f, UIObject::CENTER);
    UIText Icicler1Text(NES, "400x", 40, 1, {GetScreenWidth()/4.0f, Icicler1.dst.y + Icicler1.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Monster1Text(NES, "800x", 40, 1, {GetScreenWidth()/4.0f, Monster1.dst.y + Monster1.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Vegetabler1Text(NES, "300x", 40, 1, {GetScreenWidth()/4.0f, Vegetabler1.dst.y + Vegetabler1.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Block1Text(NES, "10x", 40, 1, {GetScreenWidth()/4.0f, Block1.dst.y + Block1.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Icicler1N(NES, "00", 40, 1, {Icicler1Text.pos.x + Icicler1Text.size.x + 10, Icicler1Text.pos.y});
    UIText Monster1N(NES, "00", 40, 1, {Monster1Text.pos.x + Monster1Text.size.x + 10, Monster1Text.pos.y});
    UIText Vegetabler1N(NES, "00", 40, 1, {Vegetabler1Text.pos.x + Vegetabler1Text.size.x + 10, Vegetabler1Text.pos.y});
    UIText Block1N(NES, "00", 40, 1, {Block1Text.pos.x + Block1Text.size.x + 10, Block1Text.pos.y});
    UIText Total1(NES, "000000", 40, 1, {GetScreenWidth()/4.0f, SmallFrame1.dst.y + 10}, UIObject::UP_CENTER);

    UISprite BigFrame2("Assets/Sprites/Record-frame2.png", {GetScreenWidth() - GetScreenWidth()/4.0f, GetScreenHeight()/2.0f}, {GetScreenWidth()/2.0f - 100, GetScreenHeight() - 40.0f}, UIObject::CENTER);
    UIText Player2Text(NES, "Player 2", 40, 1, {GetScreenWidth() - GetScreenWidth()/4.0f, 100}, UIObject::CENTER);
    UIText Player2Status(NES, "YOU LOOSE!", 40, 1, {GetScreenWidth() - GetScreenWidth()/4.0f, Player2Text.pos.y + 60}, UIObject::CENTER);
    UISprite Icicler2("Assets/Sprites/Ranking-icicle.png", {GetScreenWidth() - 3*GetScreenWidth()/8.0f, Player2Text.pos.y + 150}, 4.0f, 4.0f, UIObject::CENTER);
    UISprite Monster2("Assets/Sprites/Ranking-nutpicker.png", {GetScreenWidth() - 3*GetScreenWidth()/8.0f, Icicler2.dst.y + Icicler2.dst.height + 60}, 4.0f, 4.0f, UIObject::CENTER);
    UISprite Vegetabler2("Assets/Sprites/Fruit_Lettuce.png", {GetScreenWidth() - 3*GetScreenWidth()/8.0f, Monster2.dst.y + Monster2.dst.height + 60}, 4.0f, 4.0f, UIObject::CENTER);
    UISprite Block2("Assets/Sprites/Ranking-block.png", {GetScreenWidth() - 3*GetScreenWidth()/8.0f, Vegetabler2.dst.y + Vegetabler2.dst.height + 60}, 4.0f, 4.0f, UIObject::CENTER);
    UISprite SmallFrame2("Assets/Sprites/Record-frames2.png", {GetScreenWidth() - GetScreenWidth()/4.0f, Block2.dst.y + Block2.dst.height + 80}, 3.5f, 3.5f, UIObject::CENTER);
    UIText Icicler2Text(NES, "400x", 40, 1, {GetScreenWidth() - GetScreenWidth()/4.0f, Icicler2.dst.y + Icicler2.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Monster2Text(NES, "800x", 40, 1, {GetScreenWidth() - GetScreenWidth()/4.0f, Monster2.dst.y + Monster2.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Vegetabler2Text(NES, "300x", 40, 1, {GetScreenWidth() - GetScreenWidth()/4.0f, Vegetabler2.dst.y + Vegetabler2.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Block2Text(NES, "10x", 40, 1, {GetScreenWidth() - GetScreenWidth()/4.0f, Block2.dst.y + Block2.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Icicler2N(NES, "00", 40, 1, {Icicler2Text.pos.x + Icicler2Text.size.x + 10, Icicler2Text.pos.y});
    UIText Monster2N(NES, "00", 40, 1, {Monster2Text.pos.x + Monster2Text.size.x + 10, Monster2Text.pos.y});
    UIText Vegetabler2N(NES, "00", 40, 1, {Vegetabler2Text.pos.x + Vegetabler2Text.size.x + 10, Vegetabler2Text.pos.y});
    UIText Block2N(NES, "00", 40, 1, {Block2Text.pos.x + Block2Text.size.x + 10, Block2Text.pos.y});
    
    UIText Total2(NES, "000000", 40, 1, {GetScreenWidth() - GetScreenWidth()/4.0f, SmallFrame2.dst.y + 10}, UIObject::UP_CENTER);
    GameSystem::Start();
    while(!finished) {
        BeginDrawing();
        ClearBackground(BLACK);
        if (IsKeyPressed(KEY_M)) {
            play_music = !play_music;
        }
        if (play_music) {
            BGM.Play();
        }

        if (IsGamepadAvailable(0)) {
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                paused = !paused;
            }
        } else if (IsKeyPressed(KEY_BACKSPACE)){
            paused = !paused;
        }
        if(acabar) {
            if(current_objects_offset <= objects_offset) {
                BackGrounds[level].Draw();
                float shift = block_height * 6.0f * GetFrameTime();
                current_objects_offset  += shift;
                GameSystem::Move({0,shift});
                BackGrounds[level].Move({0,shift});
                fruits_harvested1 = Player_1->getComponent<Script, PopoBehavior>().frutasRecogidas;
                blocks_destroyed1 = Player_1->getComponent<Script, PopoBehavior>().bloquesDestruidos;
                if(numPlayers == 2){
                    fruits_harvested2 = Player_2->getComponent<Script, PopoBehavior>().frutasRecogidas;
                    blocks_destroyed2 = Player_2->getComponent<Script, PopoBehavior>().bloquesDestruidos;
                    win2 = Player_2->getComponent<Script, PopoBehavior>().victory;
                }
                win1 = Player_1->getComponent<Script, PopoBehavior>().victory;  
                
            } else {
                if (timeToShowScores < 3.0) timeToShowScores += GetFrameTime();
                else {
                    GameSystem::DestroyAll();
                    BigFrame1.Draw();
                    Player1Text.Draw();
                    if (win1)
                        Player1Status.SetText("YOU WIN!", false);
                    Player1Status.Draw();
                    Icicler1.Draw();
                    Icicler1Text.Draw();
                    Icicler1N.Draw();
                    Monster1.Draw();
                    Monster1Text.Draw();
                    Monster1N.Draw();
                    Vegetabler1.Draw();
                    Vegetabler1Text.Draw();
                    Vegetabler1N.SetText(std::to_string(fruits_harvested1), false);
                    Vegetabler1N.Draw();
                    Block1N.SetText(std::to_string(blocks_destroyed1), false);
                    Block1.Draw();
                    Block1Text.Draw();

                    Block1N.Draw();
                    SmallFrame1.Draw();
                    auto pts1 = fruits_harvested1 * 300 + blocks_destroyed1 *10;
                    if (!game_over) {
                        pts1+=3000;
                    }
                    Total1.SetText(std::to_string(pts1), false);
                    Total1.Draw();

                    if(numPlayers == 2) {
                        BigFrame2.Draw();
                        Player2Text.Draw();
                        if (win2)
                            Player2Status.SetText("YOU WIN!", false);
                        Player2Status.Draw();
                        Icicler2.Draw();
                        Icicler2Text.Draw();
                        Monster2.Draw();
                        Monster2Text.Draw();
                        Vegetabler2.Draw();
                        Vegetabler2N.SetText(std::to_string(fruits_harvested2));
                        Vegetabler2Text.Draw();
                        Block2.Draw();
                        Block2N.SetText(std::to_string(blocks_destroyed1));
                        Block2N.Draw();
                        Block2Text.Draw();
                        SmallFrame2.Draw();
                        auto pts2 = fruits_harvested2 * 300 + blocks_destroyed1 *10;
                        if (!game_over) {
                            pts2+=3000;
                        }
                        Total2.SetText(std::to_string(pts2), false);
                        Total2.Draw();
                    }
                    if(IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
                        finished = true;
                    }
                }
            }
        } else {
            
            BackGrounds[level].Draw();
            if (!paused) {
                float delta_time = GetFrameTime();
                chrono_time += delta_time;

                if (speed_run) {
                    time_limit = time_limit - delta_time;

                    if (time_limit < 0) {
                        time_limit = 0;
                        game_over = true;
                        acabar = true;
                    }
                }

                if((Player_1->getComponent<Script, PopoBehavior>().bonusLevel ||
                    Player_1->getComponent<Script, PopoBehavior>().bonusLevel || 
                    Player_1->getComponent<Script, PopoBehavior>().bonusLevel ||
                    Player_1->getComponent<Script, PopoBehavior>().bonusLevel) &&
                    !onBonus) {
                    objects_offset = ((GAME_HEIGHT - 6.0f * block_height) - bonusLevel->getComponent<Transform2D>().position.y);
                    onBonus = true;
                    moving_camera = true;
                    GameSystem::DestroyByTag("Enemy");
                    PopoBehavior::setFinal(true);
                }
                if(Player_1->getComponent<Script, PopoBehavior>().lifes > 0)
                    LifePopo1.Draw();
                if(Player_1->getComponent<Script, PopoBehavior>().lifes > 1)
                    LifePopo2.Draw();
                if(Player_1->getComponent<Script, PopoBehavior>().lifes > 2)
                    LifePopo3.Draw();
                if(numPlayers > 1) {
                    if(Player_2->getComponent<Script, PopoBehavior>().lifes > 0)
                        LifeNana1.Draw();
                    if(Player_2->getComponent<Script, PopoBehavior>().lifes > 1)
                        LifeNana2.Draw();
                    if(Player_2->getComponent<Script, PopoBehavior>().lifes > 2)
                        LifeNana3.Draw();
                }
                if(numPlayers > 2) {
                    if(Player_3->getComponent<Script, PopoBehavior>().lifes > 0)
                        LifeAmam1.Draw();
                    if(Player_3->getComponent<Script, PopoBehavior>().lifes > 1)
                        LifeAmam2.Draw();
                    if(Player_3->getComponent<Script, PopoBehavior>().lifes > 2)
                        LifeAmam3.Draw();
                }
                if(numPlayers > 3) {
                    if(Player_4->getComponent<Script, PopoBehavior>().lifes > 0)
                        LifeLili1.Draw();
                    if(Player_4->getComponent<Script, PopoBehavior>().lifes > 1)
                        LifeLili2.Draw();
                    if(Player_4->getComponent<Script, PopoBehavior>().lifes > 2)
                        LifeLili3.Draw();
                }
                if (!moving_camera && Player_1->getComponent<Script, PopoBehavior>().isGrounded && (
                    Player_1->getComponent<Transform2D>().position.y < (4.0f * block_height) || (numPlayers == 2 &&
                    Player_2->getComponent<Transform2D>().position.y < (4.0f * block_height))) && level < 2) {
                    moving_camera = true;
                    switch (level_phase) {
                    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                        level_phase++;
                        break;
                    }
                }

                if (!onBonus && JosephInstance->getComponent<Script, JosephBehavior>().shakeGround) {
                    JosephInstance->getComponent<Script, JosephBehavior>().shakeGround = false;
                    moving_camera = true;
                }
                
                if (!moving_camera) {
                    GameSystem::Update();
                    if (Player_1->getComponent<Script, PopoBehavior>().lifes <= 0 && 
                        (numPlayers == 1 || Player_2->getComponent<Script, PopoBehavior>().lifes <= 0) && !acabar) {
                        std::cout << "GAME OVER!\n";
                        game_over = true;
                        acabar = true;
                        objects_offset = 0;
                        current_objects_offset = 1;
                    } else if(Player_1->getComponent<Script, PopoBehavior>().victory || (numPlayers == 2 &&
                        Player_2->getComponent<Script, PopoBehavior>().victory)){
                        objects_offset = 20.0f * block_height;
                        acabar = true;
                    }
                } else {
                    float shift = block_height * 6.0f * GetFrameTime();
                    current_objects_offset  += shift;
                    if (current_objects_offset <= objects_offset) {
                        GameSystem::Move({0,shift});
                        BackGrounds[level].Move({0,shift});   
                        if(!onBonus)
                            for(auto enemy : Enemies) {
                                auto second_tags = enemy->second_tags;
                                if (second_tags.find("Topi") != second_tags.end())
                                    enemy->getComponent<Script, TopiBehavior>().Move({0,shift});
                            }
                    } else {
                        objects_offset = old_offset;
                        current_objects_offset = 0;
                        moving_camera = false;
                    }
                    GameSystem::Render();
                }
                
                if (speed_run) {
                    std::string speedrun_string = seconds_to_time(chrono_time);
                    auto dimensions = MeasureTextEx(NES, speedrun_string.c_str(), 35, 2);
                    DrawTextPro(NES, speedrun_string.c_str(), {GAME_WIDTH-dimensions.x-2.0f, 2.0f}, {0,0}, 0, 30, 2, WHITE);
                    std::string time_limit_string = seconds_to_time(time_limit);
                    dimensions = MeasureTextEx(NES, time_limit_string.c_str(), 35, 2);
                    DrawTextPro(NES, time_limit_string.c_str(), {2.0f, 2.0f}, {0,0}, 0, 30, 2, WHITE);
                }
            } else {
                DrawTexturePro(Pause_frame, src_0, dst_1, Vector2{0,0}, 0, WHITE);
                if (show) {
                    if (paused_showtime <= 0) {
                        paused_showtime = 0.75;
                        show = false;
                    } else {
                        DrawTextPro(NES, "PAUSED", Vector2{GAME_WIDTH/2.0f-55, GAME_HEIGHT/2.0f}, Vector2{0,0}, 0, 30, 1.5, WHITE);
                    }
                } else {
                    if (paused_showtime <= 0){
                        paused_showtime = 0.75;
                        show = true;
                    }
                }
                paused_showtime -= GetFrameTime();
            }
        }
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
            GameSystem::DestroyAll();
            finished = true;
            break;
        }
        if (IsKeyPressed(KEY_R)) {
            Player_1->getComponent<Transform2D>().position = Vector2{600,70};
            if(numPlayers == 2) Player_2->getComponent<Transform2D>().position = Vector2{600,70};
        }
        EndDrawing();
    }
    UnloadTexture(Pause_frame);
    Popo.Destroy();
    Nana.Destroy();
    Amam.Destroy();
    Lili.Destroy();
    LevelFloor_0.Destroy();
    LevelFloor_0_Ice.Destroy();
    LevelFloor_1.Destroy();
    LevelFloor_2.Destroy();
    LevelFloor_3.Destroy();
    LevelFloor_4.Destroy();
    BaseFloor.Destroy();
    GrassBlock.Destroy();
    DirtBlock.Destroy();
    IceBlock.Destroy();
    GrassHole.Destroy();
    DirtHole.Destroy();
    IceHole.Destroy();
    GrassWall.Destroy();
    DirtWall.Destroy();
    IceWall.Destroy();
    IceSlidingBlock.Destroy();
    IceSlidingHole.Destroy();
    Wall.Destroy();
    LevelWall_0.Destroy();
    LevelWall_1.Destroy();
    LevelWall_2.Destroy();
    Topi.Destroy();
    Joseph.Destroy();
    Nutpicker.Destroy();
    Eggplant.Destroy();
    Lettuce.Destroy();
    Icicle.Destroy();
    Condor.Destroy();
    Cloud.Destroy();
    SmallCloud.Destroy();
    Death.Destroy();
    ArenaFloor.Destroy();
    ArenaGrassWall.Destroy();
    DirtWallThin.Destroy();
    IceWallSuperThin.Destroy();
    BGM.Unload();
}

//-----------------------------------------------------------------------------
// Menus
// ----------------------------------------------------------------------------
enum MENU_NAVIGATION { 
    MAIN_MENU,
    NEW_GAME,
    NORMAL_GAME,
    SETTINGS,
    VIDEO_SETTINGS,
    AUDIO_SETTINGS,
    CONTROL_SETTINGS
};

int main() {

    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<float> D(0, (float)GetScreenWidth());
    std::uniform_int_distribution<int> I(0,1);
    init_config();
    UISystem::init_UI_system(900, 600);

    //MusicSource Main_title_music("Assets/Sounds/02-Main-Title.wav", true);
    Music MainTitleOST = LoadMusicStream("Assets/Sounds/02-Main-Title.mp3");
    MainTitleOST.looping = true; 
    NES = LoadFont("Assets/Fonts/Pixel_NES.otf");

    UISprite Sign("Assets/Sprites/Titlescreen/06_Sign.png", {GetScreenWidth()/2.0f, 20}, 2.7f, 2.7f, UIObject::UP_CENTER, false, 0.8);
    UISprite Copy("Assets/Sprites/Titlescreen/07_Copyright.png", Vector2{GetScreenWidth()/2.0f, GetScreenHeight()-20.0f}, 2.2f, 2.2f, UIObject::DOWN_CENTER);
    
    int background = I(e2);
    UISprite Background("Assets/Sprites/bg00.png", {0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    UISprite Background1("Assets/Sprites/bg01.png", {0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    UISprite Transparent("Assets/Sprites/UI_Transparent.png", {0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});

    int mscroll_s = -5;
    UISprite Mountain("Assets/Sprites/Titlescreen/03_Mountain.png", Vector2{GetScreenWidth()/2.0f, GetScreenHeight()+10.0f}, 3.6f, 3.6f, UIObject::DOWN_CENTER);

    int fpparallax_s = 15;
    UISprite ForePines("Assets/Sprites/Titlescreen/01_Fore_Pines.png", Vector2{GetScreenWidth()/2.0f, GetScreenHeight()+10.0f}, {(float)GetScreenWidth(), 150}, UIObject::DOWN_CENTER);
    
    int mpparallax_s = 7;
    UISprite MidPines("Assets/Sprites/Titlescreen/02_Mid_Pines.png", Vector2{GetScreenWidth()/2.0f, GetScreenHeight()+10.0f}, {(float)GetScreenWidth(), 200}, UIObject::DOWN_CENTER);

    enum MENU_ENUM { INTRO, MAIN_MENU, NEW_GAME, NORMAL_GAME, SELECT_LEVEL, SETTINGS, CONTROL_SETTINGS };
    // INTRO:
    int intro_state = 0;
    // - Nintendo logo:
    bool  nlinout     = false;
    float nlfadein_s  = 0.3;
    float nlfadeout_s = 0.9;
    float nl_opacity  = 0;
    UISprite Nintendo_logo("Assets/Sprites/Nintendo_logo.png", {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, 0.3f, 0.3f, UIObject::CENTER);

    // - Team logo:
    bool tlinout      = false;
    float tlfadein_s  = 0.3;
    float tlfadeout_s = 0.9;
    float tl_opacity  = 0;
    UISprite Team_logo("Assets/Sprites/Team_logo.png", {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, 2.8f, 2.8f, UIObject::CENTER);

    bool psshow = true;
    float pscurrent_time = 0;
    float pschange_time = 1;
    UIText PressStartText(NES, "Press <ENTER> to start", 30, 1, {GetScreenWidth()/2.0f, GetScreenHeight()-60.0f}, UIObject::DOWN_CENTER);

    // MAIN MENU:
    UIText NewGameText(NES, "NEW GAME", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 45.0f}, UIObject::DOWN_CENTER);
    UIText SettingsText(NES, "SETTINGS", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 105.0f}, UIObject::DOWN_CENTER);
    UIText ExitText(NES, "EXIT", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 165.0f}, UIObject::DOWN_CENTER);
    // NEW GAME MENU:
    UIText NormalModeText(NES, "NORMAL GAME", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 45.0f}, UIObject::DOWN_CENTER);
    UIText BrawlModeText(NES, "BRAWL!", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 105.0f}, UIObject::DOWN_CENTER);
    UIText NewGameReturnText(NES, "RETURN", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 165.0f}, UIObject::DOWN_CENTER);
    // NORMAL GAME MENU:
    int nplayers = 0;
    Texture2D space_button = LoadTexture("Assets/Sprites/Keys/space.png");
    UIText ContinueText(NES, "CONTINUE", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 105.0f}, UIObject::DOWN_CENTER);
    UIText NormalGameReturnText(NES, "RETURN", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 165.0f}, UIObject::DOWN_CENTER);

    bool p1show = false;
    float p1current_time = 0;
    float p1flicker_time = 1;
    UIText P1(NES, "P1", 33, 1, {GetScreenWidth()/2.0f - 90, GetScreenHeight()/2.0f - 130}, UIObject::DOWN_CENTER);
    UIText P1Press(NES, "PRESS", 27, 1, {GetScreenWidth()/2.0f - 90, GetScreenHeight()/2.0f - 75}, UIObject::CENTER);
    UISprite P1Space(space_button, {GetScreenWidth()/2.0f - 130, GetScreenHeight()/2.0f - 50}, 1.5f, 1.5f);
    UISprite P1Char("Assets/Sprites/popo.png", {GetScreenWidth()/2.0f - 130, GetScreenHeight()/2.0f - 90}, 4.5f, 4.5f);
    bool p2show = false;
    float p2current_time = 0;
    float p2flicker_time = 1;
    UIText P2(NES, "P2", 33, 1, {GetScreenWidth()/2.0f + 90, GetScreenHeight()/2.0f - 130}, UIObject::DOWN_CENTER);
    UIText P2Press(NES, "PRESS", 27, 1, {GetScreenWidth()/2.0f + 92, GetScreenHeight()/2.0f - 75}, UIObject::CENTER);
    UISprite P2Space(space_button, {GetScreenWidth()/2.0f + 57, GetScreenHeight()/2.0f - 50}, 1.5f, 1.5f);
    UISprite P2Char("Assets/Sprites/nana.png", {GetScreenWidth()/2.0f + 50, GetScreenHeight()/2.0f - 90}, 4.5f, 4.5f);

    Texture2D OptionsButton = LoadTexture("Assets/Sprites/Buttons/MR(PS4).png");
    UISprite P1Options(OptionsButton, {GetScreenWidth()/2.0f - 125, GetScreenHeight()/2.0f - 50}, 2.7f, 2.7f);
    UISprite P2Options(OptionsButton, {GetScreenWidth()/2.0f + 60, GetScreenHeight()/2.0f - 50}, 2.7f, 2.7f);

    // SELECT LEVEL:
    Texture2D cross = LoadTexture("Assets/Sprites/UI_Cross.png");
    bool CONFIGURE_MATCH = false, fuego_amigo = false, speed_run = false;
    int selected_level = 0;
    UISprite Level1("Assets/Sprites/Level1_preview.png", {GetScreenWidth()/2.0f, 100}, 2.3f, 2.3f, UIObject::UP_CENTER);
    UIText Level1Text(NES, "LEVEL 1: BASICS", 33, 1, {GetScreenWidth()/2.0f, 50}, UIObject::UP_CENTER);
    //UISprite Level2("Assets/Sprites/Level2_preview.png", {GetScreenWidth()/2.0f, 100}, 2.3f, 2.3f, UIObject::UP_CENTER);
    UIText Level2Text(NES, "LEVEL 2: ADVANCED MECHANICS", 33, 1, {GetScreenWidth()/2.0f, 50}, UIObject::UP_CENTER);
    UIText Level3Text(NES, "LEVEL 3: TRYING TO SURVIVE", 33, 1, {GetScreenWidth()/2.0f, 50}, UIObject::UP_CENTER);
    UIText Level3WIP(NES, "NO TIME FOR THIS LEVEL :(", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, UIObject::CENTER);
    UIText Level4Text(NES, "LEVEL 4: A NEW CHALLENGER", 33, 1, {GetScreenWidth()/2.0f, 50}, UIObject::UP_CENTER);
    UIText Level4WIP(NES, "NO TIME FOR THIS LEVEL :(", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, UIObject::CENTER);


    UIText StartText(NES, "START!", 27, 1, {GetScreenWidth()/2.0f + 120, GetScreenHeight()/2.0f + 50}, UIObject::CENTER_LEFT);
    UIText SpeedRunText(NES, "SPEED RUN?", 27, 1, {GetScreenWidth()/2.0f + 120, GetScreenHeight()/2.0f + 100}, UIObject::CENTER_LEFT);
    UISprite SpeedRunCheck(cross, {GetScreenWidth() - 78.0f, SpeedRunText.pos.y - 1}, 2.0f, 2.0f);
    UIText FriendlyFireText(NES, "FRIEND FIRE?", 27, 1, {GetScreenWidth()/2.0f + 120, GetScreenHeight()/2.0f + 150}, UIObject::CENTER_LEFT);
    UISprite FriendlyFireCheck(cross, {GetScreenWidth() - 78.0f, FriendlyFireText.pos.y - 1}, 2.0f, 2.0f);
    UIText BindigsText(NES, "BINDINGS", 27, 1, {GetScreenWidth()/2.0f + 120, GetScreenHeight()/2.0f + 200}, UIObject::CENTER_LEFT);

    // SETTINGS MENU:
    Texture2D LArrow = LoadTexture("Assets/Sprites/UI_Arrow_left.png"), RArrow = LoadTexture("Assets/Sprites/UI_Arrow_right.png");
    UIText OldStyleText(NES, "OLD STYLE?", 33, 1, {200, GetScreenHeight()/2.0f - 180}, UIObject::CENTER_LEFT);
    UISprite OldStyleCheck(cross, {GetScreenWidth()/2.0f + 157, OldStyleText.pos.y - 3}, 2.0f, 2.0f);
    
    UIText DisplayModeText(NES, "DISPLAY MODE", 33, 1, {200, GetScreenHeight()/2.0f - 120}, UIObject::CENTER_LEFT);
    UIText SelectedDisplayModeText(NES, to_string((DISPLAY_MODE_ENUM)std::get<int>(ini["Graphics"]["DisplayMode"])),
        25, 1, {GetScreenWidth()/2.0f + 170, DisplayModeText.pos.y}, UIObject::UP_CENTER);
    UISprite DisplayModeLArrow(LArrow, {GetScreenWidth()/2.0f + 60, DisplayModeText.pos.y}, 1.0f, 1.0f);
    UISprite DisplayModeRArrow(RArrow, {GetScreenWidth()/2.0f + 260, DisplayModeText.pos.y}, 1.0f, 1.0f);
    
    UIText ResolutionText(NES, "RESOLUTION", 33, 1, {200, GetScreenHeight()/2.0f - 60}, UIObject::CENTER_LEFT);
    UISprite ResolutionLArrow(LArrow, {GetScreenWidth()/2.0f + 60, ResolutionText.pos.y}, 1.0f, 1.0f);
    UIText SelectedResolutionText(NES, std::to_string(std::get<int>(ini["Graphics"]["ScreenWidth"])) + "x" + std::to_string(std::get<int>(ini["Graphics"]["ScreenHeight"])), 
        25, 1, {GetScreenWidth()/2.0f + 170, ResolutionText.pos.y}, UIObject::UP_CENTER);
    UISprite ResolutionRArrow(RArrow, {GetScreenWidth()/2.0f + 260, ResolutionText.pos.y}, 1.0f, 1.0f);
    
    UIText FPSLimitText(NES, "FPS LIMIT", 33, 1, {200, GetScreenHeight()/2.0f}, UIObject::CENTER_LEFT);
    UISprite FPSLimitLArrow(LArrow, {GetScreenWidth()/2.0f + 60, FPSLimitText.pos.y}, 1.0f, 1.0f);
    UIText SelectedFPSLimitText(NES, std::to_string(std::get<int>(ini["Graphics"]["FPSLimit"])), 25, 1, {GetScreenWidth()/2.0f + 170, FPSLimitText.pos.y}, UIObject::UP_CENTER);
    UISprite FPSLimitRArrow(RArrow, {GetScreenWidth()/2.0f + 260, FPSLimitText.pos.y}, 1.0f, 1.0f);
    
    UIText VolumeText(NES, "VOLUME", 33, 1, {200, GetScreenHeight()/2.0f + 60}, UIObject::CENTER_LEFT);
    UIText VolumePercentageText(NES, std::to_string((int)(std::get<float>(ini["Audio"]["Volume"]) * 100)), 27, 1, {715, VolumeText.pos.y + 15}, UIObject::CENTER);
    
    UIText ChangeKeybindingsText(NES, "CHANGE KEYBINDINGS", 33, 1, {200, GetScreenHeight()/2.0f + 120}, UIObject::CENTER_LEFT);
    UIText SettingsReturnText(NES, "RETURN", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 180}, UIObject::CENTER);

    // KEY BINDING SETTNGS:
    bool SELECTED = false, SHOW_ERROR_MESSAGE = false;
    int PLAYER = 0, CONTROLLER = Controller::KEYBOARD;
    UIText PlayerText(NES, "PLAYER", 33, 1, {200, GetScreenHeight()/2.0f - 185}, UIObject::CENTER_LEFT);
    UISprite PlayerLArrow(LArrow, {GetScreenWidth()/2.0f + 7, PlayerText.pos.y}, 1.0f, 1.0f);
    UIText SelectedPlayer(NES, "1", 25, 1, {GetScreenWidth()/2.0f + 130, PlayerText.pos.y}, UIObject::UP_CENTER);
    UISprite PlayerRArrow(RArrow, {GetScreenWidth()/2.0f + 240, PlayerText.pos.y}, 1.0f, 1.0f);

    UIText ControllerText(NES, "CONTROLLER", 33, 1, {200, GetScreenHeight()/2.0f - 125}, UIObject::CENTER_LEFT);
    UISprite ControllerLArrow(LArrow, {GetScreenWidth()/2.0f + 7, ControllerText.pos.y}, 1.0f, 1.0f);
    UIText SelectedController(NES, to_string(Controller::KEYBOARD), 25, 1, {GetScreenWidth()/2.0f + 130, ControllerText.pos.y}, UIObject::UP_CENTER);
    UISprite ControllerRArrow(RArrow, {GetScreenWidth()/2.0f + 240, ControllerText.pos.y}, 1.0f, 1.0f);
    
    UIText LeftActionText(NES, to_string(Controller::LEFT), 27, 1, {250, GetScreenHeight()/2.0f - 65}, UIObject::CENTER_LEFT);
    //UISprite LeftActionLArrow(LArrow, {GetScreenWidth()/2.0f + 80, LeftActionText.pos.y}, 1.0f, 1.0f);
    UIText LeftActionKeybind(NES, controllers[PLAYER]->getActionBind(Controller::LEFT), 25, 1, {GetScreenWidth()/2.0f + 130, LeftActionText.pos.y}, UIObject::UP_CENTER);
    //UISprite LeftActionRArrow(RArrow, {GetScreenWidth()/2.0f + 400, LeftActionText.pos.y}, 1.0f, 1.0f);

    UIText RightActionText(NES, to_string(Controller::RIGHT), 27, 1, {250, GetScreenHeight()/2.0f - 5}, UIObject::CENTER_LEFT);
    //UISprite RightActionLArrow(LArrow, {GetScreenWidth()/2.0f + 80, RightActionText.pos.y}, 1.0f, 1.0f);
    UIText RightActionKeybind(NES, controllers[PLAYER]->getActionBind(Controller::RIGHT), 25, 1, {GetScreenWidth()/2.0f + 130, RightActionText.pos.y}, UIObject::UP_CENTER);
    //UISprite RightActionRArrow(RArrow, {GetScreenWidth()/2.0f + 400, RightActionText.pos.y}, 1.0f, 1.0f);

    UIText DownActionText(NES, to_string(Controller::DOWN), 27, 1, {250, GetScreenHeight()/2.0f + 55}, UIObject::CENTER_LEFT);
    //UISprite DownActionLArrow(LArrow, {GetScreenWidth()/2.0f + 80, DownActionText.pos.y}, 1.0f, 1.0f);
    UIText DownActionKeybind(NES, controllers[PLAYER]->getActionBind(Controller::DOWN), 25, 1, {GetScreenWidth()/2.0f + 130, DownActionText.pos.y}, UIObject::UP_CENTER);
    //UISprite DownActionRArrow(RArrow, {GetScreenWidth()/2.0f + 400, DownActionText.pos.y}, 1.0f, 1.0f);

    UIText UpActionText(NES, to_string(Controller::JUMP), 27, 1, {250, GetScreenHeight()/2.0f + 115}, UIObject::CENTER_LEFT);
    //UISprite UpActionLArrow(LArrow, {GetScreenWidth()/2.0f + 80, UpActionText.pos.y}, 1.0f, 1.0f);
    UIText UpActionKeybind(NES, controllers[PLAYER]->getActionBind(Controller::JUMP), 25, 1, {GetScreenWidth()/2.0f + 130, UpActionText.pos.y}, UIObject::UP_CENTER);
    //UISprite UpActionRArrow(RArrow, {GetScreenWidth()/2.0f + 400, UpActionText.pos.y}, 1.0f, 1.0f);

    UIText AttackActionText(NES, to_string(Controller::ATTACK), 27, 1, {250, GetScreenHeight()/2.0f + 175}, UIObject::CENTER_LEFT);
    //UISprite AttackActionLArrow(LArrow, {GetScreenWidth()/2.0f + 80, AttackActionText.pos.y}, 1.0f, 1.0f);
    UIText AttackActionKeybind(NES, controllers[PLAYER]->getActionBind(Controller::ATTACK), 25, 1, {GetScreenWidth()/2.0f + 130, AttackActionText.pos.y}, UIObject::UP_CENTER);
    //UISprite AttackActionRArrow(RArrow, {GetScreenWidth()/2.0f + 400, AttackActionText.pos.y}, 1.0f, 1.0f);

    UIText BindingErrorText(NES, "Binding conflict with (PLAYER: -1, CONTROL: NO_CONTROL)", 20, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 250}, UIObject::DOWN_CENTER);

    // GENERAL UI:
    bool BLOCK = false;
    UISprite Hammer("Assets/Sprites/UI_Old_Hammer.png", {NewGameText.pos.x - 70, NewGameText.pos.y + NewGameText.size.y/2.0f}, 4.0f, 4.0f, UIObject::CENTER_RIGHT);
    int HV = 0; // HammerView
    std::unordered_map<int, std::vector<Vector2>> HammerRefs {
        {0,{
            {Hammer.dst.x*640/900, Hammer.dst.y*480/600},
            {Hammer.dst.x*800/900, Hammer.dst.y},
            {Hammer.dst.x, Hammer.dst.y}, 
            {Hammer.dst.x*1024/900, Hammer.dst.y*768/600},
            {Hammer.dst.x*1280/900, Hammer.dst.y*720/600 - 10},
            {Hammer.dst.x*1366/900, Hammer.dst.y*768/600 - 10},
            {Hammer.dst.x*1680/900, Hammer.dst.y*1050/600 - 10},
            {Hammer.dst.x*1920/900, Hammer.dst.y*1080/600 - 20}
        }}, {1, {
            {(OldStyleText.pos.x-70)*640/900, OldStyleText.pos.y*480/600},
            {(OldStyleText.pos.x-70)*800/900, OldStyleText.pos.y},
            {(OldStyleText.pos.x-70), OldStyleText.pos.y}, 
            {(OldStyleText.pos.x-70)*1024/900, OldStyleText.pos.y*768/600},
            {(OldStyleText.pos.x-70)*1280/900, OldStyleText.pos.y*720/600 - 5},
            {(OldStyleText.pos.x-70)*1366/900, OldStyleText.pos.y*768/600 - 5},
            {(OldStyleText.pos.x-70)*1680/900, OldStyleText.pos.y*1050/600 - 5},
            {(OldStyleText.pos.x-70)*1920/900, OldStyleText.pos.y*1080/600 - 15}
        }}, {2, {
            {(ContinueText.pos.x-70)*640/900, ContinueText.pos.y*480/600},
            {(ContinueText.pos.x-70)*800/900, ContinueText.pos.y},
            {(ContinueText.pos.x-70), ContinueText.pos.y}, 
            {(ContinueText.pos.x-70)*1024/900, ContinueText.pos.y*768/600},
            {(ContinueText.pos.x-70)*1280/900, ContinueText.pos.y*720/600 - 5},
            {(ContinueText.pos.x-70)*1366/900, ContinueText.pos.y*768/600 - 5},
            {(ContinueText.pos.x-70)*1680/900, ContinueText.pos.y*1050/600 - 5},
            {(ContinueText.pos.x-70)*1920/900, ContinueText.pos.y*1080/600 - 15}
        }}, {3, {
            {(StartText.pos.x-70)*640/900, StartText.pos.y*480/600},
            {(StartText.pos.x-70)*800/900, StartText.pos.y},
            {(StartText.pos.x-70), StartText.pos.y}, 
            {(StartText.pos.x-70)*1024/900, StartText.pos.y*768/600},
            {(StartText.pos.x-70)*1280/900, StartText.pos.y*720/600 - 5},
            {(StartText.pos.x-70)*1366/900, StartText.pos.y*768/600 - 5},
            {(StartText.pos.x-70)*1680/900, StartText.pos.y*1050/600 - 5},
            {(StartText.pos.x-70)*1920/900, StartText.pos.y*1080/600 - 15}
        }}
    };
    // KEYS (KEYBOARD):
    bool keyboard = true;

    UIText EnterText(NES, "SELECT", 15, 1, {GetScreenWidth()-20.0f, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite EnterKey("Assets/Sprites/Keys/enter.png", {GetScreenWidth() - EnterText.size.x - 30.0f, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);

    Texture2D RightKey = LoadTexture("Assets/Sprites/Keys/pader.png"), LeftKey = LoadTexture("Assets/Sprites/Keys/paizq.png");
    UIText ChangeOptionText(NES, "CHANGE", 15, 1, {EnterKey.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite RightKeySettings(RightKey, {ChangeOptionText.pos.x - 10.0f, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite LeftKeySettings(LeftKey, {RightKeySettings.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    
    UIText SelectLevelText(NES, "CHANGE LEVEL", 15, 1, {EnterKey.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite RightKeyLevel(RightKey, {SelectLevelText.pos.x - 10.0f, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite LeftKeyLevel(LeftKey, {RightKeyLevel.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    
    Texture2D UpKey = LoadTexture("Assets/Sprites/Keys/parriba.png"), DownKey = LoadTexture("Assets/Sprites/Keys/pabajo.png");
    UIText MoveTextNormal(NES, "MOVE", 15, 1, {EnterKey.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite UpKeyNormal(UpKey, {MoveTextNormal.pos.x - 10, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite DownKeyNormal(DownKey, {UpKeyNormal.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);

    UIText MoveTextSettings(NES, "MOVE", 15, 1, {LeftKeySettings.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite UpKeySettings(UpKey, {MoveTextSettings.pos.x - 10, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite DownKeySettings(DownKey, {UpKeySettings.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);

    Texture EscKey = LoadTexture("Assets/Sprites/Keys/esc.png"), ReturnKey = LoadTexture("Assets/Sprites/Keys/backspace.png");
    UIText ReturnTextNormal(NES, "RETURN", 15, 1, {DownKeyNormal.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite EscKeyNormal(EscKey, {ReturnTextNormal.pos.x - 10, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite ReturnKeyNormal(ReturnKey, {EscKeyNormal.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);

    UIText ReturnTextSettings(NES, "RETURN", 15, 1, {DownKeySettings.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite EscKeySettings(EscKey, {ReturnTextSettings.pos.x - 10, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite ReturnKeySettings(ReturnKey, {EscKeySettings.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);

    // KEYS (PS4)
    UIText XText(NES, "SELECT", 15, 1, {GetScreenWidth()-20.0f, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite XButton("Assets/Sprites/Buttons/RFDown(PS4).png", {GetScreenWidth() - EnterText.size.x - 30.0f, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);

    Texture2D DpadRight = LoadTexture("Assets/Sprites/Buttons/LFRight(PS4).png"), DpadLeft = LoadTexture("Assets/Sprites/Buttons/LFLeft(PS4).png");
    UIText DpadOptionText(NES, "CHANGE", 15, 1, {XButton.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite DpadRightSettings(DpadRight, {DpadOptionText.pos.x - 10.0f, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
    UISprite DpadLeftSettings(DpadLeft, {DpadRightSettings.dst.x - 5, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
    
    UIText DpadSelectLevelText(NES, "CHANGE LEVEL", 15, 1, {XButton.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite DpadRightLevel(DpadRight, {DpadSelectLevelText.pos.x - 10.0f, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
    UISprite DpadLeftLevel(DpadLeft, {DpadRightLevel.dst.x - 5, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);

    Texture2D DpadUp = LoadTexture("Assets/Sprites/Buttons/LFUp(PS4).png"), DpadDown = LoadTexture("Assets/Sprites/Buttons/LFDown(PS4).png");
    UIText DpadMoveTextNormal(NES, "MOVE", 15, 1, {XButton.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite DpadUpNormal(DpadUp, {DpadMoveTextNormal.pos.x - 10, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
    UISprite DpadDownNormal(DpadDown, {DpadUpNormal.dst.x - 5, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
 
    UIText DpadMoveTextSettings(NES, "MOVE", 15, 1, {DpadLeftSettings.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite DpadUpSettings(DpadUp, {DpadMoveTextSettings.pos.x - 10, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
    UISprite DpadDownSettings(DpadDown, {DpadUpSettings.dst.x - 5, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);


    Texture OButton = LoadTexture("Assets/Sprites/Buttons/RFRight(PS4).png");
    UIText OTextNormal(NES, "RETURN", 15, 1, {DpadDownNormal.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite OButtonNormal(OButton, {OTextNormal.pos.x - 10, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
    
    UIText OTextSettings(NES, "RETURN", 15, 1, {DpadDownSettings.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite OButtonSettings(OButton, {OTextSettings.pos.x - 10, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);

/*

    Texture EscKey = LoadTexture("Assets/Sprites/Keys/esc.png"), ReturnKey = LoadTexture("Assets/Sprites/Keys/backspace.png");
    UIText ReturnTextNormal(NES, "RETURN", 15, 1, {DownKeyNormal.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite EscKeyNormal(EscKey, {ReturnTextNormal.pos.x - 10, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite ReturnKeyNormal(ReturnKey, {EscKeyNormal.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    
    UIText ReturnTextSettings(NES, "RETURN", 15, 1, {DownKeySettings.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite EscKeySettings(EscKey, {ReturnTextSettings.pos.x - 10, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite ReturnKeySettings(ReturnKey, {EscKeySettings.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
*/




    std::vector<int> HammerOffsets {
        60, 60, 60, 50
    };

    // /*** Main Loop ***/ //
    int OPTION  = 0;
    int OPTIONS = 3;
    MENU_ENUM CURRENT_MENU = INTRO, OLD_MENU = INTRO;
    update_window();
    UISystem::Reescale();
    
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (GamepadDetected(0)) {
            keyboard = false;
        } else if (KeyboardDetected()) {
            keyboard = true;
        }

        if (background == 0) {
            Background.Draw();
        } else {
            Background1.Draw();
        }

        Mountain.Draw();
        if (Mountain.dst.x + Mountain.dst.width >= 0) {
            Mountain.dst.x += mscroll_s * GetFrameTime();
        } else {
            Mountain.dst.x = GetScreenWidth() + 100;
        }
        
        MidPines.Draw();
        MidPines.src.x += mpparallax_s * GetFrameTime();

        ForePines.Draw();
        ForePines.src.x += fpparallax_s * GetFrameTime();

        if (CURRENT_MENU == INTRO) {
            if (intro_state == 0) {
                Nintendo_logo.Draw(Fade(WHITE, nl_opacity));
                if (!nlinout) {
                    if (nl_opacity < 1) {
                        nl_opacity += nlfadein_s * GetFrameTime();
                    } else {
                        nlinout = true;
                    }
                } else {
                    if (nl_opacity > 0) {
                        nl_opacity -= nlfadeout_s * GetFrameTime();
                    } else {
                        intro_state++;
                    }
                }
            } else if (intro_state == 1) {
                Team_logo.Draw(Fade(WHITE, tl_opacity));
                if (!tlinout) {
                    if (tl_opacity < 1) {
                        tl_opacity += tlfadein_s * GetFrameTime();
                    } else {
                        tlinout = true;
                    }
                } else {
                    if (tl_opacity > 0) {
                        tl_opacity -= tlfadeout_s * GetFrameTime();
                    } else {
                        intro_state++;
                    }
                }
            } else {

                Sign.Draw();
                Copy.Draw();

                if (pscurrent_time < pschange_time) {
                    pscurrent_time += GetFrameTime();
                } else {
                    pscurrent_time = 0;
                    psshow = !psshow;
                }
                if (psshow) {
                    PressStartText.Draw(COYOTEBROWN);
                }
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                if (intro_state++ == 2) {
                    CURRENT_MENU = MAIN_MENU;
                    PlayMusicStream(MainTitleOST);
                }
            }

        } else {
            UpdateMusicStream(MainTitleOST);

            if (CURRENT_MENU == MAIN_MENU) {
                Sign.Draw();
                NewGameText.Draw();
                SettingsText.Draw();
                ExitText.Draw(BLUE);

                auto new_height = 80.0f * GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF;
                DrawRectangleV({0,GetScreenHeight() - new_height+20.0f*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, {(float)GetScreenWidth(), new_height}, Fade(BLACK, 0.65));
                
                if (keyboard) {
                    EnterText.Draw();
                    EnterKey.Draw();
                    MoveTextNormal.Draw();
                    UpKeyNormal.Draw();
                    DownKeyNormal.Draw();
                } else {
                    XText.Draw();
                    XButton.Draw();
                    DpadMoveTextNormal.Draw();
                    DpadUpNormal.Draw();
                    DpadDownNormal.Draw();
                }

                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                    if (OPTION == 0) {
                        CURRENT_MENU = NEW_GAME;
                    } else {
                        if (OPTION == 1) {
                            CURRENT_MENU = SETTINGS;
                            OPTIONS = 7;
                            HV = 1;
                        } else if (OPTION == 2) {
                            break;
                        }
                        OPTION = 0;
                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y}); 
                    }
                } else if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                    break; 
                }

            } else {
                // - BACKGROUND:
                DrawRectangleV({0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()}, Fade(BLACK, 0.65));
                if (CURRENT_MENU == SELECT_LEVEL) {

                    DrawRectangleLinesEx({Level1.dst.x-7, Level1.dst.y-7, Level1.dst.width+14, Level1.dst.height+14}, 7, COYOTEBROWN);
                    if (selected_level == 0) {
                        Level1.Draw();
                        Level1Text.Draw();
                    } else if (selected_level == 1) {
                        //Level2.Draw();
                        std::cout << 1553 << std::endl;
                        DrawRectangleV({Level1.dst.x, Level1.dst.y}, {Level1.dst.width, Level1.dst.height}, GRAY);
                        std::cout << 1555 << std::endl;
                        Level2Text.Draw();
                        std::cout << 1557 << std::endl;
                    } else if (selected_level == 2) {
                        DrawRectangleV({Level1.dst.x, Level1.dst.y}, {Level1.dst.width, Level1.dst.height}, GRAY);
                        Level3Text.Draw();
                        Level3WIP.Draw();
                    } else if (selected_level == 3) {
                        DrawRectangleV({Level1.dst.x, Level1.dst.y}, {Level1.dst.width, Level1.dst.height}, GRAY);
                        Level4Text.Draw();
                        Level4WIP.Draw();
                    }


                    if (!CONFIGURE_MATCH) {
                        auto new_height = 80.0f * GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF;
                        DrawRectangleV({0,GetScreenHeight() - new_height+20.0f*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, {(float)GetScreenWidth(), new_height}, Fade(BLACK, 0.65));
                        if (keyboard) {
                            EnterText.Draw();
                            EnterKey.Draw();
                            SelectLevelText.Draw();
                            RightKeyLevel.Draw();
                            LeftKeyLevel.Draw();
                        } else {
                            XText.Draw();
                            XButton.Draw();
                            DpadSelectLevelText.Draw();
                            DpadRightLevel.Draw();
                            DpadLeftLevel.Draw();
                        }


                        if (IsKeyPressed(KEY_ESCAPE)  || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            CURRENT_MENU = NORMAL_GAME;
                            BLOCK = false;
                            HV = 2;
                            Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y});
                            OPTION = 0;
                            OPTIONS = 2;
                        } else if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                            selected_level = mod(selected_level-1, 4);
                        } else if (IsKeyPressed(KEY_RIGHT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                            selected_level = mod(selected_level+1, 4);
                        } else if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                            OPTION  = 0;
                            CONFIGURE_MATCH = true;
                            BLOCK = false;
                        }
                    } else {
                        auto menuv_x = GetScreenWidth() - (20*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF) - (400*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF);
                        auto menuv_y = GetScreenHeight()/2.0f;
                        DrawRectangleV({menuv_x,menuv_y}, {400*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 250*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, Fade(BLACK, 0.65));
                        StartText.Draw(PINK);
                        SpeedRunText.Draw();
                        DrawRectangleV({GetScreenWidth() - (72*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF), SpeedRunText.pos.y+(4*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {20*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 20*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, WHITE);
                        DrawRectangleV({GetScreenWidth() - (70*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF), SpeedRunText.pos.y+(6*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {15*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 15*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, BLACK);
                        if (speed_run) {
                            SpeedRunCheck.Draw();
                        }
                        FriendlyFireText.Draw();
                        DrawRectangleV({GetScreenWidth() - (72*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF), FriendlyFireText.pos.y+(4*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {20*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 20*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, WHITE);
                        DrawRectangleV({GetScreenWidth() - (70*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF), FriendlyFireText.pos.y+(6*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {15*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 15*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, BLACK);
                        if (fuego_amigo) {
                            FriendlyFireCheck.Draw();
                        }
                        BindigsText.Draw(YELLOW);

                        auto new_height = 80.0f * GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF;
                        DrawRectangleV({0,GetScreenHeight() - new_height+20.0f*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, {(float)GetScreenWidth(), new_height}, Fade(BLACK, 0.65));
                        
                        if (keyboard) {
                            EnterText.Draw();
                            EnterKey.Draw();
                            MoveTextNormal.Draw();
                            UpKeyNormal.Draw();
                            DownKeyNormal.Draw();
                            ReturnTextNormal.Draw();
                            ReturnKeyNormal.Draw();
                            EscKeyNormal.Draw();
                        } else {
                            XText.Draw();
                            XButton.Draw();
                            DpadMoveTextNormal.Draw();
                            DpadUpNormal.Draw();
                            DpadDownNormal.Draw();
                            OTextNormal.Draw();
                            OButtonNormal.Draw();
                        }


                        if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                            if (OPTION == 0) {
                                Game(nplayers, selected_level, speed_run);
                            } else if (OPTION == 1) {
                                speed_run = !speed_run;
                            } else if (OPTION == 2) {
                                fuego_amigo = !fuego_amigo;
                            } else if (OPTION == 3) {
                                OLD_MENU = SELECT_LEVEL;
                                CURRENT_MENU = CONTROL_SETTINGS;
                                HV = 1;
                                OPTIONS = 7;
                                OPTION = 0;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y}); 
                            }
                        } else if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            BLOCK = true;
                            CONFIGURE_MATCH = false;
                            speed_run = fuego_amigo = false;
                        }
                    }
                } else {

                    auto new_height = 80.0f * GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF;
                    DrawRectangleV({0,GetScreenHeight() - new_height+20.0f*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, {(float)GetScreenWidth(), new_height}, Fade(BLACK, 0.65));
                    if (keyboard) {
                        EnterText.Draw();
                        EnterKey.Draw();
                    } else {
                        XText.Draw();
                        XButton.Draw();
                    }


                    if (CURRENT_MENU == NEW_GAME) {

                        if (keyboard) {
                            MoveTextNormal.Draw();
                            UpKeyNormal.Draw();
                            DownKeyNormal.Draw();

                            ReturnTextNormal.Draw();
                            EscKeyNormal.Draw();
                            ReturnKeyNormal.Draw();
                        } else {
                            DpadMoveTextNormal.Draw();
                            DpadUpNormal.Draw();
                            DpadDownNormal.Draw();
                            OTextNormal.Draw();
                            OButtonNormal.Draw();
                        }


                        NormalModeText.Draw(WHITE);
                        BrawlModeText.Draw(GRAY);
                        NewGameReturnText.Draw(BLUE);

                        if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                            if (OPTION == 0) {
                                CURRENT_MENU = NORMAL_GAME;
                                OPTIONS = 2;
                                HV = 2;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y}); 
                            } else {
                                if (OPTION == 1) {
                                    std::cout << "NO REIMPLEMENTADO: BRAWL!\n";
                                } else if (OPTION == 2) {
                                    CURRENT_MENU = MAIN_MENU;
                                }
                                OPTION = 0;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y}); 
                            }
                        } else if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            CURRENT_MENU = MAIN_MENU;
                            OPTION = 0;
                            Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y}); 
                        }

                    } else if (CURRENT_MENU == NORMAL_GAME) {

                        if (keyboard) {
                            MoveTextNormal.Draw();
                            UpKeyNormal.Draw();
                            DownKeyNormal.Draw();

                            ReturnTextNormal.Draw();
                            EscKeyNormal.Draw();
                            ReturnKeyNormal.Draw();
                        } else {
                            DpadMoveTextNormal.Draw();
                            DpadUpNormal.Draw();
                            DpadDownNormal.Draw();
                            OTextNormal.Draw();
                            OButtonNormal.Draw();
                        }


                        if (nplayers == 0) {
                            ContinueText.Draw(GRAY);
                            P1.Draw(GRAY);
                            if (p1current_time < p1flicker_time) {
                                p1current_time += GetFrameTime();
                            } else {
                                p1current_time = 0;
                                p1show = !p1show;
                            }
                            if (p1show) {
                                P1Press.Draw();
                                if (keyboard) {
                                    P1Space.Draw();
                                } else {
                                    P1Options.Draw();
                                }
                            }

                            P2.Draw(GRAY);
                            if (p2current_time < p2flicker_time) {
                                p2current_time += GetFrameTime();
                            } else {
                                p2current_time = 0;
                                p2show = !p2show;
                            }
                            if (p2show) {
                                P2Press.Draw();
                                if (keyboard) {
                                    P2Space.Draw();
                                } else {
                                    P2Options.Draw();
                                }
                            }
                        } else {
                            ContinueText.Draw(WHITE);
                            P1.Draw(WHITE);
                            P1Char.Draw();

                            if (nplayers == 1) {
                                P2.Draw(GRAY);
                                if (p2current_time < p2flicker_time) {
                                    p2current_time += GetFrameTime();
                                } else {
                                    p2current_time = 0;
                                    p2show = !p2show;
                                }
                                if (p2show) {
                                    P2Press.Draw();
                                    if (keyboard) {
                                        P2Space.Draw();
                                    } else {
                                        P2Options.Draw();
                                    }
                                }
                            } else {
                                P2.Draw(WHITE);
                                P2Char.Draw();
                            }
                        }

                        NormalGameReturnText.Draw(BLUE);
                        if (IsKeyPressed(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) || IsGamepadButtonPressed(1, GAMEPAD_BUTTON_MIDDLE_RIGHT) ) {
                            p1current_time = p2current_time = 0;
                            p1show = p2show = false;
                            nplayers++;
                        } else if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                            if (OPTION == 0) {
                                if (nplayers > 0) {
                                    CURRENT_MENU = SELECT_LEVEL;
                                    OPTION  = 0;
                                    BLOCK = true;
                                    OPTIONS = 4;
                                    HV = 3;
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y}); 
                                }
                            } else if (OPTION == 1) {
                                CURRENT_MENU = NEW_GAME;
                                OPTION  = 0;
                                OPTIONS = 3;
                                HV = 0;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y});
                                p1show = p2show = false;
                                nplayers = 0;
                            }
                        } else if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            if (nplayers > 0) {
                                nplayers--;
                            } else {
                                CURRENT_MENU = NEW_GAME;
                                OPTION  = 0;
                                OPTIONS = 3;
                                HV = 0;
                                BLOCK = false;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y});
                            }
                            p1current_time = p2current_time = 0;
                            p1show = p2show = false;
                        }
                    } else if (CURRENT_MENU == SETTINGS) {

                        if (keyboard) {
                            ChangeOptionText.Draw();
                            RightKeySettings.Draw();
                            LeftKeySettings.Draw();

                            MoveTextSettings.Draw();
                            UpKeySettings.Draw();
                            DownKeySettings.Draw();

                            ReturnTextSettings.Draw();
                            EscKeySettings.Draw();
                            ReturnKeySettings.Draw();
                        } else {
                            DpadOptionText.Draw();
                            DpadRightSettings.Draw();
                            DpadLeftSettings.Draw();

                            DpadMoveTextSettings.Draw();
                            DpadUpSettings.Draw();
                            DpadDownSettings.Draw();

                            OTextSettings.Draw();
                            OButtonSettings.Draw();
                        }

                        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            CURRENT_MENU = MAIN_MENU;
                            OPTION = 1;
                            OPTIONS = 3;
                            HV = 0;
                            Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                        } else {
                            if (OPTION == 0) {
                                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                                    ini["Graphics"]["OldFashioned"] = !std::get<bool>(ini["Graphics"]["OldFashioned"]);
                                }
                            } else if (OPTION == 1) {
                                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                                    DISPLAY_MODE_OPTION = (DISPLAY_MODE_OPTION+1)%3;
                                    ini["Graphics"]["DisplayMode"] = DISPLAY_MODE_OPTION;
                                    if (DISPLAY_MODE_OPTION == WINDOWED) {
                                        if (IsWindowFullscreen()) {
                                            ToggleFullscreen();
                                        }
                                        RESOLUTION_OPTION = OLD_RESOLUTION_OPTION;
                                        OLD_RESOLUTION_OPTION = RESOLUTION_OPTION;
                                        SetWindowPosition((GetMonitorWidth(0) - WINDOW_WIDTH)/2, (GetMonitorHeight(0) - WINDOW_HEIGHT)/2);
                                        SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                                    } else if (DISPLAY_MODE_OPTION == WINDOWED_FULLSCREEN) {
                                        if (IsWindowFullscreen()) {
                                            ToggleFullscreen();
                                        }
                                        SetWindowPosition(0, 30);
                                        SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                                        OLD_RESOLUTION_OPTION = RESOLUTION_OPTION;
                                        RESOLUTION_OPTION = RESOLUTION_OPTIONS.size()-1;
                                    } else if (DISPLAY_MODE_OPTION == FULLSCREEN) {
                                        if (!IsWindowFullscreen()) {
                                            OLD_RESOLUTION_OPTION = RESOLUTION_OPTION;
                                            RESOLUTION_OPTION = RESOLUTION_OPTIONS.size()-1;
                                            SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                                            ToggleFullscreen();
                                        }
                                    }
                                    SelectedDisplayModeText.SetText(to_string((DISPLAY_MODE_ENUM)DISPLAY_MODE_OPTION));
                                    UISystem::Reescale();
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                                } else if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                                    DISPLAY_MODE_OPTION = (DISPLAY_MODE_OPTION+2)%3;
                                    ini["Graphics"]["DisplayMode"] = DISPLAY_MODE_OPTION;
                                    if (DISPLAY_MODE_OPTION == WINDOWED) {
                                        if (IsWindowFullscreen()) {
                                            ToggleFullscreen();
                                        }
                                        RESOLUTION_OPTION = OLD_RESOLUTION_OPTION;
                                        OLD_RESOLUTION_OPTION = RESOLUTION_OPTION;
                                        SetWindowPosition((GetMonitorWidth(0) - WINDOW_WIDTH)/2, (GetMonitorHeight(0) - WINDOW_HEIGHT)/2);
                                        SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                                    } else if (DISPLAY_MODE_OPTION == WINDOWED_FULLSCREEN) {
                                        if (IsWindowFullscreen()) {
                                            ToggleFullscreen();
                                        }
                                        SetWindowPosition(0, 30);
                                        SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                                        OLD_RESOLUTION_OPTION = RESOLUTION_OPTION;
                                        RESOLUTION_OPTION = RESOLUTION_OPTIONS.size()-1;
                                    } else if (DISPLAY_MODE_OPTION == FULLSCREEN) {
                                        if (!IsWindowFullscreen()) {
                                            OLD_RESOLUTION_OPTION = RESOLUTION_OPTION;
                                            RESOLUTION_OPTION = RESOLUTION_OPTIONS.size()-1;
                                            SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                                            ToggleFullscreen();
                                        }
                                    }
                                    SelectedDisplayModeText.SetText(to_string((DISPLAY_MODE_ENUM)DISPLAY_MODE_OPTION));
                                    UISystem::Reescale();
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                                }
                            } else if (OPTION == 2) {
                                if (DISPLAY_MODE_OPTION == WINDOWED) {
                                    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                                        RESOLUTION_OPTION = (RESOLUTION_OPTION+1)%RESOLUTION_OPTIONS.size();
                                        ini["Graphics"]["ScreenWidth"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].first;
                                        ini["Graphics"]["ScreenHeight"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].second;
                                        SetWindowPosition((GetMonitorWidth(0) - WINDOW_WIDTH)/2, (GetMonitorHeight(0) - WINDOW_HEIGHT)/2 + 30);
                                        SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                                        UISystem::Reescale();
                                        SelectedResolutionText.SetText(std::to_string(std::get<int>(ini["Graphics"]["ScreenWidth"])) + "x" + std::to_string(std::get<int>(ini["Graphics"]["ScreenHeight"])));
                                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                                    } else if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                                        RESOLUTION_OPTION = (RESOLUTION_OPTION+RESOLUTION_OPTIONS.size()-1)%RESOLUTION_OPTIONS.size();
                                        ini["Graphics"]["ScreenWidth"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].first;
                                        ini["Graphics"]["ScreenHeight"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].second;
                                        SetWindowPosition((GetMonitorWidth(0) - WINDOW_WIDTH)/2, (GetMonitorHeight(0) - WINDOW_HEIGHT)/2 + 30);
                                        SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                                        UISystem::Reescale();
                                        SelectedResolutionText.SetText(std::to_string(std::get<int>(ini["Graphics"]["ScreenWidth"])) + "x" + std::to_string(std::get<int>(ini["Graphics"]["ScreenHeight"])));
                                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                                    }
                                }
                            } else if (OPTION == 3) {
                                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                                    FPS_LIMIT_OPTION = ((FPS_LIMIT_OPTION+1)%FPS_LIMIT_OPTIONS.size());
                                    ini["Graphics"]["FPSLimit"] = FPS_LIMIT_OPTIONS[FPS_LIMIT_OPTION];
                                    SetTargetFPS(std::get<int>(ini["Graphics"]["FPSLimit"]));
                                    SelectedFPSLimitText.SetText(std::to_string(std::get<int>(ini["Graphics"]["FPSLimit"])));
                                } else if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                                    FPS_LIMIT_OPTION = ((FPS_LIMIT_OPTION + FPS_LIMIT_OPTIONS.size() - 1) % FPS_LIMIT_OPTIONS.size());
                                    ini["Graphics"]["FPSLimit"] = FPS_LIMIT_OPTIONS[FPS_LIMIT_OPTION];
                                    SetTargetFPS(std::get<int>(ini["Graphics"]["FPSLimit"]));
                                    SelectedFPSLimitText.SetText(std::to_string(std::get<int>(ini["Graphics"]["FPSLimit"])));
                                }
                            } else if (OPTION == 4) {
                                if (IsKeyDown(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                                    if (std::get<float>(ini["Audio"]["Volume"]) < 1) {
                                        ini["Audio"]["Volume"] = std::get<float>(ini["Audio"]["Volume"]) + 0.05f;
                                    } else {
                                        ini["Audio"]["Volume"] = 1.0f;
                                    }
                                } else if (IsKeyDown(KEY_LEFT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                                    if (std::get<float>(ini["Audio"]["Volume"]) > 0) {
                                        ini["Audio"]["Volume"] = std::get<float>(ini["Audio"]["Volume"]) - 0.05f;
                                    } else {
                                        ini["Audio"]["Volume"] = 0.0f;
                                    }
                                }
                                SetMasterVolume(std::get<float>(ini["Audio"]["Volume"]));
                                VolumePercentageText.SetText(std::to_string((int)(std::get<float>(ini["Audio"]["Volume"]) * 100)));
                            } else if (OPTION == 5) {
                                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                                    CURRENT_MENU = CONTROL_SETTINGS;
                                    OPTION = 0;
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y});
                                }
                            } else if (OPTION == 6) {
                                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                                    CURRENT_MENU = MAIN_MENU;
                                    OPTION = 1;
                                    OPTIONS = 3;
                                    HV = 0;
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                                }
                            }
                        }

                        // - OLD STYLE OPTION:
                        OldStyleText.Draw();
                        DrawRectangleV({ResolutionLArrow.dst.x + (100*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF), OldStyleText.pos.y+(2*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {20*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 20*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, WHITE);
                        DrawRectangleV({ResolutionLArrow.dst.x + (102*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF), OldStyleText.pos.y+(4*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {15*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 15*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, BLACK);
                        if (std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                            OldStyleCheck.Draw();
                        }
                        // - DISPLAY MODE OPTION: 0, 1, 2. Look up for the options explanation.
                        DisplayModeText.Draw();
                        DisplayModeLArrow.Draw();
                        SelectedDisplayModeText.Draw(JELLYFISH);
                        DisplayModeRArrow.Draw();

                        // - RESOLUTION. 640x480, 800x600, 900x600, 1024x768, 1280x720, 1920x1080:
                        ResolutionText.Draw(DISPLAY_MODE_OPTION == WINDOWED ? WHITE : GRAY);
                        ResolutionLArrow.Draw();
                        SelectedResolutionText.Draw(DISPLAY_MODE_OPTION == WINDOWED ? JELLYFISH : JELLYBEANBLUE);
                        ResolutionRArrow.Draw();

                        // - FPS LIMIT
                        FPSLimitText.Draw();
                        FPSLimitLArrow.Draw();
                        SelectedFPSLimitText.Draw(JELLYFISH);
                        FPSLimitRArrow.Draw();

                        // - VOLUME 
                        VolumeText.Draw();
                        DrawRectangleV({ResolutionLArrow.dst.x, VolumeText.pos.y + (15*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {150*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF,5*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF}, GRAY);
                        DrawRectangleV({ResolutionLArrow.dst.x, VolumeText.pos.y + (15*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {std::get<float>(ini["Audio"]["Volume"])*150*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF,5*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF}, JELLYFISH);
                        VolumePercentageText.Draw(JELLYFISH);

                        // - KEY BINDINGS
                        ChangeKeybindingsText.Draw(YELLOW);
                        SettingsReturnText.Draw(BLUE);

                    } else if (CURRENT_MENU == CONTROL_SETTINGS) {

                        if (keyboard) {
                            ChangeOptionText.Draw();
                            RightKeySettings.Draw();
                            LeftKeySettings.Draw();

                            MoveTextSettings.Draw();
                            UpKeySettings.Draw();
                            DownKeySettings.Draw();

                            ReturnTextSettings.Draw();
                            EscKeySettings.Draw();
                            ReturnKeySettings.Draw();
                        } else {
                            DpadOptionText.Draw();
                            DpadRightSettings.Draw();
                            DpadLeftSettings.Draw();

                            DpadMoveTextSettings.Draw();
                            DpadUpSettings.Draw();
                            DpadDownSettings.Draw();

                            OTextSettings.Draw();
                            OButtonSettings.Draw();
                        }


                        // CURRENT ACTION
                        Controller::Control currAction = Controller::NO_CONTROL;
                        switch (OPTION) {
                            case 2: currAction = Controller::LEFT;      break;
                            case 3: currAction = Controller::RIGHT;     break;
                            case 4: currAction = Controller::DOWN;      break;
                            case 5: currAction = Controller::JUMP;      break;
                            case 6: currAction = Controller::ATTACK;    break;
                        }
                        if (OPTION == 0) {
                            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                                PLAYER = mod(PLAYER+1, 4);
                                SelectedPlayer.SetText(std::to_string(PLAYER+1));
                                CONTROLLER = controllers[PLAYER]->type;
                                SelectedController.SetText(to_string((Controller::Type)CONTROLLER));
                                LeftActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::LEFT));
                                RightActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::RIGHT));
                                DownActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::DOWN));
                                UpActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::JUMP));
                                AttackActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::ATTACK));
                            } else if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                                PLAYER = mod(PLAYER-1, 4);
                                SelectedPlayer.SetText(std::to_string(PLAYER+1));
                                CONTROLLER = controllers[PLAYER]->type;
                                SelectedController.SetText(to_string((Controller::Type)CONTROLLER));
                                LeftActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::LEFT));
                                RightActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::RIGHT));
                                DownActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::DOWN));
                                UpActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::JUMP));
                                AttackActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::ATTACK));
                            }
                        } else if (OPTION == 1) {
                            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                                CONTROLLER = mod(CONTROLLER+1,5);
                                SelectedController.SetText(to_string((Controller::Type)CONTROLLER));
                                controllers[PLAYER]->type = (Controller::Type)CONTROLLER;
                                LeftActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::LEFT));
                                RightActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::RIGHT));
                                DownActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::DOWN));
                                UpActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::JUMP));
                                AttackActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::ATTACK));
                            } else if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                                CONTROLLER = mod(CONTROLLER-1,5);
                                SelectedController.SetText(to_string((Controller::Type)CONTROLLER));
                                controllers[PLAYER]->type = (Controller::Type)CONTROLLER;
                                LeftActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::LEFT));
                                RightActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::RIGHT));
                                DownActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::DOWN));
                                UpActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::JUMP));
                                AttackActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::ATTACK));
                            }
                        } else {
                            if (!SELECTED) {
                                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                                    SELECTED = true;
                                    BLOCK    = true;
                                }
                            } else {
                                int binding = 0;
                                int axisOffset = 0;
                                //Keyboard
                                if ((controllers[PLAYER]->type == Controller::KEYBOARD)) {
                                    binding = GetKeyPressed();
                                }
                                //Gamepad Trigger
                                else if (GetGamepadAxisMovement(CONTROLLER, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.5) {
                                    binding = GAMEPAD_EJE_GAT_IZQ;  axisOffset = (currAction < 4) ? ((currAction%2 == 0) ? 1 : -1) : 0;
                                } else if (GetGamepadAxisMovement(CONTROLLER, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.5) { 
                                    binding = GAMEPAD_EJE_GAT_DER; axisOffset = (currAction < 4) ? ((currAction%2 == 0) ? 1 : -1) : 0;
                                }
                                //Gamepad Axis
                                else if (std::fabs(GetGamepadAxisMovement(CONTROLLER, GAMEPAD_AXIS_LEFT_X)) > 0.5) {
                                    binding = GAMEPAD_EJE_IZQ_X;  axisOffset = (currAction < 4) ? ((currAction%2 == 0) ? 1 : -1) : 0;
                                } else if (std::fabs(GetGamepadAxisMovement(CONTROLLER, GAMEPAD_AXIS_LEFT_Y)) > 0.5) {
                                    binding = GAMEPAD_EJE_IZQ_Y;  axisOffset = (currAction < 4) ? ((currAction%2 == 0) ? 1 : -1) : 0;
                                } else if (std::fabs(GetGamepadAxisMovement(CONTROLLER, GAMEPAD_AXIS_RIGHT_X)) > 0.5) {
                                    binding = GAMEPAD_EJE_DER_X; axisOffset = (currAction < 4) ? ((currAction%2 == 0) ? 1 : -1) : 0;
                                } else if (std::fabs(GetGamepadAxisMovement(CONTROLLER, GAMEPAD_AXIS_RIGHT_Y)) > 0.5) {
                                    binding = GAMEPAD_EJE_DER_Y; axisOffset = (currAction < 4) ? ((currAction%2 == 0) ? 1 : -1) : 0;
                                } else {
                                    //Gamepad Button
                                    binding = GetGamepadButtonPressed();
                                }

                                bool validKey = (controllers[PLAYER]->type == Controller::KEYBOARD) ? validKeys[binding] : binding;
                                auto conflict = isBindingConflicted(PLAYER, currAction, binding);

                                if (conflict.first != -1) {
                                    BindingErrorText.SetText("Binding conflict with (PLAYER: " + std::to_string(conflict.first) + ", CONTROL: " + to_string(conflict.second) + ")");
                                    SHOW_ERROR_MESSAGE = true;
                                } else if (validKey) {
                                    if (CONTROLLER == Controller::KEYBOARD) {
                                        controllers[PLAYER]->kb_controls[(Controller::Control)currAction] = binding;
                                    } else {
                                        controllers[PLAYER]->gp_controls[(Controller::Control)currAction] = binding;
                                    }
                                    if (axisOffset != 0) {
                                        controllers[PLAYER]->gp_controls[(Controller::Control)(currAction + axisOffset)] = binding;
                                    }
                                    save_controls(PLAYER);
                                    SELECTED = false;
                                    BLOCK    = false;
                                    SHOW_ERROR_MESSAGE = false;
                                }
                                LeftActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::LEFT));
                                RightActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::RIGHT));
                                DownActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::DOWN));
                                UpActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::JUMP));
                                AttackActionKeybind.SetText(controllers[PLAYER]->getActionBind(Controller::ATTACK));
                            }
                        }

                        // Error Message:
                        if (SHOW_ERROR_MESSAGE) {
                            BindingErrorText.Draw(RED);
                        }
                        // Player:
                        PlayerText.Draw();
                        PlayerLArrow.Draw();
                        SelectedPlayer.Draw();
                        PlayerRArrow.Draw();
                        // Controller type:
                        ControllerText.Draw();
                        ControllerLArrow.Draw();
                        SelectedController.Draw();
                        ControllerRArrow.Draw();
                        // Move Left:
                        if (SELECTED && currAction == 0) {
                            LeftActionText.Draw(RED);
                        } else {
                            LeftActionText.Draw(GREEN);
                            LeftActionKeybind.Draw(JELLYFISH);
                        }
                        // Move Right:
                        if (SELECTED && currAction == 1) {
                            RightActionText.Draw(RED);
                        } else {
                            RightActionText.Draw(GREEN);
                            RightActionKeybind.Draw(JELLYFISH);
                        }
                        // Crouch:
                        if (SELECTED && currAction == 2) {
                            DownActionText.Draw(RED);
                        } else {
                            DownActionText.Draw(GREEN);
                            DownActionKeybind.Draw(JELLYFISH);
                        }
                        // Jump:
                        if (SELECTED && currAction == 3) {
                            UpActionText.Draw(RED);
                        } else {
                            UpActionText.Draw(GREEN);
                            UpActionKeybind.Draw(JELLYFISH);
                        }
                        // Attack:
                        if (SELECTED && currAction == 4) {
                            AttackActionText.Draw(RED);
                        } else {
                            AttackActionText.Draw(GREEN);
                            AttackActionKeybind.Draw(JELLYFISH);
                        }

                        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            if (!SELECTED) {
                                if (OLD_MENU == SELECT_LEVEL) {
                                    CURRENT_MENU = SELECT_LEVEL;
                                    OPTION = 3;
                                    OPTIONS = 4;
                                    HV = 3;
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF}); 
                                } else {
                                    CURRENT_MENU = SETTINGS;
                                    OPTION = 5;
                                    OPTIONS = 7;
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF}); 
                                }
                            } else {
                                SELECTED = false;
                                BLOCK = false;
                                SHOW_ERROR_MESSAGE = false;
                            }
                        }
                    }
                }
            }
            // Hammer movement:
            if (!BLOCK) {
                Hammer.Draw();
                if (IsKeyPressed(KEY_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
                    OPTION = mod(OPTION+1, OPTIONS);
                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                }
                if (IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
                    OPTION = mod(OPTION-1, OPTIONS);
                    Hammer.Translate({Hammer.dst.x, Hammer.dst.y - 20});
                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + (OPTIONS - (OPTIONS-OPTION))*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                }
            }
        }

        if (IsKeyPressed(KEY_TWO)) {
            RESOLUTION_OPTION = (RESOLUTION_OPTION+1)%RESOLUTION_OPTIONS.size();
            SetWindowSize(RESOLUTION_OPTIONS[RESOLUTION_OPTION].first, RESOLUTION_OPTIONS[RESOLUTION_OPTION].second);
            UISystem::Reescale();
            Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});

        } else if (IsKeyPressed(KEY_ONE)) {
            RESOLUTION_OPTION = (RESOLUTION_OPTION+RESOLUTION_OPTIONS.size()-1)%RESOLUTION_OPTIONS.size();
            SetWindowSize(RESOLUTION_OPTIONS[RESOLUTION_OPTION].first, RESOLUTION_OPTIONS[RESOLUTION_OPTION].second);
            UISystem::Reescale();
            Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
        }
        DrawText((std::to_string(RESOLUTION_OPTIONS[RESOLUTION_OPTION].first) + "x" + std::to_string(RESOLUTION_OPTIONS[RESOLUTION_OPTION].second)).c_str(), 5, 5, 30, BLACK);

        EndDrawing();

    }
	save_config();
    UnloadMusicStream(MainTitleOST);
    UnloadFont(NES);
    UISystem::RemoveAll();
}
