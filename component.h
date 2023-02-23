#pragma once
#ifndef COMPONENT_H
#define COMPONENT_H

class Component {
private:
    //...
public:
    enum COMPONENT_ENUM { ANIMATOR, AUDIOSOURCE };
    // Un poco de spoiler de lo que se viene...
    Component() {}

    //virtual Unload() = 0

};

//void Unload(Component& c) {
//    c.Unload();
//}

#endif