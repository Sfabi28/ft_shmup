#pragma once

# include <ncurses.h>
# include <string>

constexpr int MIN_LINES = 40;
constexpr int MIN_COLS = 120;
constexpr int TOP_ROW = 5;



void game_loop(int mode);
void draw_frame(WINDOW *frame, int score);
void ascii_art(WINDOW *win, int y, int x, const std::string &text, int area_width, bool center);
