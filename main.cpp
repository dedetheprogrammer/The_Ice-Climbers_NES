#include "EngineECS.h"
#include "settings.h"
#include "Popo.h"
#include "Grass_block.h"

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

void Game() {

    //MusicSource BGM("Assets/NES - Ice Climber - Sound Effects/Go Go Go - Nightcore.mp3", true);
    MusicSource BGM("Assets/Sounds/Mick Gordon - The Only Thing They Fear Is You.mp3", true);

    // ¿Como construyo un GameObject para Popo?
    // 1. Creamos el GameObject. Recuerda:
    //  - El GameObject no tiene ningún componente nada más crearlo.
    //  - El GameObject solo puede tener un elemento de cada tipo. Si le vuelves 
    //    a meter otro, perderá el primero.
    GameObject Popo("Popo", "Player", {}, {"Floor"});
    // 2.a Añadimos el componente Transform. Es muy importante este componente ya que es el que indica las propiedades
    //  del objeto, como posicion, tamaño o rotación. De momento solo usamos tamaño.
    Popo.addComponent<Transform2D>();
    // 2.b. Se podría haber ahorrado el addComponent<Transform2D> y crearlo en el GameObject directamente:
    // GameObject Popo(Vector2{600,500});
    // 3. Añadimos el componente de Animaciones. Como veis, hay que indicarle de que tipo es la lista {...},
    // si no, dará error.
    Popo.addComponent<Animator>("Idle", std::unordered_map<std::string, Animation> {
        {"Idle", Animation("Assets/Sprites/Popo/00_Idle.png", 16, 24, 3, 0.75, true)},
        {"Walk", Animation("Assets/Sprites/Popo/02_Walk.png", 16, 24, 3, 0.135, true)},
        {"Brake", Animation("Assets/Sprites/Popo/03_Brake.png", 16, 24, 3, 0.3, true)},
        {"Jump", Animation("Assets/Sprites/Popo/04_Jump.png", 20, 25, 3, 0.9, false)},
        {"Attack", Animation("Assets/Sprites/Popo/05_Attack.png", 21, 25, 3, 0.3, false)},
        //{"Crouch", Animation("Assets/Sprites/Popo/06_Crouch.png", 0,0,0,0, false)},
    });
    // 3. Añadimos el componente de Audio:
    Popo.addComponent<AudioPlayer>(std::unordered_map<std::string, std::shared_ptr<AudioSource>> {
        {"Jump", std::make_shared<SoundSource>(SoundSource("Assets/Sounds/09-Jump.wav"))},
    });
    // 4. Añadimos el Rigidbody:
    Popo.addComponent<RigidBody2D>(1, 98, Vector2{100,0}, Vector2{100,190});
    // 5. Añadimos el Collider. Este es el componente más jodido, necesitas:
    //  - El Transform2D que tiene la posición del objeto.
    //  - El Animator que tiene el tamaño del sprite según en que animación esté, en este
    //    caso, es la animación inicial.
    Popo.addComponent<Collider2D>(&Popo.getComponent<Transform2D>().position, Popo.getComponent<Animator>().GetViewDimensions());
    Popo.addComponent<Script, Movement>();
    GameSystem::Instantiate(Popo, GameObjectOptions{.position = {400,450}});

    // Rectangles = Sprites component?
    // Mountain background:
    Texture2D Mountain_sprite = LoadTexture("Assets/Sprites/00_Mountain.png");
    float Mountain_view_height = (Mountain_sprite.width * WINDOW_HEIGHT)/(float)WINDOW_WIDTH + 40;
    Rectangle Mountain_src{0, Mountain_sprite.height - Mountain_view_height - 10, (float)Mountain_sprite.width, Mountain_view_height};
    Rectangle Mountain_dst{0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
    
    // PAUSE frame:
    Texture2D Pause_frame = LoadTexture("Assets/Sprites/Small_frame.png");
    float paused_showtime = 0.75;
    bool show = true;
    Rectangle src_0{0, 0, (float)Pause_frame.width, (float)Pause_frame.height};
    Rectangle dst_1{(WINDOW_WIDTH - Pause_frame.width*3.0f)/2.0f + 4, (WINDOW_HEIGHT - Pause_frame.height)/2.0f - 3, Pause_frame.width*3.0f, Pause_frame.height*3.0f};
    
    GameObject Floor("Base Floor", "Floor");
    Floor.addComponent<Transform2D>();
    Floor.addComponent<Collider2D>(&Floor.getComponent<Transform2D>().position, Vector2{1224, 30}, PINK);
    GameSystem::Instantiate(Floor, GameObjectOptions{.position{-100,560}});

    GameObject Block("Grass Block", "Floor");
    Block.addComponent<Transform2D>();
    Block.addComponent<Sprite>("Assets/Sprites/Grass_block_large.png", Vector2{3.62f, 3.0f});
    int block_width = Block.getComponent<Sprite>().GetViewDimensions().x;
    Block.addComponent<Collider2D>(&Block.getComponent<Transform2D>().position, Block.getComponent<Sprite>().GetViewDimensions(), Color{20,200,20,255});
    Block.addComponent<Script, GrassBlockBehavior>();
    for (int i = 0; i < 24; i++) {
        GameSystem::Instantiate(Block, GameObjectOptions{.position{113.0f + block_width * i, 423}});
    }
    
    GameSystem::Printout();
    bool play_music = false;
    bool paused = false;
    BGM.Init();

    // GameSystem::Printout();

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        if (IsKeyPressed(KEY_M)) {
            play_music = !play_music;
        }
        if (play_music) {
            BGM.Play();
        }
    
        DrawTexturePro(Mountain_sprite, Mountain_src, Mountain_dst, Vector2{0,0}, 0, WHITE);
        if (IsGamepadAvailable(0)) {
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                paused = !paused;
            }
        } else if (IsKeyPressed(KEY_BACKSPACE)){
            paused = !paused;
        }
        if (!paused) {
            GameSystem::Update();
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
            Popo.getComponent<Transform2D>().position = Vector2{600,400};
        }
        DrawText("Press [M] to mute the music", 20, 20, 20, WHITE);
        EndDrawing();
    }
    UnloadTexture(Mountain_sprite);
    UnloadTexture(Pause_frame);
    Popo.Destroy();
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
    NES = LoadFont("Assets/NES - Ice Climber - Fonts/Pixel_NES.otf");

    // ---- Music
    Music ts_music = LoadMusicStream("Assets/NES - Ice Climber - Sound Effects/01-Main-Title.mp3");
    ts_music.looping = true;
    // bool play_music = false;

    // Initial trailer --------------------------------------------------------
    // int state = 0, shown = 0;
    /*
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
    */
    Game();
    /*
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

        if (play_music) {
            UpdateMusicStream(ts_music);
        }

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
                            PlayMusicStream(ts_music);
                        }
                        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                            SignDst.y = height_dst;
                            CopyDst.y = copy_height_dst;
                            show_background = true;
                            full_black_fade = 0;
                            first_enter = true;
                            PlayMusicStream(ts_music);
                            play_music = true;
                        }
                    }
                } else {
                    DrawTexturePro(OldSign, OldSignSrc, OldSignDst, {0,0}, 0, WHITE);
                    if (!show_background) {
                        if (OldSignDst.y < height_dst) {
                            OldSignDst.y += SignSpeed * deltaTime;
                        } else {
                            show_background = true;
                            PlayMusicStream(ts_music);
                            play_music = true;
                        }
                        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                            OldSignDst.y = height_dst;
                            CopyDst.y = copy_height_dst;
                            show_background = true;
                            full_black_fade = 0;
                            first_enter = true;
                            PlayMusicStream(ts_music);
                            play_music = true;
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
                        if (fst_player) {
                            StopMusicStream(ts_music);
                            Game();
                            PlayMusicStream(ts_music);
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
                DrawTextEx(NES, "CONTROLS", {500, menu_start + (option_offset * 3.0f)}, 35, 2, GRAY);
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
                    case 1: case 2:
                        std::cout << "Hola\n";
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
            case AUDIO_SETTINGS: case CONTROL_SETTINGS:
                break;
            }

            if (IsKeyPressed(KEY_DOWN)) {
                current_option = ((current_option+1)%OPTIONS);
            }
            if (IsKeyPressed(KEY_UP)) {
                current_option = ((current_option-1)%OPTIONS + OPTIONS) % OPTIONS;
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
    */
    UnloadFont(NES);
    UnloadMusicStream(ts_music);
    CloseAudioDevice();
    save_config();
}