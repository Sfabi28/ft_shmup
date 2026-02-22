#include "Game.hpp"

#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>

void start_ncurses(void)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
}

int menu()
{
    timeout(100);

    mvprintw(1, 0, "Welcome on ft_shmup!");
    mvprintw(3, 0, "Please select a Game Mode and start to play!");
    mvprintw(5, 0, "1) World 1");
    mvprintw(6, 0, "2) World 2");
    mvprintw(7, 0, "3) World 3");
    mvprintw(8, 0, "4) Endless");

    while (true)
    {
        int answer = getch();
        if (answer == 27)
            return (-1);
        else if (answer == '1')
            return (1);
        else if (answer == '2')
            return (2);
        else if (answer == '3')
            return (3);
        else if (answer == '4')
            return (4);
    }
}

int main()
{
    start_ncurses();

    int mode = menu();
    if (mode == -1)
    {
        endwin();
        return (0);
    }
    clear();
    Game game;
    game.game_loop(mode);
    endwin();
}