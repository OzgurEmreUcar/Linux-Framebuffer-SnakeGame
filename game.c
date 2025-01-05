#include <fcntl.h> // Header that defines constants and functions for file control operations, e.g., O_RDWR.
#include <linux/fb.h> // Provides structure definitions and constants related to the framebuffer device.
#include <sys/ioctl.h> // Contains definitions for the ioctl function used to manipulate devices or retrieve device-specific information.
#include <sys/mman.h> // Contains definitions for memory mapping functions.
#include <stdio.h> // Standard input-output library.
#include <stdlib.h> // Standard library for memory allocation, process control, and other utilities.
#include <unistd.h> // Defines constants and types related to system calls such as file handling and process control.
#include <string.h> // Contains string manipulation functions.
#include <termios.h>
#include <stdbool.h>

#define BLOCK_SIZE 20 // Defines the width of the rectangle to be drawn.
#define BLOCK_OFFSET 30
#define COLOR 0x00FF00 // Defines the color of the rectangle to be drawn (green in hex).

int snake_length = 4,framebuffer_fd,snake_start_X,snake_start_Y;
char *framebuffer_data;
struct fb_var_screeninfo vinfo; // Structure to store variable screen information (e.g., resolution, bits per pixel).
struct fb_fix_screeninfo finfo; // Structure to store fixed screen information (e.g., line length, memory start address).
long screensize;
struct Point {int x,y;};
struct termios orig_termios;
struct Point *snake;
struct Point food;
int dx = -BLOCK_SIZE, dy = 0;
char input;

int initialize_fb();
void draw_block(int x,int y,int color,int sblock_id);
void release_resources();
void enable_raw_mode();
void reset_terminal();
void render_framebuffer();
int initialize_snake_food();
void game_routine();
bool check_input();
void draw_snake();
void check_food_collision();

int main()
{
    initialize_fb();
    enable_raw_mode();
    initialize_snake_food();
    game_routine();
    release_resources();
    return 0;
}

int initialize_fb()
{
    framebuffer_fd = open("/dev/fb0", O_RDWR); // Opens the framebuffer device in read-write mode.

    if (framebuffer_fd == -1) {  // Checks if the framebuffer device was successfully opened.
        perror("Error opening framebuffer device"); // Prints an error message if the device fails to open.
        return 1;
    }

    if (ioctl(framebuffer_fd, FBIOGET_FSCREENINFO, &finfo) == -1) // Retrieves fixed screen information and stores it in finfo.
    { 
        perror("Error reading fixed information"); // Prints an error message if the operation fails.
        close(framebuffer_fd); // Closes the framebuffer device.
        return 1;
    }

    if (ioctl(framebuffer_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) // Retrieves variable screen information and stores it in vinfo.
    { 
        perror("Error reading variable information"); // Prints an error message if the operation fails.
        close(framebuffer_fd); // Closes the framebuffer device.
        return 1;
    }

    screensize = vinfo.yres_virtual * finfo.line_length; // Calculates the total screen size in bytes.
    framebuffer_data = malloc(screensize); // Maps the framebuffer device to program memory.
    memset(framebuffer_data, 0, screensize); // Clear screen
    if (framebuffer_data == MAP_FAILED) // Checks if the memory mapping failed.
    {
        perror("Error mapping framebuffer device to memory"); // Prints an error message if mapping fails.
        close(framebuffer_fd); // Closes the framebuffer device.
        return 1;
    }

    int center_x = vinfo.xres / 2; // Horizontal center of the screen.
    int center_y = vinfo.yres / 2; // Vertical center of the screen.
    snake_start_X = center_x - BLOCK_SIZE / 2; // Top-left corner of the rectangle (X-coordinate).
    snake_start_Y = center_y - BLOCK_SIZE / 2; // Top-left corner of the rectangle (Y-coordinate).
}

int initialize_snake_food()
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

bool check_input(){
    if (read(STDIN_FILENO, &input, 1) == 1) {
        if (input == 'q') return false;
        if (input == 'w' && dy == 0) { dx = 0; dy = -BLOCK_SIZE; }
        if (input == 's' && dy == 0) { dx = 0; dy = BLOCK_SIZE; }
        if (input == 'a' && dx == 0) { dx = -BLOCK_SIZE; dy = 0; }
        if (input == 'd' && dx == 0) { dx = BLOCK_SIZE; dy = 0; }
    }
    return true;
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

void check_food_collision()
{
    if (snake[0].x < food.x + BLOCK_SIZE && snake[0].x + BLOCK_SIZE > food.x && snake[0].y < food.y + BLOCK_SIZE && snake[0].y + BLOCK_SIZE > food.y) 
    {
        food.x = (rand() % (vinfo.xres / BLOCK_SIZE)) * BLOCK_SIZE;
        food.y = (rand() % (vinfo.yres / BLOCK_SIZE)) * BLOCK_SIZE;
        snake_length++;
        snake = realloc(snake, snake_length * sizeof(struct Point));
        snake[snake_length - 1] = snake[snake_length - 2];
    }
}

void draw_block(int x,int y,int color,int sblock_id)
{
    for(int i = 0;i<BLOCK_SIZE;i++)
    {
        for(int j=0;j<BLOCK_SIZE;j++)
        {
            int px = x+i;
            int py = y+j;
            if(px>=vinfo.xres || py>=vinfo.yres) continue;

            long location = (px * vinfo.bits_per_pixel / 8) + (py * finfo.line_length);
            if(vinfo.bits_per_pixel == 32)
            {
                *(framebuffer_data + location + 0) = color & 0xFF;         // Blue
                *(framebuffer_data + location + 1) = (color >> 8) & 0xFF;  // Green
                *(framebuffer_data + location + 2) = (color >> 16) & 0xFF; // Red
                *(framebuffer_data + location + 3) = 0;                   // Alpha
            }
        }
    }
}

void release_resources()
{    
    free(snake);
    munmap(framebuffer_data, screensize); // Unmap the framebuffer memory.
    close(framebuffer_fd); // Close the framebuffer device.
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

void render_framebuffer() {
    size_t screensize = finfo.line_length * vinfo.yres;
    pwrite(framebuffer_fd, framebuffer_data, screensize, 0);
}

