#include <linux/fb.h>  // Add this to define fb_var_screeninfo and fb_fix_screeninfo

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

extern int framebuffer_fd;
extern char *framebuffer_data;
extern struct fb_var_screeninfo vinfo;
extern struct fb_fix_screeninfo finfo;

void game_routine();
int initialize_fb();
void draw_block(int x, int y, int color, int sblock_id);
void release_resources();
void render_framebuffer();

#endif
