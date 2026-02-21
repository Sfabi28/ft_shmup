#include "Game.hpp"

void Game::draw_frame(WINDOW *frame)
{
    box(frame, 0, 0);
    mvwhline(frame, TOP_ROW, 1, ACS_HLINE, MIN_COLS - 2);
    mvwaddch(frame, TOP_ROW, 0, ACS_LTEE);
    mvwaddch(frame, TOP_ROW, MIN_COLS - 1, ACS_RTEE);

    std::string score_str = "score: " + std::to_string(getScore());

    ascii_art(frame, -1, 2, score_str, 0, false);

    wrefresh(frame);
}