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

    if (_y <= Game::TOP_ROW ||           // Tocca/attraversa la linea score (y <= 5)
        _y >= game.getHeight() - 1 ||    // Tocca il bordo inferiore (y >= 39)
        _x <= 1 ||                       // Tocca il bordo sinistro
        _x >= game.getWidth() - 2) {     // Tocca il bordo destro
        _hp = 0;
    }
}