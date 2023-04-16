 /*
  "PSP Post it editor" homebrew for PSP

	Author: Nicolas Lopez F.
		pyroesp (pspgen, devsgen, lan.St)


	Release date: 13.04.2010

	Version: 1.0

	What is this ?:
		"PSP Post ot editor" is a homebrew that allows you to add, remove or edit
		notes for the PSP Post it plugin/prx.

	Controls:
		- Main menu:
			Up, Down: Choose an option
			Cross: Confirm option

		- View text file:
			R trigger, L trigger, Select: Move text to the left and to the right
			Start: Return to main menu

		- Remove note:
			Up, Down: Choose an option
			Cross: Confirm option

		- Add Note:
			Up, Down, Left, Right: Choose character in keyboard
			Cross: Select charachter
			Square: Delete
			Triangle: Caps lock on/off
			R trigger, L trigger: Move cursor
			O+R trigger, O+L trigger, O+Select: Move text to the left and to the right

		- Edit Note:
			See Remove Note and Add Note.

		- Credits:
			Start: Return to main menu

	Special Thanks to:
		- Raytwo and kevinr / K91220
		- Benhur for intraFont
		- Everyone else I forgot to mention (sorry ^^)

	Note: This is an open source homebrew. You can use this code for your own, but I'll appreciate
		if you write my name or pseudo in your program/readme/whatever (and the name of the homebrew).

	Thank you, I hope you'll appreciate this program.
*/

#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

typedef struct
{
    char start_text[NB_C];
    intraFont *font;
}Keyboard;

typedef struct
{
    int x, y, up;
}Select;

char* Start_Keyboard(Keyboard *k, int *prunning);

void Draw_Keyboard(Keyboard *k, Select s);
void drawRectScreen(int x0, int y0, int width, int height, Color c);

#endif // KEYBOARD_H_INCLUDED
