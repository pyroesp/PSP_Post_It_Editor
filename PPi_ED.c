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

#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>

#include <intraFont.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "graphics.h"
#include "PPi_ED.h"


int Number_Lines_File(char *pbf)
{
    int i = 0,  n = 0;

    for (i = 0; i < strlen(pbf); i++)
    {
        if ((pbf[i] == '\n') || (pbf[i] == '\0'))
            ++n;
    }

    return n;
}

int Search_Endline(char *pbf, int pos)
{
    int i = 0;

    for (i = pos; i < strlen(pbf); ++i)
    {
        if (pbf[i] == '\n')
            break;
    }

    return i+1;
}


int Menu(intraFont *pltn[2], int *prunning)
{
    int option = 0, exit_menu = 0, i = 0, x = 0, anim = 0;
    SceCtrlData pad, oldpad;
	Image *main;
	
	main = loadImage("ms0:/PSP/GAME/PPi Editor/images/main.png");

    sceCtrlReadBufferPositive(&oldpad, 1);

    char menu_t[6][22] = {{"View text file"},{"Remove note"},{"Add note"},
                            {"Edit note"},{"Exit and Save"}, {"Credits"}};

    while (*prunning && !exit_menu)
    {
        sceCtrlReadBufferPositive(&pad, 1);

		if (!(oldpad.Buttons & PSP_CTRL_CROSS) && (pad.Buttons & PSP_CTRL_CROSS) && !anim)
            exit_menu = 1;

        if (!(oldpad.Buttons & PSP_CTRL_UP) && (pad.Buttons & PSP_CTRL_UP) && !anim)
            --option;

        else if (!(oldpad.Buttons & PSP_CTRL_DOWN) && (pad.Buttons & PSP_CTRL_DOWN) && !anim)
            ++option;

        oldpad = pad;

        if (option < 0)
            anim = 1;
        else if (option > 5)
            anim = 2;

        if ((anim == 1) & (option < 5))
        {
            ++option;
            sceKernelDelayThread(90000);
        }
        else if ((anim == 2) & (option > 0))
        {
            --option;
            sceKernelDelayThread(90000);
        }

        if ((anim == 1) & (option == 5))
            anim = 0;
        else if ((anim == 2) & (option == 0))
            anim = 0;

        clearScreen(WHITE);
		
		
		if (main){
			blitImageToScreen(0, 0, main->imageWidth, main->imageHeight, main, 0, 0);
		}
		
        guStart();

        intraFontSetStyle(pltn[0], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
        intraFontPrint(pltn[0], 70, 18, "PSP Post it editor:");

        for (i = 0; i < 6; ++i)
        {
            if (i == option)
            {
                x = 140;
                intraFontSetStyle(pltn[1], 0.9f, RED, DARKGRAY, 0.0f, 0);

                drawLineScreen(100, 26, 100, 66 + (20 * i), BLACK);
                drawLineScreen(100, 66 + (20 * i), 125, 66 + (20 * i), BLACK);
                fillScreenRect(BLACK, 124, 65 + (20 * i), 3, 3);
                fillScreenRect(BLACK, 99, 65 + (20 * i), 3, 3);
            }
            else
            {
                intraFontSetStyle(pltn[1], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
                x = 120;
            }

            intraFontPrintf(pltn[1], x, 70 + (20 * i), "%s", menu_t[i]);

            drawLineScreen(70, 26, 230, 26, BLACK);
        }
		
		
		intraFontSetStyle(pltn[1], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
		intraFontPrintf(pltn[1], 10, 200, 
			"img ptr = %#X\nw = %d | h = %d\ntw = %d | th = %d\nbd = %d | ct = %d\nrb = %d\ndata ptr = %#X", 
			main, main->imageWidth, main->imageHeight,
			main->textureWidth, main->textureHeight,
			main->bit_depth, main->color_type,
			main->rowbytes,
			main->data
		);

        for (i = 0; i < option; ++i)
            fillScreenRect(BLACK, 99, 65 + (20 * i), 3, 3);

		sceGuFinish();
		sceGuSync(0,0);
		
		sceDisplayWaitVblankStart();
		flipScreen();
    }
	
	if (main)
		freeImage(main);

    return option;
}

void View_PPi(int nb_l_f, char pfl[NB_L][NB_C], intraFont *pltn, int *prunning)
{
    int view_file = 1, i = 0, x = 7, y = 13;
    SceCtrlData pad;

    while (*prunning && view_file)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_START)
            view_file = 0;
        else if (pad.Buttons & PSP_CTRL_SELECT)
            x = 7;
        else if ((x > -167) && (pad.Buttons & PSP_CTRL_LTRIGGER))
        {
            x -= 3;
        }
        else if ((x < 15) && (pad.Buttons & PSP_CTRL_RTRIGGER))
        {
            x += 3;
        }

        clearScreen(WHITE);
        guStart();

        intraFontSetStyle(pltn, 0.9f, BLACK, DARKGRAY, 0.0f, 0);

        for (i = 0; i <= nb_l_f; i++)
        {
            intraFontPrintf(pltn, x, y,"Note %d:", i + 1);
            intraFontPrintf(pltn, x + 67, y, "%s", pfl[i]);
            y += 18;
        }

        y = 13;

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();
    }
}

int Choose_Note(intraFont *pltn[2], int nb_l_f, int *prunning)
{
    int option = 0, exit_menu = 0, i = 0, x = 0, anim = 0;
    SceCtrlData pad, oldpad;

    sceCtrlReadBufferPositive(&oldpad, 1);

    while (*prunning && !exit_menu)
    {
        sceCtrlReadBufferPositive(&pad, 1);

		if (!(oldpad.Buttons & PSP_CTRL_CROSS) && (pad.Buttons & PSP_CTRL_CROSS) && !anim)
            exit_menu = 1;

        if (!(oldpad.Buttons & PSP_CTRL_UP) && (pad.Buttons & PSP_CTRL_UP) && !anim)
            --option;

        else if (!(oldpad.Buttons & PSP_CTRL_DOWN) && (pad.Buttons & PSP_CTRL_DOWN) && !anim)
            ++option;

        oldpad = pad;

        if (option < 0)
            anim = 1;
        else if (option > nb_l_f)
            anim = 2;

        if ((anim == 1) & (option < nb_l_f))
        {
            ++option;
            sceKernelDelayThread(90000);
        }
        else if ((anim == 2) & (option > 0))
        {
            --option;
            sceKernelDelayThread(90000);
        }

        if ((anim == 1) & (option == nb_l_f))
            anim = 0;
        else if ((anim == 2) & (option == 0))
            anim = 0;

        clearScreen(WHITE);
        guStart();

        intraFontSetStyle(pltn[0], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
        intraFontPrint(pltn[0], 70, 18, "Choose note:");
        drawLineScreen(70, 26, 180, 26, BLACK);

        for (i = 0; i <= nb_l_f; ++i)
        {
            if (i == option)
            {
                x = 160;
                intraFontSetStyle(pltn[1], 0.9f, RED, DARKGRAY, 0.0f, 0);

                drawLineScreen(100, 26, 100, 43 + (15 * i), BLACK);
                drawLineScreen(100, 43 + (15 * i), 145, 43 + (15 * i), BLACK);
                fillScreenRect(BLACK, 144, 42 + (15 * i), 3, 3);
                fillScreenRect(BLACK, 99, 42 + (15 * i), 3, 3);
            }
            else
            {
                intraFontSetStyle(pltn[1], 0.9f, BLACK, DARKGRAY, 0.0f, 0);
                x = 140;
            }

            intraFontPrintf(pltn[1], x, 46 + (15 * i), "Note %d", i+1);
        }

        for (i = 0; i < option; ++i)
            fillScreenRect(BLACK, 99, 42 + (15 * i), 3, 3);

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();
    }

    return option;
}

void View_Credit(int *prunning, intraFont *pltn)
{
    int view_credit = 1;
    SceCtrlData pad;

    while (*prunning && view_credit)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_START)
            view_credit = 0;

        clearScreen(WHITE);
        guStart();

        intraFontSetStyle(pltn, 1.1f, BLACK, DARKGRAY, 0.0f, 0);
        intraFontPrint(pltn, 15, 22, "Made by pyroesp.\n\n\n\n      Special thanks to:\n\n\n            Benhur for intraFont\n\n            Raytwo and kevinr/K91220");

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();
    }
}

