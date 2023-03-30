#pragma once
#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <memory>
#include <string>
#include <unordered_map>
#include "component.h"
#include "raylib.h"

class AudioSource {
private:
    // ...
public:
    AudioSource() {}
    virtual ~AudioSource() = default;
    virtual void Play() = 0;
    virtual void Unload() = 0;
};

class SoundSource : public AudioSource {
private:
    Sound source;
public:

    SoundSource(const char *fileName) {
        source = LoadSound(fileName);
    }

    void Play() override {
        PlaySound(source);
    }

    void Unload() override {
        UnloadSound(source);
    }
};

class MusicSource : public AudioSource {
private:
    Music source;
public:
    MusicSource (const char *fileName, bool loop) {
        source = LoadMusicStream(fileName);
        source.looping = loop;
    }

    void Init() {
        PlayMusicStream(source);
    }

    void Play() override {
        UpdateMusicStream(source);
    }

    void Unload() override {
        UnloadMusicStream(source);
    }
};

class Audioplayer : public Component {
private:
    std::unordered_map<std::string, std::shared_ptr<AudioSource>> audiosources;
public:
    Audioplayer() {}
    Audioplayer(std::unordered_map<std::string, std::shared_ptr<AudioSource>> audiosources) {
        this->audiosources = audiosources;
    }

    void Unload() {
        for (auto& audiosource : audiosources) {
            audiosource.second->Unload();
        }
    }

    void operator[ ](std::string audiosource) {
        audiosources[audiosource]->Play();
    }

};

#endif 
