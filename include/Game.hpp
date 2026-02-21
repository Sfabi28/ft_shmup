#pragma once

# include <ncurses.h>
# include <string>

class Game
{
	public:
		static constexpr int MIN_LINES = 40;
		static constexpr int MIN_COLS = 120;
		static constexpr int TOP_ROW = 5;

		Game() noexcept;
		void game_loop(int mode);
		void draw_frame(WINDOW *frame);
		void ascii_art(WINDOW *win, int y, int x, const std::string &text, int area_width, bool center);

		int getWidth() const;
		int getHeight() const;
		int getScore() const;
		void addScore(int points);
		void resetScore();

	private:
		int _score;
		bool handle_resize();
};
