#include <iostream>
#include "ConsoleEngine.h"
#include "Game.h"

int main(){
    ConsoleEngine engine{};
    engine.print("aaaa", "\n", "bbbb");
    Line4Game game{};
    game.play();
    return 0;
}