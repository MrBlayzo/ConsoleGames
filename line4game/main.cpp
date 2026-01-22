#include <iostream>
#include "ConsoleEngine.h"
#include "Game.h"

int main(){
    ConsoleEngine engine{};
    //Line4Game game = Line4Game::ComputerVsComputer(7, 10, ComputeParams{MoveTypes::minimax, -1}, ComputeParams{MoveTypes::minimax, -1});
    Line4Game game = Line4Game::HumanVsComputer(5, 5, ComputeParams{MoveTypes::minimax, 10});
    game.play();
    return 0;
}