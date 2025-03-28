#include "tetris.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> //rand
#include <time.h>

// Initialize the board
void init_game(Game *game)
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            game->board[i][j] = ' ';
        }
    }

    game->score = 0;
    srand(time(NULL)); // get current time,transmit to srand function
                       // initialize srand function
    spawn_tetromino(game);
}

const TetrisLocation TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][TETRIS] = {
    // I
    {{{1, 0}, {1, 1}, {1, 2}, {1, 3}},
     {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
     {{3, 0}, {3, 1}, {3, 2}, {3, 3}},
     {{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
    // J
    {{{0, 0}, {1, 0}, {1, 1}, {1, 2}},
     {{0, 1}, {0, 2}, {1, 1}, {2, 1}},
     {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
     {{0, 1}, {1, 1}, {2, 0}, {2, 1}}},
    // L
    {{{0, 2}, {1, 0}, {1, 1}, {1, 2}},
     {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
     {{1, 0}, {1, 1}, {1, 2}, {2, 0}},
     {{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
    // O
    {{{0, 1}, {0, 2}, {1, 1}, {1, 2}},
     {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
     {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
     {{0, 1}, {0, 2}, {1, 1}, {1, 2}}},
    // S
    {{{0, 1}, {0, 2}, {1, 0}, {1, 1}},
     {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
     {{1, 1}, {1, 2}, {2, 0}, {2, 1}},
     {{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
    // T
    {{{0, 1}, {1, 0}, {1, 1}, {1, 2}},
     {{0, 1}, {1, 1}, {1, 2}, {2, 1}},
     {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
     {{0, 1}, {1, 0}, {1, 1}, {2, 1}}},
    // Z
    {{{0, 0}, {0, 1}, {1, 1}, {1, 2}},
     {{0, 2}, {1, 1}, {1, 2}, {2, 1}},
     {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
     {{0, 1}, {1, 0}, {1, 1}, {2, 0}}},
};

// create a random block
void spawn_tetromino(Game *game)
{
    game->current.type = rand() % NUM_TETROMINOS;
    game->current.orientation = 0;
    game->current.row = 0; // set int the row 0 and the middle of column

    int width = 3; // 大多数方块宽度
    if (game->current.type == TYPE_O)
        width = 2;
    else if (game->current.type == TYPE_I)
        width = 4;

    game->current.col = (COLS - width) / 4; // 居中计算// 精确居中
}

void rotate_tetromino(Game *game)
{
    int new_orientation = (game->current.orientation + 1) % NUM_ORIENTATIONS;

    Tetromino old = game->current;
    game->current.orientation = new_orientation;

    if (!can_move(game, game->current.row, game->current.col))
    {
        game->current = old;
    }
}
void display_board(Game *game, WINDOW *win)
{
    werase(win);
    box(win, 0, 0); // 重绘边框

    // 绘制已固定的方块
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (game->board[i][j] != ' ')
            {
                wattron(win, COLOR_PAIR(game->board[i][j] - '0' + 1) | A_REVERSE);
                mvwprintw(win, i + 1, j * 2 + 1, "  "); // 两个空格
                wattroff(win, COLOR_PAIR(game->board[i][j] - '0' + 1) | A_REVERSE);
            }
        }
    }

    // 绘制当前下落方块
    int type = game->current.type;
    int orientation = game->current.orientation;
    wattron(win, COLOR_PAIR(type + 1) | A_REVERSE);
    for (int t = 0; t < TETRIS; t++)
    {
        int r = game->current.row + TETROMINOS[type][orientation][t].row;
        int c = game->current.col + TETROMINOS[type][orientation][t].col;
        mvwprintw(win, r + 1, c * 2 + 1, "  ");
    }
    wattroff(win, COLOR_PAIR(type + 1) | A_REVERSE);

    wrefresh(win); // 必须刷新！
}

bool move_tetromino(Game *game, int dir)
{
    int new_row = game->current.row;
    int new_col = game->current.col;

    if (dir == KEY_LEFT)
    {
        new_col--;
    }
    else if (dir == KEY_RIGHT)
    {
        new_col++; // 先计算目标位置
        // 直接使用 can_move 检测
        if (can_move(game, new_row, new_col))
        {
            game->current.col = new_col; // 确认安全后更新
            return true;
        }
    }
    else if (dir == KEY_UP)
    {
        rotate_tetromino(game);
    }
    else if (dir == KEY_DOWN)
    {
        new_row++;
    }

    // 左右移动或普通下落的检测
    if (can_move(game, new_row, new_col))
    {
        game->current.row = new_row;
        game->current.col = new_col;
        return true;
    }

    return false;
}

void hard_drop(Game *game)
{
    while (can_move(game, game->current.row + 1, game->current.col))
    {
        game->current.row++; // 持续下落直到不能移动
    }
    if (fix_tetromino(game))
    {
        game_over(game);
    }
}

bool fix_tetromino(Game *game)
{
    int type = game->current.type;
    int orientation = game->current.orientation;
    for (int t = 0; t < TETRIS; t++)
    {
        int r = game->current.row + TETROMINOS[type][orientation][t].row;
        int c = game->current.col + TETROMINOS[type][orientation][t].col;
        game->board[r][c] = '0' + type;
    }

    check_line(game);

    spawn_tetromino(game);

    return !can_move(game, game->current.row, game->current.col);
}

bool can_move(Game *game, int new_row, int new_col)
{
    int type = game->current.type;
    int orientation = game->current.orientation;
    for (int t = 0; t < TETRIS; t++)
    {
        int r = new_row + TETROMINOS[type][orientation][t].row; // 二维访问
        int c = new_col + TETROMINOS[type][orientation][t].col;
        if (r >= GAME_AREA_HEIGHT || c < 0 || c >= GAME_AREA_WIDTH || game->board[r][c] != ' ')
        {
            return false;
        }
    }

    return true;
}

void falling(Game *game)
{
    if (can_move(game, game->current.row + 1, game->current.col))
    {
        game->current.row++;
    }
    else if (fix_tetromino(game))
    {
        game_over(game);
    }
}

// 检查有一行全都有方块，那么这一行消失，上面的方块用for循环往下移动
void check_line(Game *game)
{
    int lines_cleared = 0;
    // 从底部向上检查
    for (int i = GAME_AREA_HEIGHT - 1; i >= 0; i--)
    {
        bool line_full = true;

        // 检查当前行是否已满（注意只检查有效区域）
        for (int j = 0; j < GAME_AREA_WIDTH; j++)
        {
            if (game->board[i][j] == ' ')
            {
                line_full = false;
                break;
            }
        }

        // 如果行已满
        if (line_full)
        {
            lines_cleared++;
            // 将上方所有行下移
            for (int k = i; k > 0; k--)
            {
                for (int j = 0; j < GAME_AREA_WIDTH; j++)
                {
                    game->board[k][j] = game->board[k - 1][j];
                }
            }
            // 清空最顶层
            for (int j = 0; j < GAME_AREA_HEIGHT; j++)
            {
                game->board[0][j] = ' ';
            }
            // 因为下移了一行，需要再次检查当前行
            i++;
        }
    }
    if (lines_cleared > 0)
    {
        game->score += lines_cleared * 100;
    }
}

void game_over(Game *game)
{
    WINDOW *win = newwin(10, 30, (ROWS - 10) / 2, (COLS - 30) / 2 + 5);
    box(win, 0, 0);
    mvwprintw(win, 2, 2, "GAME OVER!");
    mvwprintw(win, 4, 2, "Final Score:%d", game->score);
    mvwprintw(win, 6, 2, "PRESS R TO RESTART");
    mvwprintw(win, 7, 2, "PRESS q TO quit");
    wrefresh(win);

    int ch;
    while ((ch = getch()) != 'q' && ch != 'r')
    {
    }

    if (ch == 'r')
    {
        init_game(game);
    }
    else
    {
        endwin();
        exit(0);
    }

    delwin(win);
}