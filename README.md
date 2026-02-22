# ft_shmup

🚀 Terminal shmup in C++ using ncurses. Fight through three worlds of waves and face a final boss, or jump into endless mode and chase the timer.

## Highlights ✨
- 🧩 Three worlds with hand-crafted wave patterns
- 👾 Boss fight at the end of each world with scaling HP
- ⏱️ Endless mode with a center timer
- 🎨 ASCII art entities rendered in ncurses
- 🔥 Fast, tight gameplay tuned for the terminal

## Requirements 🧰
- c++ (C++17)
- ncurses
- make

## Build 🔧
```bash
make
```

## Run ▶️
```bash
./ft_shmup
```

## Controls 🎮
- Arrow keys: move
- Space: shoot
- Esc: quit (if supported by the menu)

## Game Modes 🗺️
- World 1/2/3: finite waves, then a boss with scaling HP
- Endless: infinite waves with a center timer

## Boss Fight 🧨
Each world ends with a boss. The boss moves horizontally and fires centered shots from the bottom of its sprite. HP scales by world for a clean difficulty ramp.

## Tips 💡
- Keep moving to avoid enemy volleys.
- Focus fire on the boss to end the world quickly.
- Use the center lanes to align your shots.

## Valgrind (with suppressions) 🧪
```bash
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
  --suppressions=ignore_ncurses.supp ./ft_shmup
```

## Project Layout 🗂️
- include/: headers
- src/: implementation files
- Makefile
- ignore_ncurses.supp

## Notes 📝
This project uses ncurses. Some Valgrind reports are expected and can be suppressed with the provided file.
