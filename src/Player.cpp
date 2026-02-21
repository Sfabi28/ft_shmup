#include "Player.hpp"
#include "Game.hpp"

Player::Player(float x, float y) : AGameEntity(x, y, '@'), _shootCooldown(0.5f), _shootTimer(0), _invincible(false), _moveUp(false), _moveDown(false), _moveLeft(false), _moveRight(false), _shooting(false) {
	_width = 3;
	_height = 3;
	_colorPair = 3;
	_speed = 25.0f;
	setAsciiArt(" A ", "/_\\", "m m");
}
Player::~Player() {}

void Player::update(float dt, Game &game)
{
	(void)game;
	_shootTimer += dt;
	if (_shooting && _shootTimer >= _shootCooldown)
	{
		_shootTimer = 0;
	}

	float moveX = 0, moveY = 0;
	if (_moveUp) moveY -= 1;
	if (_moveDown) moveY += 1;
	if (_moveLeft) moveX -= 1;
	if (_moveRight) moveX += 1;

	setDx(moveX * getSpeed());
	setDy(moveY * getSpeed());

	float newX = getX() + getDx() * dt;
	float newY = getY() + getDy() * dt;
	
	const int MIN_X = 2;
	const int MAX_X = game.getWidth() - 5;
	const int MIN_Y = 7;
	const int MAX_Y = game.getHeight() - 4;
	
	if (newX >= MIN_X && newX <= MAX_X)
		setX(newX);
	if (newY >= MIN_Y && newY <= MAX_Y)
		setY(newY);
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

void Player::render(WINDOW *win) const {
	AGameEntity::render(win);
}

void Player::die(Game &game) {
	(void)game;
	_isAlive = false;
}

int Player::getLives() const {
	return _hp;
}