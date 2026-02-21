#ifndef AGAMEENTITY_HPP
#define AGAMEENTITY_HPP

class Game;

class AGameEntity {
	protected:
		float _x, _y;
		float _dx, _dy;
		int _speed;

		int _hp;
		bool _isAlive;

		char _symbol;
		int _colorPair;
		int _width;
		int _height;

	public:
		AGameEntity(float x, float y, char sym, int hp = 1, int speed = 1, int width = 1, int height = 1);
		virtual ~AGameEntity();

		void takeDamage(int dmg);
		virtual void update(float dt, Game &game) = 0;
		virtual void render() const;

		float getX() const;
		float getY() const;
		float getDx() const;
		float getDy() const;
		float getSpeed() const;
		int getHP() const;
		bool getIsAlive() const;
		char getSymbol() const;
		int getColorPair() const;
		int getWidth() const;
		int getHeight() const;
		
		void setX(float x);
		void setY(float y);
		void setDx(float dx);
		void setDy(float dy);
};

#endif