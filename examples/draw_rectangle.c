#include <fcntl.h> // Header that defines constants and functions for file control operations, e.g., O_RDWR.
#include <linux/fb.h> // Provides structure definitions and constants related to the framebuffer device.
#include <sys/ioctl.h> // Contains definitions for the ioctl function used to manipulate devices or retrieve device-specific information.
#include <sys/mman.h> // Contains definitions for memory mapping functions.
#include <stdio.h> // Standard input-output library.
#include <stdlib.h> // Standard library for memory allocation, process control, and other utilities.
#include <unistd.h> // Defines constants and types related to system calls such as file handling and process control.
#include <string.h> // Contains string manipulation functions.

#define RECT_WIDTH 200 // Defines the width of the rectangle to be drawn.
#define RECT_HEIGHT 100 // Defines the height of the rectangle to be drawn.
#define COLOR 0x00FF00 // Defines the color of the rectangle to be drawn (green in hex).

int main() {

    int fb_fd = open("/dev/fb0", O_RDWR); // Opens the framebuffer device in read-write mode.

    if (fb_fd == -1) {  // Checks if the framebuffer device was successfully opened.
        perror("Error opening framebuffer device"); // Prints an error message if the device fails to open.
        return 1;
    }

    struct fb_var_screeninfo vinfo; // Structure to store variable screen information (e.g., resolution, bits per pixel).
    struct fb_fix_screeninfo finfo; // Structure to store fixed screen information (e.g., line length, memory start address).

    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) == -1) // Retrieves fixed screen information and stores it in finfo.
    { 
        perror("Error reading fixed information"); // Prints an error message if the operation fails.
        close(fb_fd); // Closes the framebuffer device.
        return 1;
    }

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) // Retrieves variable screen information and stores it in vinfo.
    { 
        perror("Error reading variable information"); // Prints an error message if the operation fails.
        close(fb_fd); // Closes the framebuffer device.
        return 1;
    }

    long screensize = vinfo.yres_virtual * finfo.line_length; // Calculates the total screen size in bytes.
    char *fb_data = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0); // Maps the framebuffer device to program memory.

    if (fb_data == MAP_FAILED) // Checks if the memory mapping failed.
    {
        perror("Error mapping framebuffer device to memory"); // Prints an error message if mapping fails.
        close(fb_fd); // Closes the framebuffer device.
        return 1;
    }

    // Calculate the center of the screen
    int center_x = vinfo.xres / 2; // Horizontal center of the screen.
    int center_y = vinfo.yres / 2; // Vertical center of the screen.
    int start_x = center_x - RECT_WIDTH / 2; // Top-left corner of the rectangle (X-coordinate).
    int start_y = center_y - RECT_HEIGHT / 2; // Top-left corner of the rectangle (Y-coordinate).

    // Draw the rectangle
    for (int y = 0; y < RECT_HEIGHT; y++) // Loop through the rectangle's height (rows).
    {
        for (int x = 0; x < RECT_WIDTH; x++) // Loop through the rectangle's width (columns).
        {
            // The screen is logically represented as a 2D grid of pixels, where each pixel has its own color value.
            // However, in the framebuffer memory, this 2D grid is stored as a continuous (1D) linear array of bytes.
            // 
            // Each row of the screen is mapped to a corresponding segment in the linear memory.
            // But to optimize memory access and align data with hardware requirements, there might be extra unused bytes 
            // (known as padding) at the end of each row in memory.
            //
            // To calculate the correct memory position (offset) of any pixel, we need to:
            //
            // 1. Determine the starting position of the desired row:
            //    (start_y + y) gives the row index (relative to the top-left corner of the rectangle).
            //    We multiply this row index by `finfo.line_length` to account for the full width of the row,
            //    including any padding bytes added at the end of the row for alignment.
            //
            // 2. Determine the position within the row for the specific pixel:
            //    (start_x + x) gives the column index (relative to the left edge of the rectangle).
            //    We then multiply this by the number of bytes each pixel occupies, calculated as:
            //    (vinfo.bits_per_pixel / 8) (since there are 8 bits in a byte).
            //
            // Combining these two calculations gives the total offset of the pixel in the linear framebuffer:
            //    (start_y + y) * finfo.line_length + (start_x + x) * (vinfo.bits_per_pixel / 8)
            //
            // This ensures we correctly account for both row padding and pixel size to locate and modify the desired pixel.
            int pixel_offset = (start_y + y) * finfo.line_length + (start_x + x) * (vinfo.bits_per_pixel / 8);

            if (vinfo.bits_per_pixel == 16) 
            {
                *((unsigned short *)(fb_data + pixel_offset)) = COLOR; // Set the pixel color for 16-bit mode (RGB565 format).
            } 
            else if (vinfo.bits_per_pixel == 32) 
            {
                *((unsigned int *)(fb_data + pixel_offset)) = 0x00FF00; // Set the pixel color for 32-bit mode (ARGB format).
            } 
            else 
            {
                fprintf(stderr, "Unsupported bits per pixel: %d\n", vinfo.bits_per_pixel); // Print error for unsupported formats.
                munmap(fb_data, screensize); // Unmap the framebuffer memory.
                close(fb_fd); // Close the framebuffer device.
                return 1;
            }
        }
    }

    // Clean up
    munmap(fb_data, screensize); // Unmap the framebuffer memory.
    close(fb_fd); // Close the framebuffer device.
    return 0;
}
