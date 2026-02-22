#pragma once
#include "AGameEntity.hpp"

class Asteroid : public AGameEntity {
    private:
        int _scoreValue;
    
    public:
        Asteroid(float x, float y, int hp = 3);
        ~Asteroid();
        
        void update(float dt, class Game &game) override;
        int getScoreValue() const;
};