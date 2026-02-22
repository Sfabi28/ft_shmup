# ft_shmup — Piano di Lavoro Completo

## 1. Panoramica del Progetto

**Nome programma:** `ft_shmup`
**Linguaggio:** C++17
**Compilatore:** `c++ -Wall -Wextra -Werror -std=c++17`
**Librerie:** ncurses (`-lncurses`), `<chrono>`, libreria standard C++
**Tipo di gioco:** Shoot-'em-up orizzontale stile R-Type / Gradius
**Persone:** 2

---

## 2. Requisiti Mandatory

| #   | Requisito                         | Note                                      |
| --- | --------------------------------- | ----------------------------------------- |
| M1  | Single-player                     | Un solo giocatore                         |
| M2  | Display con ncurses               | Tutta la grafica via ncurses              |
| M3  | Scrolling orizzontale o verticale | Lo schermo scorre nel mondo               |
| M4  | Nemici random multipli            | Nemici base, non servono abilità speciali |
| M5  | Il player può sparare             | Proiettili verso i nemici                 |
| M6  | Collisioni base                   | Nemico ti tocca → muori (perdi vita)      |
| M7  | Entità occupano 1 square          | Ogni entità = 1 carattere                 |
| M8  | Frame-based timing                | Il gioco avanza per frame                 |
| M9  | HUD: score, tempo, vite           | Informazioni visibili a schermo           |
| M10 | Clock-based timing                | Usare `<chrono>` per delta time reale     |
| M11 | Nemici che sparano                | I nemici emettono proiettili              |
| M12 | Scenery                           | Oggetti collidibili o sfondo decorativo   |

---

## 3. Requisiti Bonus

| #   | Bonus                                             | Complessità | Chi       |
| --- | ------------------------------------------------- | ----------- | --------- |
| B1  | Boss grandi (multi-square, tanti HP)              | Media       | A + B     |
| B2  | Nemici con behavior scriptati (inseguono, mirano) | Media       | B         |
| B3  | Multiplayer (stessa tastiera o rete)              | Alta        | Opzionale |
| B4  | Wave di nemici pre-scriptate                      | Bassa       | A         |

---

## 4. Architettura C++17

### 4.1 Gerarchia delle Classi

```
AGameEntity (abstract)
├── Player
├── Projectile
├── SceneryObject
└── AEnemy (abstract)
    ├── BasicEnemy
    ├── ScriptedEnemy    [BONUS]
    └── BossEnemy        [BONUS]

AMoveBehavior (abstract — Strategy pattern)
├── StraightLeft
├── SineWave
└── TrackPlayer          [BONUS]

AShootBehavior (abstract — Strategy pattern)
├── NoShoot
├── ShootRandom
├── AimAtPlayer          [BONUS]
└── BurstPattern         [BONUS]

Game                     (engine, game loop, gestione entità)
Renderer                 (wrapper ncurses)
HUD                      (punteggio, vite, tempo)
Spawner                  (gestione wave e spawn nemici)
```

### 4.2 Classe Base — AGameEntity

```cpp
class AGameEntity {
protected:
    float   _x, _y;
    char    _symbol;
    int     _hp;
    bool    _alive;
    Hitbox  _hitbox;    // {x, y, width, height} — predisposto per boss

public:
    AGameEntity(float x, float y, char symbol, int hp);
    virtual ~AGameEntity() = default;

    virtual void update(float dt, Game &game) = 0;
    virtual void render(Renderer &renderer) const;

    void         takeDamage(int amount);
    bool         isAlive() const;
    Hitbox       getHitbox() const;

    // no copy, allow move (C++17 idiom)
    AGameEntity(const AGameEntity &) = delete;
    AGameEntity &operator=(const AGameEntity &) = delete;
    AGameEntity(AGameEntity &&) = default;
    AGameEntity &operator=(AGameEntity &&) = default;
};
```

### 4.3 Hitbox (predisposta per Boss multi-square)

```cpp
struct Hitbox {
    int x, y, width, height;
};

// Entità normali: {x, y, 1, 1}
// Boss:           {x, y, 5, 3}

bool collides(const Hitbox &a, const Hitbox &b) {
    return a.x < b.x + b.width  && a.x + a.width  > b.x
        && a.y < b.y + b.height && a.y + a.height > b.y;
}
```

### 4.4 Strategy Pattern per Behavior (predisposto per nemici scriptati)

