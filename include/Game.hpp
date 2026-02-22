#pragma once

# include <ncurses.h>
# include <string>
# include <vector>
# include <memory>
# include <algorithm>
# include "Player.hpp"
# include "AEnemy.hpp"
# include "Projectile.hpp"

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
		void spawnProjectile(float x, float y, float dx, float dy, int color = 1, Team team = Team::Neutral);

	private:
		int _score;
		int _endlessElapsedSeconds;
		int _world1CurrentWave;
		int _world1TotalWaves;
		std::unique_ptr<Player> _player;
		std::vector<std::unique_ptr<AEnemy>> _enemies;
		std::vector<std::unique_ptr<Projectile>> _projectiles;
		
		bool handle_resize();
		bool endless(int answer, float frameDelta);
		bool world1(int answer, float frameDelta);
		bool world2(int answer, float frameDelta);
		bool world3(int answer, float frameDelta);
		void updateEntities(float dt);
		void renderEntities(WINDOW *frame);
		void spawnEnemy(float x, float y);
		void checkCollisions();
};
