#include "AGameEntity.hpp"
#include <ncurses.h>

AGameEntity::AGameEntity(float x, float y, char sym, int hp, float speed, int width, int height) : _x(x), _y(y), _hp(hp), _symbol(sym), _isAlive(true), _speed(speed), _width(width), _height(height) {}
AGameEntity::~AGameEntity() {}

void AGameEntity::takeDamage(int dmg)
{
	_hp -= dmg;
	if (_hp <= 0)
		_isAlive = false;
}

void AGameEntity::render(WINDOW *win) const {
    wattron(win, COLOR_PAIR(_colorPair));
    for (int i = 0; i < _height; i++)
        for (int j = 0; j < _width; j++)
            mvwaddch(win, static_cast<int>(_y) + i, static_cast<int>(_x) + j, _symbol);
    wattroff(win, COLOR_PAIR(_colorPair));
}

float AGameEntity::getX() const { return _x; }
float AGameEntity::getY() const { return _y; }
float AGameEntity::getDx() const { return _dx; }
float AGameEntity::getDy() const { return _dy; }
float AGameEntity::getSpeed() const { return _speed; }
int AGameEntity::getHP() const { return _hp; }
bool AGameEntity::getIsAlive() const { return _isAlive; }
char AGameEntity::getSymbol() const { return _symbol; }
int AGameEntity::getColorPair() const { return _colorPair; }
int AGameEntity::getWidth() const { return _width; }
int AGameEntity::getHeight() const { return _height; }

void AGameEntity::setX(float x) { _x = x; }
void AGameEntity::setY(float y) { _y = y; }
void AGameEntity::setDx(float dx) { _dx = dx; }
void AGameEntity::setDy(float dy) { _dy = dy; }
