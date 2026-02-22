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
# include "Boss.hpp"

class Game
{
	private:
		struct Star {
			float x;
			float y;
			char symbol;
		};
	public:
		static constexpr int MIN_LINES = 40;
		static constexpr int MIN_COLS = 120;
		static constexpr int TOP_ROW = 5;

		Game() noexcept;
		~Game();
		void game_loop(int mode, int players);
		void draw_frame(WINDOW *frame);
		void ascii_art(WINDOW *win, int y, int x, const std::string &text, int area_width, bool center);

		int getWidth() const;
		int getHeight() const;
		int getScore() const;
		void addScore(int points);
		void resetScore();
		void spawnProjectile(float x, float y, float dx, float dy, int color = 1, Team team = Team::Neutral);

	private:
		int _score;
		int _endlessElapsedSeconds;
		int _world1CurrentWave;
		int _world1TotalWaves;
		int _playerCount;
		std::unique_ptr<Player> _player;
		std::unique_ptr<Player> _player2;
		std::unique_ptr<Boss> _boss;
		std::vector<std::unique_ptr<AEnemy>> _enemies;
		std::vector<std::unique_ptr<Projectile>> _projectiles;
		std::vector<std::unique_ptr<Asteroid>> _asteroids;
		std::vector<Star> _backgroundStars;
		
		bool handle_resize();
		bool endless(const std::vector<int>& inputs, float frameDelta);
		bool world1(const std::vector<int>& inputs, float frameDelta);
		bool world2(const std::vector<int>& inputs, float frameDelta);
		bool world3(const std::vector<int>& inputs, float frameDelta);
		void updateEntities(float dt);
		void renderEntities(WINDOW *frame);
		void spawnEnemy(float x, float y);
		void checkCollisions();
		void spawnAsteroid(float x, float y);
		void initializeBackground();
		void drawBackground(WINDOW *frame);
		void updateBackground(float dt);
};
