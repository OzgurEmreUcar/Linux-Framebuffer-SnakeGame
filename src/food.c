#include "food.h"
#include "framebuffer.h"
#include <stdlib.h>
#include "snake.h"

struct Point food;

void initialize_food()
{
    food.x = (vinfo.xres / 4);
    food.y = (vinfo.yres / 4);
}

void check_food_collision()
{
    if (snake[0].x < food.x + BLOCK_SIZE && snake[0].x + BLOCK_SIZE > food.x && 
        snake[0].y < food.y + BLOCK_SIZE && snake[0].y + BLOCK_SIZE > food.y) {
        food.x = (rand() % (vinfo.xres / BLOCK_SIZE)) * BLOCK_SIZE;
        food.y = (rand() % (vinfo.yres / BLOCK_SIZE)) * BLOCK_SIZE;
        snake_length++;
        snake = realloc(snake, snake_length * sizeof(struct Point));
        snake[snake_length - 1] = snake[snake_length - 2];
    }
}
