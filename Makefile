NAME = ft_shmup
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --log-file=log_valgrind.txt

SRCS_DIR = ./src/
HEADERS_DIR = ./include/

SRCS =	main.cpp \
		$(addprefix $(SRCS_DIR), main.cpp )
HEADERS = $(addprefix $(HEADERS_DIR), )

all: $(NAME)

$(NAME): $(SRCS) $(HEADERS)
	@echo "Compiling executable..."
	@$(CXX) $(CXXFLAGS) $(SRCS) -I$(HEADERS_DIR) -o $(NAME)

clean:
	@echo "Nothing to clean..."

fclean: clean
	@echo "Removing executable..."
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
.SILENT: