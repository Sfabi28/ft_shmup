#include "Game.hpp"
#include "Player.hpp"
#include "AEnemy.hpp"
#include "Projectile.hpp"
#include "Hitbox.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <ncurses.h>
#include <cstdlib>
#include <ctime>

bool Game::handle_resize()
{
    nodelay(stdscr, FALSE);
    while (LINES < MIN_LINES || COLS < MIN_COLS)
    {
        clear();
        mvprintw(LINES / 2, (COLS - 42) / 2, "Window too small (should be at least 120x40)");
        refresh();
        int answer = getch();
        if (answer == 27)
            return (true);
    }
    nodelay(stdscr, TRUE);
    clear();
    refresh();
    return (false);
}

void Game::spawnEnemy(float x, float y)
{
    _enemies.push_back(std::make_unique<AEnemy>(x, y, 'V', 1));
}

void Game::spawnProjectile(float x, float y, float dx, float dy, int color)
{
    _projectiles.push_back(std::make_unique<Projectile>(x, y, dx, dy, color));
}

void Game::checkCollisions() {
    if (!_player || !_player->getIsAlive()) return;

    Hitbox playerBox = { _player->getX(), _player->getY(), 
                         static_cast<float>(_player->getWidth()), 
                         static_cast<float>(_player->getHeight()) };

    // PROIETTILI vs TUTTO
    for (auto &proj : _projectiles) {
        if (!proj->getIsAlive()) continue;
        Hitbox pBox = { proj->getX(), proj->getY(), 
                        static_cast<float>(proj->getWidth()), 
                        static_cast<float>(proj->getHeight()) };

        // Proiettili vs Player
        if (proj->getTeam() != Team::Player && collides(pBox, playerBox)) {
            _player->takeDamage(1);
            proj->takeDamage(1);
            continue;
        }

        // Proiettili vs Nemici
        for (auto &enemy : _enemies) {
            if (!enemy->getIsAlive()) continue;
            if (proj->getTeam() == enemy->getTeam()) continue; // Stesso team, salta
            
            Hitbox eBox = { enemy->getX(), enemy->getY(), 
                           static_cast<float>(enemy->getWidth()), 
                           static_cast<float>(enemy->getHeight()) };
            
            if (collides(pBox, eBox)) {
                enemy->takeDamage(1);
                proj->takeDamage(1);
                if (!enemy->getIsAlive()) addScore(enemy->getScoreValue());
                break;
            }
        }

        // Proiettili vs Asteroidi (Neutral danneggia tutti)
        for (auto &asteroid : _asteroids) {
            if (!asteroid->getIsAlive()) continue;
            Hitbox aBox = { asteroid->getX(), asteroid->getY(),
                           static_cast<float>(asteroid->getWidth()),
                           static_cast<float>(asteroid->getHeight()) };
            
            if (collides(pBox, aBox)) {
                asteroid->takeDamage(1);
                proj->takeDamage(1);
                if (!asteroid->getIsAlive()) addScore(asteroid->getScoreValue());
                break;
            }
        }
    }

    // ENTITÀ vs PLAYER
    for (auto &enemy : _enemies) {
        if (!enemy->getIsAlive()) continue;
        Hitbox eBox = { enemy->getX(), enemy->getY(), 
                       static_cast<float>(enemy->getWidth()), 
                       static_cast<float>(enemy->getHeight()) };

        if (collides(playerBox, eBox)) {
            _player->takeDamage(1);
            enemy->takeDamage(1);
        }
    }
    
    for (auto &asteroid : _asteroids) {
        if (!asteroid->getIsAlive()) continue;
        Hitbox aBox = { asteroid->getX(), asteroid->getY(),
                       static_cast<float>(asteroid->getWidth()),
                       static_cast<float>(asteroid->getHeight()) };

        if (collides(playerBox, aBox)) {
            _player->takeDamage(1);
            asteroid->takeDamage(1);
        }
    }
    
    // NEMICI vs ASTEROIDI
    for (auto &enemy : _enemies) {
        if (!enemy->getIsAlive()) continue;
        Hitbox eBox = { enemy->getX(), enemy->getY(),
                       static_cast<float>(enemy->getWidth()),
                       static_cast<float>(enemy->getHeight()) };
        
        for (auto &asteroid : _asteroids) {
            if (!asteroid->getIsAlive()) continue;
            Hitbox aBox = { asteroid->getX(), asteroid->getY(),
                           static_cast<float>(asteroid->getWidth()),
                           static_cast<float>(asteroid->getHeight()) };
            
            if (collides(eBox, aBox)) {
                enemy->takeDamage(1);
                asteroid->takeDamage(1);
                break;
            }
        }
    }
}