```cpp
class AMoveBehavior {
public:
    virtual ~AMoveBehavior() = default;
    virtual void move(AEnemy &self, const Game &game, float dt) = 0;
};

class AShootBehavior {
public:
    virtual ~AShootBehavior() = default;
    virtual void shoot(AEnemy &self, Game &game, float dt) = 0;
};

class AEnemy : public AGameEntity {
protected:
    std::unique_ptr<AMoveBehavior>   _moveBehavior;
    std::unique_ptr<AShootBehavior>  _shootBehavior;

public:
    AEnemy(float x, float y, char symbol, int hp,
           std::unique_ptr<AMoveBehavior> mb,
           std::unique_ptr<AShootBehavior> sb);

    void update(float dt, Game &game) override {
        _moveBehavior->move(*this, game, dt);
        _shootBehavior->shoot(*this, game, dt);
    }
};
```

Creare un nuovo tipo di nemico = combinare behavior:

```cpp
// BasicEnemy: va dritto, spara random
auto basic = std::make_unique<AEnemy>(x, y, 'E', 1,
    std::make_unique<StraightLeft>(speed),
    std::make_unique<ShootRandom>(interval));

// ScriptedEnemy: insegue il player, mira
auto scripted = std::make_unique<AEnemy>(x, y, 'S', 2,
    std::make_unique<TrackPlayer>(speed),
    std::make_unique<AimAtPlayer>(interval));

// Boss: onda sinusoidale, pattern a raffica
auto boss = std::make_unique<BossEnemy>(x, y, 30,
    std::make_unique<SineWave>(speed, amplitude),
    std::make_unique<BurstPattern>(interval, bulletCount));
```

### 4.5 Game Engine

```cpp
class Game {
private:
    Renderer    _renderer;
    HUD         _hud;
    Spawner     _spawner;
    Player      _player;

    std::vector<std::unique_ptr<AGameEntity>> _entities;

    int     _score = 0;
    float   _scrollOffset = 0.f;
    float   _elapsed = 0.f;
    bool    _running = true;

    enum class State { Title, Playing, GameOver };
    State   _state = State::Title;

public:
    void run();          // game loop principale

private:
    void processInput();
    void update(float dt);
    void render();
    void checkCollisions();
    void removeDeadEntities();
    void spawnEntity(std::unique_ptr<AGameEntity> entity);

    // Accessors per i behavior
    const Player &getPlayer() const;
    float getScrollOffset() const;
};
```

### 4.6 Game Loop con Timing

```cpp
void Game::run() {
    using clock = std::chrono::steady_clock;
    constexpr auto frameDuration = std::chrono::milliseconds(33); // ~30 FPS

    while (_running) {
        auto frameStart = clock::now();

        processInput();
        float dt = 0.033f; // fisso a 30fps, oppure calcolato
        update(dt);
        render();

        auto frameEnd = clock::now();
        auto elapsed = frameEnd - frameStart;
        if (elapsed < frameDuration)
            std::this_thread::sleep_for(frameDuration - elapsed);
    }
}
```

### 4.7 Spawner con Wave Scriptate (predisposto per bonus)

```cpp
struct WaveEvent {
    float       triggerTime;    // secondi dall'inizio
    std::string enemyType;      // "basic", "scripted", "boss"
    int         y;              // -1 = random
};

class Spawner {
    std::vector<WaveEvent>  _script;
    size_t                  _nextEvent = 0;
    float                   _randomTimer = 0.f;

public:
    Spawner();
    void update(float dt, float elapsed, Game &game);

private:
    void spawnFromScript(float elapsed, Game &game);
    void spawnRandom(float dt, Game &game);      // filler tra le wave
    std::unique_ptr<AEnemy> createEnemy(const std::string &type, int y);
};
```

### 4.8 Features C++17 da Sfruttare

| Feature                           | Dove usarla                                                      |
| --------------------------------- | ---------------------------------------------------------------- |
| `std::unique_ptr`                 | Ownership delle entità in `_entities`                            |
| `std::variant`                    | Stato del gioco (Title / Playing / GameOver) se preferito a enum |
| `std::optional`                   | Ritorno di `findNearestEnemy()` per mira scriptata               |
| `if constexpr`                    | Template per behavior generici                                   |
| Structured bindings               | `auto [x, y] = entity.getPosition();`                            |
| `std::string_view`                | Passaggio stringhe leggere (HUD, log)                            |
| `[[nodiscard]]`                   | Su funzioni come `isAlive()`, `collides()`                       |
| `[[maybe_unused]]`                | Parametri non usati nei behavior base                            |
| Fold expressions                  | Se servono funzioni variadic                                     |
| Class template argument deduction | Semplifica costruzione container                                 |
| Inline variables                  | Costanti globali (FPS, dimensioni schermo)                       |
| Nested namespaces                 | `namespace ft::shmup::behavior { }`                              |

