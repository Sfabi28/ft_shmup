#ifndef AENEMY_HPP
#define AENEMY_HPP

#include "AGameEntity.hpp"

class AEnemy : public AGameEntity {
	public:
		AEnemy(float x, float y, char sym, int hp = 1);
		virtual ~AEnemy();

		virtual void update(float dt, class Game &game);
};

#endif