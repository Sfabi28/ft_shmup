#include "Game.hpp"
#include "Player.hpp"
#include "AEnemy.hpp"
#include "StationaryEnemy.hpp"
#include "RammerEnemy.hpp"
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
    int type = rand() % 4;
    if (type == 0)
        _enemies.push_back(std::make_unique<StationaryEnemy>(x, y));
    else if (type == 1)
        _enemies.push_back(std::make_unique<RammerEnemy>(x, y));
    else
        _enemies.push_back(std::make_unique<AEnemy>(x, y, 'V', 1));
}

void Game::spawnProjectile(float x, float y, float dx, float dy, int color, Team team)
{
    _projectiles.push_back(std::make_unique<Projectile>(x, y, dx, dy, color, team));
}

void Game::checkCollisions() {
    if (!_player || !_player->getIsAlive()) return;

    Hitbox playerBox = { _player->getX(), _player->getY(), static_cast<float>(_player->getWidth()), static_cast<float>(_player->getHeight()) };

    for (auto &proj : _projectiles) {
        if (!proj->getIsAlive()) continue;
        Hitbox pBox = { proj->getX(), proj->getY(), static_cast<float>(proj->getWidth()), static_cast<float>(proj->getHeight()) };

        if (proj->getTeam() == Team::Enemy) {
            if (collides(pBox, playerBox)) {
                _player->takeDamage(1);
                proj->takeDamage(1);
            }
        }

        else if (proj->getTeam() == Team::Player) {
            for (auto &enemy : _enemies) {
                if (!enemy->getIsAlive()) continue;
                Hitbox eBox = { enemy->getX(), enemy->getY(), static_cast<float>(enemy->getWidth()), static_cast<float>(enemy->getHeight()) };
                
                if (collides(pBox, eBox)) {
                    enemy->takeDamage(1);
                    proj->takeDamage(1);
                    if (!enemy->getIsAlive()) addScore(enemy->getScoreValue());
                    break;
                }
            }
        }
    }

    for (auto &enemy : _enemies) {
        if (!enemy->getIsAlive()) continue;
        Hitbox eBox = { enemy->getX(), enemy->getY(), static_cast<float>(enemy->getWidth()), static_cast<float>(enemy->getHeight()) };

        if (collides(playerBox, eBox)) {
            _player->takeDamage(1);
            enemy->takeDamage(1);
        }
    }
}

