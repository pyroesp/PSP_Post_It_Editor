/*
	Made by kevinr
	Modified by pyroesp
*/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <psptypes.h>

#define	PSP_LINE_SIZE 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

#define A(color) ((u8)(color >> 24 & 0xFF))
#define B(color) ((u8)(color >> 16 & 0xFF))
#define G(color) ((u8)(color >> 8 & 0xFF))
#define R(color) ((u8)(color & 0xFF))


#define PURPLE		((u32)0xFFFF00FF)
#define RED			((u32)0xFF0000FF)
#define GREEN		((u32)0xFF00FF00)
#define BLUE		((u32)0xFFFF0000)
#define LIGHTCYAN 	((u32)0xFFE8CE7A)
#define CYAN 		((u32)0xFFE8BD3C)
#define DARKCYAN 	((u32)0xFF9D7B13)
#define YELLOW 		((u32)0xFF00F3F3)
#define WHITE 		((u32)0xFFFFFFFF)
#define LIGHTGRAY 	((u32)0xFFBFBFBF)
#define GRAY 		((u32)0xFF7F7F7F)
#define DARKGRAY 	((u32)0xFF3F3F3F)
#define BLACK 		((u32)0xFF000000)

#define ALPHA_0		((u32)0x00FFFFFF)
#define ALPHA_25	((u32)0x40FFFFFF)
#define ALPHA_50	((u32)0x80FFFFFF)
#define ALPHA_75	((u32)0xC0FFFFFF)
#define ALPHA_100	((u32)0xFFFFFFFF)


typedef struct{
	int x, y;
	int w, h;
}Rect;

typedef struct{
	int textureWidth;	// the real width of data, 2^n with n>=0
	int textureHeight;	// the real height of data, 2^n with n>=0
	int imageWidth;		// the image width
	int imageHeight;	// the image height
	int bit_depth;		// png bit depth (for debug)
	int color_type;		// png color type (for debug)
	int rowbytes;		// png rowbytes (for debug)
	u32* data;			// pixel data
}Image;


Image* gfx_loadImage(const char* filename);

Image* gfx_createDefaultImage(void);

void gfx_blitImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy);

void gfx_freeImage(Image* image);

void gfx_clearScreen(u32 color);

void gfx_fillScreenRect(u32 color, int x0, int y0, int width, int height);

void gfx_putPixelScreen(u32 color, int x, int y);

u32 gfx_getPixelScreen(int x, int y);

void gfx_flipScreen();

void gfx_initGraphics();

void gfx_disableGraphics();

void gfx_drawLineScreen(int x0, int y0, int x1, int y1, u32 color);

u32* getVramDrawBuffer();

u32* getVramDisplayBuffer();

void gfx_guStart();

#endif
