NAME = ft_shmup
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17
NCURSES = -lncurses

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --log-file=log_valgrind.txt

SRCS_DIR = ./src/
HEADERS_DIR = ./include/

SRCS =	$(addprefix $(SRCS_DIR), AEnemy.cpp AGameEntity.cpp ascii.cpp draw.cpp Game.cpp gameLoop.cpp Hitbox.cpp main.cpp Player.cpp Projectile.cpp StationaryEnemy.cpp RammerEnemy.cpp Boss.cpp)
HEADERS = $(addprefix $(HEADERS_DIR), AEnemy.hpp AGameEntity.hpp Game.hpp Hitbox.hpp Player.hpp Projectile.hpp StationaryEnemy.hpp RammerEnemy.hpp Boss.hpp)

all: $(NAME)

$(NAME): $(SRCS) $(HEADERS)
	@echo "Compiling executable..."
	@$(CXX) $(CXXFLAGS) $(SRCS) -I$(HEADERS_DIR) -o $(NAME) $(NCURSES)

clean:
	@echo "Nothing to clean..."

fclean: clean
	@echo "Removing executable..."
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
.SILENT: