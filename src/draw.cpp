#include "game.hpp"

void draw_frame(WINDOW *frame) {
    box(frame, 0, 0);
    wrefresh(frame);
}