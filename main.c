#define _POSIX_C_SOURCE 199309L
#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "tetris.h"

#define FALL_INTERVAL 1000000 // 1s

int main()
{
    initscr();
    curs_set(0); // 隐藏光标（必须放在 initscr() 之后！）
    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);    // I
        init_pair(2, COLOR_BLUE, COLOR_BLACK);    // J
        init_pair(3, COLOR_WHITE, COLOR_BLACK);   // L
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // O
        init_pair(5, COLOR_GREEN, COLOR_BLACK);   // S
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK); // T
        init_pair(7, COLOR_RED, COLOR_BLACK);     // Z
    }
    cbreak();
    noecho();
    keypad(stdscr, true);
    nodelay(stdscr, TRUE); // 设置非阻塞输入
    timeout(0);

    int yMax, xMax;
    Game game;
    init_game(&game);
    struct timespec last_fall_time;
    clock_gettime(CLOCK_MONOTONIC, &last_fall_time);

    getmaxyx(stdscr, yMax, xMax);
    // printw("%d,%d", yMax, xMax);                               // 28,79
    WINDOW *playwin1 = newwin(yMax - 3, (xMax / 2) - 4, 2, 2); // height,width,start_y,start_x
    box(playwin1, 0, 0);
    // WINDOW *playwin2 = newwin(yMax - 3, (xMax / 2) - 6, 2, 40); // height,width,start_y,start_x
    // box(playwin2, 0, 0);
    // printw("%d,%d", yMax - 3, (xMax / 2) - 6);   //25,33
    refresh();
    wrefresh(playwin1);
    // wrefresh(playwin2);

    while (1)
    {
        display_board(&game, playwin1);
        int ch;

        mvwprintw(playwin1, 2, 2, "Score:%d", game.score);
        wrefresh(playwin1);

        while ((ch = getch()) != ERR) // 读取所有缓冲按键,在每次按键处理清空缓冲
        {
            if (ch == 'q')
            {
                break;
            }
            else if (ch == KEY_LEFT || ch == KEY_RIGHT || ch == KEY_DOWN || ch == KEY_UP)
            {
                move_tetromino(&game, ch);
            }
            else if (ch == ' ')
            {
                hard_drop(&game);
            }
        }

        // 检查是否到了自动下落的时间
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        long elapsed_time = (current_time.tv_sec - last_fall_time.tv_sec) * 1000000L +
                            (current_time.tv_nsec - last_fall_time.tv_nsec) / 1000L;

        if (elapsed_time >= FALL_INTERVAL)
        {
            falling(&game);
            clock_gettime(CLOCK_MONOTONIC, &last_fall_time); // 更新上次下落时间
        }

        struct timespec ts = {0, 50000 * 1000L}; // 50ms
        nanosleep(&ts, NULL);
    }
    delwin(playwin1);
    // delwin(playwin2);
    endwin();
    return 0;
}
