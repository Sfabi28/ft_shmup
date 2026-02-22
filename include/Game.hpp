#pragma once

# include <ncurses.h>
# include <string>
# include <vector>
# include <memory>
# include <algorithm>
# include "Player.hpp"
# include "AEnemy.hpp"
# include "Projectile.hpp"
# include "Asteroid.hpp"

class Game
{
	public:
		static constexpr int MIN_LINES = 40;
		static constexpr int MIN_COLS = 120;
		static constexpr int TOP_ROW = 5;

		Game() noexcept;
		~Game();
		void game_loop(int mode);
		void draw_frame(WINDOW *frame);
		void ascii_art(WINDOW *win, int y, int x, const std::string &text, int area_width, bool center);

		int getWidth() const;
		int getHeight() const;
		int getScore() const;
		void addScore(int points);
		void resetScore();
		void spawnProjectile(float x, float y, float dx, float dy, int color = 1);

	private:
		int _score;
		std::unique_ptr<Player> _player;
		std::vector<std::unique_ptr<AEnemy>> _enemies;
		std::vector<std::unique_ptr<Projectile>> _projectiles;
		std::vector<std::unique_ptr<Asteroid>> _asteroids;
		
		bool handle_resize();
		void endless(int answer);
		void world1(int answer);
		void world2(int answer);
		void world3(int answer);
		void updateEntities(float dt);
		void renderEntities(WINDOW *frame);
		void spawnEnemy(float x, float y);
		void checkCollisions();
		void spawnAsteroid(float x, float y);
};
