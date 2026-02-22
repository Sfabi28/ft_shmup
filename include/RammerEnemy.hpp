#pragma once

#include "AEnemy.hpp"

class RammerEnemy : public AEnemy {
	public:
		RammerEnemy(float x, float y, char sym = 'M', int hp = 2);
		~RammerEnemy() override;

		void update(float dt, class Game &game) override;
};
