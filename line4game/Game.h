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

char to_char(Participant p);

struct ComputeParams
{
    MoveTypes move_type;
    int max_depth;
};

class Board
{
public:
    Board(int width, int height);
    void draw(int cursor);
    int get_new_cursor_pos(int cursor);
    bool try_add_piece(int cursor, Participant p);

private:
    int width;
    int height;
    ConsoleEngine engine;
    std::vector<std::vector<Participant>> board;
};

class Player
{
public:
    Player(Participant participant);
    virtual ~Player() = default;
    virtual void move(Board &board) = 0;
protected:
    Participant participant;
};

class ComputerPlayer : public Player
{
public:
    ComputerPlayer(Participant participant, ComputeParams params = ComputeParams{MoveTypes::minimax, 6});
    void move(Board &board) override;

private:
    ComputeParams compute_params;
};

class PeoplePlayer : public Player
{
public:
    PeoplePlayer(Participant participant);
    void move(Board &board) override;

private:
    int cursor = 0;;
};

class Line4Game
{
public:
    Line4Game(int width = 7, int height = 6);
    Line4Game(std::unique_ptr<Player> player2,
              int width = 7, int height = 6);
    Line4Game(std::unique_ptr<Player> player2,
              std::unique_ptr<Player> player1,
              int width = 7, int height = 6);
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