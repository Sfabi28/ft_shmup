#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "AGameEntity.hpp"

class Player : public AGameEntity {
	private:
		float _shootCooldown;
		float _shootTimer;

		bool  _invincible;

		bool _moveUp;
		bool _moveDown;
		bool _moveLeft;
		bool _moveRight;
		bool _shooting;

	public:
		Player(float x, float y);
		~Player();

		void update(float dt, Game &game) override;

		float getShootCooldown() const;
		float getShootTimer() const;
		bool getIsInvincible() const;

		void setDirection(int direction, bool pressed);

		void setShooting(bool shooting);
		void resetInput();
};

#endif