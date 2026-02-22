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

static constexpr float SPAWN_INTERVAL = 0.4f;

Game::Game() noexcept
        : _score(0),
            _endlessElapsedSeconds(-1),
            _world1CurrentWave(-1),
            _world1TotalWaves(0),
            _playerCount(1)
{
	initializeBackground();
}

Game::~Game() {}

int Game::getWidth() const {
    return MIN_COLS;
}

int Game::getHeight() const {
    return MIN_LINES;
}

int Game::getScore() const {
    return _score;
}

void Game::addScore(int points) {
    _score += points;
}

void Game::resetScore() {
    _score = 0;
}

void Game::spawnAsteroid(float x, float y)
{
    _asteroids.push_back(std::make_unique<Asteroid>(x, y, 3));
}

void Game::initializeBackground() {
    _backgroundStars.clear();
    
    srand(time(nullptr));
    for (int i = 0; i < 35; i++) {
        Star star;
        star.x = static_cast<float>(rand() % (MIN_COLS - 4) + 2);
        star.y = static_cast<float>(rand() % (MIN_LINES - TOP_ROW - 2) + TOP_ROW + 1);
        
        int type = rand() % 3;
        if (type == 0) star.symbol = '.';
        else if (type == 1) star.symbol = '*';
        else star.symbol = '+';
        
        _backgroundStars.push_back(star);
    }
}

void Game::updateBackground(float dt) {
    for (auto &star : _backgroundStars) {
        star.y += 3.0f * dt;
        
        if (star.y >= MIN_LINES - 1) {
            star.y = TOP_ROW + 1;
            star.x = static_cast<float>(rand() % (MIN_COLS - 4) + 2);
            
            int type = rand() % 3;
            if (type == 0) star.symbol = '.';
            else if (type == 1) star.symbol = '*';
            else star.symbol = '+';
        }
    }
}

void Game::drawBackground(WINDOW *frame) {
    wattron(frame, COLOR_PAIR(4) | A_DIM);
    
    for (const auto &star : _backgroundStars) {
        int drawY = static_cast<int>(star.y);
        int drawX = static_cast<int>(star.x);
        
        if (drawY > TOP_ROW && drawY < MIN_LINES - 1 && 
            drawX > 0 && drawX < MIN_COLS - 1) {
            mvwaddch(frame, drawY, drawX, star.symbol);
        }
    }
    
    wattroff(frame, COLOR_PAIR(4) | A_DIM);
}

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
    bool p1Alive = _player && _player->getIsAlive();
    bool p2Alive = _player2 && _player2->getIsAlive();
    if (!p1Alive && !p2Alive) return;

    Hitbox playerBox = {0, 0, 0, 0};
    Hitbox player2Box = {0, 0, 0, 0};
    if (p1Alive) {
        playerBox = { _player->getX(), _player->getY(), static_cast<float>(_player->getWidth()), static_cast<float>(_player->getHeight()) };
    }
    if (p2Alive) {
        player2Box = { _player2->getX(), _player2->getY(), static_cast<float>(_player2->getWidth()), static_cast<float>(_player2->getHeight()) };
    }

    // PROIETTILI vs TUTTO
    for (auto &proj : _projectiles) {
        if (!proj->getIsAlive()) continue;
        Hitbox pBox = { proj->getX(), proj->getY(), static_cast<float>(proj->getWidth()), static_cast<float>(proj->getHeight()) };

        // Proiettili nemici vs Players
        if (proj->getTeam() == Team::Enemy) {
            bool hitPlayer = false;
            if (p1Alive && collides(pBox, playerBox)) {
                _player->takeDamage(1);
                hitPlayer = true;
            } else if (p2Alive && collides(pBox, player2Box)) {
                _player->takeDamage(1);
                hitPlayer = true;
            }
            if (hitPlayer) {
                proj->takeDamage(1);
                continue;
            }
        }

        // Proiettili player vs Boss
        if (proj->getTeam() == Team::Player && _boss) {
            Hitbox bossBox = { _boss->getX(), _boss->getY(), static_cast<float>(_boss->getWidth()), static_cast<float>(_boss->getHeight()) };
            if (collides(pBox, bossBox)) {
                _boss->takeDamage(1);
                proj->takeDamage(1);
                if (_boss->getHP() <= 0) addScore(500);
                continue;
            }
        }

        // Proiettili player vs Nemici
        if (proj->getTeam() == Team::Player) {
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
            if (!proj->getIsAlive()) continue;
        }

        // Proiettili vs Asteroidi (qualsiasi team danneggia asteroidi)
        for (auto &asteroid : _asteroids) {
            if (!asteroid->getIsAlive()) continue;
            Hitbox aBox = { asteroid->getX(), asteroid->getY(), static_cast<float>(asteroid->getWidth()), static_cast<float>(asteroid->getHeight()) };

            if (collides(pBox, aBox)) {
                asteroid->takeDamage(1);
                proj->takeDamage(1);
                if (!asteroid->getIsAlive()) addScore(asteroid->getScoreValue());
                break;
            }
        }
    }

    // NEMICI vs PLAYERS
    for (auto &enemy : _enemies) {
        if (!enemy->getIsAlive()) continue;
        Hitbox eBox = { enemy->getX(), enemy->getY(), static_cast<float>(enemy->getWidth()), static_cast<float>(enemy->getHeight()) };

        if (p1Alive && collides(playerBox, eBox)) {
            _player->takeDamage(1);
            enemy->takeDamage(1);
        }
        if (p2Alive && collides(player2Box, eBox)) {
            _player->takeDamage(1);
            enemy->takeDamage(1);
        }
    }

    // ASTEROIDI vs PLAYERS
    for (auto &asteroid : _asteroids) {
        if (!asteroid->getIsAlive()) continue;
        Hitbox aBox = { asteroid->getX(), asteroid->getY(), static_cast<float>(asteroid->getWidth()), static_cast<float>(asteroid->getHeight()) };

        if (p1Alive && collides(playerBox, aBox)) {
            _player->takeDamage(1);
            asteroid->takeDamage(1);
        }
        if (p2Alive && collides(player2Box, aBox)) {
            _player->takeDamage(1);
            asteroid->takeDamage(1);
        }
    }

    // BOSS vs PLAYERS
    if (_boss) {
        Hitbox bossBox = { _boss->getX(), _boss->getY(), static_cast<float>(_boss->getWidth()), static_cast<float>(_boss->getHeight()) };
        if (p1Alive && collides(playerBox, bossBox)) {
            _player->takeDamage(1);
        }
        if (p2Alive && collides(player2Box, bossBox)) {
            _player->takeDamage(1);
        }
    }

    // NEMICI vs ASTEROIDI
    for (auto &enemy : _enemies) {
        if (!enemy->getIsAlive()) continue;
        Hitbox eBox = { enemy->getX(), enemy->getY(), static_cast<float>(enemy->getWidth()), static_cast<float>(enemy->getHeight()) };

        for (auto &asteroid : _asteroids) {
            if (!asteroid->getIsAlive()) continue;
            Hitbox aBox = { asteroid->getX(), asteroid->getY(), static_cast<float>(asteroid->getWidth()), static_cast<float>(asteroid->getHeight()) };

            if (collides(eBox, aBox)) {
                enemy->takeDamage(1);
                asteroid->takeDamage(1);
                break;
            }
        }
    }

    // BOSS vs ASTEROIDI
    if (_boss) {
        Hitbox bossBox = { _boss->getX(), _boss->getY(), static_cast<float>(_boss->getWidth()), static_cast<float>(_boss->getHeight()) };
        for (auto &asteroid : _asteroids) {
            if (!asteroid->getIsAlive()) continue;
            Hitbox aBox = { asteroid->getX(), asteroid->getY(), static_cast<float>(asteroid->getWidth()), static_cast<float>(asteroid->getHeight()) };

            if (collides(bossBox, aBox)) {
                _boss->takeDamage(1);
                asteroid->takeDamage(1);
            }
        }
    }
}

void Game::updateEntities(float dt)
{
    if (_player)
        _player->update(dt, *this);
    if (_player2)
        _player2->update(dt, *this);

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

    for (auto &asteroid : _asteroids)
        if (asteroid)
            asteroid->update(dt, *this);

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
    if (_player2)
        _player2->render(frame);

    for (auto &enemy : _enemies)
        if (enemy)
            enemy->render(frame);

    if (_boss)
        _boss->render(frame);

    for (auto &proj : _projectiles)
        if (proj)
            proj->render(frame);

    for (auto &asteroid : _asteroids)
        if (asteroid)
            asteroid->render(frame);
}

