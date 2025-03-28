#ifndef _TETRIS_H
#define _TETRIS_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>

#define ROWS 25
#define COLS 33
#define NUM_TETROMINOS 7
#define NUM_ORIENTATIONS 4
#define TETRIS 4
#define MAX_LEVEL 19
#define GAME_AREA_WIDTH (COLS - 17)
#define GAME_AREA_HEIGHT (ROWS - 2)

// **定义数据结构和函数声明**
//   方块的结构

//    - 定义方块类型。
typedef enum
{
    TYPE_I,
    TYPE_O,
    TYPE_T,
    TYPE_S,
    TYPE_Z,
    TYPE_J,
    TYPE_L
} TetrominoType;

typedef struct
{
    TetrominoType type;
    int orientation;
    int row, col; // location
} Tetromino;

// 在 tetris.h 中添加
typedef struct
{
    int row;
    int col;
} TetrisLocation;

extern const TetrisLocation TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][TETRIS];

//  - 定义棋盘大小和当前状态（`ROWS` 和 `COLS`）。
typedef struct
{
    char board[ROWS][COLS];
    Tetromino current;
    int score;
} Game;

//    - 声明函数（如 `init_game`、`spawn_tetromino`、`display_board` 等）。
void init_game(Game *game);
void spawn_tetromino(Game *game);
void display_board(Game *game, WINDOW *win);
bool move_tetromino(Game *game, int dir);
void falling(Game *game);
bool move_tetromino(Game *game, int dir);
bool can_move(Game *game, int new_row, int new_col);
bool fix_tetromino(Game *game);
void rotate_tetromino(Game *game);
void hard_drop(Game *game);
void check_line(Game *game);
void game_over(Game *game);
#endif
