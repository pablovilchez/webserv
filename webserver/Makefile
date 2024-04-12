
NAME = webserv

CC = clang++ -fsanitize=address
FLAGS = -Wall -Wextra -Werror -std=c++98
REMOVE = rm -rf

ifeq ($(DEBUG), true)
	FLAGS += -DDEBUG
endif

INC_PATH = inc
SRC_PATH = src
OBJ_PATH = obj

HEADERS = -I ./$(INC_PATH)/
SRC = $(wildcard $(SRC_PATH)/*.cpp)
OBJ = $(addprefix $(OBJ_PATH)/,$(notdir $(SRC:.cpp=.o)))

ORANGE = \033[1;38;5;208m
RESET = \033[0m
U_LINE = \033[4m
YELLOW = \033[1;38;5;226m

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(OBJ) $(HEADERS) -o $@
	@echo "\n$(ORANGE)$(U_LINE)$(NAME): Compiled$(RESET) \n"

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(FLAGS) -o $@ -c $< $(HEADERS)
	@echo "$(YELLOW)$(NAME) Compiling:$(RESET) $(notdir $<)"

clean:
	@$(REMOVE) $(OBJ_PATH)

fclean: clean
	@$(REMOVE) $(NAME)

re: fclean all

.PHONY: all clean fclean re