// Helper: gestisce input multiplayer da vector di tasti
static void handleMultiplayerInput(const std::vector<int>& inputs, Player* p1, Player* p2)
{
    if (p1) p1->setShooting(false);
    if (p2) p2->setShooting(false);

    bool p1_up = false, p1_down = false, p1_left = false, p1_right = false;
    bool p2_up = false, p2_down = false, p2_left = false, p2_right = false;

    for (int answer : inputs) {
        if (answer == 'w' || answer == 'W') p1_up = true;
        else if (answer == 's' || answer == 'S') p1_down = true;
        else if (answer == 'a' || answer == 'A') p1_left = true;
        else if (answer == 'd' || answer == 'D') p1_right = true;
        else if (answer == KEY_UP) p2_up = true;
        else if (answer == KEY_DOWN) p2_down = true;
        else if (answer == KEY_LEFT) p2_left = true;
        else if (answer == KEY_RIGHT) p2_right = true;
        else if (answer == ' ') { if (p1) p1->setShooting(true); }
        else if (answer == '\n') { if (p2) p2->setShooting(true); }
    }

    if (p1) {
        if (!p1_up) p1->setDirection(0, false);
        if (!p1_down) p1->setDirection(1, false);
        if (!p1_left) p1->setDirection(2, false);
        if (!p1_right) p1->setDirection(3, false);
        if (p1_up) p1->setDirection(0, true);
        if (p1_down) p1->setDirection(1, true);
        if (p1_left) p1->setDirection(2, true);
        if (p1_right) p1->setDirection(3, true);
    }
    if (p2) {
        if (!p2_up) p2->setDirection(0, false);
        if (!p2_down) p2->setDirection(1, false);
        if (!p2_left) p2->setDirection(2, false);
        if (!p2_right) p2->setDirection(3, false);
        if (p2_up) p2->setDirection(0, true);
        if (p2_down) p2->setDirection(1, true);
        if (p2_left) p2->setDirection(2, true);
        if (p2_right) p2->setDirection(3, true);
    }
}

// Helper: sincronizza morte tra i due giocatori (salute condivisa)
static void syncPlayerDeath(Player* p1, Player* p2)
{
    if (p1 && !p1->getIsAlive() && p2) {
        p2->takeDamage(p2->getLives());
    }
}

static bool anyPlayerAlive(Player* p1, Player* p2)
{
    return (p1 && p1->getIsAlive()) || (p2 && p2->getIsAlive());
}

// Helper: inizializza player(s) in base a playerCount
static void initPlayers(std::unique_ptr<Player>& p1, std::unique_ptr<Player>& p2, int playerCount, int minCols)
{
    if (playerCount == 2) {
        p1 = std::make_unique<Player>(minCols / 2 - 6, 30);
        p2 = std::make_unique<Player>(minCols / 2 + 4, 30);
    } else {
        p1 = std::make_unique<Player>(minCols / 2 - 2, 30);
        p2 = nullptr;
    }
    p1->setDx(0);
    p1->setDy(0);
    if (p2) {
        p2->setDx(0);
        p2->setDy(0);
    }
}

bool Game::endless(const std::vector<int>& inputs, float frameDelta)
{
    static bool initialized = false;
    static auto last_spawn = std::chrono::steady_clock::now();
    static auto last_asteroid_spawn = std::chrono::steady_clock::now();
    static auto endless_start = std::chrono::steady_clock::now();
    static constexpr float ENEMY_SPAWN_INTERVAL = 3.5f;
    static constexpr float ASTEROID_SPAWN_INTERVAL = 5.0f;

    if (!initialized) {
        initPlayers(_player, _player2, _playerCount, MIN_COLS);
        srand(time(nullptr));
        last_spawn = std::chrono::steady_clock::now();
        last_asteroid_spawn = std::chrono::steady_clock::now();
        endless_start = std::chrono::steady_clock::now();
        _endlessElapsedSeconds = 0;
        _world1CurrentWave = -1;
        _world1TotalWaves = 0;
        initialized = true;
    }

    handleMultiplayerInput(inputs, _player.get(), _player2.get());

    _endlessElapsedSeconds = static_cast<int>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - endless_start
        ).count()
    );

    auto now = std::chrono::steady_clock::now();

    // Spawn nemici
    auto elapsed_spawn = std::chrono::duration<float>(now - last_spawn).count();
    if (elapsed_spawn >= ENEMY_SPAWN_INTERVAL) {
        spawnEnemy(rand() % (MIN_COLS - 6) + 3, TOP_ROW + 1);
        last_spawn = now;
    }

    // Spawn asteroidi
    auto elapsed_asteroid = std::chrono::duration<float>(now - last_asteroid_spawn).count();
    if (elapsed_asteroid >= ASTEROID_SPAWN_INTERVAL) {
        spawnAsteroid(rand() % (MIN_COLS - 6) + 3, TOP_ROW + 1);
        last_asteroid_spawn = now;
    }

    updateEntities(frameDelta);
    checkCollisions();

    syncPlayerDeath(_player.get(), _player2.get());

    if (!anyPlayerAlive(_player.get(), _player2.get()))
        return true;
    return false;
}

