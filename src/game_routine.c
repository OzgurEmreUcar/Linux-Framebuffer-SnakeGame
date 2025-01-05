#include "food.h"
#include "framebuffer.h"
#include <stdlib.h>
#include "snake.h"
#include "string.h"
#include <stdbool.h>
#include <sys/mman.h> 
#include "input.h"

void game_routine()
{
    while(1)
    {
        memset(framebuffer_data, 0, finfo.line_length * vinfo.yres);
        draw_block(food.x, food.y, 0x00FF00,0);
        draw_snake();
        check_food_collision();
        render_framebuffer();
        if(!check_input()) break;

    }
}