void Game::updateEntities(float dt)
{
    if (_player)
        _player->update(dt, *this);

    for (auto &enemy : _enemies) {
        if (enemy)
            enemy->update(dt, *this);
    }

    for (auto &proj : _projectiles) {
        if (proj)
            proj->update(dt, *this);
    }

    for (auto &asteroid : _asteroids) {
        if (asteroid)
            asteroid->update(dt, *this);
    }

    for (auto it = _enemies.begin(); it != _enemies.end();) {
        if (*it && !(*it)->getIsAlive()) {
            it = _enemies.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = _projectiles.begin(); it != _projectiles.end();) {
        if (*it && !(*it)->getIsAlive()) {
            it = _projectiles.erase(it);
        } else {
            ++it;
        }
    }

	for (auto it = _asteroids.begin(); it != _asteroids.end();) {
        if (*it && !(*it)->getIsAlive()) {
            it = _asteroids.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::renderEntities(WINDOW *frame)
{
    if (_player)
        _player->render(frame);

    for (auto &enemy : _enemies) {
        if (enemy)
            enemy->render(frame);
	}

    for (auto &proj : _projectiles) {
		if (proj)
			proj->render(frame);
	}
	
	for (auto &asteroid : _asteroids)
	{
        if (asteroid)
            asteroid->render(frame);
	}
}

void Game::endless(int answer)
{
    static bool initialized = false;
	static auto last_asteroid_spawn = std::chrono::steady_clock::now();
    const float ASTEROID_SPAWN_INTERVAL = 5.0f;
	

    if (!initialized) {
        _player = std::make_unique<Player>(MIN_COLS / 2 - 2, 30);
        _player->setDx(0);
        _player->setDy(0);
        srand(time(nullptr));
        initialized = true;
    }

    if (_player) {
        _player->resetInput();
        
        if (answer == KEY_UP) _player->setDirection(0, true);
        else if (answer == KEY_DOWN) _player->setDirection(1, true);
        else if (answer == KEY_LEFT) _player->setDirection(2, true);
        else if (answer == KEY_RIGHT) _player->setDirection(3, true);
        else if (answer == ' ') _player->setShooting(true);
    }

	auto now = std::chrono::steady_clock::now();
    auto elapsed_asteroid = std::chrono::duration<float>(now - last_asteroid_spawn).count();
    if (elapsed_asteroid >= ASTEROID_SPAWN_INTERVAL) {
        spawnAsteroid(rand() % (MIN_COLS - 6) + 3, TOP_ROW + 1);
        last_asteroid_spawn = now;
    }
}

void Game::world1(int answer)
{
    (void)answer;
}

void Game::world2(int answer)
{
    (void)answer;
}

void Game::world3(int answer)
{
    (void)answer;
}

void Game::game_loop(int mode)
{
    resetScore();

    if (handle_resize())
        return;

    int STARTING_X = (COLS - MIN_COLS) / 2;
    int STARTING_Y = (LINES - MIN_LINES) / 2;
    WINDOW *frame = newwin(MIN_LINES, MIN_COLS, STARTING_Y, STARTING_X);

    nodelay(stdscr, TRUE);

    auto last_spawn = std::chrono::steady_clock::now();
    auto last_frame = std::chrono::steady_clock::now();
    const float SPAWN_INTERVAL = 3.5f;

    while (true)
    {
        auto frame_start = std::chrono::steady_clock::now();
        auto frame_delta = std::chrono::duration<float>(frame_start - last_frame).count();
        last_frame = frame_start;

        int answer = getch();
        if (answer == 27)
        {
            delwin(frame);
            return;
        }

        if (answer == KEY_RESIZE)
        {
            delwin(frame);
            if (handle_resize())
                return;
            STARTING_X = (COLS - MIN_COLS) / 2;
            STARTING_Y = (LINES - MIN_LINES) / 2;
            frame = newwin(MIN_LINES, MIN_COLS, STARTING_Y, STARTING_X);
        }

        if (mode == 1)
            world1(answer);
        else if (mode == 2)
            world2(answer);
        else if (mode == 3)
            world3(answer);
        else if (mode == 4) {
            endless(answer);
            
            auto now = std::chrono::steady_clock::now();
            auto elapsed_spawn = std::chrono::duration<float>(now - last_spawn).count();
            if (elapsed_spawn >= SPAWN_INTERVAL) {
                spawnEnemy(rand() % (MIN_COLS - 6) + 3, TOP_ROW + 1);
                last_spawn = now;
            }
            
            updateEntities(frame_delta);
            checkCollisions();            
            // Check if player is dead
            if (_player && !_player->getIsAlive()) {
                delwin(frame);
                return;
            }        }

        werase(frame);
        draw_frame(frame);
        if (mode == 4)
            renderEntities(frame);
        wnoutrefresh(frame);
        doupdate();

        auto frame_end = std::chrono::steady_clock::now();
        auto elapsed = frame_end - frame_start;
        if (elapsed < std::chrono::milliseconds(30))
            std::this_thread::sleep_for(std::chrono::milliseconds(30) - elapsed);
    }
}