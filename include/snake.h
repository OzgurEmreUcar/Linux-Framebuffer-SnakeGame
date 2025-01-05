#ifndef SNAKE_H
#define BLOCK_SIZE 20
#define SNAKE_H

struct Point {
    int x, y;
};

extern int snake_length,snake_start_X,snake_start_Y,dx,dy;
extern struct Point *snake;

void initialize_snake_food();
void draw_snake();
void check_food_collision();

#endif
