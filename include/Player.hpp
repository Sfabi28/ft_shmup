#pragma once

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
		void render(WINDOW *win) const override;

		void die(Game &game);

		float getShootCooldown() const;
		float getShootTimer() const;
		int   getLives() const;
		bool getIsInvincible() const;

		void setDirection(int direction, bool pressed);

		void setShooting(bool shooting);
		void resetInput();
};

