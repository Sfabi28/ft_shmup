#include "AEnemy.hpp"
#include "Game.hpp"

AEnemy::AEnemy(float x, float y, char sym, int hp) 
    : AGameEntity(x, y, sym, hp, 3.0f, 3, 3) {
    _dx = 0;
    _dy = _speed;
    _colorPair = 2;
	setAsciiArt("w w", "\\_/", " V ");
}

AEnemy::~AEnemy() {}

void AEnemy::update(float dt, Game &game) {

    _y += _dy * dt;

    if (_y >= game.getHeight()) {
        _hp = 0;
    }
}