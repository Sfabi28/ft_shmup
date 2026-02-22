#pragma once

#include "AGameEntity.hpp"

class AEnemy : public AGameEntity {
	protected:
		int   _scoreValue;
		float _shootTimer;
		float _shootInterval;
	public:
		AEnemy(float x, float y, char sym, int hp = 1);
		virtual ~AEnemy();

		virtual void update(float dt, class Game &game);

		int getScoreValue() const;
};
