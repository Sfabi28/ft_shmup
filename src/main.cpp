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
}

int menu()
{
    timeout(100);

    clear();

    mvprintw(1, 0, "Welcome on ft_shmup!");
    mvprintw(3, 0, "Please select a Game Mode and start to play!");
    mvprintw(5, 0, "1) World 1");
    mvprintw(6, 0, "2) World 2");
    mvprintw(7, 0, "3) World 3");
    mvprintw(8, 0, "4) Endless");
    mvprintw(9, 0, "Esc) Quit");


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

int select_players()
{
    clear();

    mvprintw(3, 0, "Please select the number of players");
    mvprintw(5, 0, "1) 1 Player");
    mvprintw(6, 0, "2) 2 PLayers");
    mvprintw(7, 0, "Esc) Back to menu");

    while (true)
    {
        int answer = getch();
        if (answer == 27)
            return (0);
        else if (answer == '1')
            return (1);
        else if (answer == '2')
            return (2);
    }
}

int main()
{
    start_ncurses();

    int mode = -1;
    int players = 0;

    while (!players)
    {
        mode =  menu();
        if (mode == -1)
        {
            endwin();
            return (0);
        }
        players =  select_players();
    }
    
    clear();
    Game game;
    game.game_loop(mode, players);
    endwin();
}