NAME = program

OBJ_PATH = obj
SRC = src/


SRCS = $(wildcard $(SRC)*.cpp)

OBJS = $(addprefix obj/,$(notdir $(SRCS:.cpp=.o)))
CFLAG = -Wall -Wextra -Werror
HEADERS = -I ./inc

all: $(NAME)

$(NAME): $(OBJS)
	@clang++ $(CFLAG) $(HEADERS) $^ -o $@

$(OBJ_PATH)/%.o: $(SRC)%.cpp
	@mkdir -p $(@D)
	clang++ $(CFLAG) $(HEADERS) -c $< -o $@

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
