#ifndef RESOURCE_H
#define RESOURCE_H

#define RES_IMAGE_PATH "ms0:/PSP/GAME/PSP Post It - Editor/images/"
#define RES_MAIN "main.png"
#define RES_VIEW "view.png"
#define RES_EDIT "edit.png"
#define RES_ADD "add.png"
#define RES_REMOVE "remove.png"
#define RES_SAVE "save.png"
#define RES_EXIT "exit.png"
#define RES_CREDIT "credit.png"


// use enum for quick constants
enum MainMenu{
	MM_VIEW,
	MM_EDIT,
	MM_ADD,
	MM_REMOVE,
	MM_SAVE,
	MM_CREDITS,
	MM_EXIT
};


char main_menu[7][10] = {
	"View",
	"Edit",
	"Add",
	"Remove",
	"Save",
	"Credits",
	"Exit"
};


#endif