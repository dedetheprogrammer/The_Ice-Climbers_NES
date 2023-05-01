#pragma once
#ifndef UI_ECS_H
#define UI_ECS_H

#include <functional>
#include <memory>
#include <string>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "EngineECS.h"
#include "raylib.h"
//-----------------------------------------------------------------------------
// Elemento de UI
//-----------------------------------------------------------------------------
//class UIEffect;
class UIObject {
protected:
    //UIEffect* effect;
public:
    enum PIVOT {
        UP_LEFT,
        UP_CENTER,
        UP_RIGHT, 
        CENTER_LEFT,
        CENTER,
        CENTER_RIGHT,
        DOWN_LEFT,
        DOWN_CENTER,
        DOWN_RIGHT
    };

    std::string name;
    PIVOT pivot;   // Pivot reference from where to do the transformations.
    bool hidden;
    float scale_factor;

    UIObject(std::string name = "UIObject", PIVOT pivot = UP_LEFT, bool hidden = false, float scale_factor = 1);

    virtual void Draw() = 0;
    bool IsRendered();
    virtual void Move(Vector2 translation) = 0;
    //template <typename T, typename... Args> void setEffect(Args&&... args) {
    //    effect = new T(*this, std::forward<Args>(args)...);
    //} 
    void removeEffect();
    virtual void Reescale() = 0;
    virtual void Unload();
};

/*
class UIAnimation : public UIObject {
private:
    //...
public:
    Animation animation;
    UIAnimation(Animation animation, std::string name, Vector2 pos, float scale_x, float scale_y, PIVOT pivot = UP_LEFT, bool hidden = false, float scale_factor = 1);
    void Draw() override;
    void Move(Vector2 translation) override;
    void Reescale() override;
    void Unload() override;
};
*/

class UISprite : public UIObject {
private:
    bool fullscreen;
public:
    Rectangle src; // What pixels of the sprite do I want to draw?
    Rectangle ref; // Default (window reference) position and size properties.
    Rectangle dst; // Where and how do I draw these pixels?

    Texture2D sprite;
    UISprite(const char* fileName, std::string name, Vector2 pos, float scale_x, float scale_y, PIVOT pivot = UP_LEFT, bool hidden = false, float scale_factor = 1);
    UISprite(const char* fileName, std::string name, Vector2 pos, Vector2 size, PIVOT pivot = UP_LEFT, bool hidden = false, float scale_factor = 1);
    UISprite(Texture2D sprite, std::string name, Vector2 pos, float scale_x, float scale_y, PIVOT pivot = UP_LEFT, bool hidden = false, float scale_factor = 1);
    UISprite(Texture2D sprite, std::string name, Vector2 pos, Vector2 size, PIVOT pivot =  UP_LEFT, bool hidden = false, float scale_factor = 1);
    UISprite(Texture2D sprite, std::string name, Rectangle src, Rectangle dst, PIVOT pivot = UP_LEFT, bool hidden = false, float scale_factor = 1);
    void Draw() override;
    void Move(Vector2 translation) override;
    void Reescale() override;
    void Translate(Vector2 translation);
    void Unload() override;
};

class UIText : public UIObject {
private:
    //...
    int outline;
    int outlinefont_size;
    Vector2 outline_pos;
    Vector2 outline_size;
    Color outline_color;
public:
    Font font;
    std::string text;
    int font_size;
    int ref_font_size;
    int spacing;
    int ref_spacing;
    Vector2 pos;
    Vector2 size;
    Vector2 ref_pos;
    Color color;

    UIText(
        Font font,
        std::string text,
        int font_size,
        int spacing,
        std::string name, 
        Vector2 pos, 
        Color color,
        PIVOT pivot = UP_LEFT,
        bool hidden = false,
        float scale_factor = 1,
        int outline = 0,
        Color outline_color = BLACK
    );
    void Draw() override;
    void Move(Vector2 translation) override;
    void Reescale() override;

};

