#include "Player.hpp"
#include "Game.hpp"

Player::Player(float x, float y) : AGameEntity(x, y, '@', 3, 25.0f, 3, 3, Team::Player), _shootCooldown(0.5f), _shootTimer(0), _invincible(false), _moveUp(false), _moveDown(false), _moveLeft(false), _moveRight(false), _shooting(false) {
	_width = 3;
	_height = 3;
	_colorPair = 3;
	_speed = 25.0f;
	_invincibilityTimer = 0.0f;
	setAsciiArt(" A ", "/_\\", "m m");
}
Player::~Player() {}

void Player::update(float dt, Game &game)
{
	if (_invincibilityTimer > 0) _invincibilityTimer -= dt;
	_shootTimer += dt;
	if (_shooting && _shootTimer >= _shootCooldown)
	{
		float centerX = _x + 1;
		float centerY = _y +1 ;
		game.spawnProjectile(centerX, centerY - 2, 0, -30, 3, Team::Player);
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
	const int MAX_X = game.getWidth() - _width - 1;
	const int MIN_Y = game.TOP_ROW + 1;
	const int MAX_Y = game.getHeight() - _height - 1;

	if (newX < MIN_X) newX = MIN_X;
	if (newX > MAX_X) newX = MAX_X;
	if (newY < MIN_Y) newY = MIN_Y;
	if (newY > MAX_Y) newY = MAX_Y;

	setX(newX);
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

void Player::takeDamage(int dmg) {
    if (_invincibilityTimer <= 0) {
        _hp -= dmg;
        _invincibilityTimer = 1.5f;
        if (_hp <= 0) _isAlive = false;
    }
}

void Player::render(WINDOW *win) const {
	if (_invincibilityTimer > 0) {
        int blink = static_cast<int>(_invincibilityTimer * 6.5f) % 2;
        if (blink == 0) return;
    }
	AGameEntity::render(win);
}

void Player::die(Game &game) {
	(void)game;
	_isAlive = false;
}

int Player::getLives() const {
	return _hp;
}