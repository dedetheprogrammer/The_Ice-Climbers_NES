#include "DengineUI.h"
#include "raylib.h"
#include "raylibx.h"

//-----------------------------------------------------------------------------
// Elemento de UI
//-----------------------------------------------------------------------------
UIObject::UIObject(PIVOT pivot, bool hidden, float scale_factor)
    : /*effect(nullptr),*/ pivot(pivot), hidden(hidden), scale_factor(scale_factor) {}

bool UIObject::IsRendered() {
    //if (effect == nullptr) {
        return true;
    //} else {
    //    return effect->IsRendered();
    //}
}

void UIObject::removeEffect() {
    //if (effect != nullptr) {
    //    delete effect;
    //}
}

void UIObject::Unload() {
    //if (effect != nullptr) {
    //    delete effect;
    //}
}

//--------------------
/*
UIAnimation::UIAnimation(Animation animation, std::string name, Vector2 pos, float scale_x, float scale_y, PIVOT pivot, bool hidden, float scale_factor)
    : UIObject(name, pivot, hidden, scale_factor) {

}*/

//--------------------
UISprite::UISprite(const char* fileName, Vector2 pos, float scale_x, float scale_y, PIVOT pivot, bool hidden, float scale_factor)
    : UIObject(pivot, hidden, scale_factor)
{
    fullscreen   = false;
    sprite       = LoadTexture(fileName);
    src          = {0, 0, (float)sprite.width, (float)sprite.height};
    Vector2 size{src.width * scale_x, src.height * scale_y};
    if (pivot == UP_LEFT) {
        dst = {pos.x, pos.y, size.x, size.y};
    } else if (pivot == UP_CENTER) {
        dst = {pos.x - size.x/2, pos.y, size.x, size.y};
    } else if (pivot == UP_RIGHT) {
        dst = {pos.x - size.x, pos.y, size.x, size.y};
    } else if (pivot == CENTER_LEFT) {
        dst = {pos.x, pos.y - size.y/2, size.x, size.y};
    } else if (pivot == CENTER) {
        dst = {pos.x - size.x/2, pos.y - size.y/2, size.x, size.y};
    } else if (pivot == CENTER_RIGHT) {
        dst = {pos.x - size.x, pos.y - size.y/2, size.x, size.y};
    } else if (pivot == DOWN_LEFT) {
        dst = {pos.x, pos.y - size.y, size.x, size.y};
    } else if (pivot == DOWN_CENTER) {
        dst = {pos.x - size.x/2, pos.y - size.y, size.x, size.y};
    } else if (pivot == DOWN_RIGHT) {
        dst = {pos.x - size.x, pos.y - size.y, size.x, size.y};
    }
    ref = {pos.x, pos.y, dst.width, dst.height};
    UISystem::Add(*this);
}

UISprite::UISprite(const char* fileName, Vector2 pos, Vector2 size, PIVOT pivot, bool hidden, float scale_factor)
    : UIObject(pivot, hidden, scale_factor)
{
    if (size.x == GetScreenWidth() || size.y == GetScreenHeight()) {
        fullscreen = true;
    } else {
        fullscreen = false;
    }
    sprite       = LoadTexture(fileName);
    src          = {0, 0, (float)sprite.width, (float)sprite.height};
    if (pivot == UP_LEFT) {
        dst = {pos.x, pos.y, size.x, size.y};
    } else if (pivot == UP_CENTER) {
        dst = {pos.x - size.x/2, pos.y, size.x, size.y};
    } else if (pivot == UP_RIGHT) {
        dst = {pos.x - size.x, pos.y, size.x, size.y};
    } else if (pivot == CENTER_LEFT) {
        dst = {pos.x, pos.y - size.y/2, size.x, size.y};
    } else if (pivot == CENTER) {
        dst = {pos.x - size.x/2, pos.y - size.y/2, size.x, size.y};
    } else if (pivot == CENTER_RIGHT) {
        dst = {pos.x - size.x, pos.y - size.y/2, size.x, size.y};
    } else if (pivot == DOWN_LEFT) {
        dst = {pos.x, pos.y - size.y, size.x, size.y};
    } else if (pivot == DOWN_CENTER) {
        dst = {pos.x - size.x/2, pos.y - size.y, size.x, size.y};
    } else if (pivot == DOWN_RIGHT) {
        dst = {pos.x - size.x, pos.y - size.y, size.x, size.y};
    }
    ref = {pos.x, pos.y, dst.width, dst.height};
    UISystem::Add(*this);
}