// ============================================================================
// ============================================================================
// Componentes
// ============================================================================
// ============================================================================
// ----------------------------------------------------------------------------
// Efecto generico
// ----------------------------------------------------------------------------
/*
class UIEffect {
protected:
    UIObject& uiobject;
    bool finished;
public:
    UIEffect(UIObject& uiobject);
    virtual ~UIEffect() = default;
    bool IsRendered();
    virtual void Play() = 0;
    virtual void Stop();
};

class FadeIn : public UIEffect {
private:
    float default_opacity;
    float opacity;
    float speed;
public:
    FadeIn(UIObject& uiobject);
    FadeIn(UIObject& uiobject, float speed = 1, float opacity = 0);
    void Play() override;
    void Stop() override;
};

class FadeInOut : public UIEffect {
private:
    bool inout;
    float default_opacity;
    float opacity;
    float speedin;
    float speedout;
public:
    FadeInOut(UIObject& uiobject);
    FadeInOut(UIObject& uiobject, float speedin = 1, float speedout = 1, float opacity = 0);
    void Play() override;
    void Stop() override;
};

class FadeOut : public UIEffect {
private:
    float default_opacity;
    float opacity;
    float speed;
public:
    FadeOut(UIObject& uiobject);
    FadeOut(UIObject& uiobject, float speed = 1, float opacity = 100);
    void Play() override;
    void Stop() override;
};

class FadeOutIn : public UIEffect {
private:
    bool inout;
    float default_opacity;
    float opacity;
    float speedin;
    float speedout;
public:
    FadeOutIn(UIObject& uiobject);
    FadeOutIn(UIObject& uiobject, float speedin = 1, float speedout = 1, float opacity = 0);
    void Play() override;
    void Stop() override;
};

class Flicker : public UIEffect {
private:
    bool default_view;  // Default view: starts hidden or not?
    bool hide;          // Indicates if is the momento to hide or not the UI piece.
    int changes_period; // Indicates if a whole flicker loop has ocurred (== 2)
    float current_time; // Passed time in the current state.
    float trigger_time; // Period between two states.
public:
    Flicker(UIObject& uiobject);
    Flicker(UIObject& uiobject, float trigger_time);
    Flicker(UIObject& uiobject, bool hide, float trigger_time);
    void Play() override;
    void Stop() override;
};

class Move : public UIEffect {
private:
    Vector2 default_pos;
    Vector2 current_offset;
    Vector2 max_offset;
    Vector2 speed;
public:
    Move(UIObject& uiobject);
    Move(UIObject& uiobject, Vector2 offset, Vector2 speed);
    void Play() override;
    void Stop() override;
};

class Parallax : public UIEffect {
private:
    Vector2 default_pos;
    Vector2 speed;
public:
    Parallax(UIObject& uiobject);
    Parallax(UIObject& uiobject, Vector2 speed);
    void Play() override;
    void Stop() override;
};

class Scroll : public UIEffect {
private:
    float current_time;
    float offset_time;
    Vector2 default_pos;
    Vector2 speed;
public:
    Scroll(UIObject& uiobject);
    Scroll(UIObject& uiobject, float offset_time, Vector2 speed);
    void Play() override;
    void Stop() override;
};
*/

// ============================================================================
// ============================================================================
// SYSTEMS
// ============================================================================
// ============================================================================
// ----------------------------------------------------------------------------
// UI System
// /!\ Aunque no lo creais y no lo parezca, todo esto lo hago para encapsular
// el tratamiento a pelo de los colliders y de los objetos en escena, de manera
// que con llamar a un par de funciones lo haga solo sin que vosotros tengais
// que controlarlo (+ reutilizacion de codigo + mantenimiento + generalizacion).
// ----------------------------------------------------------------------------

class UISystem {
private:
    static std::unordered_map<std::string, UIObject*> UIObjects;
public:
    static float WINDOW_WIDTH_REF;
    static float WINDOW_HEIGHT_REF;

    static void Add(UIObject& uiobject);
    static void Draw();
    static bool IsRendered(std::string name);
    static void init_UI_system(float WINDOW_WIDTH_REF, float WINDOW_HEIGHT_REF);
    static void Move(Vector2 translation);
    static void Move(Vector2 translation, std::vector<std::string> names);
    static void Reescale();
    static void Remove(std::string name);
    static void RemoveAll();
};

#endif