bool Game::world1(const std::vector<int>& inputs, float frameDelta)
{
    static bool initialized = false;
    static bool bossPhase = false;
    static bool bossDefeated = false;

    struct EnemySpawn {
        int type;
    };

    static constexpr EnemySpawn WAVE1[] = {
        {1}, {1}, {1}
    };
    static constexpr EnemySpawn WAVE2[] = {
        {0}, {1}, {0}, {1}
    };
    static constexpr EnemySpawn WAVE3[] = {
        {0}, {0}, {0}, {1}, {1}, {2}
    };
    static constexpr const EnemySpawn* WAVES[] = {WAVE1, WAVE2, WAVE3};
    static constexpr int WAVE_SIZES[] = {3, 4, 6};
    static constexpr int WAVE_COUNT = 3;
    static constexpr int BASIC_COLUMNS[] = {8, 22, 36, 50, 64, 78, 92, 106};
    static constexpr int BASIC_COL_COUNT = sizeof(BASIC_COLUMNS) / sizeof(BASIC_COLUMNS[0]);
    static constexpr int STATIONARY_COLUMNS[] = {14, 40, 66, 92};
    static constexpr int STATIONARY_COL_COUNT = sizeof(STATIONARY_COLUMNS) / sizeof(STATIONARY_COLUMNS[0]);
    static constexpr int RAMMER_COLUMNS[] = {10, 30, 58, 86, 104};
    static constexpr int RAMMER_COL_COUNT = sizeof(RAMMER_COLUMNS) / sizeof(RAMMER_COLUMNS[0]);

    static int currentWave = 0;
    static int spawnedInWave = 0;
    static auto last_spawn = std::chrono::steady_clock::now();
    static auto boss_last_shoot = std::chrono::steady_clock::now();

    if (!initialized) {
        initPlayers(_player, _player2, _playerCount, MIN_COLS);
        _enemies.clear();
        _projectiles.clear();
        _asteroids.clear();
        _boss = nullptr;
        currentWave = 0;
        spawnedInWave = 0;
        bossPhase = false;
        bossDefeated = false;
        last_spawn = std::chrono::steady_clock::now();
        boss_last_shoot = std::chrono::steady_clock::now();
        _endlessElapsedSeconds = -1;
        _world1TotalWaves = WAVE_COUNT;
        _world1CurrentWave = 1;
        initialized = true;
    }

    handleMultiplayerInput(inputs, _player.get(), _player2.get());

    auto now = std::chrono::steady_clock::now();

    if (!bossPhase && currentWave < WAVE_COUNT) {
        _world1CurrentWave = currentWave + 1;
        int waveSize = WAVE_SIZES[currentWave];
        auto elapsed_spawn = std::chrono::duration<float>(now - last_spawn).count();

        if (spawnedInWave < waveSize && elapsed_spawn >= SPAWN_INTERVAL) {
            const EnemySpawn &spawn = WAVES[currentWave][spawnedInWave];
            float spawnX = 0.0f;

            if (spawn.type == 0) {
                int basicCount = 0;
                for (int i = 0; i < spawnedInWave; i++)
                    if (WAVES[currentWave][i].type == 0) basicCount++;
                int laneIndex = (basicCount + currentWave * 2) % BASIC_COL_COUNT;
                spawnX = static_cast<float>(BASIC_COLUMNS[laneIndex]);
                _enemies.push_back(std::make_unique<AEnemy>(spawnX, TOP_ROW + 1, 'V', 1));
            }
            else if (spawn.type == 1) {
                int stationaryCount = 0;
                for (int i = 0; i < spawnedInWave; i++)
                    if (WAVES[currentWave][i].type == 1) stationaryCount++;
                int stationaryIndex = (stationaryCount + currentWave) % STATIONARY_COL_COUNT;
                spawnX = static_cast<float>(STATIONARY_COLUMNS[stationaryIndex]);
                _enemies.push_back(std::make_unique<StationaryEnemy>(spawnX, TOP_ROW + 1));
            }
            else if (spawn.type == 2) {
                int rammerCount = 0;
                for (int i = 0; i < spawnedInWave; i++)
                    if (WAVES[currentWave][i].type == 2) rammerCount++;
                int rammerIndex = (rammerCount + currentWave) % RAMMER_COL_COUNT;
                spawnX = static_cast<float>(RAMMER_COLUMNS[rammerIndex]);
                _enemies.push_back(std::make_unique<RammerEnemy>(spawnX, TOP_ROW + 1));
            }

            spawnedInWave++;
            last_spawn = now;
        }

        if (spawnedInWave >= waveSize && _enemies.empty() && _projectiles.empty()) {
            currentWave++;
            _player->setHp(_player->getHP() + 1);
            spawnedInWave = 0;
            last_spawn = now;
            if (currentWave < WAVE_COUNT)
                _world1CurrentWave = currentWave + 1;
            else
                _world1CurrentWave = WAVE_COUNT;
        }
    }

    // Boss phase
    if (currentWave >= WAVE_COUNT && !bossPhase && _enemies.empty()) {
        _boss = std::make_unique<Boss>(MIN_COLS / 2, TOP_ROW + 5, 10);
        bossPhase = true;
        _world1CurrentWave = WAVE_COUNT + 1;
        boss_last_shoot = std::chrono::steady_clock::now();
    }

    if (bossPhase && _boss) {
        _boss->update(frameDelta, *this);

        auto boss_shoot_elapsed = std::chrono::duration<float>(now - boss_last_shoot).count();
        if (boss_shoot_elapsed >= 1.2f) {
            spawnProjectile(_boss->getShootX(), _boss->getShootY(), 0.0f, 20.0f, 2, Team::Enemy);
            boss_last_shoot = now;
        }

        if (_boss->getHP() <= 0) {
            _boss = nullptr;
            bossDefeated = true;
        }
    }

    updateEntities(frameDelta);
    checkCollisions();

    syncPlayerDeath(_player.get(), _player2.get());

    if (!anyPlayerAlive(_player.get(), _player2.get()))
        return true;

    if (bossDefeated && _boss == nullptr && _projectiles.empty())
        return true;

    return false;
}

