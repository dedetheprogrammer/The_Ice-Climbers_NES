#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <unordered_map>
#include "component.h"
#include "raylib.h"

class Animation {
private:

    //-- Animation look
    Texture2D spritesheet;     // Animation spritesheet.
    //-- Animation frames
    int current_frame;         // Index of the current frame.
    int current_row;           // Index of the current frames row.
    int single_frame_width;    // Width of a single frame.
    int single_frame_height;   // Height of a single frame.
    int frames_columns;        // Frames columns in the spritesheet.
    int frames_rows;           // Frames rows in the spritesheet.
    int total_frames;          // Total number of frames.
    //-- Animation drawing
    float scale;
    Rectangle frame_src;       // Rectangle source that covers the current frame.
    Rectangle frame_dst;       // Rectangle destination where to draw the frame on.
    //-- Animation timing
    float duration;            // Animation duration.
    float time_between_frames; // Time between one frame and the next one.
    float current_frame_time;  // Time passed in the current frame.
    float current_time;
    bool hasFinished;          // Tellings us if the animation has already finished.
    bool loop;

public:

    Animation() {
        //-- Animation frames
        current_frame = 0;       // Index of the current frame.
        current_row = 0;         // Index of the current frames row.
        single_frame_width = 0;  // Width of a single frame.
        single_frame_height = 0; // Height of a single frame.
        frames_rows = 0;         // Frames rows in the spritesheet.
        total_frames = 0;        // Total number of frames.
        //-- Animation drawing
        scale = 0;
        //-- Animation timing
        duration = 0;            // Animation duration.
        time_between_frames = 0; // Time between one frame and the next one.
        current_frame_time = 0;  // Time passed in the current frame.
        current_time = 0;
        hasFinished = false;
        loop = false;
    }

    Animation(const char *fileName , int single_frame_width, int single_frame_height, float scale, float duration, bool loop) {
        // Animation look
        spritesheet = LoadTexture(fileName);
        // Animation frames
        current_frame = 0;
        current_row   = 0;
        this->single_frame_width  = single_frame_width;
        this->single_frame_height = single_frame_height;
        frames_columns = spritesheet.width/single_frame_width;
        frames_rows    = spritesheet.height/single_frame_height;
        total_frames   = frames_rows * frames_columns;
        // Animation drawing
        this->scale = scale;
        frame_src   = {0, 0, (float)single_frame_width, (float)single_frame_height};
        frame_dst   = {0, 0, single_frame_width * scale, single_frame_height*scale};
        // Animation timing
        this->duration      = duration;
        time_between_frames = duration/(float)total_frames;
        current_frame_time  = 0;
        current_time = 0;
        hasFinished = false;
        this->loop = loop;
    }

    void Flip() {
        frame_src.width *= -1;
    }

    std::pair<int, int> GetDimensions() {
        return {std::abs(single_frame_width), std::abs(single_frame_height)};
    }

    bool HasFinished() {
        return (current_time >= duration);
    }

    void Play(Vector2 position) {
        if (current_frame_time >= time_between_frames) {
            current_frame_time = 0;
            if (loop) {
                current_frame = (current_frame+1) % frames_columns;
            } else {
                if (current_frame + 1 < frames_columns) {
                    current_frame++;
                }
            }
            //if (!loop && current_frame+1 == frames_columns) 
            //if (!current_frame) {
            //    current_row = (current_row+1) % frames_rows;
            //}
            frame_src.x = (float)single_frame_width  * current_frame;
            frame_src.y = (float)single_frame_height * current_row;
        }
        frame_dst.x = position.x; frame_dst.y = position.y;
        DrawTexturePro(spritesheet, frame_src, frame_dst, Vector2{0,0}, 0, WHITE);
        float deltaTime = GetFrameTime();
        current_frame_time += deltaTime;
        current_time += deltaTime;
    }

    void Stop() {
        current_frame = current_row = 0;
        current_frame_time = 0;
        current_time = 0;
        frame_src.x = frame_src.y = 0;
    }

    void Unload() {
        UnloadTexture(spritesheet);
    }
    
};

class Animator {
private:

    std::string start_animation;   // The entry animation of the whole stream, unused.
    std::string current_animation; // Current animation playing.
    std::string waiting_animation; // Next animation that we want to play in some cases.
    std::unordered_map<std::string, Animation> animations; // The animations perse.

public:

    Animator() : current_animation("") {}
    Animator(std::string start_animation, std::unordered_map<std::string, Animation> animations) {
        this->start_animation = current_animation = start_animation;
        this->animations      = animations;
    }

    void Flip() {
        for (auto& animation : animations) {
            animation.second.Flip();
        }
    }

    std::pair<int, int> GetDimensions() {
        return animations[current_animation].GetDimensions();
    }

    bool HasFinished(std::string animation) {
        return (current_animation == animation) ? animations[current_animation].HasFinished() : true;
    }

    void Play(Vector2 position) {
        animations[current_animation].Play(position);
    }

    bool InState(std::string animation) {
        return current_animation == animation; 
    }

    bool Trigger(std::string entry_animation, std::string next_animation) {
        if (current_animation == entry_animation) {
            current_animation = next_animation;
            return true;
        } else {
            return false;
        }
    }

    void Unload() {
        for (auto& animation : animations) {
            animation.second.Unload();
        }
    }

    void operator[ ](std::string animation) {
        if (animation != current_animation) animations[animation].Stop();
        current_animation = animation;
    }
};

#endif