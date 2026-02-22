#include "Game.hpp"

Game::Game() noexcept : _score(0), _endlessElapsedSeconds(-1), _world1CurrentWave(-1), _world1TotalWaves(0) {}

Game::~Game() {}

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

void Game::spawnAsteroid(float x, float y)
{
    _asteroids.push_back(std::make_unique<Asteroid>(x, y, 3));
}