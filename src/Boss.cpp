#include "Boss.hpp"
#include "Game.hpp"

Boss::Boss(float x, float y, int maxHp)
    : AGameEntity(x, y, 'B', maxHp, 1.0f, 7, 7, Team::Enemy),
      _maxHp(maxHp),
      _direction(1)
{
    setDx(_direction * 15.0f);
    setDy(0.0f);
    setAsciiArt(
        "/-----\\",
        "|##-##|",
        "|#---#|",
        "|#-|-#|",
        "|#---#|",
        "|##-##|",
        "\\-----/"
    );
    _colorPair = 2;  // Red color
}

void Boss::update(float dt, Game & /* game */)
{
    float newX = getX() + getDx() * dt;
    
    if (newX <= 8.0f || newX >= 110.0f) {
        changeDirection();
        setDx(_direction * 15.0f);
        newX = getX() + getDx() * dt;
    }
    
    setX(newX);
}

void Boss::takeDamage(int dmg)
{
    _hp -= dmg;
    if (_hp <= 0)
        _isAlive = false;
}
