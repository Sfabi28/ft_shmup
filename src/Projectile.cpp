#include "Projectile.hpp"
#include "Game.hpp"

Projectile::Projectile(float x, float y, float dx, float dy, int color) 
    : AGameEntity(x, y, '*', 1, 1, 3, 3) {
    _dx = dx;
    _dy = dy;
    _colorPair = color;
	setAsciiArt("   ", " | ", "   ");
}

Projectile::~Projectile() {}

void Projectile::update(float dt, Game &game) {
    _x += _dx * dt;
    _y += _dy * dt;

    if (_y < 0 || _y >= game.getHeight() || _x < 0 || _x >= game.getWidth()) {
        _hp = 0;
    }
}