#include <string>
#include <iostream>
#include "Game.h"

Board::Board(int width, int height):
    board(height, std::vector<Participant>(width, Participant::none)) {}

Line4Game::Line4Game(int width = 7, int height = 6)
    : board(width, height),
      player1(std::make_unique<PeoplePlayer>()),
      player2(std::make_unique<PeoplePlayer>()) {}
Line4Game::Line4Game(int width = 7, int height = 6, std::unique_ptr<Player> player2)
    : board(width, height),
      player1(std::make_unique<PeoplePlayer>()),
      player2(std::move(player2)) {}
Line4Game::Line4Game(int width = 7, int height = 6, std::unique_ptr<Player> player2,
                     std::unique_ptr<Player> player1)
    : board(width, height),
      player1(std::move(player1)),
      player2(std::move(player2)) {}

void Line4Game::play()
{
    while (true)
    {
        player1->move(board)
    }
}

void Board::draw() {}

Participant Line4Game::check_win()
{
    if (auto result = check_row_win(); result != Participant::none)
        return result;
    if (auto result = check_col_win(); result != Participant::none)
        return result;
    if (auto result = check_diag1_win(); result != Participant::none)
        return result;
    if (auto result = check_diag2_win(); result != Participant::none)
        return result;
    return Participant::none;
}
Participant Line4Game::check_row_win() { return Participant::none; }
Participant Line4Game::check_col_win() { return Participant::none; }
Participant Line4Game::check_diag1_win() { return Participant::none; }
Participant Line4Game::check_diag2_win() { return Participant::none; }

PeoplePlayer::move(Line4Game& game){
    std::string input = game.engine.get();
    while (!input.empty())
    {
        for (auto &c : input)
        {
            if (c == 'a')
            {
                cursor = std::max(cursor - 1, 0);
            }
            else if (c == 'd')
            {
                cursor = std::min(cursor + 1, width - 1);
            }
        }
        draw();
        input = engine.get();
    }
}