#include <fcntl.h> // Header that defines constants and functions for file control operations. For example O_RDWR.
#include <linux/fb.h> // This header probides structure definitions and constants related to the framebuffer device.
#include <sys/ioctl.h> // Contains definitions for ioctl function that used for manipulate the device or get the device-spesific information.
#include <sys/mman.h> // Contains definitions for memory management functions.
#include <stdio.h> // Standart input-output library.
#include <stdlib.h> // Standard library for memory allocation, process control, and other utilities.
#include <unistd.h> // Defines constants and tyğes related to system calls such as file handling and process control.
#include <string.h> // Contains string manipulation functions.

#define RECT_WIDTH 200 // Define width of rectangle that will be draw.
#define RECT_HEIGHT 100 // Define height of rectangle that will be draw.
#define COLOR 0x00FF00 // Color of rectangle will be draw.

int main() {

    int fb_fd = open("/dev/fb0", O_RDWR); // Open framebuffer device in read-write mode.

    if (fb_fd == -1) {  // Check if framebuffer device is successfully opened.
        perror("Error opening framebuffer device"); // Print error message if occurs.
        return 1;
    }

    struct fb_var_screeninfo vinfo; // Create struct that will store variable info of screen.
    struct fb_fix_screeninfo finfo; // Create struct that will store fixed info of screen.

    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) == -1) // Get fixed info and store inside finfo if any eror occurs print message.
    { 
        perror("Error reading fixed information"); // Print error message.
        close(fb_fd); // Close framebuffer device.
        return 1;
    }

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) // Get variable info and store inside vinfo if any eror occurs print message.
    { 
        perror("Error reading variable information"); // Print error message.
        close(fb_fd); // Close framebuffer device.
        return 1;
    }

    long screensize = vinfo.yres_virtual * finfo.line_length; // Calculate screen size.
    char *fb_data = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0); // Map framebuffer device to program memory.

    if (fb_data == MAP_FAILED) // Check if mapping failed.
    {
        perror("Error mapping framebuffer device to memory"); // Print error message.
        close(fb_fd); // Close framebuffer device.
        return 1;
    }

    // Calculate the center of the screen
    int center_x = vinfo.xres / 2;
    int center_y = vinfo.yres / 2;
    int start_x = center_x - RECT_WIDTH / 2;
    int start_y = center_y - RECT_HEIGHT / 2;

    // Draw the rectangle
    for (int y = 0; y < RECT_HEIGHT; y++) {
        for (int x = 0; x < RECT_WIDTH; x++) {
            int pixel_offset = (start_y + y) * finfo.line_length + (start_x + x) * (vinfo.bits_per_pixel / 8);

            if (vinfo.bits_per_pixel == 16) {
                *((unsigned short *)(fb_data + pixel_offset)) = COLOR; // 16-bit color (RGB565)
            } else if (vinfo.bits_per_pixel == 32) {
                *((unsigned int *)(fb_data + pixel_offset)) = 0x00FF00; // 32-bit color (ARGB)
            } else {
                fprintf(stderr, "Unsupported bits per pixel: %d\n", vinfo.bits_per_pixel);
                munmap(fb_data, screensize);
                close(fb_fd);
                return 1;
            }
        }
    }

    // Clean up
    munmap(fb_data, screensize);
    close(fb_fd);
    return 0;
}
