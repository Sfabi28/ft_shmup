#pragma once

#include <ncurses.h>
#include <vector>
#include <memory>
#include "Player.hpp"

class AGameEntity;

class Game {
private:
    WINDOW *_win;
    Player _player;
    std::vector<std::unique_ptr<AGameEntity>> _entities;
    std::vector<std::unique_ptr<AGameEntity>> _pending;

    int   _score;
    float _elapsed;
    bool  _running;

	float _spawnTimer;
    float _spawnInterval;

    void processInput();
    void update(float dt);
    void render();
    void checkCollisions();
    void removeDeadEntities();
    void flushPending();
    void spawnEnemies(float dt);

public:
    Game(WINDOW *win);
    ~Game();

    void run();

    // Le entità chiamano questo per creare proiettili
    void spawnEntity(std::unique_ptr<AGameEntity> entity);
    void addScore(int points);

    const Player &getPlayer() const;
    int   getWidth() const;
    int   getHeight() const;
    float getElapsed() const;
    int   getScore() const;
};