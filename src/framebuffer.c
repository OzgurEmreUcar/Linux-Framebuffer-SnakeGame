#include "framebuffer.h"
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "snake.h"

int framebuffer_fd = 0;
char *framebuffer_data = NULL;

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long screensize;

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
    return 0;
}

void draw_block(int x, int y, int color, int sblock_id)
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
    munmap(framebuffer_data, screensize);
    close(framebuffer_fd);
}

void render_framebuffer() {
    size_t screensize = finfo.line_length * vinfo.yres;
    pwrite(framebuffer_fd, framebuffer_data, screensize, 0);
}