UISprite::UISprite(Texture2D sprite, Vector2 pos, float scale_x, float scale_y, PIVOT pivot, bool hidden, float scale_factor)
    : UIObject(pivot, hidden, scale_factor) 
{
    this->sprite = sprite;
    src = {0, 0, (float)sprite.width, (float)sprite.height};
    //dst = {pos.x, pos.y, src.width * scale_x, src.height * scale_y};
    Vector2 size{src.width * scale_x, src.height * scale_y};
    if (pivot == UP_LEFT) {
        dst = {pos.x, pos.y, size.x, size.y};
    } else if (pivot == UP_CENTER) {
        dst = {pos.x - size.x/2, pos.y, size.x, size.y};
    } else if (pivot == UP_RIGHT) {
        dst = {pos.x - size.x, pos.y, size.x, size.y};
    } else if (pivot == CENTER_LEFT) {
        dst = {pos.x, pos.y - size.y/2, size.x, size.y};
    } else if (pivot == CENTER) {
        dst = {pos.x - size.x/2, pos.y - size.y/2, size.x, size.y};
    } else if (pivot == CENTER_RIGHT) {
        dst = {pos.x - size.x, pos.y - size.y/2, size.x, size.y};
    } else if (pivot == DOWN_LEFT) {
        dst = {pos.x, pos.y - size.y, size.x, size.y};
    } else if (pivot == DOWN_CENTER) {
        dst = {pos.x - size.x/2, pos.y - size.y, size.x, size.y};
    } else if (pivot == DOWN_RIGHT) {
        dst = {pos.x - size.x, pos.y - size.y, size.x, size.y};
    }
    ref = {pos.x, pos.y, dst.width, dst.height};
    //this->ref = {pos.x, pos.y, dst.width, dst.height};
    UISystem::Add(*this);

}

UISprite::UISprite(Texture2D sprite, Vector2 pos, Vector2 size, PIVOT pivot, bool hidden, float scale_factor)
    : UIObject(pivot, hidden, scale_factor) 
{
    if (size.x == GetScreenWidth() || size.y == GetScreenHeight()) {
        fullscreen = true;
    } else {
        fullscreen = false;
    }
    this->sprite = sprite;
    src = {0, 0, (float)sprite.width, (float)sprite.height};
    dst = {pos.x, pos.y, size.x, size.y};
    this->ref = {pos.x, pos.y, dst.width, dst.height};
    UISystem::Add(*this);
}

UISprite::UISprite(Texture2D sprite, Rectangle src, Rectangle dst, PIVOT pivot, bool hidden, float scale_factor)
    : UIObject(pivot, hidden, scale_factor)
{
    if (dst.width == GetScreenWidth() || dst.height == GetScreenHeight()) {
        fullscreen = true;
    } else {
     
        fullscreen = false;
    }
    this->sprite = sprite;
    this->src    = src;
    this->dst    = this->ref = dst;
    UISystem::Add(*this);
}

void UISprite::Draw(Color color) {
    //if (effect == nullptr) {
        DrawTexturePro(sprite, src, dst, {0,0}, 0, color);
    //} else {
    //    effect->Play();
    //}
}

void UISprite::Move(Vector2 translation) {
    if (GetScreenWidth() == UISystem::WINDOW_WIDTH_REF && GetScreenHeight() == UISystem::WINDOW_HEIGHT_REF) {
        ref.x += translation.x;
        ref.y += translation.y;
    } else {
        ref.x += translation.x * UISystem::WINDOW_WIDTH_REF / GetScreenWidth();
        ref.y += translation.y * UISystem::WINDOW_HEIGHT_REF / GetScreenHeight();
    }
    dst.x += translation.x;
    dst.y += translation.y;
}

