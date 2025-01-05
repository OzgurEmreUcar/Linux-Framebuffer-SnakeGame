CC = gcc
CFLAGS = -Wall -g -I./include
SRC = src/main.c src/framebuffer.c src/snake.c src/food.c src/input.c src/game_routine.c
OBJ = $(SRC:.c=.o)
EXEC = snake_game

$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)


