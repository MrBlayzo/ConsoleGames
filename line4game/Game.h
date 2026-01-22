#pragma once
#include <vector>
#include <memory>
#include <utility>
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
    MoveTypes move_type = MoveTypes::minimax;
    int max_depth = 6;
};

class Board
{
public:
    const int width;
    const int height;

    Board(int width, int height);
    void draw(int cursor);
    int get_new_cursor_pos(int cursor);
    bool try_add_piece(int cursor, Participant p);
    Board add_piece_to_new(int cursor, Participant p);
    void set_winner(Participant p);

    Participant check_win();
    bool is_col_fill(int col);
    bool is_fill();

private:
    ConsoleEngine engine;
    std::vector<std::vector<Participant>> board;
    Participant check_win_for_current_pos(Participant &prev, Participant &curr, int &player1_count, int &player2_count);
    Participant check_row_win();
    Participant check_col_win();
    Participant check_diag1_win();
    Participant check_diag2_win();
    void draw_cursor(int cursor);
    void draw_board();
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
    struct MoveResult {
        int score;
        int column;
    };
public:
    ComputerPlayer(Participant participant, ComputeParams params = ComputeParams{MoveTypes::minimax, 6});
    void move(Board &board) override;

private:
    ComputeParams compute_params;
    void random_move(Board &board);
    void minimax_move(Board &board);
    MoveResult calculate_next_move(Board board, Participant p, int depth, int alpha = INT_MIN, int beta = INT_MAX);
};

class HumanPlayer : public Player
{
public:
    HumanPlayer(Participant participant);
    void move(Board &board) override;

private:
    int cursor = 0;
    ;
};

class Line4Game
{
public:
    Line4Game(int width, int height,
              std::unique_ptr<Player> player1,
              std::unique_ptr<Player> player2);
    static Line4Game HumanVsComputer(int width = 7, int height = 6, ComputeParams params=ComputeParams());
    static Line4Game ComputerVsHuman(int width = 7, int height = 6, ComputeParams params=ComputeParams());
    static Line4Game ComputerVsComputer(int width = 7, int height = 6,
                                        ComputeParams params_1=ComputeParams(), ComputeParams params_2=ComputeParams());
    static Line4Game HumanVsHuman(int width = 7, int height = 6);
    void play();

private:
    Board board;
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
};