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



void waitOnPress(void){
	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);
	
	while(pad.Buttons & PSP_CTRL_CIRCLE){
		sceCtrlReadBufferPositive(&pad, 1);
		sceKernelDelayThread(100000);
	}
	while(!(pad.Buttons & PSP_CTRL_CIRCLE)){
		sceCtrlReadBufferPositive(&pad, 1);
		sceKernelDelayThread(100000);
	}
	while(pad.Buttons & PSP_CTRL_CIRCLE){
		sceCtrlReadBufferPositive(&pad, 1);
		sceKernelDelayThread(100000);
	}
}



int main (int argc, char *argv[]){
	// init psp stuff
	pspDebugScreenInit();
	SetupCallbacks();
	
	// make default image
	Image *def = NULL;
	def = gfx_createDefaultImage();
	// load main and menu bg
	Image *main = NULL;
	main = gfx_loadImage(RES_IMAGE_PATH RES_MAIN);
	Image **menu_bg = NULL;
	menu_bg = (Image**)malloc(sizeof(Image*) * RES_MENU_OPTIONS);
	menu_bg[0] = gfx_loadImage(RES_IMAGE_PATH RES_VIEW);
	menu_bg[1] = gfx_loadImage(RES_IMAGE_PATH RES_EDIT);
	menu_bg[2] = gfx_loadImage(RES_IMAGE_PATH RES_ADD);
	menu_bg[3] = gfx_loadImage(RES_IMAGE_PATH RES_REMOVE);
	menu_bg[4] = gfx_loadImage(RES_IMAGE_PATH RES_SAVE);
	menu_bg[5] = gfx_loadImage(RES_IMAGE_PATH RES_CREDIT);
	menu_bg[6] = gfx_loadImage(RES_IMAGE_PATH RES_EXIT);
	
	if(!main)
		main = def;
	for (int i = 0; i < RES_MENU_OPTIONS; i++)
		if(!menu_bg[i])
			menu_bg[i] = def;

	
	// post it stuff
	PostIt *post = NULL;	
	post = post_initPostIt();
	post_readJson(post, POST_IT_PATH_REL POST_IT_FILE);
	post_convertJsonToPostIt(post);
	
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
	int selected = -1;
	int animation = 0;
	int edit_event = 0;
	int do_once = 0;

	// init gfx
	gfx_initGraphics();
	
	/* Main loop */
	while (running){
		// clear screen
		gfx_clearScreen(WHITE);
		
		// read buttons
		sceCtrlReadBufferPositive(&pad, 1);

		
		// show bg
		if (selected == -1){
			if (main)
				gfx_blitImageToScreen(0, 0, main->imageWidth, main->imageHeight, main, 0, 0);
		}else{
			if (menu_bg[selected])
				gfx_blitImageToScreen(0, 0, menu_bg[selected]->imageWidth, menu_bg[selected]->imageHeight, menu_bg[selected], 0, 0);
		}

		// check if O is pressed to return to main menu
		if (!(oldpad.Buttons & PSP_CTRL_CIRCLE) && (pad.Buttons & PSP_CTRL_CIRCLE)){
			selected = -1;
		}

		
		// start gfx
		gfx_guStart();	
		
		
		// title
		intraFontSetStyle(ltn[0], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
		intraFontPrint(ltn[0], 70, 18, "PSP Post It - Editor:");
		gfx_drawLineScreen(70, 26, 245, 26, BLACK);
		
		// show different menu based of selected variable
		switch (selected){
			case -1: // show main menu
				do_once = 0;
				edit_event = 0;
				switch (animation){
					case 0:
						// check if X is pressed
						if (!(oldpad.Buttons & PSP_CTRL_CROSS) && (pad.Buttons & PSP_CTRL_CROSS)){
							selected = option;
						}
						
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
						else if (option >= sizeof(main_menu)/sizeof(main_menu[0])){
							animation = 2;
							option = sizeof(main_menu)/sizeof(main_menu[0]) - 1;
						}
						break;
					case 1: // go from lowest to highest option
						if (option < sizeof(main_menu)/sizeof(main_menu[0]) - 1)
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
				
				// main_menu
				for (int i = 0; i < sizeof(main_menu)/sizeof(main_menu[0]); i++){
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

					intraFontPrintf(ltn[1], x, 70 + (20 * i), "%s", main_menu[i]);
				}
				break;
			case MM_EDIT:
				// check if up/down is pressed
				if (!(oldpad.Buttons & PSP_CTRL_UP) && (pad.Buttons & PSP_CTRL_UP))
					--edit_event;
				else if (!(oldpad.Buttons & PSP_CTRL_DOWN) && (pad.Buttons & PSP_CTRL_DOWN))
					++edit_event;
				
				if (edit_event < 0)
					edit_event = 0;
				else if (edit_event > (post->size - 1))
					edit_event = post->size - 1;
				
				intraFontSetStyle(ltn[1], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
				intraFontPrintf(ltn[1], 20, 40 + 45 * edit_event, ">", edit_event);
			
			case MM_VIEW:	
				// view the post structure
				if (post){
					if (pad.Buttons & PSP_CTRL_LTRIGGER || selected == MM_EDIT)
						post_displayEvents(30, 40, post, ltn[1]);
					else
						post_displayJSON(30, 40, post, ltn[1]);
				}
				break;
			case MM_ADD:
				// add event to post
				if(post && !do_once){
					intraFontSetStyle(ltn[1], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
					int add_event = post_addEvent(post);
					if (add_event){
						// test values to add to json
						post_addMessage(post, "Hello World");
						pspTime t;
						sceRtcGetCurrentClockLocalTime(&t);
						post_addDateTime(post, t);
						post_addDatePart(post, YEAR);
						post_addRepeat(post, 1);
					}
					
					post_convertJsonToPostIt(post);
					do_once = 1;
				}
				
				if (do_once)
					intraFontPrintf(ltn[1], 30, 40, "Event added.\nPress O to go back...");
				break;
			case MM_REMOVE:
				break;
			case MM_SAVE:
				if (!do_once){
					FILE *fp;
					fp = fopen(POST_IT_PATH_REL POST_IT_FILE, "w");
					if (fp){
						char *buffer = cJSON_Print(post->json);
						if(buffer){
							fwrite(buffer, sizeof(char), strlen(buffer) + 1, fp);
							free(buffer);
						}
						fclose(fp);
					}
					do_once = 1;
				}
				
				if (do_once){
					intraFontPrintf(ltn[1], 30, 40, "JSON saved.\nPress O to go back...");
				}
				break;
			case MM_CREDITS:
				break;
			// exit main loop if X is pressed on Exit option	
			case MM_EXIT:
				running = 0;
				break;
			default: // for all other values, reset
				option = 0;
				selected = -1;
				break;
		}
		
		oldpad = pad;
		
		sceGuFinish();
		sceGuSync(0,0);
		
		sceDisplayWaitVblankStart();
		gfx_flipScreen();
	}
	
	/* Cleanup */
	if (post) // cleans up json too
		post_freeAll(post);
	if(def)
		gfx_freeImage(def);
	if (main)
		gfx_freeImage(main);
	for (int i = 0; i < RES_MENU_OPTIONS; i++)
		if(menu_bg[i])
			gfx_freeImage(menu_bg[i]);
	intraFontUnload(ltn[0]);
	intraFontUnload(ltn[1]);
	
	/* Shutdown */
	gfx_disableGraphics();
	intraFontShutdown();
	sceKernelExitGame();
	
	return 0;
}