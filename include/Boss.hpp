#ifndef BOSS_HPP
#define BOSS_HPP

#include "AGameEntity.hpp"

class Boss : public AGameEntity {
private:
    int _maxHp;
    int _direction;  // 1 = right, -1 = left

public:
    Boss(float x, float y, int maxHp = 10);
    
    void update(float dt, Game &game) override;
    void takeDamage(int dmg) override;
    
    int getMaxHP() const { return _maxHp; }
    float getShootX() const { return getX() + 3.0f; }  // Centro orizzontale (7 width)
    float getShootY() const { return getY() + 7.0f; }  // Fondo del boss (7 height)
    
    void changeDirection() { _direction *= -1; }
};

#endif // BOSS_HPP
