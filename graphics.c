#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pspdisplay.h>
#include <psputils.h>
#include <pspgu.h>
#include <png.h>

#include "graphics.h"
#include "framebuffer.h"

#define IS_ALPHA(color) (((color)&0xff000000)==0xff000000?0:1)
#define FRAMEBUFFER_SIZE (PSP_LINE_SIZE*SCREEN_HEIGHT*4)
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))


unsigned int __attribute__((aligned(16))) list[262144];
static int dispBufferNumber;
static int initialized = 0;


int getNextPower2(int width)
{
	int b = width;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * width) b >>= 1;
	return b;
}


Color* getVramDrawBuffer()
{
	Color* vram = (Color*) g_vram_base;
	if (dispBufferNumber == 0) vram += FRAMEBUFFER_SIZE / sizeof(Color);
	return vram;
}

Color* getVramDisplayBuffer()
{
	Color* vram = (Color*) g_vram_base;
	if (dispBufferNumber == 1) vram += FRAMEBUFFER_SIZE / sizeof(Color);
	return vram;
}



Image* loadImage(const char* filename){
	png_structp png_ptr;
	png_infop info_ptr;
	
	FILE *fp = NULL;
	fp = fopen(filename, "rb");
	if (!fp)
		return NULL;
	
	Image* img = NULL;
	img = (Image*)malloc(sizeof(Image));
	
	if (!img)
		return NULL;
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr){
		free(img);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return NULL;
	}
	
	png_init_io(png_ptr, fp);
	png_read_info(png_ptr, info_ptr);

    // Determine the PNG image dimensions and format
    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);
	
	if (width > 512 || height > 512){
		free(img);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return NULL;
	}
	
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(png_ptr);
	}
	if (bit_depth == 16) {
		png_set_strip_16(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
	}
	png_read_update_info(png_ptr, info_ptr);

	img->bit_depth = bit_depth;
	img->color_type = color_type;
	img->imageWidth = width;
	img->imageHeight = height;
	img->textureWidth = getNextPower2(width);
	img->textureHeight = getNextPower2(height);
		
	// Allocate memory for the final image data
	Color *data = (Color*)malloc(sizeof(Color) * img->textureWidth * height);
	
	if (!data){
		free(img);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return NULL;
	}
	
	img->data = data;
	img->rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // Allocate memory for the PNG image
    png_bytep *row_pointers = (png_bytep*)png_malloc(png_ptr, sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }
	
	
	// Read the PNG image data
    png_read_image(png_ptr, row_pointers);

	// Copy the PNG image data to the final image data
	for (int y = 0; y < height; y++) {
		png_bytep row = row_pointers[y];
		for (int x = 0; x < width; x++) {
			png_bytep pixel = &(row[x * 3]);
			Color agbr = (0xFF000000) | ((Color)pixel[2] << 16) | ((Color)pixel[1] << 8) | ((Color)pixel[0]);
			data[y * img->textureWidth + x] = agbr;
		}
	}

	// Free memory allocated for the PNG image
	for (int y = 0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);
	
    // Close the PNG file and clean up memory and resources
    fclose(fp);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	
	return img;
}


void blitImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy){
	if (!initialized)
		return;
	Color* vram = getVramDrawBuffer();
	sceKernelDcacheWritebackInvalidateAll();
	guStart();
	sceGuCopyImage(GU_PSM_8888, sx, sy, width, height, source->textureWidth, source->data, dx, dy, PSP_LINE_SIZE, vram);
	sceGuFinish();
	sceGuSync(0,0);
}


void freeImage(Image* image){
	free(image->data);
	free(image);
}


void clearScreen(Color color)
{
	if (!initialized) return;
	guStart();
    sceGuClearColor(color);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
	sceGuFinish();
	sceGuSync(0, 0);
}


void fillScreenRect(Color color, int x0, int y0, int width, int height)
{
	if (!initialized) return;
	int skipX = PSP_LINE_SIZE - width;
	int x, y;
	Color* data = getVramDrawBuffer() + x0 + y0 * PSP_LINE_SIZE;
	for (y = 0; y < height; y++, data += skipX) {
		for (x = 0; x < width; x++, data++) *data = color;
	}
}

void putPixelScreen(Color color, int x, int y)
{
	Color* vram = getVramDrawBuffer();
	vram[PSP_LINE_SIZE * y + x] = color;
}


Color getPixelScreen(int x, int y)
{
	Color* vram = getVramDrawBuffer();
	return vram[PSP_LINE_SIZE * y + x];
}

void flipScreen()
{
	if (!initialized) return;
	sceGuSwapBuffers();
	dispBufferNumber ^= 1;
}

static void drawLine(int x0, int y0, int x1, int y1, int color, Color* destination, int width)
{
	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;
	
	if (dy < 0) { dy = -dy;  stepy = -width; } else { stepy = width; }
	if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
	dy <<= 1;
	dx <<= 1;
	
	y0 *= width;
	y1 *= width;
	destination[x0+y0] = color;
	if (dx > dy) {
		int fraction = dy - (dx >> 1);
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy;
			destination[x0+y0] = color;
		}
	} else {
		int fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			destination[x0+y0] = color;
		}
	}
}

void drawLineScreen(int x0, int y0, int x1, int y1, Color color)
{
	drawLine(x0, y0, x1, y1, color, getVramDrawBuffer(), PSP_LINE_SIZE);
}


#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */

void initGraphics()
{
	dispBufferNumber = 0;

	sceGuInit();

	guStart();
	sceGuDrawBuffer(GU_PSM_8888, (void*)FRAMEBUFFER_SIZE, PSP_LINE_SIZE);
	sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void*)0, PSP_LINE_SIZE);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
	sceGuDepthBuffer((void*) (FRAMEBUFFER_SIZE*2), PSP_LINE_SIZE);
	sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
	sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
	sceGuDepthRange(0xc350, 0x2710);
	sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuAlphaFunc(GU_GREATER, 0, 0xff);
	sceGuEnable(GU_ALPHA_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuTexMode(GU_PSM_8888, 0, 0, 0);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	sceGuAmbientColor(0xffffffff);
	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
	initialized = 1;
}

void disableGraphics()
{
	initialized = 0;
}

void guStart()
{
	sceGuStart(GU_DIRECT, list);
}
