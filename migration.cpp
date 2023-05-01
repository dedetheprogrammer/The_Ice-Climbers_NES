#include "EngineECS.h"
#include "EngineUI.h"
#include "raylibx.h"
#include "settings.h"

enum MENU_NAVIGATION { 
    MAIN_MENU,
    NEW_GAME,
    NORMAL_GAME,
    SETTINGS,
    VIDEO_SETTINGS,
    AUDIO_SETTINGS,
    CONTROL_SETTINGS
};

Font NES;
int main() {

    init_config();
    UISystem::init_UI_system(900, 600);
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<float> D(0, (float)GetScreenWidth());
    std::uniform_int_distribution<int> I(0,1);

    //MusicSource Main_title_music("Assets/Sounds/02-Main-Title.wav", true);
    Music MainTitleOST = LoadMusicStream("Assets/Sounds/02-Main-Title.mp3");
    MainTitleOST.looping = true; 
    NES = LoadFont("Assets/Fonts/Pixel_NES.otf");

    // De momento no habrá introducción, tengo que apañar los efectos...
    /*
    int intro_state = 0;
    // - Fore pines:
    Texture2D ForePinesTexture = LoadTexture("Assets/Sprites/Titlescreen/01_Fore_Pines.png");
    float ForePinesHeight = (GetScreenWidth() * ForePinesTexture.height)/(float)(ForePinesTexture.width);
    UISprite ForePines(ForePinesTexture, "Fore Pines", Vector2{0, GetScreenHeight() - ForePinesHeight}, {(float)GetScreenWidth(), ForePinesHeight});
    //ForePines.setEffect<Parallax>(Vector2{15,0});
    // - Mid pines
    Texture2D MidPinesTexture  = LoadTexture("Assets/Sprites/Titlescreen/02_Mid_Pines.png");
    float MidPinesHeight = (GetScreenWidth() * MidPinesTexture.height)/(float)(MidPinesTexture.width);
    UISprite MidPines(MidPinesTexture, "Mid Pines", Vector2{0, GetScreenHeight() - MidPinesHeight + 5}, {(float)GetScreenWidth(), MidPinesHeight});
    //MidPines.setEffect<Parallax>(Vector2{10,0});
    // - Mountain
    Texture2D MountainTexture = LoadTexture("Assets/Sprites/Titlescreen/03_Mountain.png");
    float MountainHeight = (GetScreenWidth() * MountainTexture.height)/(float)(MountainTexture.width);
    UISprite Mountain(MountainTexture, "Mountain", Vector2{D(e2), GetScreenHeight() - (MountainHeight)}, {(float)GetScreenWidth(), MountainHeight});
    //Mountain.setEffect<Scroll>(1, Vector2{-7.0, 0});
    // - Background fields
    Texture2D FieldsTexture = LoadTexture("Assets/Sprites/Titlescreen/04_Fields.png");
    float FieldsHeight = (GetScreenWidth() * FieldsTexture.height)/(float)(FieldsTexture.width);
    UISprite Fields(FieldsTexture, "Fields", Vector2{0, GetScreenHeight() - FieldsHeight}, {(float)GetScreenWidth(), FieldsHeight});
    // - Falling snow
    UISprite Snow("Assets/Sprites/Titlescreen/05_Snow.png", "Snow", Vector2{0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    //Snow.setEffect<Parallax>(Vector2{7,-7});
    // - Sign
    //bool sign_check = false;
    UISprite Sign("Assets/Sprites/Titlescreen/06_Sign.png", "Sign", Vector2{GetScreenWidth()/2.0f, -20}, 2.5f, 2.5f, UIObject::DOWN_CENTER);
    //Sign.setEffect<Move>(Vector2{0, 40 + Sign.dst.height}, Vector2{0, 100});

    */

    UISprite Sign("Assets/Sprites/Titlescreen/06_Sign.png", {GetScreenWidth()/2.0f, 20}, 2.7f, 2.7f, UIObject::UP_CENTER, false, 0.8);
    UISprite Copy("Assets/Sprites/Titlescreen/07_Copyright.png", Vector2{GetScreenWidth()/2.0f, GetScreenHeight()-20.0f}, 2.2f, 2.2f, UIObject::DOWN_CENTER);
    
    int background = I(e2);
    UISprite Background("Assets/Sprites/bg00.png", {0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    UISprite Background1("Assets/Sprites/bg01.png", {0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});
    UISprite Transparent("Assets/Sprites/UI_Transparent.png", {0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()});

    int mscroll_s = -5;
    UISprite Mountain("Assets/Sprites/Titlescreen/03_Mountain.png", Vector2{GetScreenWidth()/2.0f, (float)GetScreenHeight()}, 3.6f, 3.6f, UIObject::DOWN_CENTER);

    int fpparallax_s = 15;
    UISprite ForePines("Assets/Sprites/Titlescreen/01_Fore_Pines.png", Vector2{GetScreenWidth()/2.0f, (float)GetScreenHeight()}, {(float)GetScreenWidth(), 150}, UIObject::DOWN_CENTER);
    
    int mpparallax_s = 7;
    UISprite MidPines("Assets/Sprites/Titlescreen/02_Mid_Pines.png", Vector2{GetScreenWidth()/2.0f, (float)GetScreenHeight()}, {(float)GetScreenWidth(), 200}, UIObject::DOWN_CENTER);

    enum MENU_ENUM { INTRO, MAIN_MENU, NEW_GAME, NORMAL_GAME, SETTINGS, CONTROL_SETTINGS };
    // INTRO:
    int intro_state = 0;
    // - Nintendo logo:
    bool  nlinout     = false;
    float nlfadein_s  = 0.3;
    float nlfadeout_s = 0.9;
    float nl_opacity  = 0;
    UISprite Nintendo_logo("Assets/Sprites/Nintendo_logo.png", {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, 0.3f, 0.3f, UIObject::CENTER);
    //Nintendo_logo.setEffect<FadeInOut>(0.3, 0.9);

    // - Team logo:
    bool tlinout      = false;
    float tlfadein_s  = 0.3;
    float tlfadeout_s = 0.9;
    float tl_opacity  = 0;
    UISprite Team_logo("Assets/Sprites/Team_logo.png", {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, 2.8f, 2.8f, UIObject::CENTER);
    //Team_logo.setEffect<FadeInOut>(0.29, 0.9);

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

    // VIDEO SETTINGS MENU:
    Texture2D LArrow = LoadTexture("Assets/Sprites/UI_Arrow_left.png"), RArrow = LoadTexture("Assets/Sprites/UI_Arrow_right.png");
    UIText OldStyleText(NES, "OLD STYLE?", 33, 1, {200, GetScreenHeight()/2.0f - 180}, UIObject::CENTER_LEFT);
    UISprite OldStyleCheck("Assets/Sprites/UI_Cross.png", {GetScreenWidth()/2.0f + 157, OldStyleText.pos.y - 3}, 2.0f, 2.0f);
    
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
    UIText PlayerText(NES, "PLAYER", 33, 1, {200, GetScreenHeight()/2.0f - 185}, UIObject::CENTER_LEFT);
    UISprite PlayerLArrow(LArrow, {GetScreenWidth()/2.0f + 60, PlayerText.pos.y}, 1.0f, 1.0f);
    UISprite PlayerRArrow(RArrow, {GetScreenWidth()/2.0f + 260, PlayerText.pos.y}, 1.0f, 1.0f);

    UIText ControllerText(NES, "CONTROLLER", 33, 1, {200, GetScreenHeight()/2.0f - 125}, UIObject::CENTER_LEFT);
    UISprite ControllerLArrow(LArrow, {GetScreenWidth()/2.0f + 60, ControllerText.pos.y}, 1.0f, 1.0f);
    UISprite ControllerRArrow(RArrow, {GetScreenWidth()/2.0f + 260, ControllerText.pos.y}, 1.0f, 1.0f);

    // GENERAL UI:
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
        }}
    };
    std::vector<int> HammerOffsets {
        60, 60, 60
    };

    // /*** Main Loop ***/ //
    int OPTION  = 0;
    int OPTIONS = 3;
    MENU_ENUM CURRENT_MENU = INTRO;
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

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
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (intro_state++ == 2) {
                    CURRENT_MENU = MAIN_MENU;
                    PlayMusicStream(MainTitleOST);
                }
            }

        } else {
            UpdateMusicStream(MainTitleOST);

            if (CURRENT_MENU == MAIN_MENU) {
                Sign.Draw();
                Copy.Draw();
                NewGameText.Draw();
                SettingsText.Draw();
                ExitText.Draw(BLUE);

                if (IsKeyPressed(KEY_ENTER)) {
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
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    break; 
                }

            } else {
                // - BACKGROUND:
                DrawRectangleV({0,0}, {(float)GetScreenWidth(), (float)GetScreenHeight()}, Fade(BLACK, 0.65));
                auto new_height = 50.0f * GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF;
                DrawRectangleV({0,GetScreenHeight() - new_height}, {(float)GetScreenWidth(), new_height}, Fade(BLACK, 0.65));

                if (CURRENT_MENU == NEW_GAME) {

                    NormalModeText.Draw(WHITE);
                    BrawlModeText.Draw(GRAY);
                    NewGameReturnText.Draw(BLUE);

                    if (IsKeyPressed(KEY_ENTER)) {
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
                    } else if (IsKeyPressed(KEY_ESCAPE)) {
                        CURRENT_MENU = MAIN_MENU;
                        OPTION = 0;
                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y}); 
                    }

                } else if (CURRENT_MENU == NORMAL_GAME) {
                    //DrawRectangleV({GetScreenWidth()/2.0f - (150 * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF), GetScreenHeight()/2.0f - (120 * GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {120 * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 140 * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, GRAY);
                    //DrawRectangleV({GetScreenWidth()/2.0f + (30  * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF), GetScreenHeight()/2.0f - (120 * GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF)}, {120 * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 140 * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF}, GRAY);
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
                            P1Space.Draw();
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
                            P2Space.Draw();
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
                                P2Space.Draw();
                            }
                        } else {
                            P2.Draw(WHITE);
                            P2Char.Draw();
                        }
                    }

                    NormalGameReturnText.Draw(BLUE);
                    if (IsKeyPressed(KEY_SPACE)) {
                        p1current_time = p2current_time = 0;
                        p1show = p2show = false;
                        nplayers++;
                    } else if (IsKeyPressed(KEY_ENTER)) {
                        if (OPTION == 0) {
                            if (nplayers > 0) {
                                std::cout << "PARA TI ALBERTO.\n";
                                //Game();
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
                    } else if (IsKeyPressed(KEY_ESCAPE)) {
                        if (nplayers > 0) {
                            nplayers--;
                        } else {
                            CURRENT_MENU = NEW_GAME;
                            OPTION  = 0;
                            OPTIONS = 3;
                            HV = 0;
                            Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y});
                        }
                        p1current_time = p2current_time = 0;
                        p1show = p2show = false;
                    }
                } else if (CURRENT_MENU == SETTINGS) {
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
                    ResolutionText.Draw();
                    ResolutionLArrow.Draw();
                    SelectedResolutionText.Draw(JELLYFISH);
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
                    if (IsKeyPressed(KEY_ESCAPE)) {
                        CURRENT_MENU = MAIN_MENU;
                        OPTION = 1;
                        OPTIONS = 3;
                        HV = 0;
                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                    } else {
                        if (OPTION == 0) {
                            if (IsKeyPressed(KEY_ENTER)) {
                                ini["Graphics"]["OldFashioned"] = !std::get<bool>(ini["Graphics"]["OldFashioned"]);
                            }
                        } else if (OPTION == 1) {
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
                                SelectedDisplayModeText.text = to_string((DISPLAY_MODE_ENUM)DISPLAY_MODE_OPTION);
                                UISystem::Reescale();
                            } else if (IsKeyPressed(KEY_LEFT)) {
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
                                SelectedDisplayModeText.text = to_string((DISPLAY_MODE_ENUM)DISPLAY_MODE_OPTION);
                                UISystem::Reescale();
                            }
                        } else if (OPTION == 2) {
                            if (DISPLAY_MODE_OPTION != WINDOWED_FULLSCREEN) {
                                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                                    RESOLUTION_OPTION = (RESOLUTION_OPTION+1)%RESOLUTION_OPTIONS.size();
                                    ini["Graphics"]["ScreenWidth"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].first;
                                    ini["Graphics"]["ScreenHeight"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].second;
                                    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                                    UISystem::Reescale();
                                    SelectedResolutionText.text = std::to_string(std::get<int>(ini["Graphics"]["ScreenWidth"])) + "x" + std::to_string(std::get<int>(ini["Graphics"]["ScreenHeight"]));
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                                } else if (IsKeyPressed(KEY_LEFT)) {
                                    RESOLUTION_OPTION = (RESOLUTION_OPTION+RESOLUTION_OPTIONS.size()-1)%RESOLUTION_OPTIONS.size();
                                    ini["Graphics"]["ScreenWidth"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].first;
                                    ini["Graphics"]["ScreenHeight"] = RESOLUTION_OPTIONS[RESOLUTION_OPTION].second;
                                    SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                                    UISystem::Reescale();
                                    SelectedResolutionText.text = std::to_string(std::get<int>(ini["Graphics"]["ScreenWidth"])) + "x" + std::to_string(std::get<int>(ini["Graphics"]["ScreenHeight"]));
                                    Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                                }
                            }
                        } else if (OPTION == 3) {
                            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                                FPS_LIMIT_OPTION = ((FPS_LIMIT_OPTION+1)%FPS_LIMIT_OPTIONS.size());
                                ini["Graphics"]["FPSLimit"] = FPS_LIMIT_OPTIONS[FPS_LIMIT_OPTION];
                                SetTargetFPS(std::get<int>(ini["Graphics"]["FPSLimit"]));
                                SelectedFPSLimitText.text = std::to_string(std::get<int>(ini["Graphics"]["FPSLimit"]));
                            } else if (IsKeyPressed(KEY_LEFT)) {
                                FPS_LIMIT_OPTION = ((FPS_LIMIT_OPTION + FPS_LIMIT_OPTIONS.size() - 1) % FPS_LIMIT_OPTIONS.size());
                                ini["Graphics"]["FPSLimit"] = FPS_LIMIT_OPTIONS[FPS_LIMIT_OPTION];
                                SetTargetFPS(std::get<int>(ini["Graphics"]["FPSLimit"]));
                                SelectedFPSLimitText.text = std::to_string(std::get<int>(ini["Graphics"]["FPSLimit"]));
                            }
                        } else if (OPTION == 4) {
                            if (IsKeyDown(KEY_RIGHT) || IsKeyPressed(KEY_ENTER)) {
                                if (std::get<float>(ini["Audio"]["Volume"]) < 1) {
                                    ini["Audio"]["Volume"] = std::get<float>(ini["Audio"]["Volume"]) + 0.05f;
                                } else {
                                    ini["Audio"]["Volume"] = 1.0f;
                                }
                            } else if (IsKeyDown(KEY_LEFT)) {
                                if (std::get<float>(ini["Audio"]["Volume"]) > 0) {
                                    ini["Audio"]["Volume"] = std::get<float>(ini["Audio"]["Volume"]) - 0.05f;
                                } else {
                                    ini["Audio"]["Volume"] = 0.0f;
                                }
                            }
                            SetMasterVolume(std::get<float>(ini["Audio"]["Volume"]));
                            VolumePercentageText.text = std::to_string((int)(std::get<float>(ini["Audio"]["Volume"]) * 100));
                        } else if (OPTION == 5) {
                            if (IsKeyPressed(KEY_ENTER)) {
                                CURRENT_MENU = CONTROL_SETTINGS;
                                OPTION = 0;
                                // OPTIONS = ?;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y});
                            }
                        } else if (OPTION == 6) {
                            if (IsKeyPressed(KEY_ENTER)) {
                                CURRENT_MENU = MAIN_MENU;
                                OPTION = 1;
                                OPTIONS = 3;
                                HV = 0;
                                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
                            }
                        }
                    }
                } else if (CURRENT_MENU == CONTROL_SETTINGS) {
                    // Para Gabriel:
                    PlayerText.Draw();
                    PlayerLArrow.Draw();
                    PlayerRArrow.Draw();

                    ControllerText.Draw();
                    ControllerLArrow.Draw();
                    ControllerRArrow.Draw();
                    if (IsKeyPressed(KEY_ESCAPE)) {
                        CURRENT_MENU = SETTINGS;
                        OPTION = 5;
                        OPTIONS = 7;
                        Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF}); 
                    }
                }
            }
            Hammer.Draw();
            // Hammer movement:
            if (IsKeyPressed(KEY_DOWN)) {
                OPTION = mod(OPTION+1, OPTIONS);
                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + OPTION*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
            }
            if (IsKeyPressed(KEY_UP)) {
                OPTION = mod(OPTION-1, OPTIONS);
                Hammer.Translate({Hammer.dst.x, Hammer.dst.y - 20});
                Hammer.Translate({HammerRefs[HV][RESOLUTION_OPTION].x, HammerRefs[HV][RESOLUTION_OPTION].y + (OPTIONS - (OPTIONS-OPTION))*HammerOffsets[HV]*GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF});
            }
        }

        /*
        if (!intro_state) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || Nintendo_logo.IsRendered()) {
                intro_state++;
            } else {
                Nintendo_logo.Draw();
            }
        } else if (intro_state == 1) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || Team_logo.IsRendered()) {
                intro_state++;
            } else {
                Team_logo.Draw();
            }
        } else if (intro_state == 2) {
            if (!std::get<bool>(ini["Graphics"]["OldFashioned"])) {
                Fields.Draw();
                Mountain.Draw();
                MidPines.Draw();
                ForePines.Draw();
                Snow.Draw();
                Sign.Draw();
                //if (Sign.IsRendered()) {
                //    if (!sign_check) {
                //        Sign.ref.y = Sign.dst.y;
                //        Sign.pivot = UIObject::UP_CENTER;
                //        sign_check = true;
                //    }
                //    //DrawTextEx(NES, "PRESS <ENTER> TO START", {GetScre})
                //}
                Copy.Draw();
                //if (!copy_check && Copy.IsRendered()) {
                //    Copy.ref.y = Copy.dst.y;
                //    Copy.pivot = UIObject::DOWN_CENTER;
                //    copy_check = true;
                //}
                Main_title_music.Play();
            }
        }
        */

        //DrawCircleV({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, 7, RED);
        //DrawCircleV({(GetScreenWidth()-Nintendo_logo.dst.width)/2, GetScreenHeight()/2.0f}, 7, RED);
        //DrawCircleV({(GetScreenWidth()+Nintendo_logo.dst.width)/2, GetScreenHeight()/2.0f}, 7, RED);

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
    UnloadMusicStream(MainTitleOST);
    UnloadFont(NES);
    UISystem::RemoveAll();
}