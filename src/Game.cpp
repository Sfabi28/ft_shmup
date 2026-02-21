#include "Game.hpp"

Game::Game() noexcept : _score(0) {}

int Game::getWidth() const {
    return MIN_COLS;
}

int Game::getHeight() const {
    return MIN_LINES;
}

int Game::getScore() const {
    return _score;
}

void Game::addScore(int points) {
    _score += points;
}

void Game::resetScore() {
    _score = 0;
}