#include <string>
#include <iostream>
#include "Game.h"

char to_char(Participant p)
{
    switch (p)
    {
    case Participant::player1:
        return '*';
    case Participant::player2:
        return 'O';
    case Participant::none:
        return ' ';
    }
    return '?';
}

Board::Board(int width, int height)
    : width(width),
      height(height),
      engine(),
      board(height, std::vector<Participant>(width, Participant::none)) {}

int Board::get_new_cursor_pos(int cursor)
{
    draw(cursor);
    std::string input = engine.get();
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
        draw(cursor);
        input = engine.get();
    }
    return cursor;
}

void Board::draw(int cursor)
{
    engine.clear();
    for (int i = 0; i <= cursor * 2; ++i)
    {
        engine.print(' ');
    }
    engine.print('v');
    for (int i = cursor * 2; i <= (width + 1) * 2; ++i)
    {
        engine.print(' ');
    }
    engine.print('\n');
    for (auto &row : board)
    {
        for (auto &p : row)
        {
            engine.print('|', to_char(p));
        }
        engine.print('|', '\n');
    }
}

Line4Game::Line4Game(int width, int height)
    : board(width, height),
      player1(std::make_unique<PeoplePlayer>()),
      player2(std::make_unique<PeoplePlayer>()) {}
Line4Game::Line4Game(std::unique_ptr<Player> player2,
                     int width, int height)
    : board(width, height),
      player1(std::make_unique<PeoplePlayer>()),
      player2(std::move(player2)) {}
Line4Game::Line4Game(std::unique_ptr<Player> player2,
                     std::unique_ptr<Player> player1,
                     int width, int height)
    : board(width, height),
      player1(std::move(player1)),
      player2(std::move(player2)) {}

void Line4Game::play()
{
    while (true)
    {
        player1->move(board);
    }
}

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

void PeoplePlayer::move(Board &board)
{
    cursor = board.get_new_cursor_pos(cursor);
}