void Error_VNote(int *prunning, intraFont *pltn)
{
    int remove_note = 1;
    SceCtrlData pad;

    while (*prunning && remove_note)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_START)
            remove_note = 0;

        clearScreen(WHITE);
        guStart();

        intraFontSetStyle(pltn, 1.1f, BLACK, DARKGRAY, 0.0f, 0);
        intraFontPrint(pltn, 15, 22, "There are 0 notes in the file.");
        intraFontPrint(pltn, 15, 40, "You can't view them.");
        intraFontPrint(pltn, 15, 58, "Press START to exit.");

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();
    }
}

void Error_RNote(int *prunning, intraFont *pltn)
{
    int remove_note = 1;
    SceCtrlData pad;

    while (*prunning && remove_note)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_START)
            remove_note = 0;

        clearScreen(WHITE);
        guStart();

        intraFontSetStyle(pltn, 1.1f, BLACK, DARKGRAY, 0.0f, 0);
        intraFontPrint(pltn, 15, 22, "There are 0 notes in the file.");
        intraFontPrint(pltn, 15, 40, "You can't remove another one.");
        intraFontPrint(pltn, 15, 58, "Press START to exit.");

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();
    }
}

void Error_ANote(int *prunning, intraFont *pltn)
{
    int add_note = 1;
    SceCtrlData pad;

    while (*prunning && add_note)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_START)
            add_note = 0;

        clearScreen(WHITE);
        guStart();

        intraFontSetStyle(pltn, 1.1f, BLACK, DARKGRAY, 0.0f, 0);
        intraFontPrint(pltn, 15, 22, "You have reached the max number notes in the file.");
        intraFontPrint(pltn, 15, 40, "You can't add another one.");
        intraFontPrint(pltn, 15, 58, "Press START to exit.");

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();
    }
}

void Error_ENote(int *prunning, intraFont *pltn)
{
    int edit_note = 1;
    SceCtrlData pad;

    while (*prunning && edit_note)
    {
        sceCtrlReadBufferPositive(&pad, 1);

        if (pad.Buttons & PSP_CTRL_START)
            edit_note = 0;

        clearScreen(WHITE);
        guStart();

        intraFontSetStyle(pltn, 1.1f, BLACK, DARKGRAY, 0.0f, 0);
        intraFontPrint(pltn, 15, 22, "There are 0 notes in the file.");
        intraFontPrint(pltn, 15, 40, "You can't edit a note.");
        intraFontPrint(pltn, 15, 58, "Press START to exit.");

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();
    }
}
