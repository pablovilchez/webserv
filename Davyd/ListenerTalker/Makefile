NAME_LIS = listener
NAME_TALK = talker
NAME_BROD = brodcast

STR_LIS = $(NAME_LIS).cpp
STR_TALK = $(NAME_TALK).cpp
STR_BROD = $(NAME_BROD).cpp

OBJ_LIS = $(NAME_LIS).o
OBJ_TALK = $(NAME_TALK).o
OBJ_BROAD = $(NAME_BROD).o

all: $(NAME_LIS) $(NAME_TALK) $(NAME_BROD)

$(NAME_LIS): $(OBJ_LIS)
	clang++ -o $(NAME_LIS) $(OBJ_LIS)

$(NAME_TALK): $(OBJ_TALK)
	clang++  -o $(NAME_TALK) $(OBJ_TALK)

$(NAME_BROD): $(OBJ_BROAD)
	clang++  -o $(NAME_BROD) $(OBJ_BROAD)

$(OBJ_LIS): $(STR_LIS)
	clang++ -c $(STR_LIS)

$(OBJ_TALK): $(STR_TALK)
	clang++ -c $(STR_TALK)

$(OBJ_BROAD): $(STR_BROD)
	clang++ -c $(STR_BROD)

clean:
	rm -f $(NAME_LIS) $(NAME_TALK) $(OBJ_LIS) $(OBJ_TALK) $(OBJ_BROAD) $(NAME_BROD)

.PHONY: all clean
