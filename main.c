/*

	Made by pyroesp
	
	Thanks to:
		kevinr for gfx
		raytwo for images
		Benhur for intraFont
		DaveGamble for cJSON (v1.7.15)

*/

#include <pspkernel.h>
#include <psptypes.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspgu.h>

#include <stdio.h>
#include <stdlib.h>

#include <intraFont.h>

#include "resource.h"
#include "graphics.h"
#include "post_it.h"
#include "cJSON/cJSON.h"


PSP_MODULE_INFO("PSP Post It - Editor", 0, 1, 1);

static int running = 1;

int exitCallback(int arg1, int arg2, void *common) {
	running = 0;
	return 0;
}

int CallbackThread(SceSize args, void *argp) {
	int cbid = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

int SetupCallbacks(void) {
	int thid = sceKernelCreateThread("CallbackThread", CallbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
	if (thid >= 0) 
		sceKernelStartThread(thid, 0, 0);
	return thid;
}


extern void waitOnPress(void);


int main (int argc, char *argv[]){
	// init psp stuff
	pspDebugScreenInit();
	SetupCallbacks();
	
	// make default image
	Image *def = NULL;
	def = gfx_createDefaultImage();
	// load main
	Image *main = NULL;
	main = gfx_loadImage(RES_IMAGE_PATH RES_MAIN);
	
	if(!main)
		main = def;

	
	// cJSON stuff
	char *v = NULL;
	int size = 0;
	FILE *fp;
	cJSON *json = NULL;
	
	fp = fopen(POST_IT_PATH POST_IT_FILE, "r");
	if (fp){
		while(fgetc(fp) != EOF);
		size = ftell(fp) + 1;
		v = (char*)malloc(sizeof(char) * size);
		if (v){
			memset(v, 0, size);
			rewind(fp);
			fread(v, sizeof(char), size, fp);
			
			cJSON_InitHooks(NULL);
			json = cJSON_Parse(v);
		}
	
		fclose(fp);
	}
	
	// post it stuff
	PostIt *post;
	post = post_readPostIt(json);
	
	// button stuff
	SceCtrlData pad, oldpad;
	sceCtrlReadBufferPositive(&oldpad, 1);
	
	// init intrafont
	if (!intraFontInit())
		sceKernelExitGame();

	// load fonts
	intraFont *ltn[2];
	ltn[0] = intraFontLoad("flash0:/font/ltn7.pgf", 0);
	ltn[1] = intraFontLoad("flash0:/font/ltn8.pgf", 0);

	if (!ltn[0] || !ltn[1])
		sceKernelExitGame();


	// other
	int x = 0;
	int option = 0;
	int animation = 0;

	// init gfx
	gfx_initGraphics();
	
	/* Main loop */
	while (running){
		// clear screen
		gfx_clearScreen(WHITE);
		
		// read buttons
		sceCtrlReadBufferPositive(&pad, 1);
		
		switch (animation){
			case 0:
				// check if up/down is pressed
				if (!(oldpad.Buttons & PSP_CTRL_UP) && (pad.Buttons & PSP_CTRL_UP))
					--option;
				else if (!(oldpad.Buttons & PSP_CTRL_DOWN) && (pad.Buttons & PSP_CTRL_DOWN))
					++option;
				
				// set animation
				if (option < 0){
					animation = 1;
					option = 0;
				}
				else if (option >= sizeof(menu)/sizeof(menu[0])){
					animation = 2;
					option = sizeof(menu)/sizeof(menu[0]) - 1;
				}
				break;
			case 1: // go from lowest to highest option
				if (option < sizeof(menu)/sizeof(menu[0]) - 1)
					++option;
				else
					animation = 0;
				sceKernelDelayThread(75000);
				break;
			case 2: // go from highest to lowest option
				if (option > 0)
					--option;
				else
					animation = 0;
				sceKernelDelayThread(75000);
				break;
			default: // for all other values, reset
				animation = 0;
				break;
		}
		
		// check if X is pressed
		if (!(oldpad.Buttons & PSP_CTRL_CROSS) && (pad.Buttons & PSP_CTRL_CROSS)){
			switch (option){
				case VIEW:
					break;
				case ADD:
					break;
				case EDIT:
					break;
				case REMOVE:
					break;
				case SAVE:
					break;
				// exit main loop if X is pressed on Exit option	
				case EXIT:
					running = 0;
					break;
				case CREDITS:
					break;
				default: // for all other values, reset
					option = 0;
					break;
			}
		}
		
		// show main image
		if (main)
			gfx_blitImageToScreen(0, 0, main->imageWidth, main->imageHeight, main, 0, 0);

		gfx_guStart();
		
		// title
		intraFontSetStyle(ltn[0], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
		intraFontPrint(ltn[0], 70, 18, "PSP Post It - Editor:");
		gfx_drawLineScreen(70, 26, 245, 26, BLACK);
		
		// menu
		for (int i = 0; i < sizeof(menu)/sizeof(menu[0]); i++){
			if (i == option){
				gfx_drawLineScreen(100, 26, 100, 66 + (20 * i), BLACK);
				gfx_drawLineScreen(100, 66 + (20 * i), 125, 66 + (20 * i), BLACK);
				gfx_fillScreenRect(BLACK, 124, 65 + (20 * i), 3, 3);
				gfx_fillScreenRect(BLACK, 99, 65 + (20 * i), 3, 3);
				
				intraFontSetStyle(ltn[1], 0.9f, RED, DARKGRAY, 0.0f, 0);
				x = 140;
			}else{
				intraFontSetStyle(ltn[1], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
				x = 120;
			}

			intraFontPrintf(ltn[1], x, 70 + (20 * i), "%s", menu[i]);
		}
		
		// debug the post structure
		if (post){
			for (int i = 0; i < post->size; i++){
				if (&post->event[i]){
					intraFontSetStyle(ltn[1], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
					intraFontPrintf(ltn[1], 10, 200 + 10 * i, "%s", post->event[i].string);
				}
			}
		}
		
		oldpad = pad;
		
		sceGuFinish();
		sceGuSync(0,0);
		
		sceDisplayWaitVblankStart();
		gfx_flipScreen();
	}
	
	/* Cleanup */
	if (v)
		free(v);
	if(json)
		cJSON_Delete(json);
	if (post)
		post_free(post);
	if(def)
		gfx_freeImage(def);
	if (main)
		gfx_freeImage(main);
	intraFontUnload(ltn[0]);
	intraFontUnload(ltn[1]);
	
	/* Shutdown */
	gfx_disableGraphics();
	intraFontShutdown();
	sceKernelExitGame();
	
	return 0;
}