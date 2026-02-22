#pragma once

#include <ncurses.h>
#include "Hitbox.hpp"
#include <string>

class Game;

enum class Team {
    Player,
    Enemy,
    Neutral
};

class AGameEntity {
	protected:
		float _x, _y;
		float _dx, _dy;
		float _speed;

		int _hp;
		bool _isAlive;
		Team _team;

		char _symbol;
		int _colorPair;
		int _width;
		int _height;
		std::string _asciiArt[7];

	public:
		AGameEntity(float x, float y, char sym, int hp = 1, float speed = 1.f,
                int width = 1, int height = 1, Team team = Team::Neutral);
		virtual ~AGameEntity();

		virtual void takeDamage(int dmg);
		virtual void update(float dt, Game &game) = 0;
		virtual void render(WINDOW *win	) const;

		float getX() const;
		float getY() const;
		float getDx() const;
		float getDy() const;
		float getSpeed() const;
		int getHP() const;
		bool getIsAlive() const;
		Team getTeam() const;
		char getSymbol() const;
		int getColorPair() const;
		int getWidth() const;
		int getHeight() const;
		
		void setX(float x);
		void setY(float y);
		void setDx(float dx);
		void setDy(float dy);
		void setAsciiArt(const std::string &line0, const std::string &line1, const std::string &line2, const std::string &line3 = "", const std::string &line4 = "", const std::string &line5 = "", const std::string &line6 = "");
};
