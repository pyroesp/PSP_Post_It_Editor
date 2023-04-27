#ifndef GUI_H
#define GUI_H

#include <intraFont.h>
#include "graphics.h"


/* title */
#define GUI_TITLE_X_POS 70
#define GUI_TITLE_Y_POS 18
#define GUI_TITLE_UNDERLINE_Y_POS 26


/* menu stuff */
#define GUI_MENU_X_POS 120
#define GUI_MENU_X_OFFSET 20
#define GUI_MENU_Y_POS 70
#define GUI_MENU_Y_OFFSET 20 // space between options
#define GUI_MENU_H_LINE_X 100 // horizontal line start position
#define GUI_MENU_H_LINE_SIZE 25 // horizontal line size
#define GUI_MENU_V_LINE_SIZE 40 // vertical line size between options


/* scroll bar */
#define GUI_SCROLL_LINE_H		150
#define GUI_SCROLL_LINE_X		10
#define GUI_SCROLL_BAR_OFFSET	5
#define GUI_SCROLL_BAR_W		11
#define GUI_SCROLL_BAR_H		(GUI_SCROLL_LINE_H - (GUI_SCROLL_BAR_OFFSET * 2))
#define GUI_SCROLL_BAR_X		5



void gui_printTitle(char *title, intraFont *f);
void gui_mainMenu(char *menu, int menu_options, int menu_length, intraFont *f, int selected_option);

void gui_scrollBar(int total_positions, int position);


#endif