---

## 5. Struttura del Progetto

```
ft_shmup/
├── Makefile
├── include/
│   ├── Game.hpp
│   ├── Renderer.hpp
│   ├── HUD.hpp
│   ├── Spawner.hpp
│   ├── Hitbox.hpp
│   ├── AGameEntity.hpp
│   ├── Player.hpp
│   ├── Projectile.hpp
│   ├── SceneryObject.hpp
│   ├── AEnemy.hpp
│   ├── BossEnemy.hpp           [BONUS]
│   ├── behaviors/
│   │   ├── AMoveBehavior.hpp
│   │   ├── AShootBehavior.hpp
│   │   ├── StraightLeft.hpp
│   │   ├── SineWave.hpp
│   │   ├── TrackPlayer.hpp     [BONUS]
│   │   ├── NoShoot.hpp
│   │   ├── ShootRandom.hpp
│   │   ├── AimAtPlayer.hpp     [BONUS]
│   │   └── BurstPattern.hpp    [BONUS]
│   └── WaveEvent.hpp
└── src/
    ├── main.cpp
    ├── Game.cpp
    ├── Renderer.cpp
    ├── HUD.cpp
    ├── Spawner.cpp
    ├── Hitbox.cpp
    ├── AGameEntity.cpp
    ├── Player.cpp
    ├── Projectile.cpp
    ├── SceneryObject.cpp
    ├── AEnemy.cpp
    ├── BossEnemy.cpp           [BONUS]
    └── behaviors/
        ├── StraightLeft.cpp
        ├── SineWave.cpp
        ├── TrackPlayer.cpp     [BONUS]
        ├── ShootRandom.cpp
        ├── AimAtPlayer.cpp     [BONUS]
        └── BurstPattern.cpp    [BONUS]
```

---

## 6. Divisione dei Ruoli

### Persona A — "Engine & Logica"

**Responsabilità:** tutto ciò che riguarda la logica del gioco, il motore, le regole.

**File principali:** `Game.cpp`, `Spawner.cpp`, `Hitbox.cpp`, `AGameEntity.cpp`, `main.cpp`, `Makefile`

| Area                      | Dettaglio                                                                    |
| ------------------------- | ---------------------------------------------------------------------------- |
| Setup progetto            | Makefile, struttura cartelle, repo git                                       |
| AGameEntity               | Classe base astratta con HP, hitbox, takeDamage                              |
| Hitbox                    | Struct + funzione `collides()` con supporto rettangoli                       |
| Game                      | Game loop, lista entità, update/render cycle, stati (Title/Playing/GameOver) |
| Timing                    | Delta time con `<chrono>`, sleep per FPS costanti                            |
| Collisioni                | In `Game::update()`: projectile↔enemy, enemy↔player, scenery↔player          |
| Spawner                   | Timer + script wave, spawn random come filler, factory di nemici             |
| Score & Vite              | Contatore score in Game, sistema vite con respawn/invincibilità breve        |
| Game Over                 | Schermata finale, restart, title screen                                      |
| Scrolling                 | Offset globale che avanza, sposta/genera scenery                             |
| **BONUS:** Wave scriptate | Vector di WaveEvent con tempi e tipi, boss a fine livello                    |
| **BONUS:** Boss logica    | HP elevati, hitbox grande, integrazione nel spawner                          |

### Persona B — "Entità & Presentazione"

**Responsabilità:** tutto ciò che il giocatore vede e con cui interagisce.

**File principali:** `Renderer.cpp`, `HUD.cpp`, `Player.cpp`, `Enemy.cpp`, `Projectile.cpp`, `SceneryObject.cpp`, `behaviors/*.cpp`

| Area                        | Dettaglio                                                                             |
| --------------------------- | ------------------------------------------------------------------------------------- |
| Renderer                    | Wrapper ncurses: init, shutdown, drawChar, drawString, drawBox (per boss), colori     |
| Colori                      | `init_pair()` per ogni tipo di entità: player, nemico, proiettile, boss, scenery, HUD |
| Player                      | Movimento (frecce/WASD), sparo (spazio), cooldown, rendering                          |
| Projectile                  | Movimento lineare (dx per player, sx per enemy), muore fuori schermo                  |
| BasicEnemy + behavior       | `StraightLeft` + `ShootRandom`                                                        |
| SceneryObject               | Oggetti sfondo (`#`, `~`, `.`), collidibili o decorativi                              |
| HUD                         | Score, vite (♥), tempo, posizione fissa in alto/basso                                 |
| Effetti visivi              | Esplosione alla morte (`*` per 2-3 frame), flash invincibilità player                 |
| **BONUS:** BossEnemy render | Rendering multi-carattere (ASCII art 5×3), barra vita                                 |
| **BONUS:** ScriptedEnemy    | `TrackPlayer` (insegue), `AimAtPlayer` (mira), `SineWave` (ondulato)                  |
| **BONUS:** BurstPattern     | Pattern di sparo del boss (ventaglio, raffica, spirale)                               |

