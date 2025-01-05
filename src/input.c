#include "input.h"
#include <unistd.h>
#include <termios.h>
#include "snake.h"
#include <stdlib.h>  

char input;
struct termios orig_termios;

bool check_input()
{
    if (read(STDIN_FILENO, &input, 1) == 1) {
        if (input == 'q') return false;
        if (input == 'w' && dy == 0) { dx = 0; dy = -BLOCK_SIZE; }
        if (input == 's' && dy == 0) { dx = 0; dy = BLOCK_SIZE; }
        if (input == 'a' && dx == 0) { dx = -BLOCK_SIZE; dy = 0; }
        if (input == 'd' && dx == 0) { dx = BLOCK_SIZE; dy = 0; }
    }
    return true;
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(reset_terminal);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void reset_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}
