#include "Player.hpp"

Player::Player(float x, float y) : AGameEntity(x, y, '@'), _shootCooldown(0.5f), _shootTimer(0), _invincible(false), _moveUp(false), _moveDown(false), _moveLeft(false), _moveRight(false), _shooting(false) {}
Player::~Player() {}

void Player::update(float dt, Game &game)
{
	_shootTimer += dt;
	if (_shooting && _shootTimer >= _shootCooldown)
	{
		_shootTimer = 0;
		//TODO: creare un proiettile
	}



	// Movimento
	float moveX = 0, moveY = 0;
	if (_moveUp) moveY -= 1;
	if (_moveDown) moveY += 1;
	if (_moveLeft) moveX -= 1;
	if (_moveRight) moveX += 1;

	setDx(moveX * getSpeed());
	setDy(moveY * getSpeed());

	// Aggiorna posizione
	setX(getX() + getDx());
	setY(getY() + getDy());
}

float Player::getShootCooldown() const { return _shootCooldown; }
float Player::getShootTimer() const { return _shootTimer; }
bool Player::getIsInvincible() const { return _invincible; }

void Player::setDirection(int direction, bool pressed)
{
	switch (direction) {
		case 0: _moveUp = pressed; break;
		case 1: _moveDown = pressed; break;
		case 2: _moveLeft = pressed; break;
		case 3: _moveRight = pressed; break;
	}
}
void Player::setShooting(bool shooting) { _shooting = shooting; }
void Player::resetInput()
{	
	_moveUp = _moveDown = _moveLeft = _moveRight = _shooting = false;
}