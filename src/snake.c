#include "snake.h"
#include "framebuffer.h"
#include <stdlib.h>
#include "food.h"

int snake_start_X,snake_start_Y,dx,dy;
struct Point *snake;
int snake_length = 3;

void initialize_snake_food()
{
    snake = malloc(snake_length*sizeof(struct Point));
    for(int i =0;i<snake_length;i++)
    {
        snake[i].x = (vinfo.xres / 2) + i * BLOCK_SIZE;
        snake[i].y = vinfo.yres / 2;
    }
    food.x = (vinfo.xres / 4);
    food.y = (vinfo.yres / 4);
    dx = -BLOCK_SIZE;
    dy = 0;
}

void draw_snake()
{
    for (int i = 0; i < snake_length; i++) {
        draw_block(snake[i].x, snake[i].y, 0xFF0000,i);
    }
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0].x += dx;
    snake[0].y += dy;
}
