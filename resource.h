#ifndef RESOURCE_H
#define RESOURCE_H

#define RES_IMAGE_PATH	"./images/"
#define RES_MAIN		"main.png"
#define RES_VIEW 		"view.png"
#define RES_EDIT 		"edit.png"
#define RES_ADD 		"add.png"
#define RES_REMOVE 		"remove.png"
#define RES_SAVE 		"save.png"
#define RES_CREDIT 		"credit.png"


// use enum for quick constants
enum MainMenu{
	MM_VIEW = 0,
	MM_EDIT,
	MM_ADD,
	MM_REMOVE,
	MM_SAVE,
	MM_CREDITS,
	MM_EXIT
};


#define RES_MENU_OPTIONS 7

char main_menu[RES_MENU_OPTIONS][10] = {
	"View",
	"Edit",
	"Add",
	"Remove",
	"Save",
	"Credits",
	"Exit"
};

char credits[] = {
"- kevinr for the graphics library\n\
- raytwo for all the images\n\
- Benhur for intraFont\n\
- DaveGamble for cJSON\n\
- Everyone on the pspdev github and\n\
    discord community"
};


#endif