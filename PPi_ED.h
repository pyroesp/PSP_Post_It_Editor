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

#ifndef PPI_ED_H_INCLUDED
#define PPI_ED_H_INCLUDED

#define NB_L 15
#define NB_C_TXT 50
#define NB_C_EL 1
#define NB_C_TD 17
#define NB_C (NB_C_TXT + NB_C_TD + NB_C_EL)

#define OFFSET 1000

enum colors
{
    NOTHING = 0x00000000,
	RED =	0xFF0000FF,
	GREEN =	0xFF00FF00,
	BLUE =	0xFFFF0000,
	CYAN = 0xFFE8BD3C,
	DARKCYAN = 0xFF9D7B13,
	YELLOW = 0xFF00F3F3, /*
	YELLOW_A50 = 0x7F00F3F3, */
	WHITE =	0xFFFFFFFF, /*
	WHITE_A50 = 0x7FFFFFFF, */
	LITEGRAY = 0xFFBFBFBF,
	LITEGRAY_A50 = 0x7FBFBFBF, /*
	GRAY =  0xFF7F7F7F, */
	DARKGRAY = 0xFF3F3F3F,
	BLACK = 0xFF000000,
};

int Number_Lines_File(char *pbf);
int Search_Endline(char *pbf, int pos);

int Menu(intraFont *pltn[2], int *prunning);

void View_PPi(int nb_l_f, char pfl[NB_L][NB_C], intraFont *pltn, int *prunning);
int Choose_Note(intraFont *pltn[2], int nb_l_f, int *prunning);
void View_Credit(int *prunning, intraFont *pltn);

void Error_VNote(int *prunning, intraFont *pltn);
void Error_RNote(int *prunning, intraFont *pltn);
void Error_ANote(int *prunning, intraFont *pltn);
void Error_ENote(int *prunning, intraFont *pltn);

#endif // PPI_ED_H_INCLUDED