---

## 7. Interfacce Condivise (da definire INSIEME — prima sessione)

Questi header vanno scritti insieme prima di dividersi:

```cpp
// Hitbox.hpp — come si rappresentano le collisioni
struct Hitbox { int x, y, width, height; };
[[nodiscard]] bool collides(const Hitbox &a, const Hitbox &b);

// AGameEntity.hpp — contratto base per tutte le entità
class AGameEntity {
    virtual void update(float dt, Game &game) = 0;
    virtual void render(Renderer &renderer) const = 0;
    // ... (vedi sezione 4.2)
};

// Renderer.hpp — come Game/entità disegnano a schermo
class Renderer {
    void init();
    void shutdown();
    void clear();
    void present();    // chiama refresh()
    void drawChar(int x, int y, char ch, int colorPair = 0);
    void drawString(int x, int y, const std::string &str, int colorPair = 0);
    void drawBox(int x, int y, int w, int h, int colorPair = 0);  // per boss
    int  getWidth() const;
    int  getHeight() const;
};

// Game.hpp — come le entità interagiscono col motore
class Game {
    void spawnEntity(std::unique_ptr<AGameEntity> entity);
    const Player &getPlayer() const;
    float getScrollOffset() const;
    int getScreenWidth() const;
    int getScreenHeight() const;
};
```

**Regola:** una volta concordati questi header, A non tocca i file di B e viceversa. Si comunica solo attraverso le interfacce.

---

## 8. Piano di Lavoro Dettagliato

### GIORNO 1 — Mattina (3-4 ore)

**Insieme (30 min):**

-   Creare repo, cartelle, Makefile base
-   Scrivere insieme gli header condivisi (Hitbox, AGameEntity, Renderer, Game)
-   Concordare convenzioni (naming, indent, namespace)

**In parallelo:**

| Ora   | Persona A                                                                     | Persona B                                                                       |
| ----- | ----------------------------------------------------------------------------- | ------------------------------------------------------------------------------- |
| +0:30 | `AGameEntity.cpp` completo                                                    | `Renderer.cpp`: init ncurses, nodelay, noecho, curs_set(0), keypad, start_color |
| +1:30 | `Game.cpp` scheletro: loop, vector di entità, chiama update/render            | `Player.cpp`: movimento con frecce, clamp ai bordi dello schermo                |
| +2:30 | `Hitbox.cpp`: collides() con rettangoli                                       | Testa Renderer standalone: apre finestra, disegna char, chiude                  |
| +3:00 | **MERGE & TEST:** collegano Game + Renderer + Player → `@` si muove a schermo |                                                                                 |

**✅ Milestone 1: Player si muove sullo schermo**

---

### GIORNO 1 — Pomeriggio (4-5 ore)

| Ora   | Persona A                                                                | Persona B                                                       |
| ----- | ------------------------------------------------------------------------ | --------------------------------------------------------------- |
| +0:00 | `checkCollisions()` in Game: cicla entità, usa `collides()`              | `Projectile.cpp`: si muove orizzontalmente, muore fuori schermo |
| +1:00 | Logica vite: 3 vite, collisione = -1 vita, vite=0 → stato GameOver       | Player spara con spazio → crea Projectile, cooldown tra spari   |
| +1:30 | Logica score: nemico ucciso = +100, variabile in Game                    | `StraightLeft.cpp` + `ShootRandom.cpp` (behavior base)          |
| +2:00 | `Spawner.cpp`: timer, ogni N secondi crea BasicEnemy a Y random a destra | `AEnemy.cpp` + BasicEnemy con i behavior di sopra               |
| +3:00 | `removeDeadEntities()`: pulisce entità morte dal vector                  | `HUD.cpp`: riga fissa con score, vite (♥♥♥), tempo              |
| +3:30 | Difficoltà crescente: spawner accelera col tempo                         | Definisce palette colori: player=cyan, enemy=red, bullet=yellow |
| +4:00 | **MERGE & TEST:** gioco completo base, si gioca! Fix bug insieme         |                                                                 |

