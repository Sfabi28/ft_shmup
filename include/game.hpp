#pragma once

# include <ncurses.h>

constexpr int MIN_LINES = 50;
constexpr int MIN_COLS = 150;


void game_loop(int mode);
void draw_frame(WINDOW *frame);
