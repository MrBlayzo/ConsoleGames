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
        for (const auto &c : input)
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
    draw_cursor(cursor);
    draw_board();
}

void Board::draw_cursor(int cursor)
{
    for (int i = 0; i <= cursor * 2; ++i)
    {
        engine.print(' ');
    }
    engine.print('v'); // TODO: рассмотреть возможность смены цвета или самого курсора для разных игроков
    for (int i = cursor * 2; i <= (width + 1) * 2; ++i)
    {
        engine.print(' ');
    }
    engine.print('\n');
}

void Board::draw_board()
{
    for (const auto &row : board)
    {
        for (const auto &p : row)
        {
            engine.print('|', to_char(p));
        }
        engine.print('|', '\n');
    }
}

bool Board::try_add_piece(int cursor, Participant p)
{
    if (is_col_fill(cursor))
        return false;
    int row = 0;
    while (row < height && board[row][cursor] == Participant::none)
        ++row;
    board[row - 1][cursor] = p;
    draw(cursor);
    return true;
}

Board Board::add_piece_to_new(int cursor, Participant p)
{
    if (is_col_fill(cursor))
        return *this;
    Board new_board = *this;
    int row = 0;
    while (row < height && new_board.board[row][cursor] == Participant::none)
        ++row;
    new_board.board[row - 1][cursor] = p;
    return new_board;
}

void Board::set_winner(Participant p)
{
    // engine.clear();
    if (p == Participant::player1)
        engine.print("Player 1 win!!!");
    else if (p == Participant::player2)
        engine.print("Player 2 win!!!");
    else
        engine.print("Draw");
}

bool Board::is_col_fill(int col)
{
    return board[0][col] != Participant::none;
}
bool Board::is_fill()
{
    for (int i = 0; i < width; ++i)
    {
        if (!is_col_fill(i))
            return false;
    }
    return true;
}

Participant Board::check_win()
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
Participant Board::check_win_for_current_pos(Participant &prev, Participant &curr, int &player1_count, int &player2_count)
{
    if (curr != prev)
    {
        switch (curr)
        {
        case Participant::player1:
            player1_count = 1;
            player2_count = 0;
            break;
        case Participant::player2:
            player2_count = 1;
            player1_count = 0;
            break;
        default:
            player1_count = 0;
            player2_count = 0;
            break;
        }
    }
    else
    {
        switch (curr)
        {
        case Participant::player1:
            ++player1_count;
            break;
        case Participant::player2:
            ++player2_count;
            break;
        default:
            break;
        }
    }
    if (player1_count >= 4)
        return Participant::player1;
    if (player2_count >= 4)
        return Participant::player2;
    return Participant::none;
}
Participant Board::check_row_win()
{
    for (int row = 0; row < height; ++row)
    {
        int player1_count = 0;
        int player2_count = 0;
        Participant prev = Participant::none;
        for (int col = 0; col < width; ++col)
        {
            Participant &curr = board[row][col];
            Participant winner = check_win_for_current_pos(prev, curr, player1_count, player2_count);
            if (winner != Participant::none)
                return winner;
            prev = curr;
        }
    }
    return Participant::none;
}
Participant Board::check_col_win()
{
    for (int col = 0; col < width; ++col)
    {
        int player1_count = 0;
        int player2_count = 0;
        Participant prev = Participant::none;
        for (int row = 0; row < height; ++row)
        {
            Participant &curr = board[row][col];
            Participant winner = check_win_for_current_pos(prev, curr, player1_count, player2_count);
            if (winner != Participant::none)
                return winner;
            prev = curr;
        }
    }
    return Participant::none;
}
Participant Board::check_diag1_win()
{
    for (int d = 3; d < width + height - 1 - 3; ++d)
    {
        int row = std::max(0, height - d - 1);
        int col = std::max(0, d - height + 1);
        int player1_count = 0;
        int player2_count = 0;
        Participant prev = Participant::none;
        for (int i = 0; i < std::min(height - row, width - col); ++i)
        {
            Participant &curr = board[row + i][col + i];
            Participant winner = check_win_for_current_pos(prev, curr, player1_count, player2_count);
            if (winner != Participant::none)
                return winner;
            prev = curr;
        }
    }
    return Participant::none;
}
Participant Board::check_diag2_win()
{
    for (int d = 3; d < width + height - 1 - 3; ++d)
    {
        int row = std::min(height - 1, d);
        int col = std::max(0, d - height + 1);
        int player1_count = 0;
        int player2_count = 0;
        Participant prev = Participant::none;
        for (int i = 0; i < std::min(row + 1, width - col); ++i)
        {
            Participant &curr = board[row - i][col + i];
            Participant winner = check_win_for_current_pos(prev, curr, player1_count, player2_count);
            if (winner != Participant::none)
                return winner;
            prev = curr;
        }
    }
    return Participant::none;
}

ConnectFour::ConnectFour(int width, int height,
                     std::unique_ptr<Player> player1,
                     std::unique_ptr<Player> player2)
    : board(width, height),
      player1(std::move(player1)),
      player2(std::move(player2))
{
    std::srand(std::time(nullptr));
}

