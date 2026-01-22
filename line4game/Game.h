#pragma once
#include <vector>
#include <memory>
#include "ConsoleEngine.h"

enum class MoveTypes
{
    random,
    minimax
};

enum class Participant
{
    player1,
    player2,
    none
};

struct ComputeParams
{
    MoveTypes move_type;
    int max_depth;
};

class Board
{
public:
    Board(int width, int height);
    void draw();
    int get_new_cursor_pos();

private:
    int width;
    int height;
    ConsoleEngine engine;
    std::vector<std::vector<Participant>> board;
};

class Player
{
    virtual void move(Board board) = 0;
};

class ComuterPlayer : public Player
{
    ComuterPlayer(ComputeParams compute_params = ComputeParams{MoveTypes::minimax, 6});
    void move(Board board) override;

private:
    ComputeParams compute_params;
};

class PeoplePlayer : public Player
{
    PeoplePlayer() = default;
    void move(Board board) override;

private:
    int cursor;
};

class Line4Game
{
public:
    Line4Game(int width = 7, int height = 6);
    Line4Game(int width = 7, int height = 6, std::unique_ptr<Player> player2);
    Line4Game(int width = 7, int height = 6, std::unique_ptr<Player> player2,
              std::unique_ptr<Player> player1);
    void play();

private:
    Board board;
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;

    Participant check_win();
    Participant check_row_win();
    Participant check_col_win();
    Participant check_diag1_win();
    Participant check_diag2_win();
};