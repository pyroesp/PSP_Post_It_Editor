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
enum eresMenu{
	VIEW,
	EDIT,
	ADD,
	REMOVE,
	SAVE,
	EXIT,
	CREDITS	
};


char menu[7][20] = {
	"View notes",
	"Edit notes",
	"Add notes",
	"Remove notes",
	"Save notes",
	"Exit",
	"Credits"
};


#endif