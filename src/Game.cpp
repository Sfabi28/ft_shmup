#include "Game.hpp"

Game::Game() noexcept : _score(0), _endlessElapsedSeconds(-1), _world1CurrentWave(-1), _world1TotalWaves(0), _backgroundOffset(0.0f)
{
	initializeBackground();
}

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

void Game::initializeBackground() {
    _backgroundStars.clear();
    
    // Crea circa 30-40 stelle sparse
    srand(time(nullptr));
    for (int i = 0; i < 35; i++) {
        Star star;
        star.x = static_cast<float>(rand() % (MIN_COLS - 4) + 2);
        star.y = static_cast<float>(rand() % (MIN_LINES - TOP_ROW - 2) + TOP_ROW + 1);
        
        // Varia il tipo di stella
        int type = rand() % 3;
        if (type == 0) star.symbol = '.';
        else if (type == 1) star.symbol = '*';
        else star.symbol = '+';
        
        _backgroundStars.push_back(star);
    }
}

void Game::updateBackground(float dt) {
    // Muovi stelle verso il basso lentamente
    for (auto &star : _backgroundStars) {
        star.y += 3.0f * dt;  // Più lento dei nemici (che vanno a 3.0f)
        
        // Quando esce dal fondo, riappare in alto in posizione casuale
        if (star.y >= MIN_LINES - 1) {
            star.y = TOP_ROW + 1;
            star.x = static_cast<float>(rand() % (MIN_COLS - 4) + 2);
            
            // Cambia anche il tipo quando riappare
            int type = rand() % 3;
            if (type == 0) star.symbol = '.';
            else if (type == 1) star.symbol = '*';
            else star.symbol = '+';
        }
    }
}

void Game::drawBackground(WINDOW *frame) {
    wattron(frame, COLOR_PAIR(4) | A_DIM);
    
    for (const auto &star : _backgroundStars) {
        int drawY = static_cast<int>(star.y);
        int drawX = static_cast<int>(star.x);
        
        if (drawY > TOP_ROW && drawY < MIN_LINES - 1 && 
            drawX > 0 && drawX < MIN_COLS - 1) {
            mvwaddch(frame, drawY, drawX, star.symbol);
        }
    }
    
    wattroff(frame, COLOR_PAIR(4) | A_DIM);
}