void UISprite::Reescale() {
    if (GetScreenWidth() == UISystem::WINDOW_WIDTH_REF && GetScreenHeight() == UISystem::WINDOW_HEIGHT_REF) {
        dst.width = ref.width;
        dst.height = ref.height;
        dst.x = ref.x;
        dst.y = ref.y;
    } else {
        float scale = fmaxf(GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 
            GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
        if (fullscreen) {
            dst.width  = ref.width  * scale;
            dst.height = ref.height * scale;
        } else {
            dst.width  = ref.width  * scale * scale_factor;
            dst.height = ref.height * scale * scale_factor;
        }
        dst.x = ref.x * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF;
        dst.y = ref.y * GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF;
    }

    if (pivot == UIObject::UP_CENTER) {
        dst.x -= dst.width/2;
    } else if (pivot == UIObject::UP_RIGHT) {
        dst.x -= dst.width;
    } else if (pivot == UIObject::CENTER_LEFT) {
        dst.y -= dst.height/2;
    } else if (pivot == UIObject::CENTER) {
        dst.x -= dst.width/2;
        dst.y -= dst.height/2;
    } else if (pivot == UIObject::CENTER_RIGHT) {
        dst.x -= dst.width;
        dst.y -= dst.height/2;
    } else if (pivot == UIObject::DOWN_LEFT) {
        dst.y -= dst.height;
    } else if (pivot == UIObject::DOWN_CENTER) {
        dst.x -= dst.width/2;
        dst.y -= dst.height;
    } else if (pivot == UIObject::DOWN_RIGHT) {
        dst.x -= dst.width;
        dst.y -= dst.height;
    }
}

void UISprite::Translate(Vector2 translation) {
    if (GetScreenWidth() == UISystem::WINDOW_WIDTH_REF && GetScreenHeight() == UISystem::WINDOW_HEIGHT_REF) {
        ref.x = translation.x;
        ref.y = translation.y;
    } else {
        ref.x = translation.x * UISystem::WINDOW_WIDTH_REF / GetScreenWidth();
        ref.y = translation.y * UISystem::WINDOW_HEIGHT_REF / GetScreenHeight();
    }
    dst.x = translation.x;
    dst.y = translation.y;  
}

void UISprite::Unload() {
    UnloadTexture(sprite);
    UIObject::Unload();
}

//----------------------
UIText::UIText(Font font, std::string text, int font_size, int spacing, Vector2 pos, PIVOT pivot, bool hidden, float scale_factor)
    : UIObject(pivot, hidden, scale_factor)
{
    this->font = font;
    this->text = text;
    this->font_size = ref_font_size = font_size;
    this->spacing   = ref_spacing   = spacing;
    size = MeasureTextEx(font, text.c_str(), font_size, spacing);
    if (pivot == UP_LEFT) {
        this->pos = pos;
    } else if (pivot == UP_CENTER) {
        this->pos = {pos.x - size.x/2, pos.y};
    } else if (pivot == UP_RIGHT) {
        this->pos = {pos.x - size.x, pos.y};
    } else if (pivot == CENTER_LEFT) {
        this->pos = {pos.x, pos.y - size.y/2};
    } else if (pivot == CENTER) {
        this->pos = {pos.x - size.x/2, pos.y - size.y/2};
    } else if (pivot == CENTER_RIGHT) {
        this->pos = {pos.x - size.x, pos.y - size.y/2};
    } else if (pivot == DOWN_LEFT) {
        this->pos = {pos.x, pos.y - size.y};
    } else if (pivot == DOWN_CENTER) {
        this->pos = {pos.x - size.x/2, pos.y - size.y};
    } else if (pivot == DOWN_RIGHT) {
        this->pos = {pos.x - size.x, pos.y - size.y};
    }
    ref_pos     = pos;
    UISystem::Add(*this);
}

void UIText::Draw(Color color) {
    DrawTextEx(font, text.c_str(), pos, font_size, spacing, color);
}

void UIText::Move(Vector2 translation) {
    if (GetScreenWidth() == UISystem::WINDOW_WIDTH_REF && GetScreenHeight() == UISystem::WINDOW_HEIGHT_REF) {
        ref_pos += translation;
    } else {
        ref_pos.x += translation.x * UISystem::WINDOW_WIDTH_REF / GetScreenWidth();
        ref_pos.y += translation.y * UISystem::WINDOW_HEIGHT_REF / GetScreenHeight();
    }
    pos += translation;
}

void UIText::Reescale() {
    float scale = fmaxf(GetScreenWidth()/UISystem::WINDOW_WIDTH_REF, 
        GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF);
    font_size = ref_font_size * scale * scale_factor;
    //spacing   = ref_spacing * scale * scale_factor;

    pos.x = ref_pos.x * GetScreenWidth()/UISystem::WINDOW_WIDTH_REF;
    pos.y = ref_pos.y *  GetScreenHeight()/UISystem::WINDOW_HEIGHT_REF;
    size = MeasureTextEx(font, text.c_str(), font_size, spacing);
    if (pivot == UIObject::UP_CENTER) {
        pos.x -= size.x/2;
    } else if (pivot == UIObject::UP_RIGHT) {
        pos.x -= size.x;
    } else if (pivot == UIObject::CENTER_LEFT) {
        pos.y -= size.y/2;
    } else if (pivot == UIObject::CENTER) {
        pos.x -= size.x/2;
        pos.y -= size.y/2;
    } else if (pivot == UIObject::CENTER_RIGHT) {
        pos.x -= size.x;
        pos.y -= size.y/2;
    } else if (pivot == UIObject::DOWN_LEFT) {
        pos.y -= size.y;
    } else if (pivot == UIObject::DOWN_CENTER) {
        pos.x -= size.x/2;
        pos.y -= size.y;
    } else if (pivot == UIObject::DOWN_RIGHT) {
        pos.x -= size.x;
        pos.y -= size.y;
    }
}

void UIText::SetText(std::string text, bool rescale) {
    this->text = text;
    size = MeasureTextEx(font, text.c_str(), font_size, spacing);
    if (pivot == UP_LEFT) {
        this->pos = ref_pos;
    } else if (pivot == UP_CENTER) {
        this->pos = {ref_pos.x - size.x/2, ref_pos.y};
    } else if (pivot == UP_RIGHT) {
        this->pos = {ref_pos.x - size.x, ref_pos.y};
    } else if (pivot == CENTER_LEFT) {
        this->pos = {ref_pos.x, ref_pos.y - size.y/2};
    } else if (pivot == CENTER) {
        this->pos = {ref_pos.x - size.x/2, ref_pos.y - size.y/2};
    } else if (pivot == CENTER_RIGHT) {
        this->pos = {ref_pos.x - size.x, ref_pos.y - size.y/2};
    } else if (pivot == DOWN_LEFT) {
        this->pos = {ref_pos.x, ref_pos.y - size.y};
    } else if (pivot == DOWN_CENTER) {
        this->pos = {ref_pos.x - size.x/2, ref_pos.y - size.y};
    } else if (pivot == DOWN_RIGHT) {
        this->pos = {ref_pos.x - size.x, ref_pos.y - size.y};
    }
    if (rescale) {
        Reescale();
    }
}


// ============================================================================
// ============================================================================
// Componentes
// ============================================================================
// ============================================================================
// ----------------------------------------------------------------------------
// Efecto generico
// ----------------------------------------------------------------------------
/*
UIEffect::UIEffect(UIObject& uiobject)
    : uiobject(uiobject), finished(false) {}
//     : trigger(false), current_time(0.0f), trigger_time(0.0f) {}
// UIEffect::UIEffect(float trigger_time)
//     : trigger(false), current_time(0.0f), trigger_time(trigger_time) {}
// UIEffect::UIEffect(bool trigger, float trigger_time)
//     : trigger(trigger), current_time(0.0f), trigger_time(trigger_time) {}
bool UIEffect::IsRendered() {
    return finished;
}
void UIEffect::Stop() {
    finished = false;
}

FadeIn::FadeIn(UIObject& uiobject)
    : UIEffect(uiobject) {}
FadeIn::FadeIn(UIObject& uiobject, float speed, float opacity)
    : UIEffect(uiobject), default_opacity(opacity), opacity(opacity), speed(speed) {}
void FadeIn::Play() {
    if (opacity < 1.0f) {
        opacity += speed * GetFrameTime();
    } else {
        finished = true;
    }
    DrawTexturePro(uiobject.sprite, uiobject.src, uiobject.dst, {0,0}, 0, Fade(WHITE, opacity));
}
void FadeIn::Stop() {
    UIEffect::Stop();
    opacity = default_opacity;
}

FadeInOut::FadeInOut(UIObject& uiobject)
    : UIEffect(uiobject) {}
FadeInOut::FadeInOut(UIObject& uiobject, float speedin, float speedout, float opacity)
    : UIEffect(uiobject), default_opacity(opacity), opacity(opacity), speedin(speedin), speedout(speedout) {}
void FadeInOut::Play() {
    if (!inout && opacity < 1) {
        opacity += speedin * GetFrameTime();    
    } else {
        inout = true;
        if (inout) {
            if (opacity > 0) {
                opacity -= speedout * GetFrameTime();
            } else {
                finished = true;
            }
        }
    }
    DrawTexturePro(uiobject.sprite, uiobject.src, uiobject.dst, {0,0}, 0, Fade(WHITE, opacity));
}
void FadeInOut::Stop() {
    UIEffect::Stop();
    inout   = false;
    opacity = default_opacity;
}

FadeOut::FadeOut(UIObject& uiobject)
    : UIEffect(uiobject) {}
FadeOut::FadeOut(UIObject& uiobject, float speed, float opacity)
    : UIEffect(uiobject), default_opacity(opacity), opacity(opacity), speed(speed) {}
void FadeOut::Play() {
    if (opacity > 0) {
        opacity -= speed * GetFrameTime();
    } else {
        finished = true;
    }
    DrawTexturePro(uiobject.sprite, uiobject.src, uiobject.dst, {0,0}, 0, Fade(WHITE, opacity));
}
void FadeOut::Stop() {
    UIEffect::Stop();
    opacity = default_opacity;
}

FadeOutIn::FadeOutIn(UIObject& uiobject)
    : UIEffect(uiobject) {}
FadeOutIn::FadeOutIn(UIObject& uiobject, float speedin, float speedout, float opacity)
    : UIEffect(uiobject), default_opacity(opacity), opacity(opacity), speedin(speedin), speedout(speedout) {}
void FadeOutIn::Play() {
    if (!inout && opacity > 0) {
        opacity -= speedout * GetFrameTime();
    } else {
        inout = true;
        if (inout) {
            if (opacity < 0) {
                opacity += speedin * GetFrameTime();
            } else {
                finished = true;
            }
        }
    }
    DrawTexturePro(uiobject.sprite, uiobject.src, uiobject.dst, {0,0}, 0, Fade(WHITE, opacity));
}
void FadeOutIn::Stop() {
    UIEffect::Stop();
    inout   = false;
    opacity = default_opacity;
}

Flicker::Flicker(UIObject& uiobject)
    : UIEffect(uiobject) {}
Flicker::Flicker(UIObject& uiobject, float trigger_time)
    : UIEffect(uiobject), trigger_time(trigger_time) {}
Flicker::Flicker(UIObject& uiobject, bool hide, float trigger_time)
    : UIEffect(uiobject), default_view(hide), hide(hide), trigger_time(trigger_time) {}
void Flicker::Play() {
    current_time += GetFrameTime();
    if (current_time >= trigger_time) {
        hide = !hide;
        current_time = 0;
        changes_period = (changes_period+1) % 3;
        finished = changes_period == 2;
    }
    if (!hide) {
        DrawTexturePro(uiobject.sprite, uiobject.src, uiobject.dst, {0,0}, 0, WHITE);
    }
}
void Flicker::Stop() {
    UIEffect::Stop();
    hide           = default_view;
    changes_period = 0;
    current_time   = 0;
}

Move::Move(UIObject& uiobject) : UIEffect(uiobject) {
    default_pos = {uiobject.dst.x, uiobject.dst.y};
    max_offset = {0,0};
    speed  = {1,1};
}
Move::Move(UIObject& uiobject, Vector2 max_offset, Vector2 speed) : UIEffect(uiobject) {
    default_pos = {uiobject.dst.x, uiobject.dst.y};
    this->current_offset = {0,0};
    this->max_offset = max_offset;
    this->speed  = speed;
}
void Move::Play() {
    if (current_offset.x < max_offset.x) {
        auto tmp = speed.x * GetFrameTime();
        current_offset.x += std::abs(tmp);
        uiobject.dst.x   += tmp;
    }
    if (current_offset.y < max_offset.y) {
        auto tmp = speed.y * GetFrameTime();
        current_offset.y += std::abs(tmp);
        uiobject.dst.y   += tmp;
    }
    if (current_offset.x >= max_offset.x && current_offset.y >= max_offset.y) {
        finished = true;
    }
    DrawTexturePro(uiobject.sprite, uiobject.src, uiobject.dst, {0,0}, 0, WHITE);
}
void Move::Stop() {
    UIEffect::Stop();
    current_offset = {0,0};
    uiobject.dst.x = default_pos.x;
    uiobject.dst.y = default_pos.y;
}

Parallax::Parallax(UIObject& uiobject) : UIEffect(uiobject) {
    default_pos  = {uiobject.src.x, uiobject.src.y};
    speed        = {1,1};
}

Parallax::Parallax(UIObject& uiobject, Vector2 speed) : UIEffect(uiobject) {
    default_pos        = {uiobject.src.x, uiobject.src.y};
    this->speed        = speed;
}

void Parallax::Play() {
    uiobject.src.x += speed.x * GetFrameTime();
    uiobject.src.y += speed.y * GetFrameTime();
    DrawTexturePro(uiobject.sprite, uiobject.src, uiobject.dst, {0,0}, 0, WHITE);
}

void Parallax::Stop() {
    UIEffect::Stop();
    uiobject.src.x = default_pos.x;
    uiobject.src.y = default_pos.y;
}


Scroll::Scroll(UIObject& uiobject) : UIEffect(uiobject) {
    current_time = 0;
    offset_time  = 0;
    default_pos  = {uiobject.dst.x, uiobject.dst.y};
    speed        = {1,1};
}

Scroll::Scroll(UIObject& uiobject, float offset_time, Vector2 speed) : UIEffect(uiobject) {
    current_time = 0;
    this->offset_time  = offset_time;
    default_pos        = {uiobject.dst.x, uiobject.dst.y};
    this->speed        = speed;
}

void Scroll::Play() {
    if (!current_time) {
        if ((speed.x < 0 && (uiobject.dst.x + uiobject.dst.width >= 0)) ||
            (speed.x > 0 && (uiobject.dst.x < GetScreenWidth())) ||
            (speed.y < 0 && (uiobject.dst.y + uiobject.dst.width >= 0)) ||
            (speed.y > 0 && (uiobject.dst.y < GetScreenHeight()))
        ) {
            uiobject.dst.x += speed.x * GetFrameTime();
            uiobject.dst.y += speed.y * GetFrameTime();
            DrawTexturePro(uiobject.sprite, uiobject.src, uiobject.dst, {0,0}, 0, WHITE);
        } else {
            current_time += GetFrameTime();
        }
    } else {
        current_time += GetFrameTime();
        if (current_time >= offset_time) {
            current_time = 0;
        }
    }
}

void Scroll::Stop() {
    UIEffect::Stop();
    current_time   = 0;
    uiobject.dst.x = default_pos.x;
    uiobject.dst.y = default_pos.y;
    
}
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
float UISystem::WINDOW_WIDTH_REF;
float UISystem::WINDOW_HEIGHT_REF;
std::vector<UIObject*> UISystem::UIObjects;

void UISystem::Add(UIObject& uiobject) {
    UIObjects.push_back(&uiobject);
}

void UISystem::Draw() {
    for (auto& uiobject : UIObjects) {
        if (!uiobject->hidden) {
            uiobject->Draw();
        }
    }
}

void UISystem::init_UI_system(float WINDOW_WIDTH, float WINDOW_HEIGHT) {
    WINDOW_WIDTH_REF  = WINDOW_WIDTH;
    WINDOW_HEIGHT_REF = WINDOW_HEIGHT;
}

void UISystem::Move(Vector2 translation) {
    for (auto& uiobject : UIObjects) {
        uiobject->Move(translation);
    }
}


void UISystem::Reescale() {
    for (auto& uiobject : UIObjects) {
        uiobject->Reescale();
    }
}


void UISystem::RemoveAll() {
    for (auto& uiobject : UIObjects) {
        uiobject->Unload();
    }
}