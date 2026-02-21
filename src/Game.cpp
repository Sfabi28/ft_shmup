#include "Game.hpp"
#include "AEnemy.hpp"
#include "Projectile.hpp"
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstdlib>

Game::Game(WINDOW *win)
    : _win(win),
      _player(0, 0),
      _score(0),
      _elapsed(0.f),
      _running(true)
{
    int w, h;
    getmaxyx(_win, h, w);

    _player = Player(w / 2, h - 3);
}

Game::~Game() {}

void Game::run() {
    using clock = std::chrono::steady_clock;
    constexpr auto targetFrame = std::chrono::milliseconds(16); // ~30 FPS

    auto lastTime = clock::now();

    while (_running) {
        // ═══ CALCOLA DT ═══
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        // Protezione: se dt è troppo grande (es. debug, lag) limita
        if (dt > 0.1f) dt = 0.1f;

        // ═══ 1. INPUT ═══
        processInput();

        // ═══ 2. UPDATE ═══
        update(dt);

        // ═══ 3. RENDER ═══
        render();

        // ═══ 4. SLEEP ═══
        auto frameEnd = clock::now();
        auto elapsed = frameEnd - now;
        if (elapsed < targetFrame)
            std::this_thread::sleep_for(targetFrame - elapsed);
    }
}

void Game::processInput() {
    _player.resetInput();

    int key;
    while ((key = wgetch(_win)) != ERR) {
        switch (key) {
            case 27:        _running = false; break;
            case KEY_UP:    _player.setDirection(0, true); break;
            case KEY_DOWN:  _player.setDirection(1, true); break;
            case KEY_LEFT:  _player.setDirection(2, true); break;
            case KEY_RIGHT: _player.setDirection(3, true); break;
            case ' ':       _player.setShooting(true); break;
        }
    }
}

void Game::update(float dt) {
    _elapsed += dt;

    // Player
    _player.update(dt, *this);

    // Clamp player dentro lo schermo
    float maxX = static_cast<float>(getWidth() - 1);
    float maxY = static_cast<float>(getHeight() - 2); // spazio per HUD
    if (_player.getX() < 0) _player.setX(0);
    if (_player.getX() > maxX) _player.setX(maxX);
    if (_player.getY() < 1) _player.setY(1);  // riga 0 = HUD
    if (_player.getY() > maxY) _player.setY(maxY);

    // Tutte le entità
    for (auto &e : _entities)
        e->update(dt, *this);

    // Collisioni
    checkCollisions();

    // Rimuovi morti
    removeDeadEntities();

    // Aggiungi nuove entità (proiettili sparati durante l'update)
    flushPending();

    // Spawna nemici
    spawnEnemies(dt);
}

void Game::checkCollisions() {
    // Proiettili player vs nemici
    for (auto &e1 : _entities) {
        if (!e1->getIsAlive()) continue;

        for (auto &e2 : _entities) {
            if (!e2->getIsAlive() || e1.get() == e2.get()) continue;

            // Collisione semplice: stessa cella
            int x1 = static_cast<int>(e1->getX());
            int y1 = static_cast<int>(e1->getY());
            int x2 = static_cast<int>(e2->getX());
            int y2 = static_cast<int>(e2->getY());

            if (x1 == x2 && y1 == y2) {
                e1->takeDamage(1);
                e2->takeDamage(1);
                _score += 100;
            }
        }
    }

    // Nemici/proiettili vs player
    int px = static_cast<int>(_player.getX());
    int py = static_cast<int>(_player.getY());
    for (auto &e : _entities) {
        if (!e->getIsAlive()) continue;
        int ex = static_cast<int>(e->getX());
        int ey = static_cast<int>(e->getY());
        if (ex == px && ey == py) {
            e->takeDamage(1);
            // TODO: player perde vita
        }
    }
}

void Game::removeDeadEntities() {
    _entities.erase(
        std::remove_if(_entities.begin(), _entities.end(),
            [](const std::unique_ptr<AGameEntity> &e) {
                return !e->getIsAlive();
            }),
        _entities.end()
    );
}

void Game::flushPending() {
    for (auto &e : _pending)
        _entities.push_back(std::move(e));
    _pending.clear();
}

void Game::spawnEnemies(float dt) {
    // Per ora spawn random semplice
    // Un nemico ogni ~2 secondi
    static float spawnTimer = 0.f;
    spawnTimer += dt;

    if (spawnTimer >= 2.0f) {
        spawnTimer = 0.f;
        int randX = rand() % getWidth();
        _entities.push_back(
            std::make_unique<AEnemy>(static_cast<float>(randX), 0.f, 'V', 1)
        );
    }
}

void Game::render() {
    werase(_win);

    // Entità
    for (auto &e : _entities)
        e->render(_win);

    // Player
    _player.render(_win);

    // HUD
    mvwprintw(_win, 0, 0, "Score: %d  Time: %.0f", _score, _elapsed);

    wrefresh(_win);
}

void Game::spawnEntity(std::unique_ptr<AGameEntity> entity) {
    _pending.push_back(std::move(entity));
}

void Game::addScore(int points) { _score += points; }

const Player &Game::getPlayer() const { return _player; }
int Game::getWidth() const { int w, h; getmaxyx(_win, h, w); (void)h; return w; }
int Game::getHeight() const { int w, h; getmaxyx(_win, h, w); (void)w; return h; }
float Game::getElapsed() const { return _elapsed; }
int Game::getScore() const { return _score; }