bool Game::world2(const std::vector<int>& inputs, float frameDelta)
{
    static bool initialized = false;
    static bool bossPhase = false;
    static bool bossDefeated = false;

    struct EnemySpawn {
        int type;
    };

    static constexpr EnemySpawn WAVE1[] = {
        {0}, {0}, {0}, {0}, {0}, {0}
    };
    static constexpr EnemySpawn WAVE2[] = {
        {0}, {0}, {0}, {0}, {1}, {1}, {2}
    };
    static constexpr EnemySpawn WAVE3[] = {
        {0}, {1}, {0}, {1}, {2}, {0}, {1}, {2}
    };
    static constexpr const EnemySpawn* WAVES[] = {WAVE1, WAVE2, WAVE3};
    static constexpr int WAVE_SIZES[] = {6, 7, 8};
    static constexpr int WAVE_COUNT = 3;
    static constexpr int BASIC_COLUMNS[] = {8, 22, 36, 50, 64, 78, 92, 106};
    static constexpr int BASIC_COL_COUNT = sizeof(BASIC_COLUMNS) / sizeof(BASIC_COLUMNS[0]);
    static constexpr int STATIONARY_COLUMNS[] = {14, 40, 66, 92};
    static constexpr int STATIONARY_COL_COUNT = sizeof(STATIONARY_COLUMNS) / sizeof(STATIONARY_COLUMNS[0]);
    static constexpr int RAMMER_COLUMNS[] = {10, 30, 58, 86, 104};
    static constexpr int RAMMER_COL_COUNT = sizeof(RAMMER_COLUMNS) / sizeof(RAMMER_COLUMNS[0]);

    static int currentWave = 0;
    static int spawnedInWave = 0;
    static auto last_spawn = std::chrono::steady_clock::now();
    static auto boss_last_shoot = std::chrono::steady_clock::now();

    if (!initialized) {
        initPlayers(_player, _player2, _playerCount, MIN_COLS);
        _enemies.clear();
        _projectiles.clear();
        _asteroids.clear();
        _boss = nullptr;
        currentWave = 0;
        spawnedInWave = 0;
        bossPhase = false;
        bossDefeated = false;
        last_spawn = std::chrono::steady_clock::now();
        boss_last_shoot = std::chrono::steady_clock::now();
        _endlessElapsedSeconds = -1;
        _world1TotalWaves = WAVE_COUNT;
        _world1CurrentWave = 1;
        initialized = true;
    }

    handleMultiplayerInput(inputs, _player.get(), _player2.get());

    auto now = std::chrono::steady_clock::now();

    if (!bossPhase && currentWave < WAVE_COUNT) {
        _world1CurrentWave = currentWave + 1;
        int waveSize = WAVE_SIZES[currentWave];
        auto elapsed_spawn = std::chrono::duration<float>(now - last_spawn).count();

        if (spawnedInWave < waveSize && elapsed_spawn >= SPAWN_INTERVAL) {
            const EnemySpawn &spawn = WAVES[currentWave][spawnedInWave];
            float spawnX = 0.0f;

            if (spawn.type == 0) {
                int basicCount = 0;
                for (int i = 0; i < spawnedInWave; i++)
                    if (WAVES[currentWave][i].type == 0) basicCount++;
                int laneIndex = (basicCount + currentWave * 2) % BASIC_COL_COUNT;
                spawnX = static_cast<float>(BASIC_COLUMNS[laneIndex]);
                _enemies.push_back(std::make_unique<AEnemy>(spawnX, TOP_ROW + 1, 'V', 1));
            }
            else if (spawn.type == 1) {
                int stationaryCount = 0;
                for (int i = 0; i < spawnedInWave; i++)
                    if (WAVES[currentWave][i].type == 1) stationaryCount++;
                int stationaryIndex = (stationaryCount + currentWave) % STATIONARY_COL_COUNT;
                spawnX = static_cast<float>(STATIONARY_COLUMNS[stationaryIndex]);
                _enemies.push_back(std::make_unique<StationaryEnemy>(spawnX, TOP_ROW + 1));
            }
            else if (spawn.type == 2) {
                int rammerCount = 0;
                for (int i = 0; i < spawnedInWave; i++)
                    if (WAVES[currentWave][i].type == 2) rammerCount++;
                int rammerIndex = (rammerCount + currentWave) % RAMMER_COL_COUNT;
                spawnX = static_cast<float>(RAMMER_COLUMNS[rammerIndex]);
                _enemies.push_back(std::make_unique<RammerEnemy>(spawnX, TOP_ROW + 1));
            }

            spawnedInWave++;
            last_spawn = now;
        }

        if (spawnedInWave >= waveSize && _enemies.empty() && _projectiles.empty()) {
            currentWave++;
            _player->setHp(_player->getHP() + 1);
            spawnedInWave = 0;
            last_spawn = now;
            if (currentWave < WAVE_COUNT)
                _world1CurrentWave = currentWave + 1;
            else
                _world1CurrentWave = WAVE_COUNT;
        }
    }

    // Boss phase
    if (currentWave >= WAVE_COUNT && !bossPhase && _enemies.empty()) {
        _boss = std::make_unique<Boss>(MIN_COLS / 2, TOP_ROW + 5, 20);
        bossPhase = true;
        _world1CurrentWave = WAVE_COUNT + 1;
        boss_last_shoot = std::chrono::steady_clock::now();
    }

    if (bossPhase && _boss) {
        _boss->update(frameDelta, *this);

        auto boss_shoot_elapsed = std::chrono::duration<float>(now - boss_last_shoot).count();
        if (boss_shoot_elapsed >= 1.2f) {
            spawnProjectile(_boss->getShootX(), _boss->getShootY(), 0.0f, 20.0f, 2, Team::Enemy);
            boss_last_shoot = now;
        }

        if (_boss->getHP() <= 0) {
            _boss = nullptr;
            bossDefeated = true;
        }
    }

    updateEntities(frameDelta);
    checkCollisions();

    syncPlayerDeath(_player.get(), _player2.get());

    if (!anyPlayerAlive(_player.get(), _player2.get()))
        return true;

    if (bossDefeated && _boss == nullptr && _projectiles.empty())
        return true;

    return false;
}