ConnectFour ConnectFour::HumanVsComputer(int width, int height, ComputeParams params)
{
    return ConnectFour(width, height,
                     std::make_unique<HumanPlayer>(Participant::player1),
                     std::make_unique<ComputerPlayer>(Participant::player2, params));
}
ConnectFour ConnectFour::ComputerVsHuman(int width, int height, ComputeParams params)
{
    return ConnectFour(width, height,
                     std::make_unique<ComputerPlayer>(Participant::player1, params),
                     std::make_unique<HumanPlayer>(Participant::player2));
}
ConnectFour ConnectFour::ComputerVsComputer(int width, int height, ComputeParams params_1, ComputeParams params_2)
{
    return ConnectFour(width, height,
                     std::make_unique<ComputerPlayer>(Participant::player1, params_1),
                     std::make_unique<ComputerPlayer>(Participant::player2, params_2));
}
ConnectFour ConnectFour::HumanVsHuman(int width, int height)
{
    return ConnectFour(width, height,
                     std::make_unique<HumanPlayer>(Participant::player1),
                     std::make_unique<HumanPlayer>(Participant::player2));
}

void ConnectFour::play()
{
    Participant winner;
    while (true)
    {
        player1->move(board);
        winner = board.check_win();
        if (winner != Participant::none)
        {
            board.set_winner(winner);
            return;
        }
        if (board.is_fill())
        {
            board.set_winner(winner);
            return;
        }

        player2->move(board);
        winner = board.check_win();
        if (winner != Participant::none)
        {
            board.set_winner(winner);
            return;
        }
        if (board.is_fill())
        {
            board.set_winner(winner);
            return;
        }
    }
}

Player::Player(Participant participant) : participant(participant) {}

HumanPlayer::HumanPlayer(Participant p) : Player(p) {}

ComputerPlayer::ComputerPlayer(Participant p, ComputeParams params)
    : Player(p), compute_params(params) {}

void HumanPlayer::move(Board &board)
{
    bool valid_move = false;
    while (!valid_move)
    {
        cursor = board.get_new_cursor_pos(cursor);
        valid_move = board.try_add_piece(cursor, participant);
    }
}

void ComputerPlayer::move(Board &board)
{
    switch (compute_params.move_type)
    {
    case MoveTypes::random:
        random_move(board);
        break;
    case MoveTypes::minimax:
        minimax_move(board);
        break;
    default:
        std::cerr << "Undefined type of computer";
        throw std::runtime_error("Undefined type of computer");
        break;
    }
}

void ComputerPlayer::random_move(Board &board)
{
    bool valid_move = false;
    while (!valid_move)
    {
        int cursor = std::rand() % (board.width);
        valid_move = board.try_add_piece(cursor, participant);
    }
}

ComputerPlayer::MoveResult ComputerPlayer::calculate_next_move(Board board, Participant p, int depth, int alpha, int beta)
{
    Participant winner = board.check_win();
    if (winner != Participant::none)
    {
        int base_score = (winner == participant) ? 1 : -1;
        int scaled_score = base_score * (1000 - depth + 1);
        return {scaled_score, -1};
    }

    if (compute_params.max_depth != -1 and depth > compute_params.max_depth)
        return {0, -1};
    if (board.is_fill())
        return {0, -1};

    bool is_maximizing = (p == participant);
    int best_score = is_maximizing ? -1000 : +1000;
    int best_move = -1;

    for (int i = 0; i < board.width; ++i)
    {
        if (board.is_col_fill(i))
            continue;

        auto next_check = calculate_next_move(board.add_piece_to_new(i, p),
                                              p == Participant::player1 ? Participant::player2 : Participant::player1,
                                              depth + 1, alpha, beta);
        if (is_maximizing)
        {
            if (next_check.score > best_score)
            {
                best_score = next_check.score;
                best_move = i; // TODO: почти сразу инициализируется 0, поэтому если достигнута глубина просчета - вернет 0
            }
            alpha = std::max(alpha, best_score);
        }
        else
        {
            if (next_check.score < best_score)
            {
                best_score = next_check.score;
                best_move = i;
            }
            beta = std::min(beta, best_score);
        }

        if (beta <= alpha)
            break;
    }
    return {best_score, best_move};
}

void ComputerPlayer::minimax_move(Board &board)
{

    auto next_check = calculate_next_move(board,
                                          participant,
                                          0);
    if (next_check.column != -1)
        board.try_add_piece(next_check.column, participant);
    else
        random_move(board);
}

std::unique_ptr<Player> player_from_string(std::string params, Participant p)
{
    if (params == "human")
    {
        return std::make_unique<HumanPlayer>(p);
    }
    if (params.rfind("minimax:", 0) == 0)
    {
        int depth = std::stoi(params.substr(8));
        return std::make_unique<ComputerPlayer>(p, ComputeParams{MoveTypes::minimax, depth});
    }
    if (params.rfind("random", 0) == 0)
    {
        return std::make_unique<ComputerPlayer>(p, ComputeParams{MoveTypes::random, -1});
    }
    std::cerr << "Invalid param for player" << params << "\n"
              << "Use default value: " << "minimax:6" << "\n";
    return std::make_unique<ComputerPlayer>(p);
}