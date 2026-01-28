# Bulls And Cows
A classic logic game where you guess a secret number. After each attempt, you receive feedback:
- Bulls — correct digits in the correct position
- Cows — correct digits in the wrong position
The goal is to guess the full number within the allowed number of attempts!

# Rules
- The game generates a secret number of a given length.
- You enter your guess (must have the same length).
- After each guess:
- Bulls are highlighted with a red background.
- Cows are highlighted with a yellow background.
- Other digits are shown normally.
- You win if you guess the number exactly.
- You lose if you run out of attempts.

Digits may be unique or repeatable depending on the generator mode.

# Building

Requirements:
1) A C++20-compatible compiler (MSVC recommended)
2) CMake 3.14 or higher

# Usage
Run the game with optional command-line arguments:

## Options

| Option          | Description                                                               | Default |
| --------------- | ------------------------------------------------------------------------- | ------- |
| -l              | Length of the secret number                                               | 4       |
| -t              | Maximum number of attempts                                                | 6       |
| -g              | Generator type: unique (no repeated digits) or repeat (digits may repeat) | unique  |
| -h              | Hide previous attempts from the screen                                    | true    |
| -help           | Show this help message                                                    | —       |