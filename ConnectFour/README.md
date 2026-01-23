# Connect Four
A classic two-player connection game where players take turns dropping colored discs into a vertically suspended grid. The goal is to be the first to form a horizontal, vertical, or diagonal line of four of one's own discs.

# Rules
- Players alternate turns.
- On each turn, a player drops one disc into any column.
- The disc falls to the lowest available space in that column.
- The first player to connect four of their discs in a row (horizontally, vertically, or diagonally) wins.
- If the board fills up with no winner, the game ends in a draw.

# Building

Requirements:
1) A C++20-compatible compiler (MSVC recommended)
2) CMake 3.14 or higher

# Usage
Run the game with optional command-line arguments:

## Options

| Option          | Description                                    | Default |
| --------------- | ---------------------------------------------- | ------- |
| -w N, -width N  | Board width                                    | 7       |
| -h N, -height N | Board height                                   | 6       |
| -p1 TYPE        | Player 1 type: human, random, or minimax:DEPTH | human   |
| -p2 TYPE        | Player 2 type: human, random, or minimax:DEPTH | human   |
| -help           | Show this help message                         | —       |