#include "game.hpp"
#include <ncurses.h>

void game_loop(int mode) {
    (void)mode;
    nodelay(stdscr, TRUE);
    while (true) {
        int answer = getch();
        if (answer == 27)
            return ;
    }
}
