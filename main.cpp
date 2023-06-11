#include <sstream>

#include "Dengine.h"
#include "DengineUI.h"
#include "raylibx.h"
#include "settings.h"

#include "Cloud.h"
#include "Enemies.h"
#include "Fruit.h"
#include "Player.h"

Font NES;

void Game(int numPlayers, int level, int run_time) {

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
    int pts1 = 0, pts2 = 0;

    MusicSource BGM("Assets/Sounds/03-Play-BGM.mp3", true);
    MusicSource BGMBonus("Assets/Sounds/01-Main-Title.mp3", true);
    MusicSource BGM2("Assets/Sounds/Mick Gordon - The Only Thing They Fear Is You.mp3", true);
    //MusicSource BGM3("Assets/NES - Ice Climber - Sound Effects/Go Go Go - Nightcore.mp3", true);
    SoundSource PauseSound("Assets/Sounds/08-Pause.wav");
    SoundSource Fly("Assets/Sounds/17-Fly.wav");

    auto BackGrounds = std::vector<Canvas>{
        Canvas("Assets/Sprites/00_Mountain.png", {0.0f, GAME_HEIGHT - GAME_WIDTH*2.3f}, {GAME_WIDTH, GAME_WIDTH*2.3f}),
        Canvas("Assets/Sprites/01_Mountain.png", {0.0f, GAME_HEIGHT - GAME_WIDTH*2.3f}, {GAME_WIDTH, GAME_WIDTH*2.3f}),
        Canvas("Assets/Sprites/02_Brawl.png", {0.0f, GAME_HEIGHT - GAME_WIDTH}, {GAME_WIDTH, GAME_HEIGHT})
    };

    Canvas LifePopo1("Assets/Sprites/Popo/Life.png", {GetScreenWidth() - 150.0f, 16.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifePopo2("Assets/Sprites/Popo/Life.png", {GetScreenWidth() - 120.0f, 16.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifePopo3("Assets/Sprites/Popo/Life.png", {GetScreenWidth() -  90.0f, 16.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeNana1("Assets/Sprites/Nana/Life.png", {GetScreenWidth() - 150.0f, 25.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeNana2("Assets/Sprites/Nana/Life.png", {GetScreenWidth() - 120.0f, 25.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});
    Canvas LifeNana3("Assets/Sprites/Nana/Life.png", {GetScreenWidth() -  90.0f, 25.0f * vertical_scale}, {8.0f * horizontal_scale, 8.0f * vertical_scale});

    // Suelo base
    GameObject BaseFloor("Base Floor", "Floor");
    BaseFloor.addComponent<Collider2D>(&BaseFloor.getComponent<Transform2D>().position, Vector2{GAME_WIDTH * 1.5f, 8.0f*vertical_scale*2.0f}, PINK);
    BaseFloor.addComponent<Script, FloorBehavior>();

    GameObject Stalactite("Stalactite", "Stalactite", {"Stalactite"}, {"Floor", "SlidingFloor", "Player", "Wall", "Cloud"});
    Stalactite.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Fall", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/15-Icicle-Falling.wav"))},
    });
    // Bloque de hierba
    GameObject GrassBlock("Grass Block", "Floor", {"Block"});
    GrassBlock.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Break", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/04-Block_Break.wav"))},
    });
    GameObject GrassBlockThin("Thin Grass Block", "Floor", {"Block"});
    GrassBlockThin.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Break", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/04-Block_Break.wav"))},
    });
    GameObject GrassHole("Grass Hole", "Hole");

    // Me llevo arriba la definición del sprite para tener a mano las variables de abajo
    GrassBlock.addComponent<Sprite>("Assets/Sprites/Blocks/Grass_block_large.png", horizontal_scale, vertical_scale);
    float block_width = GrassBlock.getComponent<Sprite>().GetViewDimensions().x;
    float block_height = GrassBlock.getComponent<Sprite>().GetViewDimensions().y;
    float collider_width  = block_width-5.0f;
    float collider_offset = (collider_width)/2;

    // Estalactitas
    Stalactite.addComponent<Animator>("NONE", std::unordered_map<std::string, Animation> {
            {"NONE", Animation()},
            {"SMALL", Animation("Assets/Sprites/Stalactite/01_Small.png", 6, 3, scale, 0.5, true)},
            {"MEDIUM", Animation("Assets/Sprites/Stalactite/02_Medium.png", 6, 8, scale, 0.5, true)},
            {"BIG", Animation("Assets/Sprites/Stalactite/03_Big.png", 8, 12, scale, 0.5, true)},
            {"FALLING", Animation("Assets/Sprites/Stalactite/03_Big.png", 8, 12, scale, 0, true)},
            {"BREAKING", Animation("Assets/Sprites/Stalactite/04_Breaking.png", 8, 12, scale, 0, true)},
            {"BROKEN", Animation("Assets/Sprites/Stalactite/05_Broken.png", 8, 15, scale, 0.5, true)}
        }
    );
    Stalactite.addComponent<RigidBody2D>(1, block_height * 50.0f, Vector2{0.0f,block_height * 70.0f}, Vector2{block_width * 3.0f, 0});
    Vector2 stalactite_size = { 8.0f * horizontal_scale, 8.0f * vertical_scale };
    Stalactite.addComponent<Collider2D>(&Stalactite.getComponent<Transform2D>().position, Vector2{collider_width, stalactite_size.y}, Vector2{stalactite_size.x/2 - collider_offset, 0});
    Stalactite.addComponent<Script, StalactiteBehavior>();

    // Cambiamos el template de las estalactitas para tener distintos ratios en función del nivel
    Stalactite.getComponent<Script, StalactiteBehavior>().spawning_ratio = (level+1) * 0.002;

    GrassBlock.addComponent<Collider2D>(&GrassBlock.getComponent<Transform2D>().position, GrassBlock.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    GrassBlock.addComponent<Script, BlockBehavior>();
    GrassBlock.getComponent<Script, BlockBehavior>().hole = &GrassHole;
    GrassBlock.getComponent<Script, BlockBehavior>().stalactiteTemplate = &Stalactite;

    GrassBlockThin.addComponent<Sprite>("Assets/Sprites/Blocks/Grass_block_thin.png", horizontal_scale, vertical_scale);
    GrassBlockThin.addComponent<Collider2D>(&GrassBlockThin.getComponent<Transform2D>().position, GrassBlockThin.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    GrassBlockThin.addComponent<Script, BlockBehavior>();
    GrassBlockThin.getComponent<Script, BlockBehavior>().hole = &GrassHole;
    GrassBlockThin.getComponent<Script, BlockBehavior>().stalactiteTemplate = &Stalactite;

    GrassHole.addComponent<Collider2D>(&GrassHole.getComponent<Transform2D>().position, Vector2{block_width, block_height}, RED);
    GrassHole.addComponent<Script, HoleBehavior>();
    GrassHole.getComponent<Script, HoleBehavior>().original_block = &GrassBlock;

    // ==== Bloque de tierra ====
    GameObject DirtBlock("Dirt Block", "Floor", {"Block"});
    DirtBlock.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Break", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/04-Block_Break.wav"))},
    });
    GameObject DirtBlockThin("Thin Dirt Block", "Floor", {"Block"});
    DirtBlockThin.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Break", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/04-Block_Break.wav"))},
    });
    GameObject DirtHole("Dirt Hole", "Hole");

    DirtBlock.addComponent<Sprite>("Assets/Sprites/Blocks/Dirt_block_large.png", horizontal_scale, vertical_scale);
    DirtBlock.addComponent<Collider2D>(&DirtBlock.getComponent<Transform2D>().position, DirtBlock.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    DirtBlock.addComponent<Script, BlockBehavior>();
    DirtBlock.getComponent<Script, BlockBehavior>().hole = &DirtHole;
    DirtBlock.getComponent<Script, BlockBehavior>().stalactiteTemplate = &Stalactite;

    DirtBlockThin.addComponent<Sprite>("Assets/Sprites/Blocks/Dirt_block_thin.png", horizontal_scale, vertical_scale);
    DirtBlockThin.addComponent<Collider2D>(&DirtBlockThin.getComponent<Transform2D>().position, DirtBlockThin.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    DirtBlockThin.addComponent<Script, BlockBehavior>();
    DirtBlockThin.getComponent<Script, BlockBehavior>().hole = &DirtHole;
    DirtBlockThin.getComponent<Script, BlockBehavior>().stalactiteTemplate = &Stalactite;

    DirtHole.addComponent<Collider2D>(&DirtHole.getComponent<Transform2D>().position, Vector2{block_width, block_height}, RED);
    DirtHole.addComponent<Script, HoleBehavior>();
    DirtHole.getComponent<Script, HoleBehavior>().original_block = &DirtBlock;

    // Bloque de hielo
    GameObject IceBlock("Ice Block", "Floor", {"Ice","Block"});
    IceBlock.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Break", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/04-Block_Break.wav"))},
    });
    GameObject IceBlockThin("Thin Ice Block", "Floor", {"Ice","Block"});
    IceBlockThin.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Break", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/04-Block_Break.wav"))},
    });
    GameObject IceSlidingBlock("Ice Sliding Block", "SlidingFloor", {"Ice","Block"});
    IceSlidingBlock.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Break", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/04-Block_Break.wav"))},
    });
    GameObject IceHole("Ice Hole", "Hole");
    GameObject IceSlidingHole("Ice Sliding Hole", "Hole");

    IceBlock.addComponent<Sprite>("Assets/Sprites/Blocks/Ice_block_large.png", horizontal_scale, vertical_scale);
    IceBlock.addComponent<Collider2D>(&IceBlock.getComponent<Transform2D>().position, IceBlock.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    IceBlock.addComponent<Script, BlockBehavior>();
    IceBlock.getComponent<Script, BlockBehavior>().hole = &IceHole;
    IceBlock.getComponent<Script, BlockBehavior>().stalactiteTemplate = &Stalactite;

    IceBlockThin.addComponent<Sprite>("Assets/Sprites/Blocks/Ice_block_thin.png", horizontal_scale, vertical_scale);
    IceBlockThin.addComponent<Collider2D>(&IceBlockThin.getComponent<Transform2D>().position, IceBlockThin.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    IceBlockThin.addComponent<Script, BlockBehavior>();
    IceBlockThin.getComponent<Script, BlockBehavior>().hole = &IceHole;
    IceBlockThin.getComponent<Script, BlockBehavior>().stalactiteTemplate = &Stalactite;

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

    float LevelFloor_0_width = block_width * 9.0f,
        LevelFloor_1_width = block_width * 6.0f,
        LevelFloor_2_width = block_width * 4.0f,
        LevelFloor_3_width = block_width * 3.0f,
        LevelFloor_4_width = block_width * 7.0f;

    LevelFloor_0.addComponent<Collider2D>(&LevelFloor_0.getComponent<Transform2D>().position, Vector2{LevelFloor_0_width, block_height}, PINK);
    LevelFloor_0.addComponent<Script, FloorBehavior>();

    LevelFloor_0_Ice.addComponent<Collider2D>(&LevelFloor_0_Ice.getComponent<Transform2D>().position, Vector2{LevelFloor_0_width, block_height}, PINK);
    LevelFloor_0_Ice.addComponent<Script, FloorBehavior>();

    LevelFloor_1.addComponent<Collider2D>(&LevelFloor_1.getComponent<Transform2D>().position, Vector2{LevelFloor_1_width, block_height}, PINK);
    LevelFloor_1.addComponent<Script, FloorBehavior>();

    LevelFloor_2.addComponent<Collider2D>(&LevelFloor_2.getComponent<Transform2D>().position, Vector2{LevelFloor_2_width, block_height}, PINK);
    LevelFloor_2.addComponent<Script, FloorBehavior>();

    LevelFloor_3.addComponent<Collider2D>(&LevelFloor_3.getComponent<Transform2D>().position, Vector2{LevelFloor_3_width, block_height}, PINK);
    LevelFloor_3.addComponent<Script, FloorBehavior>();

    LevelFloor_4.addComponent<Collider2D>(&LevelFloor_4.getComponent<Transform2D>().position, Vector2{LevelFloor_4_width, block_height}, PINK);
    LevelFloor_4.addComponent<Script, FloorBehavior>();

    BonusLevelFloor_0.addComponent<Collider2D>(&BonusLevelFloor_0.getComponent<Transform2D>().position, Vector2{LevelFloor_2_width, block_height}, PINK);
    BonusLevelFloor_0.addComponent<Script, FloorBehavior>();

    BonusLevelFloor_1.addComponent<Collider2D>(&BonusLevelFloor_1.getComponent<Transform2D>().position, Vector2{LevelFloor_1_width, block_height}, PINK);
    BonusLevelFloor_1.addComponent<Script, FloorBehavior>();

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
    GameObject Popo("Popo", "Player", {}, {"Floor", "Wall", "Cloud", "Enemy", "Goal", "Hole", "Fruit", "SlidingFloor", "Stalactite"});
    GameObject Nana("Nana", "Player", {}, {"Floor", "Wall", "Cloud", "Enemy", "Goal", "Hole", "Fruit", "SlidingFloor", "Stalactite"});
    GameObject Amam("Amam", "Player", {}, {"Floor", "Wall", "Cloud", "Enemy", "Goal", "Hole", "Fruit", "SlidingFloor", "Stalactite"});
    GameObject Lili("Lili", "Player", {}, {"Floor", "Wall", "Cloud", "Enemy", "Goal", "Hole", "Fruit", "SlidingFloor", "Stalactite"});
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
        {"IdleBonus", Animation("Assets/Sprites/Popo/00_Idle_Bonus.png", 16, 24, scale, 0.75, true)},
        {"Walk", Animation("Assets/Sprites/Popo/01_Walk.png", 16, 24, scale, 0.135, true)},
        {"WalkBonus", Animation("Assets/Sprites/Popo/01_Walk_Bonus.png", 16, 24, scale, 0.135, true)},
        {"Brake", Animation("Assets/Sprites/Popo/02_Brake.png", 16, 24, scale, 0.3, true)},
        {"BrakeBonus", Animation("Assets/Sprites/Popo/02_Brake_Bonus.png", 16, 24, scale, 0.3, true)},
        {"Jump", Animation("Assets/Sprites/Popo/03_Jump.png", 20, 25, scale, 0.5, false)},
        {"JumpBonus", Animation("Assets/Sprites/Popo/03_Jump_Bonus.png", 20, 25, scale, 0.5, false)},
        {"Attack", Animation("Assets/Sprites/Popo/04_Attack.png", 21, 25, scale, 0.3, false)},
        {"AttackBonus", Animation("Assets/Sprites/Popo/04_Attack_Bonus.png", 21, 25, scale, 0.3, false)},
        {"Stunned", Animation("Assets/Sprites/Popo/05_Stunned.png", 16, 21, scale, 0.5, true)},
        {"StunnedBonus", Animation("Assets/Sprites/Popo/05_Stunned.png", 16, 24, scale, 0.5, true)},
        {"Fall", Animation("Assets/Sprites/Popo/06_Fall.png", 21, 25, scale, 0.3, false)},
        {"FallBonus", Animation("Assets/Sprites/Popo/06_Fall_Bonus.png", 21, 25, scale, 0.3, false)},
        {"Crouch", Animation("Assets/Sprites/Popo/07_Crouch.png", 16, 24, scale, 0.75, true)},
        {"CrouchBonus", Animation("Assets/Sprites/Popo/07_Crouch_Bonus.png", 16, 24, scale, 0.75, true)},
    });

    Nana.addComponent<Animator>("Idle", std::unordered_map<std::string, Animation> {
        {"Idle", Animation("Assets/Sprites/Nana/00_Idle.png", 16, 24, scale, 0.75, true)},
        {"IdleBonus", Animation("Assets/Sprites/Nana/00_Idle_Bonus.png", 16, 24, scale, 0.75, true)},
        {"Walk", Animation("Assets/Sprites/Nana/01_Walk.png", 16, 24, scale, 0.135, true)},
        {"WalkBonus", Animation("Assets/Sprites/Nana/01_Walk_Bonus.png", 16, 24, scale, 0.135, true)},
        {"Brake", Animation("Assets/Sprites/Nana/02_Brake.png", 16, 24, scale, 0.3, true)},
        {"BrakeBonus", Animation("Assets/Sprites/Nana/02_Brake_Bonus.png", 16, 24, scale, 0.3, true)},
        {"Jump", Animation("Assets/Sprites/Nana/03_Jump.png", 20, 25, scale, 0.5, false)},
        {"JumpBonus", Animation("Assets/Sprites/Nana/03_Jump_Bonus.png", 20, 25, scale, 0.5, false)},
        {"Attack", Animation("Assets/Sprites/Nana/04_Attack.png", 21, 25, scale, 0.3, false)},
        {"AttackBonus", Animation("Assets/Sprites/Nana/04_Attack_Bonus.png", 21, 25, scale, 0.3, false)},
        {"Stunned", Animation("Assets/Sprites/Nana/05_Stunned.png", 16, 21, scale, 0.5, true)},
        {"StunnedBonus", Animation("Assets/Sprites/Nana/05_Stunned.png", 16, 24, scale, 0.5, true)},
        {"Fall", Animation("Assets/Sprites/Nana/06_Fall.png", 21, 25, scale, 0.3, false)},
        {"FallBonus", Animation("Assets/Sprites/Nana/06_Fall_Bonus.png", 21, 25, scale, 0.3, false)},
        {"Crouch", Animation("Assets/Sprites/Nana/07_Crouch.png", 16, 24, scale, 0.75, true)},
        {"CrouchBonus", Animation("Assets/Sprites/Nana/07_Crouch_Bonus.png", 16, 24, scale, 0.75, true)},
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
        {"Victory", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/04-Victory.mp3"))},
        {"Hit", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/10-Player-Hit.wav"))}
    });
    Nana.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/09-Jump.wav"))},
        {"Victory", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/04-Victory.mp3"))},
        {"Hit", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/10-Player-Hit.wav"))}
    });
    Amam.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/09-Jump.wav"))},
    });
    Lili.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/09-Jump.wav"))},
    });
    // 4. Añadimos el Rigidbody:
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
    Popo.addComponent<Script, Player>(Controller_0);
    Nana.addComponent<Script, Player>(Controller_1);
    Amam.addComponent<Script, Player>(Controller_2);
    Lili.addComponent<Script, Player>(Controller_3);

    GameObject* Player_1 = nullptr;
    GameObject* Player_2 = nullptr;
    GameObject* Player_3 = nullptr;
    GameObject* Player_4 = nullptr;

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
    GameObject Condor("Red Condor", "Goal", {}, {"Player"});
    Condor.addComponent<Animator>("Fly", std::unordered_map<std::string, Animation> {
        {"Fly", Animation("Assets/Sprites/Red_condor_fly.png", 32, 16, scale, 1.0, true)}
    });
    Condor.addComponent<Collider2D>(&Condor.getComponent<Transform2D>().position, Condor.getComponent<Animator>().GetViewDimensions(), ORANGE);
    Condor.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{200, 0});
    Condor.addComponent<Script, RedCondorBehavior>();

    // Icicle
    GameObject Icicle("Icicle", "Icicle", {}, {"Hole", "Player", "Floor"});
    Icicle.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Rebuild", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/05-Rebuild.wav"))},
    });
    Icicle.addComponent<Sprite>("Assets/Sprites/Icicle.png", horizontal_scale, vertical_scale);
    Icicle.addComponent<Collider2D>(&Icicle.getComponent<Transform2D>().position, Icicle.getComponent<Sprite>().GetViewDimensions());
    Icicle.addComponent<RigidBody2D>(1, 98, Vector2{0,0}, Vector2{0,0});
    Icicle.addComponent<Script, IcicleBehavior>();

    // Frutas
    GameObject Eggplant("Fruit", "Fruit");
    Eggplant.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Pick", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/03-Pick-Vegetable.wav"))},
    });
    Eggplant.addComponent<Sprite>("Assets/Sprites/Fruit_Eggplant.png", horizontal_scale, vertical_scale);
    Eggplant.addComponent<Collider2D>(&Eggplant.getComponent<Transform2D>().position, Eggplant.getComponent<Sprite>().GetViewDimensions(), ORANGE);
    Eggplant.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{100, 0});
    Eggplant.addComponent<Script, FruitBehavior>();
    float eggplant_height = Eggplant.getComponent<Sprite>().GetViewDimensions().y;

    GameObject Lettuce("Fruit", "Fruit");
    Lettuce.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Pick", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/03-Pick-Vegetable.wav"))},
    });
    Lettuce.addComponent<Sprite>("Assets/Sprites/Fruit_Lettuce.png", horizontal_scale, vertical_scale);
    Lettuce.addComponent<Collider2D>(&Lettuce.getComponent<Transform2D>().position, Lettuce.getComponent<Sprite>().GetViewDimensions(), ORANGE);
    Lettuce.addComponent<RigidBody2D>(1, 0, Vector2{0,0}, Vector2{100, 0});
    Lettuce.addComponent<Script, FruitBehavior>();
    float lettuce_height = Lettuce.getComponent<Sprite>().GetViewDimensions().y;

    // Enemies
    GameObject Topi("Topi", "Enemy", {"Topi"}, {"Floor", "SlidingFloor", "Hole", "Player", "Icicle", "Wall"});
    Topi.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Hit", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/02-Topi-Hit.wav"))},
    });
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

    GameObject Nutpicker("Nutpicker", "Enemy", {}, {"Player", "Floor", "Wall"});
    Nutpicker.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Hit", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/14-Nutpicker-Hit.wav"))},
    });
    Nutpicker.addComponent<Animator>("Idle", std::unordered_map<std::string, Animation> {
            {"Idle", Animation("Assets/Sprites/Nutpicker_Idle.png", 16, 16, scale, 0.5, true)},
            {"Walk", Animation("Assets/Sprites/Nutpicker - Spritesheet 01 - Fly.png", 16, 16, scale, 0.3, true)},
            {"Stunned", Animation("Assets/Sprites/Nutpicker - Spritesheet 02 - Stunned.png", 16, 16, scale, 0.5, true)},
        }
    );
    Nutpicker.addComponent<RigidBody2D>(1, block_height * 23.0f, Vector2{0,0}, Vector2{block_width * 6.0f,0});
    Nutpicker.addComponent<Collider2D>(&Nutpicker.getComponent<Transform2D>().position, Vector2{collider_width, topi_size.y}, Vector2{topi_size.x/2 - collider_offset, 0});
    Nutpicker.addComponent<Script, NutpickerBehavior>(Icicle);

    std::vector<GameObject*> Enemies{};
    std::vector<GameObject*> Josephs{};
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
        // Piso 1 (Suelo base):
        auto floor = &GameSystem::Instantiate(BaseFloor, GameObjectOptions{.position{-100,floor_levels[0]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 1;

        // Piso 2:
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 4.0f - LevelFloor_0_width, floor_levels[1]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 2;
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*4.0f, floor_levels[1]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 2;

        // Piso 3:
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 5.0f - LevelFloor_0_width, floor_levels[2]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 3;
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*5.0f, floor_levels[2]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 3;

        // Piso 4:
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 5.0f - LevelFloor_0_width, floor_levels[3]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 4;
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*5.0f, floor_levels[3]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 4;

        // Piso 5:
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 5.0f - LevelFloor_0_width, floor_levels[4]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 5;
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*5.0f, floor_levels[4]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 5;

        // Piso 6:
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 6.0f - LevelFloor_0_width, floor_levels[5]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 6;
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*6.0f, floor_levels[5]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 6;

        // Piso 7:
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 6.0f - LevelFloor_0_width, floor_levels[6]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 7;
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*6.0f, floor_levels[6]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 7;

        // Piso 8:
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 6.0f - LevelFloor_0_width, floor_levels[7]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 8;
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{GAME_WIDTH - block_width*6.0f, floor_levels[7]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 8;

        // Piso 9:
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 25.0f, floor_levels[8]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 9;
        floor = &GameSystem::Instantiate(LevelFloor_0, GameObjectOptions{.position{block_width * 7.0f - LevelFloor_0_width, floor_levels[8]}});
        floor->getComponent<Script, FloorBehavior>().floor_level = 9;

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
        playerTransforms.push_back(&Player_1->getComponent<Script, Player>().transform);
        if (numPlayers > 1) {
            Player_2 = &GameSystem::Instantiate(Nana, GameObjectOptions{.position = {block_width * 13.0f, floor_levels[0] - player_size.y}});
            playerTransforms.push_back(&Player_2->getComponent<Script, Player>().transform);
        }
        if (numPlayers > 2) {
            Player_3 = &GameSystem::Instantiate(Amam, GameObjectOptions{.position = {block_width * 18.0f,floor_levels[0] - player_size.y}});
            playerTransforms.push_back(&Player_3->getComponent<Script, Player>().transform);
        }
        if (numPlayers > 3) {
            Player_4 = &GameSystem::Instantiate(Lili, GameObjectOptions{.position = {block_width * 23.0f,floor_levels[0] - player_size.y}});
            playerTransforms.push_back(&Player_4->getComponent<Script, Player>().transform);
        }

        // Zona de muerte y Condor en la cima de la montaña
        GameSystem::Instantiate(Death, GameObjectOptions{.position{0, GAME_HEIGHT - block_height * 95.0f}});
        GameSystem::Instantiate(Death, GameObjectOptions{.position{block_width * 26.0f, GAME_HEIGHT - block_height * 95.0f}});
        auto a_condor = &GameSystem::Instantiate(Condor, GameObjectOptions{.position={400, GAME_HEIGHT - block_height * 104.0f}});
        a_condor->getComponent<Script, RedCondorBehavior>().players = {Player_1, Player_2, Player_3, Player_4};

        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_real_distribution<> E(0, 1);

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

            //int f2_current_blocks = 24;
            for (float i = 0.0f; i < 24; i++) {
                auto block = &GameSystem::Instantiate(GrassBlock, GameObjectOptions{.position{block_width * (4.0f + i), floor_levels[1]}});
                block->getComponent<Script, BlockBehavior>().floor_level    = 2;
                //block->getComponent<Script, BlockBehavior>().current_blocks = &f2_current_blocks;
                if(i == 0 || i == 1 || i == 5 || i == 6 || (i > 11 && i < 21)) {
                    block = &GameSystem::Instantiate(GrassBlock, GameObjectOptions{.position{block_width * (4.0f + i), floor_levels[1] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level    = 2;
                }
                if(i == 2 || i == 4 || i == 7 || i == 11 || i == 21) {
                    block = &GameSystem::Instantiate(GrassBlockThin, GameObjectOptions{.position{block_width * (4.0f + i), floor_levels[1] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level    = 2;
                }
            }

            int f3_current_blocks = 22, f5_current_blocks = 22;
            for (int i = 0; i < 22; i++) {
                auto block = &GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2]}});
                block->getComponent<Script, BlockBehavior>().current_blocks = &f3_current_blocks;
                block->getComponent<Script, BlockBehavior>().floor_level = 3;
                if(i < 6 || (i > 8 && i < 12) || i > 18) {           
                    block = &GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 3;
                }
                if(i == 6 || i == 8 || i == 12 || i == 18) {          
                    block = &GameSystem::Instantiate(DirtBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 3;
                }
                block = &GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3]}});
                block->getComponent<Script, BlockBehavior>().floor_level = 4;
                if(i == 0 || (i > 3 && i < 7) || (i > 9 && i < 13) || (i > 15 && i < 19)) {            
                    block = &GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 4;
                }
                if(i == 1 || i == 3 || i == 7 || i == 9 || i == 13 || i == 15 || i == 19 ) {          
                    block = &GameSystem::Instantiate(DirtBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 4;
                }
                block = &GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[4]}});
                block->getComponent<Script, BlockBehavior>().floor_level = 5;
                block->getComponent<Script, BlockBehavior>().current_blocks = &f5_current_blocks;
                if(i < 12 || i == 15 || i > 18) {          
                    block = &GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[4] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 5;
                }
                if(i == 12 || i == 14 || i == 16 || i == 18) {        
                    block = &GameSystem::Instantiate(DirtBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[4] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 5;
                }
            }   

            int f7_current_blocks = 20;
            for (int i = 0; i < 20; i++) {
                auto block = &GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[5]}});
                block->getComponent<Script, BlockBehavior>().floor_level = 6;
                if(i == 0 || (i > 2 && i < 9) || i > 15) {           
                    block = &GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[5] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 6;
                    //block->getComponent<Script, BlockBehavior>().current_blocks = &f7_current_blocks;
                }
                if(i == 1 || i == 2 || i == 9 || i == 15) {        
                    block = &GameSystem::Instantiate(IceBlockThin, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[5] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 6;
                }

                block = &GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[6]}});
                block->getComponent<Script, BlockBehavior>().current_blocks = &f7_current_blocks;
                block->getComponent<Script, BlockBehavior>().floor_level = 7;
                if(i < 5 || (i > 9 && i < 13) || (i > 15 && i < 19)) {
                    block = &GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[6] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 7;
                }
                if(i == 5 || i == 9 || i == 13 || i == 15 || i == 19) {
                    block = &GameSystem::Instantiate(IceBlockThin, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[6] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 7;
                }

                block = &GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[7]}});
                block->getComponent<Script, BlockBehavior>().floor_level = 8;
                if((i > 2 && i < 5) || (i > 7 && i < 11) || (i > 13 && i < 16)) {
                    block = &GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[7] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 8;
                }
                if(i == 2 || i == 5 || i == 7 || i == 11 || i == 13 || i == 16) {
                    block = &GameSystem::Instantiate(IceBlockThin, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[7] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 8;
                }
            }

            auto a_floor = &GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 9.0f, floor_levels[8]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 9;

            a_floor = &GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 17.0f, floor_levels[8]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 9;

            bonusLevel = &GameSystem::Instantiate(BonusLevelFloor_0, GameObjectOptions{.position{block_width * 6.0f, bonus_floor_levels[0]}});
            bonusLevel->getComponent<Script, FloorBehavior>().floor_level = 10;
            a_floor = &GameSystem::Instantiate(BonusLevelFloor_1, GameObjectOptions{.position{block_width * 13.0f, bonus_floor_levels[0]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 10;
            a_floor = &GameSystem::Instantiate(BonusLevelFloor_0, GameObjectOptions{.position{block_width * 22.0f, bonus_floor_levels[0]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 10;

            a_floor = &GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 10.0f, bonus_floor_levels[2]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 11;

            a_floor = &GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 18.0f, bonus_floor_levels[3]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 12;

            a_floor = &GameSystem::Instantiate(LevelFloor_4, GameObjectOptions{.position{block_width * 5.0f, bonus_floor_levels[4]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 13;

            a_floor = &GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 21.0f, bonus_floor_levels[5]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 14;

            a_floor = &GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 12.0f, bonus_floor_levels[6]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 15;

            a_floor = &GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, bonus_floor_levels[7]}});
            a_floor->getComponent<Script, CloudBehavior>().floor_level = 16;

            a_floor = &GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 16.0f, bonus_floor_levels[8]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 17;

            a_floor = &GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 8.0f, bonus_floor_levels[9]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 18;

            a_floor = &GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 19.0f, bonus_floor_levels[10]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 19;

            a_floor = &GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 12.0f, bonus_floor_levels[11]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 20;

            a_floor = &GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 14.0f, bonus_floor_levels[12]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 21;

            GameSystem::Instantiate(LevelWall_2, GameObjectOptions{.position{block_width * 6.0f, wall_levels[6]}});
            GameSystem::Instantiate(LevelWall_2, GameObjectOptions{.position{block_width * 23.0f, wall_levels[6]}});
            a_floor = &GameSystem::Instantiate(LevelFloor_4, GameObjectOptions{.position{block_width * 6.0f, bonus_floor_levels[13]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 22;
            a_floor = &GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 20.0f, bonus_floor_levels[13]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 22;

            Player_1->getComponent<Script, Player>().last_level = 22;
            if (Player_2 != nullptr) {
                Player_2->getComponent<Script, Player>().last_level = 22;
            }
            if (Player_3 != nullptr) {
                Player_3->getComponent<Script, Player>().last_level = 22;
            }
            if (Player_4 != nullptr) {
                Player_4->getComponent<Script, Player>().last_level = 22;
            }

            GameSystem::Instantiate(Eggplant, GameObjectOptions{.position={block_width * 6.0f, bonus_floor_levels[0] - eggplant_height}});
            GameSystem::Instantiate(Eggplant, GameObjectOptions{.position={block_width * 24.0f, bonus_floor_levels[0] - eggplant_height}});
            GameSystem::Instantiate(Eggplant, GameObjectOptions{.position={block_width * 20.0f, bonus_floor_levels[3] - eggplant_height}});
            GameSystem::Instantiate(Eggplant, GameObjectOptions{.position={block_width * 12.0f, bonus_floor_levels[6] - eggplant_height}});

            // 1st floor topi:
            auto a_topi = &GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[0] - (topi_size.y + 1)}});
            Enemies.push_back(a_topi);
            // 3rd floor topi:
            a_topi = &GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[2] - (topi_size.y + 1)}});
            a_topi->getComponent<Script, TopiBehavior>().total_blocks   = 22;
            a_topi->getComponent<Script, TopiBehavior>().current_blocks = &f3_current_blocks;
            a_topi->getComponent<Script, TopiBehavior>().floor_level    = 3;
            a_topi->getComponent<Script, TopiBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
            Enemies.push_back(a_topi);
            // 5th floor topi:
            a_topi = &GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[4] - (topi_size.y + 1)}});
            a_topi->getComponent<Script, TopiBehavior>().total_blocks   = 22;
            a_topi->getComponent<Script, TopiBehavior>().current_blocks = &f5_current_blocks;
            a_topi->getComponent<Script, TopiBehavior>().floor_level    = 5;
            a_topi->getComponent<Script, TopiBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
            Enemies.push_back(a_topi);
            // 7th floor topi:
            a_topi = &GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[6] - (topi_size.y + 1)}});
            a_topi->getComponent<Script, TopiBehavior>().total_blocks   = 20;
            a_topi->getComponent<Script, TopiBehavior>().current_blocks = &f7_current_blocks;
            a_topi->getComponent<Script, TopiBehavior>().floor_level    = 7;
            a_topi->getComponent<Script, TopiBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
            Enemies.push_back(a_topi);
            // 4th floor Nutpicker:
            if (std::get<bool>(ini["Game"]["AdvancedAI"])) {
                GameObject* a_nutpicker = nullptr;
                if (E(e2) > 0.7) {
                    a_nutpicker = &GameSystem::Instantiate(Nutpicker, GameObjectOptions{.position{10, floor_levels[3] - 120}});
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_level = 4;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().vertical_scale = vertical_scale;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_levels = floor_levels;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
                }
                if (E(e2) > 0.7) {
                    a_nutpicker = &GameSystem::Instantiate(Nutpicker, GameObjectOptions{.position{GetScreenWidth()-100.0f, floor_levels[3] - 70}});
                    a_nutpicker->getComponent<Animator>().Flip();
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().isRight = -1;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_level = 4;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().vertical_scale = vertical_scale;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_levels = floor_levels;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
                }
                if (E(e2) > 0.7) {
                    a_nutpicker = &GameSystem::Instantiate(Nutpicker, GameObjectOptions{.position{10, floor_levels[5] - 60}});
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_level = 6;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().vertical_scale = vertical_scale;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_levels = floor_levels;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
                }
            } else {
                GameSystem::Instantiate(Nutpicker, GameObjectOptions{.position{100000,90000}});
            }

            auto jose = &GameSystem::Instantiate(Joseph, GameObjectOptions{.position{(float)WINDOW_WIDTH, floor_levels[1] - (joseph_size.y + 1)}});
            jose->getComponent<Script, JosephBehavior>().playerTransforms = playerTransforms;
            Josephs.push_back(jose);
            jose = &GameSystem::Instantiate(Joseph, GameObjectOptions{.position{-(joseph_size.x + block_width), floor_levels[3] - (joseph_size.y + 1)}});
            jose->getComponent<Script, JosephBehavior>().playerTransforms = playerTransforms;
            Josephs.push_back(jose);
            jose = &GameSystem::Instantiate(Joseph, GameObjectOptions{.position{WINDOW_WIDTH/2.0f, floor_levels[5] - (joseph_size.y + 1)}});
            jose->getComponent<Script, JosephBehavior>().playerTransforms = playerTransforms;
            Josephs.push_back(jose);
            
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

            auto a_floor = &GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, floor_levels[1]}});
            a_floor->getComponent<Script, CloudBehavior>().floor_level = 2;

            GameSystem::Instantiate(GrassWall, GameObjectOptions{.position{0, floor_levels[2] + block_height}});
            GameSystem::Instantiate(GrassWall, GameObjectOptions{.position{GetScreenWidth() - block_width * 4.0f, floor_levels[2] + block_height}});
            //int f1_current_blocks = 24;
            for (int i = 0; i < 24; i++) {
                if(i != 7) {
                    GameSystem::Instantiate(GrassHole, GameObjectOptions{.position{block_width * (4.0f + i), floor_levels[1]}});
                }
            }

            int f3_current_blocks = 14, f4_current_blocks = 20;
            for (int i = 0; i < 22; i++) {
                if(i < 5 || i > 12) {
                    auto block = &GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2]}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 3;
                    //block->getComponent<Script, BlockBehavior>().current_blocks = &f3_current_blocks;
                }
                else if(i == 5 || i == 10) {
                    auto block = &GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2]}});
                    block->getComponent<Script, FloorBehavior>().floor_level = 3;
                }
                else if(i == 8 || i == 9) {
                    GameSystem::Instantiate(DirtHole, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2]}});
                }
                if((i > 2 && i < 7) || i == 11 || (i > 14 && i < 18)) {
                    auto block = &GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 3;
                }
                if(i == 0 || i == 2 || i == 12 || i == 14 || i == 18) {
                    auto block = &GameSystem::Instantiate(DirtBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[2] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 3;
                }
                if(i != 6 && i != 11) {
                    auto block = &GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3]}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 4;
                    //block->getComponent<Script, BlockBehavior>().current_blocks = &f4_current_blocks;
                }
                if(i < 4 || (i > 7 && i < 10) || (i > 14 && i < 18)) {
                    auto block = &GameSystem::Instantiate(DirtBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 4;
                }
                if(i == 5 || i == 10 || i == 14 || i == 18) {
                    auto block = &GameSystem::Instantiate(DirtBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[3] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 4;
                }
            }
            a_floor = &GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, floor_levels[4]}});
            a_floor->getComponent<Script, CloudBehavior>().floor_level = 5;

            GameSystem::Instantiate(DirtWall, GameObjectOptions{.position{0, floor_levels[5] + block_height}});
            GameSystem::Instantiate(DirtWall, GameObjectOptions{.position{GetScreenWidth() - block_width * 5.0f, floor_levels[5] + block_height}});

            int f6_current_blocks = 20, f8_current_blocks = 10;
            for (int i = 0; i < 20; i++) {
                auto block = &GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[5]}});
                block->getComponent<Script, BlockBehavior>().floor_level = 6;
                //block->getComponent<Script, BlockBehavior>().current_blocks = &f6_current_blocks;
                
                if(i == 0 || (i > 9 && i < 17)) {
                    block = &GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[5] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 6;
                }
                if(i == 1 || i == 9 || i == 17) {
                    block = &GameSystem::Instantiate(IceBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[5] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 6;
                }

                GameSystem::Instantiate(IceHole, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[6]}});
                if((i > 1 && i < 6 ) || (i > 9 && i < 14 ) || (i > 17)) {
                    block = &GameSystem::Instantiate(IceSlidingBlock, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[7]}});
                    block->getComponent<Script, SlidingBlockBehavior>().floor_level = 8;
                } else {
                    GameSystem::Instantiate(IceSlidingHole, GameObjectOptions{.position{block_width * (6.0f + i), floor_levels[7]}});
                }       
                if((i > 2 && i < 5) || (i > 9 && i < 14 )) {
                    block = &GameSystem::Instantiate(IceBlock, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[7] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 8;
                }
                if(i == 2 || i == 5) {
                    block = &GameSystem::Instantiate(IceBlockThin, GameObjectOptions{.position{block_width * (5.0f + i), floor_levels[7] + block_height}});
                    block->getComponent<Script, BlockBehavior>().floor_level = 8;
                }
            }

            a_floor = &GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, floor_levels[6]}});
            a_floor->getComponent<Script, CloudBehavior>().floor_level = 7;

            GameSystem::Instantiate(IceWall, GameObjectOptions{.position{0, floor_levels[7] + block_height}});
            GameSystem::Instantiate(IceWall, GameObjectOptions{.position{GetScreenWidth() - block_width * 6.0f, floor_levels[7] + block_height}});

            GameSystem::Instantiate(Wall, GameObjectOptions{.position{block_width * 11.0f, floor_levels[1]}});
            GameSystem::Instantiate(Wall, GameObjectOptions{.position{block_width * 11.0f, floor_levels[3]}});
            GameSystem::Instantiate(Wall, GameObjectOptions{.position{block_width * 16.0f, floor_levels[3]}});

            a_floor = &GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 9.0f, floor_levels[8]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 9;
            a_floor = &GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 17.0f, floor_levels[8]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 9;

            // Fase bonus
            a_floor = &GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 6.0f, bonus_floor_levels[0]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 10;
            a_floor = &GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 13.0f, bonus_floor_levels[0]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 10;
            a_floor = &GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 22.0f, bonus_floor_levels[0]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 10;

            a_floor = &GameSystem::Instantiate(Cloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, bonus_floor_levels[1]}});
            a_floor->getComponent<Script, CloudBehavior>().floor_level = 11;

            a_floor = &GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 18.0f, bonus_floor_levels[2]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 12;
            a_floor = &GameSystem::Instantiate(LevelFloor_4, GameObjectOptions{.position{block_width * 5.0f, bonus_floor_levels[3]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 13;

            a_floor = &GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 21.0f, bonus_floor_levels[4]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 14;
            a_floor = &GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 12.0f, bonus_floor_levels[5]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 15;

            a_floor = &GameSystem::Instantiate(SmallCloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, bonus_floor_levels[6]}});
            a_floor->getComponent<Script, CloudBehavior>().floor_level = 16;
            a_floor = &GameSystem::Instantiate(SmallCloud, GameObjectOptions{.position{GetScreenWidth() + 10.0f, bonus_floor_levels[7]}});
            a_floor->getComponent<Script, CloudBehavior>().floor_level = 17;

            a_floor = &GameSystem::Instantiate(LevelFloor_3, GameObjectOptions{.position{block_width * 10.0f, bonus_floor_levels[8]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 18;
            a_floor = &GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 17.0f, bonus_floor_levels[8]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 18;
            a_floor = &GameSystem::Instantiate(LevelFloor_2, GameObjectOptions{.position{block_width * 14.0f, bonus_floor_levels[9]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 19;

            GameSystem::Instantiate(LevelWall_2, GameObjectOptions{.position{block_width * 6.0f, wall_levels[6]}});
            GameSystem::Instantiate(LevelWall_2, GameObjectOptions{.position{block_width * 23.0f, wall_levels[6]}});
            a_floor = &GameSystem::Instantiate(LevelFloor_4, GameObjectOptions{.position{block_width * 6.0f, bonus_floor_levels[10]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 20;
            a_floor = &GameSystem::Instantiate(LevelFloor_1, GameObjectOptions{.position{block_width * 20.0f, bonus_floor_levels[10]}});
            a_floor->getComponent<Script, FloorBehavior>().floor_level = 20;

            Player_1->getComponent<Script, Player>().last_level = 20;
            if (Player_2 != nullptr) {
                Player_2->getComponent<Script, Player>().last_level = 20;
            }
            if (Player_3 != nullptr) {
                Player_3->getComponent<Script, Player>().last_level = 20;
            }
            if (Player_4 != nullptr) {
                Player_4->getComponent<Script, Player>().last_level = 20;
            }

            GameSystem::Instantiate(Lettuce, GameObjectOptions{.position={block_width * 6.0f, bonus_floor_levels[0] - lettuce_height}});
            GameSystem::Instantiate(Lettuce, GameObjectOptions{.position={block_width * 24.0f, bonus_floor_levels[0] - lettuce_height}});
            GameSystem::Instantiate(Lettuce, GameObjectOptions{.position={block_width * 20.0f, bonus_floor_levels[2] - lettuce_height}});
            GameSystem::Instantiate(Lettuce, GameObjectOptions{.position={block_width * 12.0f, bonus_floor_levels[5] - lettuce_height}});

            auto a_topi = &GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width), floor_levels[0] - (topi_size.y + 1)}});
            Enemies.push_back(a_topi);

            a_topi = &GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width), floor_levels[2] - (topi_size.y + 1)}});
            a_topi->getComponent<Script, TopiBehavior>().total_blocks   = 16;
            a_topi->getComponent<Script, TopiBehavior>().current_blocks = &f3_current_blocks;
            a_topi->getComponent<Script, TopiBehavior>().floor_level    = 3;
            a_topi->getComponent<Script, TopiBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
            Enemies.push_back(a_topi);

            a_topi = &GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[3] - (topi_size.y + 1)}});
            a_topi->getComponent<Script, TopiBehavior>().total_blocks   = 20;
            a_topi->getComponent<Script, TopiBehavior>().current_blocks = &f4_current_blocks;
            a_topi->getComponent<Script, TopiBehavior>().floor_level    = 4;
            a_topi->getComponent<Script, TopiBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
            Enemies.push_back(a_topi);

            a_topi = &GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[5] - (topi_size.y + 1)}});
            a_topi->getComponent<Script, TopiBehavior>().total_blocks   = 20;
            a_topi->getComponent<Script, TopiBehavior>().current_blocks = &f6_current_blocks;
            a_topi->getComponent<Script, TopiBehavior>().floor_level    = 6;
            a_topi->getComponent<Script, TopiBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
            Enemies.push_back(a_topi);

            a_topi = &GameSystem::Instantiate(Topi, GameObjectOptions{.position{-(topi_size.x + block_width),floor_levels[7] - (topi_size.y + 1)}});
            a_topi->getComponent<Script, TopiBehavior>().total_blocks   = 20;
            a_topi->getComponent<Script, TopiBehavior>().current_blocks = &f8_current_blocks;
            a_topi->getComponent<Script, TopiBehavior>().floor_level    = 8;
            a_topi->getComponent<Script, TopiBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
            Enemies.push_back(a_topi);
            
            // 4th floor Nutpicker:
            if (std::get<bool>(ini["Game"]["AdvancedAI"])) {
                GameObject* a_nutpicker = nullptr;
                if (E(e2) > 0.7) {
                    a_nutpicker = &GameSystem::Instantiate(Nutpicker, GameObjectOptions{.position{10, floor_levels[3] - 120}});
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_level = 4;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().vertical_scale = vertical_scale;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_levels = floor_levels;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
                }
                if (E(e2) > 0.7) {
                    a_nutpicker = &GameSystem::Instantiate(Nutpicker, GameObjectOptions{.position{GetScreenWidth()-100.0f, floor_levels[3] - 70}});
                    a_nutpicker->getComponent<Animator>().Flip();
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().isRight = -1;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_level = 4;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().vertical_scale = vertical_scale;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_levels = floor_levels;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
                }
                if (E(e2) > 0.7) {
                    a_nutpicker = &GameSystem::Instantiate(Nutpicker, GameObjectOptions{.position{10, floor_levels[5] - 60}});
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_level = 6;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().vertical_scale = vertical_scale;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().floor_levels = floor_levels;
                    a_nutpicker->getComponent<Script, NutpickerBehavior>().players = {Player_1, Player_2, Player_3, Player_4};
                }
            } else {
                GameSystem::Instantiate(Nutpicker, GameObjectOptions{.position{100000,90000}});
            }

            auto jose = &GameSystem::Instantiate(Joseph, GameObjectOptions{.position{(float)WINDOW_WIDTH, floor_levels[3] - (joseph_size.y + 1)}});
            jose->getComponent<Script, JosephBehavior>().playerTransforms = playerTransforms;
            Josephs.push_back(jose);
            jose = &GameSystem::Instantiate(Joseph, GameObjectOptions{.position{-(joseph_size.x + block_width), floor_levels[5] - (joseph_size.y + 1)}});
            jose->getComponent<Script, JosephBehavior>().playerTransforms = playerTransforms;
            Josephs.push_back(jose);

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
        if (numPlayers > 1) {
            Player_2 = &GameSystem::Instantiate(Nana, GameObjectOptions{.position = {block_width * 13.0f, levels[0] - player_size.y}});
        }
        if (numPlayers > 2) {
            Player_3 = &GameSystem::Instantiate(Amam, GameObjectOptions{.position = {block_width * 18.0f,levels[0] - player_size.y}});
        }
        if (numPlayers > 3) {
            Player_4 = &GameSystem::Instantiate(Lili, GameObjectOptions{.position = {block_width * 23.0f,levels[0] - player_size.y}});
        }

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

    bool exit_game = false;
    float timeToShowScores = 0.0f;
    bool finished = false;
    bool play_music = true;
    bool moving_camera = false;
    float objects_offset = 6.0f * block_height, current_objects_offset = 0;
    float old_offset = objects_offset;
    float chrono_time = 0.0f;
    float time_limit = run_time;
    float current_time = 0.0f;
    bool onBonus = false;
    BGM.Init();
    BGMBonus.Init();
    BGM2.Init();

    UIText ContinueText(NES, "CONTINUE", 40, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f - 30}, UIObject::CENTER);
    UIText ExitText(NES, "EXIT", 40, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 30}, UIObject::CENTER);
    UISprite Hammer("Assets/Sprites/UI_Old_Hammer.png", {ContinueText.pos.x - 50, ContinueText.pos.y}, 4.0f, 4.0f, UIObject::UP_RIGHT);
    int OPTION = 0, OPTIONS = 2; // HammerView

    UISprite TimerBox("Assets/Sprites/timer.png", {20, 20}, 3.5f, 3.5f);
    UIText TimerCount(NES, "0:00", 41, 1, {TimerBox.dst.x + TimerBox.dst.width/2.0f, 60}, UIObject::UP_CENTER);

    // Ranking of player 1
    UISprite BigFrame1("Assets/Sprites/Record-frame1.png", {GetScreenWidth()/4.0f, GetScreenHeight()/2.0f}, {GetScreenWidth()/2.0f - 50, GetScreenHeight() - 40.0f}, UIObject::CENTER);
    UIText Player1Text(NES, "Player 1", 40, 1, {GetScreenWidth()/4.0f, GetScreenHeight()/2.0f - 200}, UIObject::DOWN_CENTER);
    UISprite Player1Cry("Assets/Sprites/Popo/08_Cry_Single.png", {GetScreenWidth()/8.0f + 20, GetScreenHeight()/2.0f - 123}, 3.5f, 3.5f, UIObject::DOWN_CENTER);
    UISprite Player1Cel("Assets/Sprites/Popo/09_Celebrate_Single.png", {GetScreenWidth()/8.0f + 30, GetScreenHeight()/2.0f - 123}, 3.3f, 3.3f, UIObject::DOWN_CENTER);
    UIText Player1StatusA(NES, "YOU", 37, 1, {GetScreenWidth() * (1/4.0f + 1/12.0f) - 30, GetScreenHeight()/2.0f - 150}, UIObject::DOWN_CENTER);
    UIText Player1StatusB(NES, "LOOSE!", 37, 1, {GetScreenWidth() * (1/4.0f + 1/12.0f) - 20, GetScreenHeight()/2.0f - 125}, UIObject::DOWN_CENTER);
    // - Number of destroyed icicles
    UISprite Icicler1("Assets/Sprites/Ranking-icicle.png", {GetScreenWidth()/8.0f, GetScreenHeight()/2.0f - 80}, 3.7f, 3.7f, UIObject::CENTER);
    UIText Icicler1Text(NES, "00x", 40, 1, {GetScreenWidth()/4.0f + 70, Icicler1.dst.y + Icicler1.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Icicler1N(NES, "00", 40, 1, {Icicler1Text.pos.x + Icicler1Text.size.x + 1, Icicler1Text.pos.y});
    // - Number of defeated nutpickers
    UISprite Monster1("Assets/Sprites/Ranking-nutpicker.png", Vector2{GetScreenWidth()/8.0f, GetScreenHeight()/2.0f - 20}, 3.0f, 3.0f, UIObject::CENTER);
    UIText Monster1Text(NES, "-10x", 40, 1, {GetScreenWidth()/4.0f + 70, Monster1.dst.y + Monster1.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Monster1N(NES, "00", 40, 1, {Monster1Text.pos.x + Monster1Text.size.x + 1, Monster1Text.pos.y});
    // - Number of collected vegetables
    UISprite Vegetabler1("Assets/Sprites/Fruit_Lettuce.png", {GetScreenWidth()/8.0f, GetScreenHeight()/2.0f + 40}, 3.4f, 3.4f, UIObject::CENTER);
    UIText Vegetabler1Text(NES, "300x", 40, 1, {GetScreenWidth()/4.0f + 70, Vegetabler1.dst.y + Vegetabler1.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Vegetabler1N(NES, "00", 40, 1, {Vegetabler1Text.pos.x + Vegetabler1Text.size.x + 1, Vegetabler1Text.pos.y});
    // - Number of destroyed blocks
    UISprite Block1("Assets/Sprites/Ranking-block.png", {GetScreenWidth()/8.0f, GetScreenHeight()/2.0f + 100}, 4.2f, 4.2f, UIObject::CENTER);
    UIText Block1Text(NES, (!level) ? "10x" : "20x", 40, 1, {GetScreenWidth()/4.0f + 70, Block1.dst.y + Block1.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Block1N(NES, "00", 40, 1, {Block1Text.pos.x + Block1Text.size.x + 1, Block1Text.pos.y});
    // - Total of player 1
    UIText TotalTitle1(NES, "TOTAL", 40, 0, {GetScreenWidth() * (1/4.0f - 1/24.0f), GetScreenHeight()/2.0f + 135}, UIObject::UP_CENTER);
    UISprite SmallFrame1("Assets/Sprites/Record-frames1.png", {GetScreenWidth()/4.0f, GetScreenHeight()/2.0f + 170}, 4.0f, 3.5f, UIObject::UP_CENTER);
    UIText Total1(NES, "000000", 40, 1, {GetScreenWidth()/4.0f, (GetScreenHeight() + SmallFrame1.dst.height)/2.0f + 170}, UIObject::CENTER);

    // Ranking of player 2
    UISprite BigFrame2("Assets/Sprites/Record-frame2.png", {GetScreenWidth() - GetScreenWidth()/4.0f, GetScreenHeight()/2.0f}, {GetScreenWidth()/2.0f - 50, GetScreenHeight() - 40.0f}, UIObject::CENTER);
    UIText Player2Text(NES, "Player 2", 40, 1, {GetScreenWidth() * (1 - 1/4.0f), GetScreenHeight()/2.0f - 200}, UIObject::DOWN_CENTER);
    UISprite Player2Cry("Assets/Sprites/Nana/08_Cry_Single.png", {GetScreenWidth() * (1 - 1/4.0f - 1/8.0f) + 20, GetScreenHeight()/2.0f - 123}, 3.5f, 3.5f, UIObject::DOWN_CENTER);
    UISprite Player2Cel("Assets/Sprites/Nana/09_Celebrate_Single.png", {GetScreenWidth() * (1 - 1/4.0f - 1/8.0f) + 30, GetScreenHeight()/2.0f - 123}, 3.3f, 3.3f, UIObject::DOWN_CENTER);
    UIText Player2StatusA(NES, "YOU", 37, 1, {GetScreenWidth() * (1 - 1/4.0f + 1/12.0f) - 30, GetScreenHeight()/2.0f - 150}, UIObject::DOWN_CENTER);
    UIText Player2StatusB(NES, "LOOSE!", 37, 1, {GetScreenWidth() * (1 - 1/4.0f + 1/12.0f) - 20, GetScreenHeight()/2.0f - 125}, UIObject::DOWN_CENTER);
    // - Number of destroyed icicles
    UISprite Icicler2("Assets/Sprites/Ranking-icicle.png", {GetScreenWidth() * (1-1/4.0f-1/8.0f), GetScreenHeight()/2.0f - 80}, 3.7f, 3.7f, UIObject::CENTER);
    UIText Icicler2Text(NES, "00x", 40, 1, {GetScreenWidth() * (1-1/4.0f) + 70, Icicler2.dst.y + Icicler2.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Icicler2N(NES, "00", 40, 1, {Icicler2Text.pos.x + Icicler2Text.size.x + 1, Icicler2Text.pos.y});
    // - Number of defeated nutpickers
    UISprite Monster2("Assets/Sprites/Ranking-nutpicker.png", {GetScreenWidth() * (1-1/4.0f-1/8.0f), GetScreenHeight()/2.0f - 20}, 3.0f, 3.0f, UIObject::CENTER);
    UIText Monster2Text(NES, "-10x", 40, 1, {GetScreenWidth() * (1-1/4.0f) + 70, Monster2.dst.y + Monster2.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Monster2N(NES, "00", 40, 1, {Monster2Text.pos.x + Monster2Text.size.x + 1, Monster2Text.pos.y});
    // - Number of collected vegetables
    UISprite Vegetabler2("Assets/Sprites/Fruit_Lettuce.png", {GetScreenWidth() * (1-1/4.0f-1/8.0f), GetScreenHeight()/2.0f + 40}, 3.4f, 3.4f, UIObject::CENTER);
    UIText Vegetabler2Text(NES, "300x", 40, 1, {GetScreenWidth() * (1-1/4.0f) + 70, Vegetabler2.dst.y + Vegetabler2.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Vegetabler2N(NES, "00", 40, 1, {Vegetabler2Text.pos.x + Vegetabler2Text.size.x + 1, Vegetabler2Text.pos.y});
    // - Number of destroyed blocks
    UISprite Block2("Assets/Sprites/Ranking-block.png", {GetScreenWidth() * (1-1/4.0f-1/8.0f), GetScreenHeight()/2.0f + 100}, 4.2f, 4.2f, UIObject::CENTER);
    UIText Block2Text(NES, (!level) ? "10x" : "20x", 40, 1, {GetScreenWidth() * (1-1/4.0f) + 70, Block2.dst.y + Block2.dst.height/2}, UIObject::CENTER_RIGHT);
    UIText Block2N(NES, "00", 40, 1, {Block2Text.pos.x + Block2Text.size.x + 1, Block2Text.pos.y});
    // - Total of player 2
    UIText TotalTitle2(NES, "TOTAL", 40, 0, {GetScreenWidth() * (1 - 1/4.0f - 1/24.0f), GetScreenHeight()/2.0f + 135}, UIObject::UP_CENTER);
    UISprite SmallFrame2("Assets/Sprites/Record-frames2.png", {GetScreenWidth() * (1 - 1/4.0f), GetScreenHeight()/2.0f + 170}, 4.0f, 3.5f, UIObject::UP_CENTER);
    UIText Total2(NES,"000000", 40, 1, {GetScreenWidth() * (1-1/4.0f), (GetScreenHeight() + SmallFrame2.dst.height)/2.0f + 170}, UIObject::CENTER);
    float transition_time = 6.0f; bool play_first = true;

    enum GAME_MENU {GAME, PAUSED, RANKING};
    GAME_MENU CURRENT_MENU = GAME;
    GameSystem::Start();
    while(!finished) {
        // Condition to close the window directly
        if (WindowShouldClose()) {
            finished = exit_game = true;
        }
        // Screen buffer refresh loop
        BeginDrawing();
        ClearBackground(BLACK);
        if (CURRENT_MENU == RANKING) {
            if (current_objects_offset <= objects_offset) {
                BackGrounds[level].Draw();
                if (transition_time > 0.0f && Player_1->getComponent<Script, Player>().victory) {
                    GameSystem::Render();
                    transition_time -= GetFrameTime();
                } else {
                    if (play_first) {
                        Fly.Play();
                        play_first = false;
                    }
                    float shift = block_height * 6.0f * GetFrameTime();
                    current_objects_offset += shift;
                    GameSystem::Move({0,shift});
                    BackGrounds[level].Move({0,shift});
                }
            } else {
                if (timeToShowScores < 3.0) {
                    timeToShowScores += GetFrameTime();
                } else {
                    BigFrame1.Draw();
                    // Victoria o derrota del jugador 1
                    Player1Text.Draw();
                    if (Player_1->getComponent<Script, Player>().victory) {
                        if (level == 0) {
                            pts1 = Player_1->getComponent<Script, Player>().frutasRecogidas*300
                                 + Player_1->getComponent<Script, Player>().bloquesDestruidos*(level+1)*10
                                 + Player_1->getComponent<Script, Player>().nutpickerGolpeados*-10;
                            pts1 += 3000;
                            if (std::get<bool>(ini["Game"]["AdvancedAI"])) {
                                pts1 *= 2;
                            }
                            Vegetabler1Text.SetText("300x", false);
                        } else {
                            pts1 = Player_1->getComponent<Script, Player>().frutasRecogidas*500
                                 + Player_1->getComponent<Script, Player>().bloquesDestruidos*(level+1)*10
                                 + Player_1->getComponent<Script, Player>().nutpickerGolpeados*-10;
                            pts1 += 7000;
                            if (std::get<bool>(ini["Game"]["AdvancedAI"])) {
                                pts1 *= 3;
                            }
                            Vegetabler1Text.SetText("500x", false);
                        }
                        if (run_time > 0) {
                            pts1 *= level + ((float)time_limit/run_time);
                        }
                        Player1Cel.Draw();
                        Player1StatusB.SetText("WON!", false);
                    } else {
                        if (level == 0) {
                            pts1 = Player_1->getComponent<Script, Player>().frutasRecogidas*300
                                 + Player_1->getComponent<Script, Player>().bloquesDestruidos*(level+1)*10
                                 + Player_1->getComponent<Script, Player>().nutpickerGolpeados*-10;
                            Vegetabler1Text.SetText("300x", false);
                        } else {
                            pts1 = Player_1->getComponent<Script, Player>().frutasRecogidas*500
                                 + Player_1->getComponent<Script, Player>().bloquesDestruidos*(level+1)*10
                                 + Player_1->getComponent<Script, Player>().nutpickerGolpeados*-10;
                            Vegetabler1Text.SetText("500x", false);
                        }
                        Player1Cry.Draw();
                    }
                    Player1StatusA.Draw();
                    Player1StatusB.Draw();
                    // Carambanos destruidos
                    Icicler1.Draw();
                    Icicler1Text.Draw();
                    Icicler1N.SetText(fill_string(std::to_string(Player_1->getComponent<Script,Player>().icicleDestruido),2), false);
                    Icicler1N.Draw();
                    // Numero de enemigos derrotados
                    Monster1.Draw();
                    Monster1Text.Draw();
                    Monster1N.SetText(fill_string(std::to_string(Player_1->getComponent<Script, Player>().nutpickerGolpeados),2), false);
                    Monster1N.Draw();
                    // Verduras cogidas
                    Vegetabler1.Draw();
                    Vegetabler1Text.Draw();
                    Vegetabler1N.SetText(fill_string(std::to_string(Player_1->getComponent<Script, Player>().frutasRecogidas),2), false);
                    Vegetabler1N.Draw();
                    // Bloques destruidos
                    Block1N.SetText(fill_string(std::to_string(Player_1->getComponent<Script, Player>().bloquesDestruidos),2), false);
                    Block1.Draw();
                    Block1Text.Draw();
                    Block1N.Draw();
                    // Puntos totales del jugador 1
                    TotalTitle1.Draw();
                    SmallFrame1.Draw();
                    Total1.SetText(fill_string(std::to_string(pts1), 6), false);
                    Total1.Draw();

                    if (Player_2 != nullptr) {
                        BigFrame2.Draw();
                        // Victoria o derrota del jugador 2
                        Player2Text.Draw();
                        if (Player_1->getComponent<Script, Player>().victory || Player_2->getComponent<Script, Player>().victory) {
                            if (level == 0) {
                                pts2 = Player_2->getComponent<Script, Player>().frutasRecogidas*300
                                     + Player_2->getComponent<Script, Player>().bloquesDestruidos*(level+1)*10
                                     + Player_2->getComponent<Script, Player>().nutpickerGolpeados*-10;
                                pts2 += 3000;
                                if (std::get<bool>(ini["Game"]["AdvancedAI"])) {
                                    pts2 *= 2;
                                }
                                Vegetabler2Text.SetText("300x");
                            } else {
                                pts2 = Player_2->getComponent<Script, Player>().frutasRecogidas*500
                                    + Player_2->getComponent<Script, Player>().bloquesDestruidos*(level+1)*10
                                    + Player_2->getComponent<Script, Player>().nutpickerGolpeados*-10;
                                pts2 += 7000;
                                if (std::get<bool>(ini["Game"]["AdvancedAI"])) {
                                    pts2 *= 3;
                                }
                                Vegetabler2Text.SetText("500x");
                            }
                            if (run_time > 0) {
                                pts2 *= level + ((float)time_limit/run_time);
                            }
                            Player2Cel.Draw();
                            Player2StatusB.SetText("WON!", false); 
                        } else {
                            if (level == 0) {
                                pts2 = Player_2->getComponent<Script, Player>().frutasRecogidas*300 
                                    + Player_2->getComponent<Script, Player>().bloquesDestruidos*(level+1)*10
                                    + Player_2->getComponent<Script, Player>().nutpickerGolpeados*-10;
                                Vegetabler2Text.SetText("300x");
                            } else {
                                pts2 = Player_2->getComponent<Script, Player>().frutasRecogidas*500
                                     + Player_2->getComponent<Script, Player>().bloquesDestruidos*(level+1)*10
                                     + Player_2->getComponent<Script, Player>().nutpickerGolpeados*-10;
                                Vegetabler2Text.SetText("500x");
                            }
                            Player2Cry.Draw();
                        }
                        Player2StatusA.Draw();
                        Player2StatusB.Draw();
                        // Carambanos destruidos
                        Icicler2.Draw();
                        Icicler2Text.Draw();
                        Icicler2N.SetText(fill_string(std::to_string(Player_2->getComponent<Script,Player>().icicleDestruido),2), false);
                        Icicler2N.Draw();
                        // Numero de enemigos derrotados
                        Monster2.Draw();
                        Monster2Text.Draw();
                        Monster2N.SetText(fill_string(std::to_string(Player_2->getComponent<Script, Player>().nutpickerGolpeados),2), false);
                        Monster2N.Draw();
                        // Verduras cogidas
                        Vegetabler2.Draw();
                        Vegetabler2Text.Draw();
                        Vegetabler2N.SetText(fill_string(std::to_string(Player_2->getComponent<Script, Player>().frutasRecogidas),2), false);
                        Vegetabler2N.Draw();
                        // Bloques destruidos
                        Block2.Draw();
                        Block2N.SetText(fill_string(std::to_string(Player_2->getComponent<Script, Player>().bloquesDestruidos),2), false);
                        Block2N.Draw();
                        Block2Text.Draw();
                        // Puntos totales del jugador 2
                        TotalTitle2.Draw();
                        SmallFrame2.Draw();
                        Total2.SetText(fill_string(std::to_string(pts2),6), false);
                        Total2.Draw();
                    }
                    if(KeyboardDetected() || GamepadDetected(0)){
                        break;
                    }
                }
            }
        } else {
            if (IsKeyPressed(KEY_M)) {
                play_music = !play_music;
            }
            if (play_music) {
                if (!onBonus) {
                    BGM.Play();
                } else {
                    BGMBonus.Play();
                }
            } else {
                BGM2.Play();
            }

            BackGrounds[level].Draw();
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                if (CURRENT_MENU == PAUSED) {
                    CURRENT_MENU = GAME;
                } else if (CURRENT_MENU == GAME) {
                    PauseSound.Play();
                    CURRENT_MENU = PAUSED;
                }
            }
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                if (CURRENT_MENU == PAUSED) {
                    CURRENT_MENU = GAME;
                }
            }

            if (CURRENT_MENU == GAME) {
                float delta_time = GetFrameTime();
                chrono_time += delta_time;

                if (!onBonus) {
                    for (auto JosephInstance : Josephs) {
                        if (JosephInstance->getComponent<Script, JosephBehavior>().shakeGround) {
                            JosephInstance->getComponent<Script, JosephBehavior>().shakeGround = false;
                            moving_camera = true;
                        }
                    }

                    if (Player_1->getComponent<Script, Player>().bonusLevel ||
                        (Player_2 != nullptr && Player_2->getComponent<Script, Player>().bonusLevel))
                    {
                        objects_offset = ((GAME_HEIGHT - 6.0f * block_height) - bonusLevel->getComponent<Transform2D>().position.y);
                        onBonus = true;
                        moving_camera = true;
                        GameSystem::DestroyByTag("Enemy");
                    }
                }

                if (!moving_camera && (
                    (Player_1->getComponent<Script, Player>().isGrounded && Player_1->getComponent<Transform2D>().position.y < (5.0f * block_height)) ||
                    (Player_2 != nullptr && Player_2->getComponent<Script, Player>().isGrounded && Player_2->getComponent<Transform2D>().position.y < (5.0f * block_height))
                )) {
                    moving_camera = true;
                    if (level_phase < 8) {
                        level_phase++;
                    }
                }

                if (!moving_camera) {
                    GameSystem::Update();
                    if (
                        (Player_1->getComponent<Script, Player>().lifes <= 0 && Player_2 == nullptr) ||
                        (Player_1->getComponent<Script, Player>().lifes <= 0 
                            && Player_2 != nullptr && Player_2->getComponent<Script, Player>().lifes <= 0)
                    ) {
                        CURRENT_MENU = RANKING;
                        objects_offset = 0;
                        current_objects_offset = 1;
                    } else if (
                        (Player_1->getComponent<Script, Player>().victory) ||
                        (Player_2 != nullptr && Player_2->getComponent<Script, Player>().victory)
                    ) {
                        CURRENT_MENU = RANKING;
                        objects_offset = 20.0f * block_height;
                    }
                } else {
                    float shift = block_height * 6.0f * GetFrameTime();
                    current_objects_offset  += shift;
                    if (current_objects_offset <= objects_offset) {
                        GameSystem::Move({0,shift});
                        BackGrounds[level].Move({0,shift});   
                        if(!onBonus) {
                            for(auto enemy : Enemies) {
                                if (enemy->second_tags.find("Topi") != enemy->second_tags.end()) {
                                    enemy->getComponent<Script, TopiBehavior>().Move({0,shift});
                                }
                            }
                        }
                    } else {
                        objects_offset = old_offset;
                        current_objects_offset = 0;
                        moving_camera = false;
                    }
                    GameSystem::Render();
                }

                if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_R)) {
                    Player_1->getComponent<Transform2D>().position = Vector2{600,70};
                }

                if (run_time > 0) {
                    time_limit = time_limit - delta_time;
                    if (time_limit < 0) {
                        time_limit = 0;
                        CURRENT_MENU = RANKING;
                    } else {
                        TimerBox.Draw();
                        TimerCount.SetText(seconds_to_time(time_limit, true), false);
                        TimerCount.Draw((time_limit < run_time*0.3) ? RED : WHITE);
                    }
                } else if (run_time == 0) {
                    current_time += delta_time;
                    TimerBox.Draw();
                    TimerCount.SetText(seconds_to_time(current_time, true), false);
                    TimerCount.Draw();
                }

                if (!onBonus) {
                    if(Player_1->getComponent<Script, Player>().lifes > 0)
                        LifePopo1.Draw();
                    if(Player_1->getComponent<Script, Player>().lifes > 1)
                        LifePopo2.Draw();
                    if(Player_1->getComponent<Script, Player>().lifes > 2)
                        LifePopo3.Draw();
                    if(numPlayers > 1) {
                        if(Player_2->getComponent<Script, Player>().lifes > 0)
                            LifeNana1.Draw();
                        if(Player_2->getComponent<Script, Player>().lifes > 1)
                            LifeNana2.Draw();
                        if(Player_2->getComponent<Script, Player>().lifes > 2)
                            LifeNana3.Draw();
                    }
                }

            } else if (CURRENT_MENU == PAUSED) {
                ContinueText.Draw();
                ExitText.Draw();
                Hammer.Draw();
                if (IsKeyPressed(KEY_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
                    OPTION = mod(OPTION+1, OPTIONS);
                    Hammer.Translate({ContinueText.pos.x - 70, ContinueText.pos.y + OPTION*60*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                }
                if (IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
                    OPTION = mod(OPTION-1, OPTIONS);
                    Hammer.Translate({ContinueText.pos.x - 70, ContinueText.pos.y + (OPTIONS - (OPTIONS-OPTION))*60*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                }
                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                    if (OPTION == 0) {
                        CURRENT_MENU = GAME;
                    } else {
                        CURRENT_MENU = RANKING;
                    }
                }
            }
        }
        EndDrawing();
    }
    if (!exit_game) {
        if (Player_2 == nullptr) {
            if (run_time > 0 && Player_1->getComponent<Script, Player>().victory && (P1_TOTAL_SEC[level] == 0 || P1_TOTAL_SEC[level] > current_time)) {
                P1_TOTAL_SEC[level] = current_time;
            }
        } else {
            if (run_time > 0 && Player_1->getComponent<Script, Player>().victory && Player_2->getComponent<Script, Player>().victory && (P2_TOTAL_SEC[level] == 0 || P2_TOTAL_SEC[level] > current_time)) {
                P2_TOTAL_SEC[level] = current_time;
            }
        }
        if (P1_TOTAL_PTS[level] == 0 || P1_TOTAL_PTS[level] < pts1) {
            P1_TOTAL_PTS[level] = pts1;
        }
        if (Player_2 != nullptr && (P2_TOTAL_PTS[level] == 0 || P2_TOTAL_PTS[level] < pts2)) {
            P2_TOTAL_PTS[level] = pts2;
        }
        if (Player_1->getComponent<Script, Player>().victory) {
            P1_HAS_PASSED[level] = 1;
        }
        if (Player_2 != nullptr && Player_2->getComponent<Script, Player>().victory) {
            P2_HAS_PASSED[level] = 2;
        }
        save_sav();
    }
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
    GameSystem::DestroyAll();
    BGM.Unload();
    BGM2.Unload();
    if (exit_game) {
        exit(0);
    }
}

//-----------------------------------------------------------------------------
// Menus
// ----------------------------------------------------------------------------
int main() {

    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<float> D(0, (float)GetScreenWidth());
    std::uniform_int_distribution<int> I(0,1);
    init_config();
    UISystem::init_UI_system(900, 600);

    Music MainTitleOST = LoadMusicStream("Assets/Sounds/02-Main-Title.mp3");
    MainTitleOST.looping = true; 
    Music OldMainTitleOst = LoadMusicStream("Assets/Sounds/01-Main-Title.mp3");
    OldMainTitleOst.looping = true;
    NES = LoadFont("Assets/Fonts/Pixel_NES.otf");

    UISprite Sign("Assets/Sprites/Titlescreen/06_Sign.png", {GetScreenWidth()/2.0f, 20}, 2.7f, 2.7f, UIObject::UP_CENTER, false, 0.8);
    UISprite OldSign("Assets/Sprites/Titlescreen/Old_03_Sign.png", {GetScreenWidth()/2.0f, 20}, 2.7f, 2.7f, UIObject::UP_CENTER, false, 0.8);
    UISprite Copy("Assets/Sprites/Titlescreen/07_Copyright.png", Vector2{GetScreenWidth()/2.0f, GetScreenHeight()-20.0f}, 2.2f, 2.2f, UIObject::DOWN_CENTER);
    
    int background = I(e2);
    UISprite Background("Assets/Sprites/bg00.png", {0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    UISprite Background1("Assets/Sprites/bg01.png", {0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    UISprite OldBackground("Assets/Sprites/Titlescreen/Old_background.png", {0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    UISprite Transparent("Assets/Sprites/UI_Transparent.png", {0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});

    int mscroll_s = -5;
    UISprite Mountain("Assets/Sprites/Titlescreen/03_Mountain.png", Vector2{GetScreenWidth()/2.0f, GetScreenHeight()+10.0f}, 3.6f, 3.6f, UIObject::DOWN_CENTER);

    int fpparallax_s = 15;
    UISprite ForePines("Assets/Sprites/Titlescreen/01_Fore_Pines.png", Vector2{GetScreenWidth()/2.0f, GetScreenHeight()+10.0f}, {(float)GetScreenWidth(), 150}, UIObject::DOWN_CENTER);
    
    int mpparallax_s = 7;
    UISprite MidPines("Assets/Sprites/Titlescreen/02_Mid_Pines.png", Vector2{GetScreenWidth()/2.0f, GetScreenHeight()+10.0f}, {(float)GetScreenWidth(), 200}, UIObject::DOWN_CENTER);

    enum MENU_ENUM { INTRO, MAIN_MENU, NEW_GAME, NORMAL_GAME, SELECT_LEVEL, SETTINGS, CONTROL_SETTINGS, RANKINGS, ARE_YOU_SURE };
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
    UISprite PressStart("Assets/Sprites/press_key.png", {GetScreenWidth()/2.0f, GetScreenHeight()-140.0f}, 0.5f, 0.5f, UIObject::DOWN_CENTER);

    // MAIN MENU:
    UIText NewGameText(NES, "NEW GAME", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 25}, UIObject::DOWN_CENTER);
    UIText Rankings(NES, "RANKINGS", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 85}, UIObject::DOWN_CENTER);
    UIText SettingsText(NES, "SETTINGS", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 145}, UIObject::DOWN_CENTER);
    UIText ExitText(NES, "EXIT", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 205}, UIObject::DOWN_CENTER);
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
    Texture2D LArrow = LoadTexture("Assets/Sprites/UI_Arrow_left.png"), RArrow = LoadTexture("Assets/Sprites/UI_Arrow_right.png");

    bool CONFIGURE_MATCH = false;
    int selected_level = 0;
    UISprite Level1("Assets/Sprites/Level1_preview.png", {GetScreenWidth()/2.0f, 100}, 2.3f, 2.3f, UIObject::UP_CENTER);
    UIText Level1Text(NES, "LEVEL 1: BASICS", 33, 1, {GetScreenWidth()/2.0f, 50}, UIObject::UP_CENTER);
    UISprite Level2("Assets/Sprites/Level2_preview.png", {GetScreenWidth()/2.0f, 100}, {Level1.dst.width, Level1.dst.height}, UIObject::UP_CENTER);
    UIText Level2Text(NES, "LEVEL 2: ADVANCED MECHANICS", 33, 1, {GetScreenWidth()/2.0f, 50}, UIObject::UP_CENTER);
    UIText Level3Text(NES, "LEVEL 3: TRYING TO SURVIVE", 33, 1, {GetScreenWidth()/2.0f, 50}, UIObject::UP_CENTER);
    UIText Level3WIP(NES, "NO TIME FOR THIS LEVEL :(", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, UIObject::CENTER);
    UIText Level4Text(NES, "LEVEL 4: A NEW CHALLENGER", 33, 1, {GetScreenWidth()/2.0f, 50}, UIObject::UP_CENTER);
    UIText Level4WIP(NES, "NO TIME FOR THIS LEVEL :(", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, UIObject::CENTER);

    int selected_mod = 0, sudden_death_time = 0;
    std::vector<std::string> mods { "NO MOD", "SUDDEN DEATH", "SPEED RUN" };
    UIText StartText(NES, "START!", 27, 1, {GetScreenWidth()/2.0f + 100, GetScreenHeight()/2.0f + 50}, UIObject::CENTER_LEFT);
    UIText ModsText(NES, "NO MODS", 27, 1, {GetScreenWidth()/2.0f + 100, GetScreenHeight()/2.0f + 100}, UIObject::CENTER_LEFT);
    UIText ModsConfig(NES, "NO CONFIG NEEDED", 20, 1, {GetScreenWidth()/2.0f + 150, GetScreenHeight()/2.0f + 150}, UIObject::CENTER_LEFT);
    UIText BindigsText(NES, "SETTINGS", 27, 1, {GetScreenWidth()/2.0f + 100, GetScreenHeight()/2.0f + 200}, UIObject::CENTER_LEFT);
    bool show_error = false;
    UIText SuddenDeathError(NES, "SUDDENT DEATH NEEDS A TIME ABOVE ZERO...", 30, 1, {GetScreenWidth()/2.0f, 20}, UIObject::UP_CENTER);

    // RANKINGS MENU:
    Texture2D LevelClearedTexture = LoadTexture("Assets/Sprites/Cleared-stamp.png"), OldLevelClearedTexture = LoadTexture("Assets/Sprites/Old-Cleared-stamp.png");
    int level_ranking = 0, levels = 4;
    UIText Level1RankingTitle(NES, "LEVEL 1: BASICS", 33, 1, {GetScreenWidth()/2.0f, 30}, UIObject::CENTER);
    UIText Level2RankingTitle(NES, "LEVEL 2: ADVANCED MECHANICS", 33, 1, {GetScreenWidth()/2.0f, 30}, UIObject::CENTER);
    UIText Level3RankingTitle(NES, "LEVEL 3: TRYING TO SURVIVE", 33, 1, {GetScreenWidth()/2.0f, 30}, UIObject::CENTER);
    UIText Level4RankingTitle(NES, "LEVEL 4: A NEW CHALLENGER", 33, 1, {GetScreenWidth()/2.0f, 30}, UIObject::CENTER);
    UIText NoImplementedRanking(NES, "NO TIME FOR THIS ONE :(", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, UIObject::CENTER);
    UIText RankingsDeleteText(NES, "DELETE RANKINGS", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 145}, UIObject::DOWN_CENTER);
    UIText RankingsReturnText(NES, "RETURN", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 205}, UIObject::DOWN_CENTER);
    
    // 1st player rankings:
    UIText RankingsP1(NES, "P1", 33, 1, {GetScreenWidth()/4.0f, GetScreenHeight()/8.0f + 20}, UIObject::DOWN_CENTER);
    UISprite RankingP1Ch("Assets/Sprites/Popo/09_Celebrate_Single.png", {GetScreenWidth()/4.0f + 10, RankingsP1.pos.y + RankingsP1.size.y/2.0f + 20}, 4.0f, 4.0f, UIObject::UP_CENTER);
    
    UIText RankingsP1Total(NES, "TOTAL POINTS", 26, 1, {GetScreenWidth()/2.0f - 50, RankingsP1.pos.y + RankingsP1.size.y/2.0f}, UIObject::UP_CENTER);
    UIText RankingsP1NPts(NES, fill_string(std::to_string(P1_TOTAL_PTS[0]), 6), 35, 4, {GetScreenWidth()/2.0f - 10, RankingsP1.pos.y + RankingsP1.size.y/2.0f + 25}, UIObject::UP_CENTER);
    UIText RankingsP1PtsText(NES, "PTS", 34, 1, {RankingsP1NPts.pos.x + RankingsP1NPts.size.x + 5, RankingsP1.pos.y + RankingsP1.size.y/2.0f + 25});
    
    UIText RankingsP1SpeedRun(NES, "MIN SPEEDRUN TIME", 26, 1, {RankingsP1Total.pos.x + 4, RankingsP1.pos.y + RankingsP1.size.y/2.0f + 60});
    UIText RankingsP1NTime(NES, seconds_to_time(P1_TOTAL_SEC[0]), 35, 1, {GetScreenWidth()/2.0f - 10, RankingsP1.pos.y + RankingsP1.size.y/2.0f + 85}, UIObject::UP_CENTER);
    
    UISprite LevelClearedP1(LevelClearedTexture, {RankingsP1SpeedRun.pos.x + RankingsP1SpeedRun.size.x + 30, RankingsP1SpeedRun.pos.y - 5}, 1.5f, 1.5f, UIObject::CENTER_LEFT);
    UISprite OldLevelClearedP1(OldLevelClearedTexture, {RankingsP1SpeedRun.pos.x + RankingsP1SpeedRun.size.x + 30, RankingsP1SpeedRun.pos.y - 5}, 1.5f, 1.5f, UIObject::CENTER_LEFT);
    UIText LevelClearedP1Text(NES, "CLEARED!", 26, 1, {LevelClearedP1.dst.x + LevelClearedP1.dst.width/2.0f, LevelClearedP1.dst.y + LevelClearedP1.dst.height/2.0f}, UIObject::UP_CENTER);

    // 2nd player rankings:
    UIText RankingsP2(NES, "P2", 33, 1, {GetScreenWidth()/4.0f, GetScreenHeight() * (1/4.0f + 1/8.0f)}, UIObject::UP_CENTER);
    UISprite RankingP2Ch("Assets/Sprites/Nana/09_Celebrate_Single.png", {GetScreenWidth()/4.0f + 10, RankingsP2.pos.y + RankingsP2.size.y/2.0f + 20}, 4.0f, 4.0f, UIObject::UP_CENTER);
    
    UIText RankingsP2Total(NES, "TOTAL POINTS", 26, 1, {GetScreenWidth()/2.0f - 50, RankingsP2.pos.y + RankingsP2.size.y/2.0f}, UIObject::UP_CENTER);
    UIText RankingsP2NPts(NES, fill_string(std::to_string(P2_TOTAL_PTS[0]), 6), 35, 4, {GetScreenWidth()/2.0f - 10, RankingsP2.pos.y + RankingsP2.size.y/2.0f + 25}, UIObject::UP_CENTER);
    UIText RankingsP2PtsText(NES, "PTS", 34, 1, {RankingsP2NPts.pos.x + RankingsP2NPts.size.x + 5, RankingsP2.pos.y + RankingsP2.size.y/2.0f + 25});
    
    UIText RankingsP2SpeedRun(NES, "MIN SPEEDRUN TIME", 26, 1, {RankingsP2Total.pos.x + 4, RankingsP2.pos.y + RankingsP2.size.y/2.0f + 60});
    UIText RankingsP2NTime(NES, seconds_to_time(P2_TOTAL_SEC[0]), 35, 1, {GetScreenWidth()/2.0f - 10, RankingsP2.pos.y + RankingsP2.size.y/2.0f + 85}, UIObject::UP_CENTER);

    UISprite LevelClearedP2(LevelClearedTexture, {RankingsP2SpeedRun.pos.x + RankingsP2SpeedRun.size.x + 30, RankingsP2SpeedRun.pos.y - 5}, 1.5f, 1.5f, UIObject::CENTER_LEFT);
    UISprite OldLevelClearedP2(OldLevelClearedTexture, {RankingsP2SpeedRun.pos.x + RankingsP2SpeedRun.size.x + 30, RankingsP2SpeedRun.pos.y - 5}, 1.5f, 1.5f, UIObject::CENTER_LEFT);
    UIText LevelClearedP2Text(NES, "CLEARED!", 26, 1, {LevelClearedP2.dst.x + LevelClearedP2.dst.width/2.0f, LevelClearedP2.dst.y + LevelClearedP2.dst.height/2.0f}, UIObject::UP_CENTER);


    // ARE YOU SURE MENU:
    UIText AreYouSureText(NES, "ARE YOU SURE?!", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f - 20}, UIObject::DOWN_CENTER);
    UIText AreYouSureYes(NES, "YES", 40, 1, {GetScreenWidth()/2.0f - 60, GetScreenHeight()/2.0f + 10}, UIObject::UP_CENTER);
    UIText AreYouSureNo(NES, "NO", 40, 1, {GetScreenWidth()/2.0f + 60, GetScreenHeight()/2.0f + 10}, UIObject::UP_CENTER);

    // SETTINGS MENU:
    UIText AdvancedAIText(NES, "ADVANCED AI", 33, 1, {200, GetScreenHeight()/2.0f - 196}, UIObject::CENTER_LEFT);
    UISprite AdvancedAICheck(cross, {GetScreenWidth()/2.0f + 157, AdvancedAIText.pos.y - 3}, 2.0f, 2.0f);

    UIText OldStyleText(NES, "OLD STYLE?", 33, 1, {200, GetScreenHeight()/2.0f - 140}, UIObject::CENTER_LEFT);
    UISprite OldStyleCheck(cross, {GetScreenWidth()/2.0f + 157, OldStyleText.pos.y - 3}, 2.0f, 2.0f);
    
    UIText DisplayModeText(NES, "DISPLAY MODE", 33, 1, {200, GetScreenHeight()/2.0f - 84}, UIObject::CENTER_LEFT);
    UIText SelectedDisplayModeText(NES, to_string((DISPLAY_MODE_ENUM)std::get<int>(ini["Graphics"]["DisplayMode"])),
        25, 1, {GetScreenWidth()/2.0f + 170, DisplayModeText.pos.y}, UIObject::UP_CENTER);
    UISprite DisplayModeLArrow(LArrow, {GetScreenWidth()/2.0f + 60, DisplayModeText.pos.y}, 1.0f, 1.0f);
    UISprite DisplayModeRArrow(RArrow, {GetScreenWidth()/2.0f + 260, DisplayModeText.pos.y}, 1.0f, 1.0f);
    
    UIText ResolutionText(NES, "RESOLUTION", 33, 1, {200, GetScreenHeight()/2.0f - 28}, UIObject::CENTER_LEFT);
    UISprite ResolutionLArrow(LArrow, {GetScreenWidth()/2.0f + 60, ResolutionText.pos.y}, 1.0f, 1.0f);
    UIText SelectedResolutionText(NES, std::to_string(std::get<int>(ini["Graphics"]["ScreenWidth"])) + "x" + std::to_string(std::get<int>(ini["Graphics"]["ScreenHeight"])), 
        25, 1, {GetScreenWidth()/2.0f + 170, ResolutionText.pos.y}, UIObject::UP_CENTER);
    UISprite ResolutionRArrow(RArrow, {GetScreenWidth()/2.0f + 260, ResolutionText.pos.y}, 1.0f, 1.0f);
    
    UIText FPSLimitText(NES, "FPS LIMIT", 33, 1, {200, GetScreenHeight()/2.0f + 28}, UIObject::CENTER_LEFT);
    UISprite FPSLimitLArrow(LArrow, {GetScreenWidth()/2.0f + 60, FPSLimitText.pos.y}, 1.0f, 1.0f);
    UIText SelectedFPSLimitText(NES, std::to_string(std::get<int>(ini["Graphics"]["FPSLimit"])), 25, 1, {GetScreenWidth()/2.0f + 170, FPSLimitText.pos.y}, UIObject::UP_CENTER);
    UISprite FPSLimitRArrow(RArrow, {GetScreenWidth()/2.0f + 260, FPSLimitText.pos.y}, 1.0f, 1.0f);
    
    UIText VolumeText(NES, "VOLUME", 33, 1, {200, GetScreenHeight()/2.0f + 84}, UIObject::CENTER_LEFT);
    UIText VolumePercentageText(NES, std::to_string((int)(std::get<float>(ini["Audio"]["Volume"]) * 100)), 27, 1, {715, VolumeText.pos.y + 15}, UIObject::CENTER);
    
    UIText ChangeKeybindingsText(NES, "CHANGE KEYBINDINGS", 33, 1, {200, GetScreenHeight()/2.0f + 140}, UIObject::CENTER_LEFT);
    UIText SettingsReturnText(NES, "RETURN", 33, 1, {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f + 196}, UIObject::CENTER);

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
    //UIText LeftActionKeybind(NES, controllers[PLAYER]->getActionBind(Controller::LEFT), 25, 1, {GetScreenWidth()/2.0f + 130, LeftActionText.pos.y}, UIObject::UP_CENTER);
    UISprite LeftActionKeybind(controllers[PLAYER]->getActionTexture(Controller::LEFT), {GetScreenWidth()/2.0f + 130, LeftActionText.pos.y + LeftActionText.size.y/2.0f}, 1.3f, 1.3f, UIObject::CENTER);

    UIText RightActionText(NES, to_string(Controller::RIGHT), 27, 1, {250, GetScreenHeight()/2.0f - 5}, UIObject::CENTER_LEFT);
    //UIText RightActionKeybind(NES, controllers[PLAYER]->getActionBind(Controller::RIGHT), 25, 1, {GetScreenWidth()/2.0f + 130, RightActionText.pos.y}, UIObject::UP_CENTER);
    UISprite RightActionKeybind(controllers[PLAYER]->getActionTexture(Controller::RIGHT), {GetScreenWidth()/2.0f + 130, RightActionText.pos.y + RightActionText.size.y/2.0f}, 1.3f, 1.3f, UIObject::CENTER);

    UIText DownActionText(NES, to_string(Controller::DOWN), 27, 1, {250, GetScreenHeight()/2.0f + 55}, UIObject::CENTER_LEFT);
    //UIText DownActionKeybind(NES, controllers[PLAYER]->getActionBind(Controller::DOWN), 25, 1, {GetScreenWidth()/2.0f + 130, DownActionText.pos.y}, UIObject::UP_CENTER);
    UISprite DownActionKeybind(controllers[PLAYER]->getActionTexture(Controller::DOWN), {GetScreenWidth()/2.0f + 130, DownActionText.pos.y + DownActionText.size.y/2.0f}, 1.3f, 1.3f, UIObject::CENTER);

    UIText UpActionText(NES, to_string(Controller::JUMP), 27, 1, {250, GetScreenHeight()/2.0f + 115}, UIObject::CENTER_LEFT);
    //UIText UpActionKeybind(NES, controllers[PLAYER]->getActionBind(Controller::JUMP), 25, 1, {GetScreenWidth()/2.0f + 130, UpActionText.pos.y}, UIObject::UP_CENTER);
    UISprite UpActionKeybind(controllers[PLAYER]->getActionTexture(Controller::JUMP), {GetScreenWidth()/2.0f + 130, UpActionText.pos.y + UpActionText.size.y/2.0f}, 1.3f, 1.3f, UIObject::CENTER);

    UIText AttackActionText(NES, to_string(Controller::ATTACK), 27, 1, {250, GetScreenHeight()/2.0f + 175}, UIObject::CENTER_LEFT);
    //UIText AttackActionKeybind(NES, controllers[PLAYER]->getActionBind(Controller::ATTACK), 25, 1, {GetScreenWidth()/2.0f + 130, AttackActionText.pos.y}, UIObject::UP_CENTER);
    UISprite AttackActionKeybind(controllers[PLAYER]->getActionTexture(Controller::ATTACK), {GetScreenWidth()/2.0f + 130, AttackActionText.pos.y + AttackActionText.size.y/2.0f}, 1.3f, 1.3f, UIObject::CENTER);

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
            {(PlayerText.pos.x-70)*640/900, PlayerText.pos.y*480/600},
            {(PlayerText.pos.x-70)*800/900, PlayerText.pos.y},
            {(PlayerText.pos.x-70), PlayerText.pos.y}, 
            {(PlayerText.pos.x-70)*1024/900, PlayerText.pos.y*768/600},
            {(PlayerText.pos.x-70)*1280/900, PlayerText.pos.y*720/600 - 5},
            {(PlayerText.pos.x-70)*1366/900, PlayerText.pos.y*768/600 - 5},
            {(PlayerText.pos.x-70)*1680/900, PlayerText.pos.y*1050/600 - 5},
            {(PlayerText.pos.x-70)*1920/900, PlayerText.pos.y*1080/600 - 15}
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
            {(StartText.pos.x-50)*640/900, StartText.pos.y*480/600},
            {(StartText.pos.x-50)*800/900, StartText.pos.y},
            {(StartText.pos.x-50), StartText.pos.y}, 
            {(StartText.pos.x-50)*1024/900, StartText.pos.y*768/600},
            {(StartText.pos.x-50)*1280/900, StartText.pos.y*720/600 - 5},
            {(StartText.pos.x-50)*1366/900, StartText.pos.y*768/600 - 5},
            {(StartText.pos.x-50)*1680/900, StartText.pos.y*1050/600 - 5},
            {(StartText.pos.x-50)*1920/900, StartText.pos.y*1080/600 - 15}
        }}, {4, {
            {(AdvancedAIText.pos.x-70)*640/900, (AdvancedAIText.pos.y+2)*480/600},
            {(AdvancedAIText.pos.x-70)*800/900, (AdvancedAIText.pos.y+2)},
            {(AdvancedAIText.pos.x-70), (AdvancedAIText.pos.y+2)}, 
            {(AdvancedAIText.pos.x-70)*1024/900, (AdvancedAIText.pos.y+2)*768/600},
            {(AdvancedAIText.pos.x-70)*1280/900, (AdvancedAIText.pos.y+2)*720/600 - 5},
            {(AdvancedAIText.pos.x-70)*1366/900, (AdvancedAIText.pos.y+2)*768/600 - 5},
            {(AdvancedAIText.pos.x-70)*1680/900, (AdvancedAIText.pos.y+2)*1050/600 - 5},
            {(AdvancedAIText.pos.x-70)*1920/900, (AdvancedAIText.pos.y+2)*1080/600 - 15}
        }}, {5, {
            {(RankingsDeleteText.pos.x-70)*640/900, (RankingsDeleteText.pos.y+2)*480/600},
            {(RankingsDeleteText.pos.x-70)*800/900, (RankingsDeleteText.pos.y+2)},
            {(RankingsDeleteText.pos.x-70), (RankingsDeleteText.pos.y+2)}, 
            {(RankingsDeleteText.pos.x-70)*1024/900, (RankingsDeleteText.pos.y+2)*768/600},
            {(RankingsDeleteText.pos.x-70)*1280/900, (RankingsDeleteText.pos.y+2)*720/600 - 5},
            {(RankingsDeleteText.pos.x-70)*1366/900, (RankingsDeleteText.pos.y+2)*768/600 - 5},
            {(RankingsDeleteText.pos.x-70)*1680/900, (RankingsDeleteText.pos.y+2)*1050/600 - 5},
            {(RankingsDeleteText.pos.x-70)*1920/900, (RankingsDeleteText.pos.y+2)*1080/600 - 15}
        }}, {6, {
            {(NormalModeText.pos.x-70)*640/900, (NormalModeText.pos.y+2)*480/600},
            {(NormalModeText.pos.x-70)*800/900, (NormalModeText.pos.y+2)},
            {(NormalModeText.pos.x-70), (NormalModeText.pos.y+2)}, 
            {(NormalModeText.pos.x-70)*1024/900, (NormalModeText.pos.y+2)*768/600},
            {(NormalModeText.pos.x-70)*1280/900, (NormalModeText.pos.y+2)*720/600 - 5},
            {(NormalModeText.pos.x-70)*1366/900, (NormalModeText.pos.y+2)*768/600 - 5},
            {(NormalModeText.pos.x-70)*1680/900, (NormalModeText.pos.y+2)*1050/600 - 5},
            {(NormalModeText.pos.x-70)*1920/900, (NormalModeText.pos.y+2)*1080/600 - 15}
        }}, {7, {
            {(AreYouSureYes.pos.x-60)*640/900, (AreYouSureYes.pos.y+2)*480/600},
            {(AreYouSureYes.pos.x-60)*800/900, (AreYouSureYes.pos.y+2)},
            {(AreYouSureYes.pos.x-60), (AreYouSureYes.pos.y+2)}, 
            {(AreYouSureYes.pos.x-60)*1024/900, (AreYouSureYes.pos.y+2)*768/600},
            {(AreYouSureYes.pos.x-60)*1280/900, (AreYouSureYes.pos.y+2)*720/600 - 5},
            {(AreYouSureYes.pos.x-60)*1366/900, (AreYouSureYes.pos.y+2)*768/600 - 5},
            {(AreYouSureYes.pos.x-60)*1680/900, (AreYouSureYes.pos.y+2)*1050/600 - 5},
            {(AreYouSureYes.pos.x-60)*1920/900, (AreYouSureYes.pos.y+2)*1080/600 - 15}
        }}
    };

    // Hammer offsets:
    // 0: MAIN MENU
    // 1: SETTINGS
    // 2:
    // 3:
    std::vector<int> HammerOffsets {
        60, 60, 60, 50, 56, 60, 60, 273
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

    // RANKING MENU (again)
    UIText SelectRankText(NES, "CHANGE RANKING", 15, 1, {DownKeyNormal.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite RightKeySelectRank(RightKey, {SelectRankText.pos.x - 10, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite LeftKeySelectRank(LeftKey, {RightKeySelectRank.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UIText ReturnTextRankings(NES, "RETURN", 15, 1, {LeftKeySelectRank.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite EscKeyRankings(EscKey, {ReturnTextRankings.pos.x - 10, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite ReturnKeyRankings(ReturnKey, {EscKeyRankings.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);

    UIText DpadSelectRankingText(NES, "CHANGE RANKING", 15, 1, {DpadDownNormal.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite DpadRightSelectRanking(DpadRight, {DpadSelectRankingText.pos.x - 10.0f, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
    UISprite DpadLeftSelectRanking(DpadLeft, {DpadRightSelectRanking.dst.x - 5, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
    UIText OTextRanking(NES, "RETURN", 15, 1, {DpadLeftSelectRanking.dst.x - 20, GetScreenHeight() - 22.0f}, UIObject::DOWN_RIGHT);
    UISprite OButtonRanking(OButton, {OTextRanking.pos.x - 10, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
    

    UISprite RightKeyAreYouSure(RightKey, {MoveTextNormal.pos.x - 10, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite LeftKeyAreYouSure(LeftKey, {UpKeyNormal.dst.x - 5, GetScreenHeight() - 17.0f}, 1.0f, 1.0f, UIObject::DOWN_RIGHT);
    UISprite DpadRightAreYouSure(DpadRight, {DpadMoveTextNormal.pos.x - 10, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);
    UISprite DpadLeftAreYouSure(DpadLeft, {DpadUpNormal.dst.x - 5, GetScreenHeight() - 17.0f}, 1.7f, 1.7f, UIObject::DOWN_RIGHT);



    // /*** Main Loop ***/ //
    int OPTION  = 0;
    int OPTIONS = 4;
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

        if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
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
        } else {
            OldBackground.Draw();
        }

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

                if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                    Sign.Draw();
                } else {
                    OldSign.Draw();
                }
                Copy.Draw();

                if (pscurrent_time < pschange_time) {
                    pscurrent_time += GetFrameTime();
                } else {
                    pscurrent_time = 0;
                    psshow = !psshow;
                }
                if (psshow) {
                    PressStart.Draw();
                }
            }
            if (KeyboardDetected() || GamepadDetected(0)) {
                if (intro_state++ == 2) {
                    CURRENT_MENU = MAIN_MENU;
                    if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                        PlayMusicStream(MainTitleOST);
                    } else {
                        PlayMusicStream(OldMainTitleOst);
                    }
                }
            }

        } else {
            if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                PlayMusicStream(MainTitleOST);
                UpdateMusicStream(MainTitleOST);
            } else {
                PlayMusicStream(OldMainTitleOst);
                UpdateMusicStream(OldMainTitleOst);
            }

            if (CURRENT_MENU == MAIN_MENU) {
                if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                    Sign.Draw();
                } else {
                    OldSign.Draw();
                }
                NewGameText.Draw();
                Rankings.Draw();
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
                        OPTIONS = 3;
                        HV = 6;
                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y});
                    } else {
                        if (OPTION == 1) {
                            CURRENT_MENU = RANKINGS;
                            OPTION = 0;
                            OPTIONS = 2;
                            HV = 5;
                        }
                        else if (OPTION == 2) {
                            CURRENT_MENU = SETTINGS;
                            OPTION = 0;
                            OPTIONS = 8;
                            HV = 4;
                        } else if (OPTION == 3) {
                            break;
                        }
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
                        Level2.Draw();
                        Level2Text.Draw();
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
                            if (selected_level < 2) {
                                OPTION  = 0;
                                CONFIGURE_MATCH = true;
                                BLOCK = false;
                            }
                        }
                    } else {
                        auto menuv_x = GetScreenWidth() - (20*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF) - (400*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF);
                        auto menuv_y = GetScreenHeight()/2.0f;
                        DrawRectangleV({menuv_x,menuv_y}, {400*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 250*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, Fade(BLACK, 0.65));
                        StartText.Draw((selected_mod == 1 && !sudden_death_time) ? GRAY : PINK);
                        ModsText.SetText(mods[selected_mod]);
                        ModsText.Draw();
                        if (selected_mod == 1) {
                            ModsConfig.SetText(seconds_to_time(sudden_death_time));
                            ModsConfig.Draw();
                        } else {
                            ModsConfig.SetText("NO CONFIG NEEDED");
                            ModsConfig.Draw(GRAY);
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

                        if (show_error) {
                            SuddenDeathError.Draw(RED);
                        }

                        if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                            if (OPTION == 0) {
                                if (selected_mod == 1) {
                                    if (sudden_death_time == 0) {
                                        show_error = true;
                                    } else {
                                        show_error = false;
                                        Game(nplayers, selected_level, sudden_death_time);
                                        RankingsP1NPts.SetText(fill_string(std::to_string(P1_TOTAL_PTS[level_ranking]),6));
                                        RankingsP2NPts.SetText(fill_string(std::to_string(P2_TOTAL_PTS[level_ranking]),6));
                                        RankingsP1NTime.SetText(seconds_to_time(P1_TOTAL_SEC[level_ranking]));
                                        RankingsP2NTime.SetText(seconds_to_time(P2_TOTAL_SEC[level_ranking]));
                                    }
                                } else {
                                    show_error = false;
                                    Game(nplayers, selected_level, (selected_mod == 0) ? -1 : 0);
                                    RankingsP1NPts.SetText(fill_string(std::to_string(P1_TOTAL_PTS[level_ranking]),6));
                                    RankingsP2NPts.SetText(fill_string(std::to_string(P2_TOTAL_PTS[level_ranking]),6));
                                    RankingsP1NTime.SetText(seconds_to_time(P1_TOTAL_SEC[level_ranking]));
                                    RankingsP2NTime.SetText(seconds_to_time(P2_TOTAL_SEC[level_ranking]));
                                }
                            } else if (OPTION == 1) {
                                show_error = false;
                                selected_mod = (selected_mod+1)%3;
                            } else if (OPTION == 2) {
                                if (selected_mod == 1 && sudden_death_time < 3600) {
                                    show_error = false;
                                    sudden_death_time += 5;
                                }
                            } else if (OPTION == 3) {
                                show_error = false;
                                OLD_MENU = SELECT_LEVEL;
                                CURRENT_MENU = SETTINGS;
                                OPTION = 0;
                                OPTIONS = 8;
                                HV = 4;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y}); 
                            }
                        } else if (IsKeyPressed(KEY_RIGHT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                            if (OPTION == 1) {
                                show_error = false;
                                selected_mod = (selected_mod+1)%3;
                            } 
                        } else if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                            if (OPTION == 1) {
                                show_error = false;
                                selected_mod = (selected_mod+2)%3;
                            }
                        } else if (IsKeyDown(KEY_RIGHT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                            if (OPTION == 2) {
                                if (selected_mod == 1 && sudden_death_time < 3600) {
                                    show_error = false;
                                    sudden_death_time += 5;
                                }
                            }
                        } else if (IsKeyDown(KEY_LEFT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                            if (OPTION == 2) {
                                if (selected_mod == 1 && sudden_death_time > 0) {
                                    show_error = false;
                                    sudden_death_time -= 5;
                                }
                            }
                        } else if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            show_error = false;
                            BLOCK = true;
                            CONFIGURE_MATCH = false;
                            selected_mod = 0;
                            sudden_death_time = 0;
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
                                    OPTIONS = 4;
                                    HV = 0;
                                }
                                OPTION = 0;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y}); 
                            }
                        } else if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            CURRENT_MENU = MAIN_MENU;
                            OPTIONS = 4;
                            OPTION = 0;
                            HV = 0;
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
                            if (nplayers < 2) {
                                nplayers++;
                            }
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
                                HV = 6;
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
                                HV = 6;
                                BLOCK = false;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y});
                            }
                            p1current_time = p2current_time = 0;
                            p1show = p2show = false;
                        }
                    } else if (CURRENT_MENU == RANKINGS || CURRENT_MENU == ARE_YOU_SURE) {

                        if (level_ranking < 2) {
                            if (!level_ranking) {
                                Level1RankingTitle.Draw();
                            } else {
                                Level2RankingTitle.Draw();
                            }
                            // P1 Rankings:
                            RankingsP1.Draw();
                            RankingP1Ch.Draw();
                            RankingsP1Total.Draw();
                            RankingsP1NPts.Draw(DEEPSAFFRON);
                            RankingsP1PtsText.Draw(DEEPSAFFRON);
                            RankingsP1SpeedRun.Draw();
                            RankingsP1NTime.Draw(DEEPSAFFRON);
                            if (P1_HAS_PASSED[level_ranking]) {
                                if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                                    LevelClearedP1.Draw();
                                } else {
                                    OldLevelClearedP1.Draw();
                                }
                                LevelClearedP1Text.Draw();
                            }

                            // P2 Rankings:
                            RankingsP2.Draw();
                            RankingP2Ch.Draw();
                            RankingsP2Total.Draw();
                            RankingsP2NPts.Draw(DEEPSAFFRON);
                            RankingsP2PtsText.Draw(DEEPSAFFRON);
                            RankingsP2SpeedRun.Draw();
                            RankingsP2NTime.Draw(DEEPSAFFRON);
                            if (P2_HAS_PASSED[level_ranking]) {
                                if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                                    LevelClearedP2.Draw();
                                } else {
                                    OldLevelClearedP2.Draw();
                                }
                                LevelClearedP2Text.Draw();
                            }
                        } else {
                            if (level_ranking == 2) {
                                Level3RankingTitle.Draw();
                            } else {
                                Level4RankingTitle.Draw();
                            }
                            NoImplementedRanking.Draw();
                        }

                        // Menu options:
                        RankingsDeleteText.Draw(RED);
                        RankingsReturnText.Draw(BLUE);

                        if (CURRENT_MENU == RANKINGS) {

                            if (keyboard) {
                                MoveTextNormal.Draw();
                                UpKeyNormal.Draw();
                                DownKeyNormal.Draw();
                                SelectRankText.Draw();
                                RightKeySelectRank.Draw();
                                LeftKeySelectRank.Draw();
                                ReturnTextRankings.Draw();
                                EscKeyRankings.Draw();
                                ReturnKeyRankings.Draw();
                            } else {
                                DpadMoveTextNormal.Draw();
                                DpadUpNormal.Draw();
                                DpadDownNormal.Draw();
                                DpadSelectRankingText.Draw();
                                DpadRightSelectRanking.Draw();
                                DpadLeftSelectRanking.Draw();
                                OTextRanking.Draw();
                                OButtonRanking.Draw();
                            }

                            if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ) {
                                if (OPTION == 0) {
                                    CURRENT_MENU = ARE_YOU_SURE;
                                    HV = 7;
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y});
                                } else {
                                    CURRENT_MENU = MAIN_MENU;
                                    OPTIONS = 4;
                                    OPTION = 1;
                                    HV = 0;
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                                }
                            } else if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                                level_ranking = mod(level_ranking-1, levels);
                                RankingsP1NPts.SetText(fill_string(std::to_string(P1_TOTAL_PTS[level_ranking]),6));
                                RankingsP2NPts.SetText(fill_string(std::to_string(P2_TOTAL_PTS[level_ranking]),6));
                                RankingsP1NTime.SetText(seconds_to_time(P1_TOTAL_SEC[level_ranking]));
                                RankingsP2NTime.SetText(seconds_to_time(P2_TOTAL_SEC[level_ranking]));
                            } else if (IsKeyPressed(KEY_RIGHT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                                level_ranking = mod(level_ranking+1, levels);
                                RankingsP1NPts.SetText(fill_string(std::to_string(P1_TOTAL_PTS[level_ranking]),6));
                                RankingsP2NPts.SetText(fill_string(std::to_string(P2_TOTAL_PTS[level_ranking]),6));
                                RankingsP1NTime.SetText(seconds_to_time(P1_TOTAL_SEC[level_ranking]));
                                RankingsP2NTime.SetText(seconds_to_time(P2_TOTAL_SEC[level_ranking]));
                            } else if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                                CURRENT_MENU = MAIN_MENU;
                                OPTIONS = 4;
                                OPTION = 1;
                                HV = 0;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                            }
                        } else {
                            if (keyboard) {
                                MoveTextNormal.Draw();
                                LeftKeyAreYouSure.Draw();
                                RightKeyAreYouSure.Draw();

                                ReturnTextNormal.Draw();
                                EscKeyNormal.Draw();
                                ReturnKeyNormal.Draw();
                            } else {
                                DpadMoveTextNormal.Draw();
                                DpadLeftAreYouSure.Draw();
                                DpadRightAreYouSure.Draw();
                                OTextNormal.Draw();
                                OButtonNormal.Draw();
                            }

                            DrawRectanglePro({GetScreenWidth()/2.0f - 190 * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, GetScreenHeight()/2.0f - 90 * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 380 * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 180 * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, {0,0}, 0, Fade(BLACK, 0.9));
                            AreYouSureText.Draw();
                            AreYouSureYes.Draw(RED);
                            AreYouSureNo.Draw();

                            if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ) {
                                if (OPTION == 0) {
                                    CURRENT_MENU = RANKINGS;
                                    HV = 5;
                                    backup_sav();
                                    P1_TOTAL_PTS = P1_TOTAL_SEC = P1_HAS_PASSED = P2_TOTAL_PTS = P2_TOTAL_SEC = P2_HAS_PASSED = {0,0,0,0};
                                    save_sav();
                                    RankingsP1NPts.SetText(fill_string(std::to_string(P1_TOTAL_PTS[level_ranking]),6));
                                    RankingsP2NPts.SetText(fill_string(std::to_string(P2_TOTAL_PTS[level_ranking]),6));
                                    RankingsP1NTime.SetText(seconds_to_time(P1_TOTAL_SEC[level_ranking]));
                                    RankingsP2NTime.SetText(seconds_to_time(P2_TOTAL_SEC[level_ranking]));
                                } else {
                                    CURRENT_MENU = RANKINGS;
                                    OPTION = 0;
                                    HV = 5;
                                }
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF}); 
                            } else if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ) {
                                CURRENT_MENU = RANKINGS;
                                OPTION = 0;
                                HV = 5;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF}); 
                            }
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
                            if (OLD_MENU == SELECT_LEVEL) {
                                CURRENT_MENU = SELECT_LEVEL;
                                OPTION = 3;
                                OPTIONS = 4;
                                HV = 3;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF}); 
                            } else {
                                CURRENT_MENU = MAIN_MENU;
                                OPTION = 2;
                                OPTIONS = 4;
                                HV = 0;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                            }
                        	save_config();
                        } else {
                            if (OPTION == 0) {
                                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                                    ini["Game"]["AdvancedAI"] = !std::get<bool>(ini["Game"]["AdvancedAI"]);
                                }
                            } else if (OPTION == 1) {
                                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                                    ini["Graphics"]["OldFashioned"] = !std::get<bool>(ini["Graphics"]["OldFashioned"]);
                                }
                            } else if (OPTION == 2) {
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
                            } else if (OPTION == 3) {
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
                            } else if (OPTION == 4) {
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
                            } else if (OPTION == 5) {
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
                            } else if (OPTION == 6) {
                                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                                    CURRENT_MENU = CONTROL_SETTINGS;
                                    OPTIONS = 7;
                                    OPTION  = 0;
                                    HV = 1;
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y});
                                }
                            } else if (OPTION == 7) {
                                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                                    if (OLD_MENU == SELECT_LEVEL) {
                                        CURRENT_MENU = SELECT_LEVEL;
                                        OPTION = 3;
                                        OPTIONS = 4;
                                        HV = 3;
                                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF}); 
                                    } else {
                                        CURRENT_MENU = MAIN_MENU;
                                        OPTION = 2;
                                        OPTIONS = 4;
                                        HV = 0;
                                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                                    }
                                	save_config();
                                }
                            }
                        }

                        // - ADVANCED AI OPTION:
                        AdvancedAIText.Draw();
                        DrawRectangleV({ResolutionLArrow.dst.x + (100*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF), AdvancedAIText.pos.y+(2*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {20*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 20*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, WHITE);
                        DrawRectangleV({ResolutionLArrow.dst.x + (102*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF), AdvancedAIText.pos.y+(4*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {15*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 15*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, BLACK);
                        if (std::get<bool>(ini["Game"]["AdvancedAI"])) {
                            AdvancedAICheck.Draw();
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
                                LeftActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::LEFT), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                RightActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::RIGHT), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                DownActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::DOWN), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                UpActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::JUMP), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                AttackActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::ATTACK), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                            } else if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                                PLAYER = mod(PLAYER-1, 4);
                                SelectedPlayer.SetText(std::to_string(PLAYER+1));
                                CONTROLLER = controllers[PLAYER]->type;
                                SelectedController.SetText(to_string((Controller::Type)CONTROLLER));
                                LeftActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::LEFT), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                RightActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::RIGHT), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                DownActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::DOWN), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                UpActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::JUMP), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                AttackActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::ATTACK), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                            }
                        } else if (OPTION == 1) {
                            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                                CONTROLLER = mod(CONTROLLER+1,5);
                                SelectedController.SetText(to_string((Controller::Type)CONTROLLER));
                                controllers[PLAYER]->type = (Controller::Type)CONTROLLER;
                                LeftActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::LEFT), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                RightActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::RIGHT), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                DownActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::DOWN), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                UpActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::JUMP), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                AttackActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::ATTACK), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                            } else if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                                CONTROLLER = mod(CONTROLLER-1,5);
                                SelectedController.SetText(to_string((Controller::Type)CONTROLLER));
                                controllers[PLAYER]->type = (Controller::Type)CONTROLLER;
                                LeftActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::LEFT), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                RightActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::RIGHT), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                DownActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::DOWN), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                UpActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::JUMP), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                AttackActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::ATTACK), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
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
                                LeftActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::LEFT), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                RightActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::RIGHT), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                DownActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::DOWN), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                UpActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::JUMP), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
                                AttackActionKeybind.SetTexture(controllers[PLAYER]->getActionTexture(Controller::ATTACK), (controllers[PLAYER]->type == Controller::Type::KEYBOARD) ? Vector2{1.3f, 1.3f} : Vector2{2.0f, 2.0f});
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
                            LeftActionKeybind.Draw();
                        }
                        // Move Right:
                        if (SELECTED && currAction == 1) {
                            RightActionText.Draw(RED);
                        } else {
                            RightActionText.Draw(GREEN);
                            RightActionKeybind.Draw();
                        }
                        // Crouch:
                        if (SELECTED && currAction == 2) {
                            DownActionText.Draw(RED);
                        } else {
                            DownActionText.Draw(GREEN);
                            DownActionKeybind.Draw();
                        }
                        // Jump:
                        if (SELECTED && currAction == 3) {
                            UpActionText.Draw(RED);
                        } else {
                            UpActionText.Draw(GREEN);
                            UpActionKeybind.Draw();
                        }
                        // Attack:
                        if (SELECTED && currAction == 4) {
                            AttackActionText.Draw(RED);
                        } else {
                            AttackActionText.Draw(GREEN);
                            AttackActionKeybind.Draw();
                        }

                        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                            if (!SELECTED) {
                                CURRENT_MENU = SETTINGS;
                                OPTION = 6;
                                OPTIONS = 8;
                                HV = 4;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF}); 
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
                if (CURRENT_MENU != ARE_YOU_SURE) {
                    if (IsKeyPressed(KEY_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
                        OPTION = mod(OPTION+1, OPTIONS);
                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                    }
                    if (IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
                        OPTION = mod(OPTION-1, OPTIONS);
                        Hammer.Translate({Hammer.dst.x, Hammer.dst.y - 20});
                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + (OPTIONS - (OPTIONS-OPTION))*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                    }
                } else if (CURRENT_MENU == ARE_YOU_SURE) {
                    if (IsKeyPressed(KEY_RIGHT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                        OPTION = mod(OPTION+1, OPTIONS);
                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x + OPTION*HammerOffsets[HV]*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, HammerRefs[HV][RESOLUTION_OPTION].y});
                    }
                    if (IsKeyPressed(KEY_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                        OPTION = mod(OPTION-1, OPTIONS);
                        Hammer.Translate({Hammer.dst.x, Hammer.dst.y - 20});
                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x + (OPTIONS - (OPTIONS-OPTION))*HammerOffsets[HV]*GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, HammerRefs[HV][RESOLUTION_OPTION].y});
                    }
                }
            }
        }
        /*
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
        */
        EndDrawing();

    }
	save_config();
    UnloadMusicStream(MainTitleOST);
    UnloadFont(NES);
    UISystem::RemoveAll();
}
