#include "RammerEnemy.hpp"
#include "Game.hpp"

RammerEnemy::RammerEnemy(float x, float y, char sym, int hp)
	: AEnemy(x, y, sym, hp)
{
	_dx = 12.0f;
	_dy = 1.8f;
	_colorPair = 2;
	_scoreValue = 25;
	setAsciiArt("/M\\", "|_|", "v v");
}

RammerEnemy::~RammerEnemy() {}

void RammerEnemy::update(float dt, Game &game)
{
	_x += _dx * dt;
	_y += _dy * dt;

	if (_x <= 1) {
		_x = 1;
		_dx = -_dx;
	}
	if (_x + _width >= game.getWidth() - 1) {
		_x = game.getWidth() - _width - 1;
		_dx = -_dx;
	}

	if (_y >= game.getHeight()) {
		_hp = 0;
	}
}
