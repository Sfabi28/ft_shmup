#include "StationaryEnemy.hpp"
#include "Game.hpp"

StationaryEnemy::StationaryEnemy(float x, float y, char sym, int hp)
	: AEnemy(x, y, sym, hp)
{
	_dy = 0;
	_colorPair = 2;
	_projectileColor = 2;
	_shootInterval = 3.0f;
	_scoreValue = 20;
	setAsciiArt("_=_", " | ", "_=_");
}

StationaryEnemy::~StationaryEnemy() {}

void StationaryEnemy::update(float dt, Game &game)
{
	_shootTimer += dt;
	if (_shootTimer >= _shootInterval) {
		float centerX = _x + 1;
		float centerY = _y + 1;
		game.spawnProjectile(centerX, centerY + 2, 0, 8, _projectileColor, Team::Enemy);
		_shootTimer = 0;
	}
}