bool Game::world3(const std::vector<int>& inputs, float frameDelta)
{
    static bool initialized = false;
    static bool bossPhase = false;
    static bool bossDefeated = false;

    struct EnemySpawn {
        int type;
    };

    static constexpr EnemySpawn WAVE1[] = {
        {1}, {1}, {1}, {0}, {0}, {0}, {2}
    };
    static constexpr EnemySpawn WAVE2[] = {
        {2}, {0}, {1}, {0}, {2}, {1}, {0}, {1}, {2}
    };
    static constexpr EnemySpawn WAVE3[] = {
        {0}, {1}, {2}, {0}, {1}, {2}, {0}, {1}, {2}, {0}, {1}
    };
    static constexpr const EnemySpawn* WAVES[] = {WAVE1, WAVE2, WAVE3};
    static constexpr int WAVE_SIZES[] = {7, 9, 11};
    static constexpr int WAVE_COUNT = 3;
    static constexpr int BASIC_COLUMNS[] = {8, 22, 36, 50, 64, 78, 92, 106};
    static constexpr int BASIC_COL_COUNT = sizeof(BASIC_COLUMNS) / sizeof(BASIC_COLUMNS[0]);
    static constexpr int STATIONARY_COLUMNS[] = {14, 40, 66, 92};
    static constexpr int STATIONARY_COL_COUNT = sizeof(STATIONARY_COLUMNS) / sizeof(STATIONARY_COLUMNS[0]);
    static constexpr int RAMMER_COLUMNS[] = {10, 30, 58, 86, 104};
    static constexpr int RAMMER_COL_COUNT = sizeof(RAMMER_COLUMNS) / sizeof(RAMMER_COLUMNS[0]);

    static int currentWave = 0;
    static int spawnedInWave = 0;
    static auto last_spawn = std::chrono::steady_clock::now();
    static auto boss_last_shoot = std::chrono::steady_clock::now();

    if (!initialized) {
        initPlayers(_player, _player2, _playerCount, MIN_COLS);
        _enemies.clear();
        _projectiles.clear();
        _asteroids.clear();
        _boss = nullptr;
        currentWave = 0;
        spawnedInWave = 0;
        bossPhase = false;
        bossDefeated = false;
        last_spawn = std::chrono::steady_clock::now();
        boss_last_shoot = std::chrono::steady_clock::now();
        _endlessElapsedSeconds = -1;
        _world1TotalWaves = WAVE_COUNT;
        _world1CurrentWave = 1;
        initialized = true;
    }

    handleMultiplayerInput(inputs, _player.get(), _player2.get());

    auto now = std::chrono::steady_clock::now();

    if (!bossPhase && currentWave < WAVE_COUNT) {
        _world1CurrentWave = currentWave + 1;
        int waveSize = WAVE_SIZES[currentWave];
        auto elapsed_spawn = std::chrono::duration<float>(now - last_spawn).count();

        if (spawnedInWave < waveSize && elapsed_spawn >= SPAWN_INTERVAL) {
            const EnemySpawn &spawn = WAVES[currentWave][spawnedInWave];
            float spawnX = 0.0f;

            if (spawn.type == 0) {
                int basicCount = 0;
                for (int i = 0; i < spawnedInWave; i++)
                    if (WAVES[currentWave][i].type == 0) basicCount++;
                int laneIndex = (basicCount + currentWave * 2) % BASIC_COL_COUNT;
                spawnX = static_cast<float>(BASIC_COLUMNS[laneIndex]);
                _enemies.push_back(std::make_unique<AEnemy>(spawnX, TOP_ROW + 1, 'V', 1));
            }
            else if (spawn.type == 1) {
                int stationaryCount = 0;
                for (int i = 0; i < spawnedInWave; i++)
                    if (WAVES[currentWave][i].type == 1) stationaryCount++;
                int stationaryIndex = (stationaryCount + currentWave) % STATIONARY_COL_COUNT;
                spawnX = static_cast<float>(STATIONARY_COLUMNS[stationaryIndex]);
                _enemies.push_back(std::make_unique<StationaryEnemy>(spawnX, TOP_ROW + 1));
            }
            else if (spawn.type == 2) {
                int rammerCount = 0;
                for (int i = 0; i < spawnedInWave; i++)
                    if (WAVES[currentWave][i].type == 2) rammerCount++;
                int rammerIndex = (rammerCount + currentWave) % RAMMER_COL_COUNT;
                spawnX = static_cast<float>(RAMMER_COLUMNS[rammerIndex]);
                _enemies.push_back(std::make_unique<RammerEnemy>(spawnX, TOP_ROW + 1));
            }

            spawnedInWave++;
            last_spawn = now;
        }

        if (spawnedInWave >= waveSize && _enemies.empty() && _projectiles.empty()) {
            currentWave++;
            _player->setHp(_player->getHP() + 1);
            spawnedInWave = 0;
            last_spawn = now;
            if (currentWave < WAVE_COUNT)
                _world1CurrentWave = currentWave + 1;
            else
                _world1CurrentWave = WAVE_COUNT;
        }
    }

    // Boss phase
    if (currentWave >= WAVE_COUNT && !bossPhase && _enemies.empty()) {
        _boss = std::make_unique<Boss>(MIN_COLS / 2, TOP_ROW + 5, 30);
        bossPhase = true;
        _world1CurrentWave = WAVE_COUNT + 1;
        boss_last_shoot = std::chrono::steady_clock::now();
    }

    if (bossPhase && _boss) {
        _boss->update(frameDelta, *this);

        auto boss_shoot_elapsed = std::chrono::duration<float>(now - boss_last_shoot).count();
        if (boss_shoot_elapsed >= 1.2f) {
            spawnProjectile(_boss->getShootX(), _boss->getShootY(), 0.0f, 20.0f, 2, Team::Enemy);
            boss_last_shoot = now;
        }

        if (_boss->getHP() <= 0) {
            _boss = nullptr;
            bossDefeated = true;
        }
    }

    updateEntities(frameDelta);
    checkCollisions();

    syncPlayerDeath(_player.get(), _player2.get());

    if (!anyPlayerAlive(_player.get(), _player2.get()))
        return true;

    if (bossDefeated && _boss == nullptr && _projectiles.empty())
        return true;

    return false;
}

