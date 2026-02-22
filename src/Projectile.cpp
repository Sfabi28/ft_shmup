#include "Projectile.hpp"
#include "Game.hpp"

Projectile::Projectile(float x, float y, float dx, float dy, int color) 
    : AGameEntity(x, y, '|', 1, 1, 1, 1) {
    _dx = dx;
    _dy = dy;
    _colorPair = color;
    // Assign team based on color: 1=white (neutral), 2=red (enemy), 3=green (player)
    if (color == 3) {
        _team = Team::Player;
    } else if (color == 2) {
        _team = Team::Enemy;
    } else {
        _team = Team::Neutral;
    }
}

Projectile::~Projectile() {}

void Projectile::update(float dt, Game &game) {
    _x += _dx * dt;
    _y += _dy * dt;

    if (_y < Game::TOP_ROW + 1 || _y >= game.getHeight() - 1 || 
        _x < 1 || _x >= game.getWidth() - 1) {
        _hp = 0;
    }
}