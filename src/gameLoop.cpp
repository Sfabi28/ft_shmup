#include "game.hpp"
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>

bool handle_resize() {
    nodelay(stdscr, FALSE);
    while (LINES < MIN_LINES || COLS < MIN_COLS)
    {
        clear();
        mvprintw(LINES / 2, (COLS - 42) / 2, "Window too small (should be at least 150x50)");
        refresh();
        int answer = getch();
        if (answer == 27)
            return (true);
    }
    nodelay(stdscr, TRUE);
    clear();
    return (false);
}

void game_loop(int mode) {
    (void)mode;
    if (handle_resize())
        return ;
    
    int STARTING_X = (COLS - MIN_COLS) / 2;
    int STARTING_Y = (LINES - MIN_LINES) / 2;

    WINDOW *frame = newwin(MIN_LINES, MIN_COLS, STARTING_Y, STARTING_X);
    nodelay(stdscr, TRUE);

    draw_frame(frame);

    while (true) {
        auto start = std::chrono::steady_clock::now();
        int answer = getch();
        if (answer == 27) {
            delwin(frame);
            return ;
        }
        if (answer == KEY_RESIZE) {
            if (handle_resize()) {
                delwin(frame);
                return ;
            }
            werase(frame);
            wrefresh(frame);
            STARTING_X = (COLS - MIN_COLS) / 2;
            STARTING_Y = (LINES - MIN_LINES) / 2;
            mvwin(frame, STARTING_Y, STARTING_X);
        }
        auto end = std::chrono::steady_clock::now();
        auto elapsed = end - start;
        if (elapsed < std::chrono::milliseconds(16))
            std::this_thread::sleep_for(std::chrono::milliseconds(16) - elapsed);
        draw_frame(frame);
    }
}
