#include "gui.h"



void gui_printTitle(char *title, intraFont *f){
	// title
	intraFontSetStyle(f, 0.9f, BLACK, DARKGRAY, 0.0f, 0);
	int title_length = (int)intraFontPrint(f, GUI_TITLE_X_POS, GUI_TITLE_Y_POS, title);
	gfx_drawLineScreen(
		GUI_TITLE_X_POS, GUI_TITLE_UNDERLINE_Y_POS, 
		title_length, GUI_TITLE_UNDERLINE_Y_POS, 
		BLACK
	);
}


void gui_mainMenuOptions(char *menu, int menu_options, int menu_length, intraFont *f, int selected_option){
	// main menu
	int x;
	for (int i = 0; i < menu_options; i++){
		if (i <= selected_option)
			// draw rect in front of all options before the selected one
			gfx_fillScreenRect(
				BLACK, GUI_MENU_H_LINE_X - 1, 
				GUI_TITLE_UNDERLINE_Y_POS + GUI_MENU_V_LINE_SIZE + (GUI_MENU_Y_OFFSET * i) - 1, 
				3, 3
			);
		
		if (i == selected_option){
			// draw vertical line from title to option
			gfx_drawLineScreen(
				GUI_MENU_H_LINE_X, GUI_TITLE_UNDERLINE_Y_POS, GUI_MENU_H_LINE_X, 
				GUI_TITLE_UNDERLINE_Y_POS + GUI_MENU_V_LINE_SIZE + (GUI_MENU_Y_OFFSET * i), 
				BLACK
			);
			// draw horizontal line to selected option
			gfx_drawLineScreen(
				GUI_MENU_H_LINE_X, GUI_TITLE_UNDERLINE_Y_POS + GUI_MENU_V_LINE_SIZE + (GUI_MENU_Y_OFFSET * i), 
				GUI_MENU_H_LINE_X + GUI_MENU_H_LINE_SIZE, 
				GUI_TITLE_UNDERLINE_Y_POS + GUI_MENU_V_LINE_SIZE + (GUI_MENU_Y_OFFSET * i), 
				BLACK
			);
			// draw rectangle to selected option
			gfx_fillScreenRect(
				BLACK, GUI_MENU_H_LINE_X + GUI_MENU_H_LINE_SIZE - 1, 
				GUI_TITLE_UNDERLINE_Y_POS + GUI_MENU_V_LINE_SIZE + (GUI_MENU_Y_OFFSET * i) - 1, 3, 3
			);
			
			intraFontSetStyle(f, 0.9f, RED, DARKGRAY & ALPHA_50, 0.0f, 0);
			x = GUI_MENU_X_OFFSET + GUI_MENU_X_POS;
		}else{
			intraFontSetStyle(f, 0.9f, BLACK, DARKGRAY & ALPHA_50, 0.0f, 0);
			x = GUI_MENU_X_POS;
		}

		// print options on screen
		intraFontPrint(f, x, GUI_MENU_Y_POS + (GUI_MENU_Y_OFFSET * i), &menu[i * menu_length]);
	}
}



void gui_scrollBar(int total_positions, int position){
	Rect r;
	r.w = GUI_SCROLL_BAR_W;
	r.h = GUI_SCROLL_BAR_H / total_positions;
	r.x = GUI_SCROLL_BAR_X;
	r.y = (SCREEN_HEIGHT - GUI_SCROLL_BAR_H) / 2 + position * r.h;
	
	// draw vertical line
	gfx_drawLineScreen(
		GUI_SCROLL_LINE_X, (SCREEN_HEIGHT - GUI_SCROLL_LINE_H) / 2, 
		GUI_SCROLL_LINE_X, (SCREEN_HEIGHT - GUI_SCROLL_LINE_H) / 2 + GUI_SCROLL_LINE_H, 
		BLACK
	);
	// draw 'shadow'
	gfx_fillScreenRect(LIGHTCYAN & ALPHA_25, r.x + 2, r.y + 2, r.w, r.h);
	// draw scroll bar
	gfx_fillScreenRect(DARKCYAN, r.x, r.y, r.w, r.h);
}



void gui_displayEvent(PostIt *post, int event_index, intraFont *f){
	post_displayEvents(GUI_EVENT_POS_X, GUI_EVENT_POS_Y, post, f, event_index, POST_IT_MAX_ON_SCREEN);	
}

void gui_cursorEvent(char indicator, int position, intraFont *f){
	intraFontSetStyle(f, 0.9f, BLACK, DARKGRAY & ALPHA_50, 0.0f, 0);
	intraFontPrintf(f, GUI_CURSOR_POS_X, GUI_EVENT_POS_Y + GUI_EVENT_Y_OFFSET * position, "%c", indicator);
}
