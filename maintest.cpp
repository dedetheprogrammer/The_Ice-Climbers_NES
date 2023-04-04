#include <sstream>
#include <iostream>
#include <typeindex>
#include "raylib.h"
#include "raylibx.h"
#include "EngineECS.h"

int main() {
    GameObject a("Tu puta madre");
    a.addComponent<Transform2D>(Vector2{1,1});
    std::cout << a.getComponent<Transform2D>().position * 8 << "\n";
    a.printout();
}