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
#include "osk.h"
#include "gui.h"


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
	menu_bg = (Image**)malloc(sizeof(Image*) * (RES_MENU_OPTIONS - 1));
	menu_bg[0] = gfx_loadImage(RES_IMAGE_PATH RES_VIEW);
	menu_bg[1] = gfx_loadImage(RES_IMAGE_PATH RES_EDIT);
	menu_bg[2] = gfx_loadImage(RES_IMAGE_PATH RES_ADD);
	menu_bg[3] = gfx_loadImage(RES_IMAGE_PATH RES_REMOVE);
	menu_bg[4] = gfx_loadImage(RES_IMAGE_PATH RES_SAVE);
	menu_bg[5] = gfx_loadImage(RES_IMAGE_PATH RES_CREDIT);
	
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
	
	//keyboard stuff
	Osk kb;
	unsigned short *desc = (unsigned short*)malloc(sizeof(unsigned short) * OSK_STRING_SIZE);
	unsigned short *intext = (unsigned short*)malloc(sizeof(unsigned short) * OSK_STRING_SIZE);
	unsigned short *outtext = (unsigned short*)malloc(sizeof(unsigned short) * OSK_STRING_SIZE);
	memset(desc, 0, sizeof(unsigned short) * OSK_STRING_SIZE);
	memset(intext, 0, sizeof(unsigned short) * OSK_STRING_SIZE);
	memset(outtext, 0, sizeof(unsigned short) * OSK_STRING_SIZE);
	osk_init(&kb, desc, intext, outtext);
	
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
	int do_once = 0;
	int option = 0;
	int selected = -1;
	int animation = 0;
	int event_start = 0;
	int add_event = 0;
	int edit_event = 0;
	int remove_event = 0;
	AddSteps step = Add_None;

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
		}else if (selected != MM_EXIT){
			if (menu_bg[selected])
				gfx_blitImageToScreen(0, 0, menu_bg[selected]->imageWidth, menu_bg[selected]->imageHeight, menu_bg[selected], 0, 0);
		}
		
		// start gfx
		gfx_guStart();	
		
		// title
		gui_printTitle("PSP Post It - Editor:", ltn[0]);
		
		// show different menu based of selected variable
		switch (selected){
			case -1: // show main menu
				do_once = 0;
				edit_event = 0;
				add_event = 0;
				remove_event = 0;
				switch (animation){
					case 0:
						// check if X is pressed
						if (!(oldpad.Buttons & PSP_CTRL_CROSS) && (pad.Buttons & PSP_CTRL_CROSS)){
							selected = option;
						}
						// check if O is pressed to return to main menu
						if (!(oldpad.Buttons & PSP_CTRL_CIRCLE) && (pad.Buttons & PSP_CTRL_CIRCLE)){
							selected = -1;
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
				gui_mainMenu(&main_menu[0][0], RES_MENU_OPTIONS, RES_MENU_SIZE, ltn[1], option);
				break;
			case MM_VIEW:
				// check if O is pressed to return to main menu
				if (!(oldpad.Buttons & PSP_CTRL_CIRCLE) && (pad.Buttons & PSP_CTRL_CIRCLE)){
					selected = -1;
				}	
				// view the post structure
				if (post){
					// check if up/down is pressed
					if (!(oldpad.Buttons & PSP_CTRL_UP) && (pad.Buttons & PSP_CTRL_UP))
						event_start -= POST_IT_ON_SCREEN;
					else if (!(oldpad.Buttons & PSP_CTRL_DOWN) && (pad.Buttons & PSP_CTRL_DOWN))
						event_start += POST_IT_ON_SCREEN;
					
					if (event_start < 0)
						event_start = 0;
					else if (event_start >= post->size)
						event_start = ((post->size - 1) / POST_IT_ON_SCREEN) * POST_IT_ON_SCREEN;
					
					// show scroll bar
					gui_scrollBar(post->size, event_start);
					
					post_displayEvents(30, 40, post, ltn[1], event_start, POST_IT_ON_SCREEN);
				}
				break;
			case MM_EDIT:
				// check if O is pressed to return to main menu
				if (!(oldpad.Buttons & PSP_CTRL_CIRCLE) && (pad.Buttons & PSP_CTRL_CIRCLE)){
					selected = -1;
				}
				// check if up/down is pressed
				if (!(oldpad.Buttons & PSP_CTRL_UP) && (pad.Buttons & PSP_CTRL_UP))
					--edit_event;
				else if (!(oldpad.Buttons & PSP_CTRL_DOWN) && (pad.Buttons & PSP_CTRL_DOWN))
					++edit_event;
				
				if (edit_event < 0)
					edit_event = 0;
				else if (edit_event >= post->size)
					edit_event = post->size - 1;
				
				// show scroll bar
				gui_scrollBar(post->size, edit_event);
				
				event_start = POST_IT_ON_SCREEN * (edit_event / POST_IT_ON_SCREEN);
				
				intraFontSetStyle(ltn[1], 0.9f, BLACK, DARKGRAY & ALPHA_50, 0.0f, 0);
				intraFontPrintf(ltn[1], 30, 40 + 45 * (edit_event - event_start), ">");
				post_displayEvents(40, 40, post, ltn[1], event_start, POST_IT_ON_SCREEN);
				break;
			case MM_ADD:
				// add event to post
				if(post){
					if (!do_once){
						add_event = post_addEvent(post);
						do_once = 1;
						step = Add_Message;
					}else{
						gfx_fillScreenRect(LIGHTGRAY, SCREEN_WIDTH / 2, 220, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 220);
						if (!add_event){
							// check if O is pressed to return to main menu
							if (!(oldpad.Buttons & PSP_CTRL_CIRCLE) && (pad.Buttons & PSP_CTRL_CIRCLE)){
								selected = -1;
							}
							intraFontSetStyle(ltn[1], 0.9f, BLACK, DARKGRAY & ALPHA_50, 0.0f, 0);
							intraFontPrintf(ltn[1], 30, 40, "Event added.\nPress O to go back...");
						}
					}
				}
				break;
			case MM_REMOVE:
				// check if O is pressed to return to main menu
				if (!(oldpad.Buttons & PSP_CTRL_CIRCLE) && (pad.Buttons & PSP_CTRL_CIRCLE)){
					selected = -1;
				}
				// check if X is pressed
				if (!(oldpad.Buttons & PSP_CTRL_CROSS) && (pad.Buttons & PSP_CTRL_CROSS)){
					if (post_removeEvent(post, remove_event)){
						post_convertJsonToPostIt(post);
					}
				}
				
				// check if up/down is pressed
				if (!(oldpad.Buttons & PSP_CTRL_UP) && (pad.Buttons & PSP_CTRL_UP))
					--remove_event;
				else if (!(oldpad.Buttons & PSP_CTRL_DOWN) && (pad.Buttons & PSP_CTRL_DOWN))
					++remove_event;
				
				if (remove_event < 0)
					remove_event = 0;
				else if (remove_event >= post->size)
					remove_event = post->size - 1;
				
				// show scroll bar
				gui_scrollBar(post->size, remove_event);
				
				event_start = POST_IT_ON_SCREEN * (remove_event / POST_IT_ON_SCREEN);
				
				intraFontSetStyle(ltn[1], 0.9f, BLACK, DARKGRAY & ALPHA_50, 0.0f, 0);
				intraFontPrintf(ltn[1], 30, 40 + 45 * (remove_event - event_start), ">");
				post_displayEvents(40, 40, post, ltn[1], event_start, POST_IT_ON_SCREEN);
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
					// check if O is pressed to return to main menu
					if (!(oldpad.Buttons & PSP_CTRL_CIRCLE) && (pad.Buttons & PSP_CTRL_CIRCLE)){
						selected = -1;
					}
					intraFontSetStyle(ltn[1], 0.9f, BLACK, DARKGRAY & ALPHA_50, 0.0f, 0);
					intraFontPrintf(ltn[1], 30, 40, "JSON saved.\nPress O to go back...");
				}
				break;
			case MM_CREDITS:
				// check if O is pressed to return to main menu
				if (!(oldpad.Buttons & PSP_CTRL_CIRCLE) && (pad.Buttons & PSP_CTRL_CIRCLE)){
					selected = -1;
				}
				
				intraFontSetStyle(ltn[1], 0.9f, BLACK, DARKGRAY & ALPHA_50, 0.0f, 0);
				intraFontPrintf(ltn[1], 30, 40, credits);
				break;
			// exit main loop if X is pressed on Exit option	
			case MM_EXIT:
				running = 0;
				break;
			default: // for all other unknown values, reset
				option = 0;
				selected = -1;
				break;
		}
		
		oldpad = pad;
		
		sceGuFinish();
		sceGuSync(0,0);
		
		
		// OSK stuff
		switch (selected){
			case MM_ADD:
				// add event to post
				if (add_event){
					char *ascii = (char*)malloc(sizeof(char) * OSK_STRING_SIZE);
					unsigned short *uni1 = (unsigned short*)malloc(sizeof(unsigned short) * OSK_STRING_SIZE);
					unsigned short *uni2 = (unsigned short*)malloc(sizeof(unsigned short) * OSK_STRING_SIZE);
					switch (step){
						case Add_Message:	
							osk_convertCharToUnsignedShort(POST_IT_JSON_MESSAGE, uni1);
							osk_updateOskParam(&kb, uni1, NULL, NULL);
							break;
						case Add_Datetime:
							pspTime t;
							sceRtcGetCurrentClockLocalTime(&t);
							sprintf(ascii, "%04hu-%02hu-%02hu %02hu:%02hu",
								t.year, t.month, t.day, t.hour, t.minutes
							);
							
							osk_convertCharToUnsignedShort(POST_IT_JSON_DATETIME, uni1);
							osk_convertCharToUnsignedShort(ascii, uni2);
							osk_updateOskParam(&kb, uni1, uni2, NULL);
							break;
						case Add_Datepart:
							osk_convertCharToUnsignedShort(POST_IT_JSON_DATEPART " (" POST_IT_DATEPART_OPTIONS ")", uni1);
							osk_updateOskParam(&kb, uni1, NULL, NULL);
							break;
						case Add_Repeat:
							osk_convertCharToUnsignedShort(POST_IT_JSON_REPEAT, uni1);
							osk_updateOskParam(&kb, uni1, NULL, NULL);
							break;
						case Add_Done:
							post_convertJsonToPostIt(post);
							add_event = 0;
							break;
						default:
							break;
					}
					free(uni1);
					free(uni2);
					
					if (step != Add_Done){
						osk_start(&kb);
						if (osk_showOsk(&kb)){
							// convert osk unicode string to ascii
							osk_convertUnsignedShortToChar(kb.data->outtext, ascii);
							
							switch (step){
								case Add_Message:
									post_addMessage(post, ascii);
									step = Add_Datetime;
									break;
								case Add_Datetime:
									pspTime t;
									sscanf(ascii, "%hu%*c%hu%*c%hu %hu%*c%hu", 
										&t.year, &t.month, &t.day, &t.hour, &t.minutes
									);
									post_addDateTime(post, t);
									step = Add_Datepart;
									break;
								case Add_Datepart:
									if(strcmp(ascii, "")){ // if not empty
										if (!strcmp(ascii, "hour"))
											post_addDatePart(post, HOUR);
										else if (!strcmp(ascii, "minute"))
											post_addDatePart(post, MINUTE);
										else if (!strcmp(ascii, "year"))
											post_addDatePart(post, YEAR);
										else if (!strcmp(ascii, "month"))
											post_addDatePart(post, MONTH);
										else if (!strcmp(ascii, "day"))
											post_addDatePart(post, DAY);
										step = Add_Repeat;
									}else{
										step = Add_Done;
									}
									break;
								case Add_Repeat:
									if(strcmp(ascii, "")){ // if not empty
										int x = 0;
										sscanf(ascii, "%d", &x);
										post_addRepeat(post, x);
									}
									step = Add_Done;
									break;
								default:
									step = Add_Done;
									break;
							}
							
							osk_stop(&kb);
						}
					}
					
					free(ascii);
				}
				break;
			default:
				break;
		}
		
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
	for (int i = 0; i < RES_MENU_OPTIONS - 1; i++)
		if(menu_bg[i])
			gfx_freeImage(menu_bg[i]);
	intraFontUnload(ltn[0]);	
	intraFontUnload(ltn[1]);
	osk_free(&kb);
	
	/* Shutdown */
	gfx_disableGraphics();
	intraFontShutdown();
	sceKernelExitGame();
	
	return 0;
}