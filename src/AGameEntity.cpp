#include "AGameEntity.hpp"
#include <ncurses.h>

AGameEntity::AGameEntity(float x, float y, char sym, int hp, float speed, int width, int height, Team team)
	: _x(x), _y(y), _dx(0), _dy(0), _speed(speed), _hp(hp), _isAlive(true), _team(team), 
	  _symbol(sym), _colorPair(1), _width(width), _height(height) {}

AGameEntity::~AGameEntity() {}

void AGameEntity::takeDamage(int dmg)
{
	_hp -= dmg;
	if (_hp <= 0)
		_isAlive = false;
}

void AGameEntity::render(WINDOW *win) const {
    wattron(win, COLOR_PAIR(_colorPair));
    for (int i = 0; i < 3; i++) {
        if (i < static_cast<int>(_asciiArt[i].length())) {
            mvwprintw(win, static_cast<int>(_y) + i, static_cast<int>(_x), "%s", _asciiArt[i].c_str());
        }
    }
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

void AGameEntity::setAsciiArt(const std::string &line0, const std::string &line1, const std::string &line2) {
	_asciiArt[0] = line0;
	_asciiArt[1] = line1;
	_asciiArt[2] = line2;
}
