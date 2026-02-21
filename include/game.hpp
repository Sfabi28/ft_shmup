#pragma once

# include <ncurses.h>

constexpr int MIN_LINES = 40;
constexpr int MIN_COLS = 100;


void game_loop(int mode);
void draw_frame(WINDOW *frame);