**✅ Milestone 2: Gioco giocabile — spari, nemici, collisioni, score, vite, HUD**

---

### GIORNO 2 — Mattina (3-4 ore)

| Ora   | Persona A                                                               | Persona B                                                                 |
| ----- | ----------------------------------------------------------------------- | ------------------------------------------------------------------------- |
| +0:00 | **Scrolling**: offset X globale, sfondo che scorre (`.` `~` pattern)    | `SceneryObject.cpp`: oggetti `#` che scrollano, collidibili               |
| +1:00 | Title screen: "FT_SHMUP — Press ENTER", stati Title→Playing→GameOver    | Effetti: esplosione alla morte (`*`/`X` per 3 frame), flash invincibilità |
| +1:30 | Game Over screen: score finale, "Press R to restart", reset stato       | `drawBox()` nel Renderer (preparazione per boss)                          |
| +2:00 | `Spawner`: implementa WaveEvent script, legge da vector predefinito     | `SineWave.cpp` behavior: nemico che oscilla su/giù                        |
| +2:30 | Definisce il livello: sequenza di wave con timing preciso               | Barra vita boss nel HUD (preparazione)                                    |
| +3:00 | **MERGE & TEST:** scrolling funziona, wave scriptate, title/gameover ok |                                                                           |

**✅ Milestone 3: Gioco completo con scrolling, scenery, wave, schermate**

---

### GIORNO 2 — Pomeriggio (3-4 ore) — BONUS

| Ora   | Persona A                                                         | Persona B                                                            |
| ----- | ----------------------------------------------------------------- | -------------------------------------------------------------------- |
| +0:00 | Boss logica: hitbox grande, 30 HP, appare a fine livello          | `BossEnemy.cpp` render: ASCII art multi-char (es. 5×3)               |
| +0:30 | Boss nel spawner: WaveEvent speciale, musica/flash all'arrivo     | `BurstPattern.cpp`: boss spara ventaglio di proiettili               |
| +1:00 | Collisioni boss: proiettili player vs hitbox grande, danno visivo | `TrackPlayer.cpp`: nemico scriptato che si muove verso il player     |
| +1:30 | Bilanciamento: HP boss, velocità proiettili, intervalli di sparo  | `AimAtPlayer.cpp`: nemico che calcola direzione verso player e spara |
| +2:00 | Aggiunge wave con nemici scriptati nel livello                    | Barra HP boss visibile, colore che cambia con HP                     |
| +2:30 | **TEST FINALE INSIEME**                                           |                                                                      |
| +3:00 | Fix crash, edge case, warning compilatore                         | Pulizia grafica, niente flickering, artefatti                        |
| +3:30 | Pulizia codice, commenti, Makefile finale                         | Review insieme, ultimo giro di test                                  |

**✅ Milestone 4: Bonus completi — boss, nemici scriptati, wave pre-definite**

---

## 9. Makefile

```makefile
NAME        = ft_shmup
CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++17
LDFLAGS     = -lncurses

SRC_DIR     = src
INC_DIR     = include
OBJ_DIR     = obj

SRCS        = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS        = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
```

---

## 10. Checklist Finale

### Mandatory

-   [ ] Si compila con `-Wall -Wextra -Werror` senza warning
-   [ ] Nessun crash (segfault, double free, etc.)
-   [ ] Single player funzionante
-   [ ] Display con ncurses
-   [ ] Scrolling orizzontale
-   [ ] Nemici random multipli
-   [ ] Player spara
-   [ ] Collisioni (nemico tocca player → perde vita)
-   [ ] Entità = 1 square
-   [ ] Frame-based timing
-   [ ] HUD: score, tempo, vite
-   [ ] Clock-based timing con `<chrono>`
-   [ ] Nemici sparano
-   [ ] Scenery presente

### Bonus

-   [ ] Boss grande (multi-square, HP elevati)
-   [ ] Nemici scriptati (inseguono e/o mirano)
-   [ ] Wave pre-scriptate con sequenze definite
-   [ ] (Opzionale) Multiplayer locale

---

## 11. Risorse da Studiare

| Cosa             | Link / Comando                                                       |
| ---------------- | -------------------------------------------------------------------- |
| ncurses tutorial | `man ncurses`, tutorial online "NCURSES Programming HOWTO"           |
| ncurses colors   | `man init_pair`, `man attron`                                        |
| `<chrono>`       | cppreference.com sezione chrono                                      |
| Strategy pattern | "Head First Design Patterns" cap. Strategy, o qualsiasi tutorial C++ |
| R-Type gameplay  | YouTube: "R-Type gameplay" per capire lo stile del gioco             |
