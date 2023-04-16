/*

	Made by pyroesp
	
	Thanks to:
		kevinr for gfx
		raytwo for images
		Benhur for intraFont

*/

#include <pspkernel.h>
#include <psptypes.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspgu.h>

#include <intraFont.h>


#include "graphics.h"


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

// use enum for quick constants
enum Menu{
	VIEW,
	EDIT,
	ADD,
	REMOVE,
	SAVE,
	EXIT,
	CREDITS	
};


int main (int argc, char *argv[]){
	// make default image
	Image *def = NULL;
	def = gfx_createDefaultImage();
	
	int x = 0;
	int option = 0;
	int animation = 0;
	char menu[7][20] = {
		"View notes",
		"Edit notes",
		"Add notes",
		"Remove notes",
		"Save notes",
		"Exit",
		"Credits"
	};
	
	// button stuff
	SceCtrlData pad, oldpad;
	sceCtrlReadBufferPositive(&oldpad, 1);
	
	// init psp stuff
	pspDebugScreenInit();
	SetupCallbacks();

	// init intrafont
	if (!intraFontInit())
		sceKernelExitGame();

	// load fonts
	intraFont *ltn[2];
	ltn[0] = intraFontLoad("flash0:/font/ltn7.pgf", 0);
	ltn[1] = intraFontLoad("flash0:/font/ltn8.pgf", 0);

	if (!ltn[0] || !ltn[1])
		sceKernelExitGame();

	// init gfx
	gfx_initGraphics();
	
	/* Main loop */
	while (running){
		//if (def)
		//	gfx_blitImageToScreen(0, 0, def->imageWidth, def->imageHeight, def, 0, 0);
		
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
				sceKernelDelayThread(90000);
				break;
			case 2: // go from highest to lowest option
				if (option > 0)
					--option;
				else
					animation = 0;
				sceKernelDelayThread(90000);
				break;
			default:
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
			}
		}
		
		gfx_clearScreen(WHITE);
		gfx_guStart();

		// title
		intraFontSetStyle(ltn[0], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
		intraFontPrint(ltn[0], 70, 18, "PSP Post It - Editor:");
		gfx_drawLineScreen(70, 26, 230, 26, BLACK);
		
		// menu
		for (int i = 0; i < sizeof(menu)/sizeof(menu[0]); i++){
			if (i == option){
				intraFontSetStyle(ltn[1], 0.9f, RED, DARKGRAY, 0.0f, 0);

				gfx_drawLineScreen(100, 26, 100, 66 + (20 * i), BLACK);
				gfx_drawLineScreen(100, 66 + (20 * i), 125, 66 + (20 * i), BLACK);
				gfx_fillScreenRect(BLACK, 124, 65 + (20 * i), 3, 3);
				gfx_fillScreenRect(BLACK, 99, 65 + (20 * i), 3, 3);
				
				x = 140;
			}else{
				intraFontSetStyle(ltn[1], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
				x = 120;
			}

			intraFontPrintf(ltn[1], x, 70 + (20 * i), "%s", menu[i]);
		}
		
		oldpad = pad;
		
		sceGuFinish();
		sceGuSync(0,0);
		
		sceDisplayWaitVblankStart();
		gfx_flipScreen();
	}
	
	/* Cleanup */
	gfx_freeImage(def);
	intraFontUnload(ltn[0]);
	intraFontUnload(ltn[1]);
	
	/* Shutdown */
	gfx_disableGraphics();
	intraFontShutdown();
	sceKernelExitGame();
	
	return 0;
}