void Game::updateEntities(float dt)
{
    if (_player)
        _player->update(dt, *this);

    for (auto &enemy : _enemies) {
        if (!enemy)
            continue;
        enemy->update(dt, *this);

        if (_player && enemy->getHP() > 0) {
            float enemyBottom = enemy->getY() + enemy->getHeight() - 1;
            if (enemyBottom >= getHeight() - 1) {
                _player->takeDamage(1);
                enemy->takeDamage(enemy->getHP());
            }
        }
    }

    for (auto &proj : _projectiles)
        if (proj)
            proj->update(dt, *this);

    for (auto it = _enemies.begin(); it != _enemies.end();) {
        if (*it && (!(*it)->getIsAlive() || (*it)->getHP() <= 0)) {
            it = _enemies.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = _projectiles.begin(); it != _projectiles.end();) {
        if (*it && (!(*it)->getIsAlive() || (*it)->getHP() <= 0)) {
            it = _projectiles.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::renderEntities(WINDOW *frame)
{
    if (_player)
        _player->render(frame);

    for (auto &enemy : _enemies)
        if (enemy)
            enemy->render(frame);

    for (auto &proj : _projectiles)
        if (proj)
            proj->render(frame);
}

bool Game::endless(int answer, float frameDelta)
{
    static bool initialized = false;
    static auto last_spawn = std::chrono::steady_clock::now();
    static auto endless_start = std::chrono::steady_clock::now();
    static constexpr float SPAWN_INTERVAL = 3.5f;

    if (!initialized) {
        _player = std::make_unique<Player>(MIN_COLS / 2 - 2, 30);
        _player->setDx(0);
        _player->setDy(0);
        srand(time(nullptr));
        last_spawn = std::chrono::steady_clock::now();
        endless_start = std::chrono::steady_clock::now();
        _endlessElapsedSeconds = 0;
        _world1CurrentWave = -1;
        _world1TotalWaves = 0;
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

    _endlessElapsedSeconds = static_cast<int>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - endless_start
        ).count()
    );

    auto now = std::chrono::steady_clock::now();
    auto elapsed_spawn = std::chrono::duration<float>(now - last_spawn).count();
    if (elapsed_spawn >= SPAWN_INTERVAL) {
        spawnEnemy(rand() % (MIN_COLS - 6) + 3, TOP_ROW + 1);
        last_spawn = now;
    }

    updateEntities(frameDelta);
    checkCollisions();

    if (_player && !_player->getIsAlive())
        return true;
    return false;
}

bool Game::world1(int answer, float frameDelta)
{
    static bool initialized = false;
    struct WaveConfig {
        int basicCount;
        int stationaryCount;
        int rammerCount;
        float spawnInterval;
    };

    static constexpr WaveConfig WAVES[] = {
        {0, 3, 0, 0.95f},
        {2, 2, 0, 0.85f},
        {2, 2, 1, 0.75f}
    };
    static constexpr int WAVE_COUNT = sizeof(WAVES) / sizeof(WAVES[0]);
    static constexpr int BASIC_COLUMNS[] = {8, 22, 36, 50, 64, 78, 92, 106};
    static constexpr int BASIC_COL_COUNT = sizeof(BASIC_COLUMNS) / sizeof(BASIC_COLUMNS[0]);
    static constexpr int STATIONARY_COLUMNS[] = {14, 40, 66, 92};
    static constexpr int STATIONARY_COL_COUNT = sizeof(STATIONARY_COLUMNS) / sizeof(STATIONARY_COLUMNS[0]);
    static constexpr int RAMMER_COLUMNS[] = {10, 30, 58, 86, 104};
    static constexpr int RAMMER_COL_COUNT = sizeof(RAMMER_COLUMNS) / sizeof(RAMMER_COLUMNS[0]);

    static int currentWave = 0;
    static int spawnedInWave = 0;
    static auto last_spawn = std::chrono::steady_clock::now();

    if (!initialized) {
        _player = std::make_unique<Player>(MIN_COLS / 2 - 2, 30);
        _player->setDx(0);
        _player->setDy(0);
        _enemies.clear();
        _projectiles.clear();
        currentWave = 0;
        spawnedInWave = 0;
        last_spawn = std::chrono::steady_clock::now();
        _endlessElapsedSeconds = -1;
        _world1TotalWaves = WAVE_COUNT;
        _world1CurrentWave = 1;
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
    if (currentWave < WAVE_COUNT) {
        _world1CurrentWave = currentWave + 1;
        const WaveConfig &wave = WAVES[currentWave];
        int waveSize = wave.basicCount + wave.stationaryCount + wave.rammerCount;
        auto elapsed_spawn = std::chrono::duration<float>(now - last_spawn).count();

        if (spawnedInWave < waveSize && elapsed_spawn >= wave.spawnInterval) {
            if (spawnedInWave < wave.basicCount) {
                int laneIndex = (spawnedInWave + currentWave * 2) % BASIC_COL_COUNT;
                float spawnX = static_cast<float>(BASIC_COLUMNS[laneIndex]);
                _enemies.push_back(std::make_unique<AEnemy>(spawnX, TOP_ROW + 1, 'V', 1));
            }
            else if (spawnedInWave < wave.basicCount + wave.stationaryCount) {
                int stationaryIndex = (spawnedInWave - wave.basicCount + currentWave) % STATIONARY_COL_COUNT;
                float spawnX = static_cast<float>(STATIONARY_COLUMNS[stationaryIndex]);
                _enemies.push_back(std::make_unique<StationaryEnemy>(spawnX, TOP_ROW + 1));
            }
            else {
                int rammerOffset = wave.basicCount + wave.stationaryCount;
                int rammerIndex = (spawnedInWave - rammerOffset + currentWave) % RAMMER_COL_COUNT;
                float spawnX = static_cast<float>(RAMMER_COLUMNS[rammerIndex]);
                _enemies.push_back(std::make_unique<RammerEnemy>(spawnX, TOP_ROW + 1));
            }

            spawnedInWave++;
            last_spawn = now;
        }

        if (spawnedInWave >= waveSize && _enemies.empty() && _projectiles.empty()) {
            currentWave++;
            spawnedInWave = 0;
            last_spawn = now;
            if (currentWave < WAVE_COUNT)
                _world1CurrentWave = currentWave + 1;
            else
                _world1CurrentWave = WAVE_COUNT;
        }
    }

    updateEntities(frameDelta);
    checkCollisions();

    if (_player && !_player->getIsAlive())
        return true;

    if (currentWave >= WAVE_COUNT && _enemies.empty() && _projectiles.empty())
        return true;

    return false;
}

bool Game::world2(int answer, float frameDelta)
{
    static bool initialized = false;
    struct WaveConfig {
        int basicCount;
        int stationaryCount;
        int rammerCount;
        float spawnInterval;
    };

    static constexpr WaveConfig WAVES[] = {
        {6, 0, 0, 0.95f},
        {4, 2, 1, 0.85f},
        {3, 3, 2, 0.75f}
    };
    static constexpr int WAVE_COUNT = sizeof(WAVES) / sizeof(WAVES[0]);
    static constexpr int BASIC_COLUMNS[] = {8, 22, 36, 50, 64, 78, 92, 106};
    static constexpr int BASIC_COL_COUNT = sizeof(BASIC_COLUMNS) / sizeof(BASIC_COLUMNS[0]);
    static constexpr int STATIONARY_COLUMNS[] = {14, 40, 66, 92};
    static constexpr int STATIONARY_COL_COUNT = sizeof(STATIONARY_COLUMNS) / sizeof(STATIONARY_COLUMNS[0]);
    static constexpr int RAMMER_COLUMNS[] = {10, 30, 58, 86, 104};
    static constexpr int RAMMER_COL_COUNT = sizeof(RAMMER_COLUMNS) / sizeof(RAMMER_COLUMNS[0]);

    static int currentWave = 0;
    static int spawnedInWave = 0;
    static auto last_spawn = std::chrono::steady_clock::now();

    if (!initialized) {
        _player = std::make_unique<Player>(MIN_COLS / 2 - 2, 30);
        _player->setDx(0);
        _player->setDy(0);
        _enemies.clear();
        _projectiles.clear();
        currentWave = 0;
        spawnedInWave = 0;
        last_spawn = std::chrono::steady_clock::now();
        _endlessElapsedSeconds = -1;
        _world1TotalWaves = WAVE_COUNT;
        _world1CurrentWave = 1;
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
    if (currentWave < WAVE_COUNT) {
        _world1CurrentWave = currentWave + 1;
        const WaveConfig &wave = WAVES[currentWave];
        int waveSize = wave.basicCount + wave.stationaryCount + wave.rammerCount;
        auto elapsed_spawn = std::chrono::duration<float>(now - last_spawn).count();

        if (spawnedInWave < waveSize && elapsed_spawn >= wave.spawnInterval) {
            if (spawnedInWave < wave.basicCount) {
                int laneIndex = (spawnedInWave + currentWave * 2) % BASIC_COL_COUNT;
                float spawnX = static_cast<float>(BASIC_COLUMNS[laneIndex]);
                _enemies.push_back(std::make_unique<AEnemy>(spawnX, TOP_ROW + 1, 'V', 1));
            }
            else if (spawnedInWave < wave.basicCount + wave.stationaryCount) {
                int stationaryIndex = (spawnedInWave - wave.basicCount + currentWave) % STATIONARY_COL_COUNT;
                float spawnX = static_cast<float>(STATIONARY_COLUMNS[stationaryIndex]);
                _enemies.push_back(std::make_unique<StationaryEnemy>(spawnX, TOP_ROW + 1));
            }
            else {
                int rammerOffset = wave.basicCount + wave.stationaryCount;
                int rammerIndex = (spawnedInWave - rammerOffset + currentWave) % RAMMER_COL_COUNT;
                float spawnX = static_cast<float>(RAMMER_COLUMNS[rammerIndex]);
                _enemies.push_back(std::make_unique<RammerEnemy>(spawnX, TOP_ROW + 1));
            }

            spawnedInWave++;
            last_spawn = now;
        }

        if (spawnedInWave >= waveSize && _enemies.empty() && _projectiles.empty()) {
            currentWave++;
            spawnedInWave = 0;
            last_spawn = now;
            if (currentWave < WAVE_COUNT)
                _world1CurrentWave = currentWave + 1;
            else
                _world1CurrentWave = WAVE_COUNT;
        }
    }

    updateEntities(frameDelta);
    checkCollisions();

    if (_player && !_player->getIsAlive())
        return true;

    if (currentWave >= WAVE_COUNT && _enemies.empty() && _projectiles.empty())
        return true;

    return false;
}

bool Game::world3(int answer, float frameDelta)
{
    static bool initialized = false;
    struct WaveConfig {
        int basicCount;
        int stationaryCount;
        int rammerCount;
        float spawnInterval;
    };

    static constexpr WaveConfig WAVES[] = {
        {3, 3, 1, 0.95f},
        {3, 4, 2, 0.85f},
        {4, 4, 3, 0.75f}
    };
    static constexpr int WAVE_COUNT = sizeof(WAVES) / sizeof(WAVES[0]);
    static constexpr int BASIC_COLUMNS[] = {8, 22, 36, 50, 64, 78, 92, 106};
    static constexpr int BASIC_COL_COUNT = sizeof(BASIC_COLUMNS) / sizeof(BASIC_COLUMNS[0]);
    static constexpr int STATIONARY_COLUMNS[] = {14, 40, 66, 92};
    static constexpr int STATIONARY_COL_COUNT = sizeof(STATIONARY_COLUMNS) / sizeof(STATIONARY_COLUMNS[0]);
    static constexpr int RAMMER_COLUMNS[] = {10, 30, 58, 86, 104};
    static constexpr int RAMMER_COL_COUNT = sizeof(RAMMER_COLUMNS) / sizeof(RAMMER_COLUMNS[0]);

    static int currentWave = 0;
    static int spawnedInWave = 0;
    static auto last_spawn = std::chrono::steady_clock::now();

    if (!initialized) {
        _player = std::make_unique<Player>(MIN_COLS / 2 - 2, 30);
        _player->setDx(0);
        _player->setDy(0);
        _enemies.clear();
        _projectiles.clear();
        currentWave = 0;
        spawnedInWave = 0;
        last_spawn = std::chrono::steady_clock::now();
        _endlessElapsedSeconds = -1;
        _world1TotalWaves = WAVE_COUNT;
        _world1CurrentWave = 1;
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
    if (currentWave < WAVE_COUNT) {
        _world1CurrentWave = currentWave + 1;
        const WaveConfig &wave = WAVES[currentWave];
        int waveSize = wave.basicCount + wave.stationaryCount + wave.rammerCount;
        auto elapsed_spawn = std::chrono::duration<float>(now - last_spawn).count();

        if (spawnedInWave < waveSize && elapsed_spawn >= wave.spawnInterval) {
            if (spawnedInWave < wave.basicCount) {
                int laneIndex = (spawnedInWave + currentWave * 2) % BASIC_COL_COUNT;
                float spawnX = static_cast<float>(BASIC_COLUMNS[laneIndex]);
                _enemies.push_back(std::make_unique<AEnemy>(spawnX, TOP_ROW + 1, 'V', 1));
            }
            else if (spawnedInWave < wave.basicCount + wave.stationaryCount) {
                int stationaryIndex = (spawnedInWave - wave.basicCount + currentWave) % STATIONARY_COL_COUNT;
                float spawnX = static_cast<float>(STATIONARY_COLUMNS[stationaryIndex]);
                _enemies.push_back(std::make_unique<StationaryEnemy>(spawnX, TOP_ROW + 1));
            }
            else {
                int rammerOffset = wave.basicCount + wave.stationaryCount;
                int rammerIndex = (spawnedInWave - rammerOffset + currentWave) % RAMMER_COL_COUNT;
                float spawnX = static_cast<float>(RAMMER_COLUMNS[rammerIndex]);
                _enemies.push_back(std::make_unique<RammerEnemy>(spawnX, TOP_ROW + 1));
            }

            spawnedInWave++;
            last_spawn = now;
        }

        if (spawnedInWave >= waveSize && _enemies.empty() && _projectiles.empty()) {
            currentWave++;
            spawnedInWave = 0;
            last_spawn = now;
            if (currentWave < WAVE_COUNT)
                _world1CurrentWave = currentWave + 1;
            else
                _world1CurrentWave = WAVE_COUNT;
        }
    }

    updateEntities(frameDelta);
    checkCollisions();

    if (_player && !_player->getIsAlive())
        return true;

    if (currentWave >= WAVE_COUNT && _enemies.empty() && _projectiles.empty())
        return true;

    return false;
}

void Game::game_loop(int mode)
{
    resetScore();
    _endlessElapsedSeconds = -1;
    _world1CurrentWave = -1;
    _world1TotalWaves = 0;

    if (handle_resize())
        return;

    int STARTING_X = (COLS - MIN_COLS) / 2;
    int STARTING_Y = (LINES - MIN_LINES) / 2;
    WINDOW *frame = newwin(MIN_LINES, MIN_COLS, STARTING_Y, STARTING_X);

    nodelay(stdscr, TRUE);

    auto last_frame = std::chrono::steady_clock::now();

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

        bool gameEnded = false;
        bool playerWon = false;

        if (mode == 1) {
            gameEnded = world1(answer, frame_delta);
            if (gameEnded)
                playerWon = (_player && _player->getIsAlive());
        }
        else if (mode == 2) {
            gameEnded = world2(answer, frame_delta);
            if (gameEnded)
                playerWon = (_player && _player->getIsAlive());
        }
        else if (mode == 3) {
            gameEnded = world3(answer, frame_delta);
            if (gameEnded)
                playerWon = (_player && _player->getIsAlive());
        }
        else if (mode == 4) {
            gameEnded = endless(answer, frame_delta);
            playerWon = false;
        }

        if (gameEnded) {
            werase(frame);
            wrefresh(frame);

            erase();
            std::string endText = playerWon ? "you won" : "you lost";
            ascii_art(stdscr, (LINES / 2) - 2, 0, endText, COLS, true);
            refresh();

            std::this_thread::sleep_for(std::chrono::seconds(3));
            delwin(frame);
            return;
        }

        werase(frame);
        draw_frame(frame);
        if (mode == 1 || mode == 2 || mode == 3 || mode == 4)
            renderEntities(frame);
        wnoutrefresh(frame);
        doupdate();

        auto frame_end = std::chrono::steady_clock::now();
        auto elapsed = frame_end - frame_start;
        if (elapsed < std::chrono::milliseconds(30))
            std::this_thread::sleep_for(std::chrono::milliseconds(30) - elapsed);
    }
}