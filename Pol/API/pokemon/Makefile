
NAME = apipokemon

CC = c++ #-fsanitize=address
CFLAGS = -Wall -Wextra -Werror 
REMOVE = rm -rf

OBJ_PATH = obj

HEADERS = -I /Users/pvilchez/.brew/opt/jsoncpp/include -lcurl -ljsoncpp

SRC_FILES = main.cpp

SRC := $(SRC_FILES)
OBJ = $(SRC:%.cpp=$(OBJ_PATH)/%.o)

ORANGE = \033[1;38;5;208m
RESET = \033[0m
U_LINE = \033[4m
YELLOW = \033[1;38;5;226m

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(OBJ) $(HEADERS) -o $@
	@echo "\n$(ORANGE)$(U_LINE)$(NAME): Compiled$(RESET) \n"

$(OBJ_PATH)/%.o: %.cpp
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(FLAGS) -o $@ -c $< $(HEADERS)
	@echo "$(YELLOW)$(NAME) Compiling:$(RESET) $(notdir $<)"

clean:
	@$(REMOVE) $(OBJ_PATH)

fclean: clean
	@$(REMOVE) $(NAME)

re: fclean all

.PHONY: all clean fclean re