#include "Game.hpp"

void Game::draw_frame(WINDOW *frame)
{
    box(frame, 0, 0);
    mvwhline(frame, TOP_ROW, 1, ACS_HLINE, MIN_COLS - 2);
    mvwaddch(frame, TOP_ROW, 0, ACS_LTEE);
    mvwaddch(frame, TOP_ROW, MIN_COLS - 1, ACS_RTEE);

    std::string score_str = "score:" + std::to_string(getScore());
    ascii_art(frame, -1, 2, score_str, 0, false);

    if (_endlessElapsedSeconds >= 0) {
        int minutes = _endlessElapsedSeconds / 60;
        int seconds = _endlessElapsedSeconds % 60;
        std::string minutes_str = (minutes < 10 ? "0" : "") + std::to_string(minutes);
        std::string seconds_str = (seconds < 10 ? "0" : "") + std::to_string(seconds);
        std::string timer_str = minutes_str + ":" + seconds_str;
        ascii_art(frame, -1, MIN_COLS / 2 - 10, timer_str, 0, false);
    }
    else if (_boss && _boss->getIsAlive()) {
        std::string boss_hp_str = std::to_string(_boss->getHP()) + "/" + std::to_string(_boss->getMaxHP());
        ascii_art(frame, -1, MIN_COLS / 2 - 10, boss_hp_str, 0, false);
    }
    else if (_world1CurrentWave > 0 && _world1TotalWaves > 0) {
        std::string wave_str = std::to_string(_world1CurrentWave) + " /" + std::to_string(_world1TotalWaves);
        ascii_art(frame, -1, MIN_COLS / 2 - 10, wave_str, 0, false);
    }

    if (_player) {
        std::string lives_str = "lives:" + std::to_string(_player->getLives());
        ascii_art(frame, -1, MIN_COLS - 33, lives_str, 0, false);
    }
}