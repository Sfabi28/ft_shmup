#include "AEnemy.hpp"
#include "Game.hpp"

AEnemy::AEnemy(float x, float y, char sym, int hp) 
    : AGameEntity(x, y, sym, hp, 3.0f, 3, 3, Team::Enemy) {
    _dx = 0;
    _dy = _speed;
    _colorPair = 2;
    _projectileColor = 2;
    _shootTimer = 0.0f;
    _shootInterval = 2.5f;
	_scoreValue = 10;
	setAsciiArt("w w", "\\_/", " V ");
}

AEnemy::~AEnemy() {}

void AEnemy::update(float dt, Game &game) {
    _y += _dy * dt;
    
    _shootTimer += dt;
    if (_shootTimer >= _shootInterval) {
        float centerX = _x + 1;
        float centerY = _y + 1;
        game.spawnProjectile(centerX, centerY + 2, 0, 8, _projectileColor, Team::Enemy);
        _shootTimer = 0;
    }

    if (_y >= game.getHeight()) {
        _hp = 0;
    }
}

int AEnemy::getScoreValue() const {
	return _scoreValue;
}