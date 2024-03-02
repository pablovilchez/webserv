NAME_LIS = listener
NAME_TALK = talker

STR_LIS = $(NAME_LIS).cpp
STR_TALK = $(NAME_TALK).cpp

OBJ_LIS = $(NAME_LIS).o
OBJ_TALK = $(NAME_TALK).o

all: $(NAME_LIS) $(NAME_TALK)

$(NAME_LIS): $(OBJ_LIS)
	clang++ -o $(NAME_LIS) $(OBJ_LIS) $(LDFLAGS)

$(NAME_TALK): $(OBJ_TALK)
	clang++  -o $(NAME_TALK) $(OBJ_TALK) $(LDFLAGS)

$(OBJ_LIS): $(STR_LIS)
	clang++ -c $(STR_LIS) $(LDFLAGS)

$(OBJ_TALK): $(STR_TALK)
	clang++ -c $(STR_TALK) $(LDFLAGS)

clean:
	rm -f $(NAME_LIS) $(NAME_TALK) $(OBJ_LIS) $(OBJ_TALK)

fclean: clean
	rm -f $(NAME_LIS) $(NAME_TALK)

.PHONY: all clean
