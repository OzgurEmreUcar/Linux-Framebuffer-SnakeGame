#include "framebuffer.h"
#include "snake.h"
#include "food.h"
#include "input.h"
#include "game_routine.h"

int main()
{
    initialize_fb();
    enable_raw_mode();
    initialize_snake_food();
    game_routine();
    release_resources();
    return 0;
}