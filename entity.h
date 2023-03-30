#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <unordered_map>
#include <vector>
#include <typeindex>
#include "components.h"

class GameObject {
private:
    //...
public:
    int id;
    std::unordered_map<std::type_index, Component*> components;

    template <typename T>
    T& getComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return dynamic_cast<T&>(*it->second);
        }
        throw std::runtime_error("Componente no encontrado.");
    }

    template <typename T, typename... Args>
    void addComponent(Args&&... args) {
        components[typeid(T)] = new T(std::forward<Args>(args)...);
    }

    template <typename T>
    void removeComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            delete it->second;
            components.erase(it);
        }
    }
};



#endif