#include "Asteroid.hpp"
#include "Game.hpp"

Asteroid::Asteroid(float x, float y, int hp) 
    : AGameEntity(x, y, '*', hp, 1.0f, 2, 2, Team::Neutral) {
    _dx = 0;
    _dy = _speed;
    _colorPair = 1;
    _scoreValue = 0;
    setAsciiArt("**", "**", "");
}

Asteroid::~Asteroid() {}

void Asteroid::update(float dt, Game &game) {
    _y += _dy * dt;
    
    if (_y >= game.getHeight()) {
        _hp = 0;
    }
}

int Asteroid::getScoreValue() const {
    return _scoreValue;
}