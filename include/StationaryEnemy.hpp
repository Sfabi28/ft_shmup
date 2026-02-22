#pragma once

#include "AEnemy.hpp"

class StationaryEnemy : public AEnemy {
	public:
		StationaryEnemy(float x, float y, char sym = '#', int hp = 1);
		~StationaryEnemy() override;

		void update(float dt, class Game &game) override;
};