void Game::game_loop(int mode, int players)
{
    _playerCount = (players == 2) ? 2 : 1;
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

        // Leggi TUTTI gli input disponibili in questa frame
        std::vector<int> inputs;
        int answer;
        while ((answer = getch()) != ERR) {
            inputs.push_back(answer);
        }

        // Controlla tasti speciali
        bool shouldExit = false;
        bool shouldResize = false;
        for (int key : inputs) {
            if (key == 27) shouldExit = true;
            if (key == KEY_RESIZE) shouldResize = true;
        }

        if (shouldExit)
        {
            delwin(frame);
            return;
        }

        if (shouldResize)
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
            gameEnded = world1(inputs, frame_delta);
            if (gameEnded)
                playerWon = anyPlayerAlive(_player.get(), _player2.get());
        }
        else if (mode == 2) {
            gameEnded = world2(inputs, frame_delta);
            if (gameEnded)
                playerWon = anyPlayerAlive(_player.get(), _player2.get());
        }
        else if (mode == 3) {
            gameEnded = world3(inputs, frame_delta);
            if (gameEnded)
                playerWon = anyPlayerAlive(_player.get(), _player2.get());
        }
        else if (mode == 4) {
            gameEnded = endless(inputs, frame_delta);
            playerWon = false;
        }

        if (gameEnded) {
            werase(frame);
            wrefresh(frame);

            erase();
            std::string endText = playerWon ? "you won" : "you lost";
            std::string pointText = std::to_string(getScore()) + " points";
            ascii_art(stdscr, (LINES / 2) - 6, 0, endText, COLS, true);
            ascii_art(stdscr, (LINES / 2) - 2, 0, pointText, COLS, true);
            refresh();

            std::this_thread::sleep_for(std::chrono::seconds(3));
            delwin(frame);
            return;
        }

        werase(frame);
        draw_frame(frame);
        updateBackground(frame_delta);
        drawBackground(frame);
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