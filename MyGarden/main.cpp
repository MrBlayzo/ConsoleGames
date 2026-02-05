#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "Game.h"

int main(int argc, char* argv[]) {
    MyGarden game = MyGarden();
    game.play();
    return 0;
}