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
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspdebug.h>

#include <intraFont.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "graphics.h"
#include "PPi_ED.h"
#include "keyboard.h"

char azerty[4][13] = {
	"1234567890()",
	"azertyuiop!?",
	"qsdfghjklm;/",
	"wxcvbn.,: -_"
};

void waitForPress(void){
	
    SceCtrlData pad;
    sceCtrlReadBufferPositive(&pad, 1);
	while(!(pad.Buttons & PSP_CTRL_START)){
		sceCtrlReadBufferPositive(&pad, 1);
	}
	while(pad.Buttons & PSP_CTRL_START){
		sceCtrlReadBufferPositive(&pad, 1);
	}
	
	return;
}

void drawRectScreen(int x0, int y0, int width, int height, Color c)
{
    drawLineScreen(x0, y0, x0, y0 + height, c);
    drawLineScreen(x0, y0, x0 + width, y0, c);
    drawLineScreen(x0 , y0 + height, x0 + width, y0 + height, c);
    drawLineScreen(x0 + width, y0, x0 + width, y0 + height, c);
}

void Draw_Keyboard(Keyboard *k, Select s)
{
    int i = 0, j = 0, x = 0, y = 0, width = 36, height = 30, offset = 5;

    intraFontSetStyle(k->font, 1.0f, DARKCYAN, LITEGRAY, 0.0f, 0);

    y = SCREEN_HEIGHT / 2 + 15;

    for (i = 0; i < 4; i++)
    {
        x = (SCREEN_WIDTH - width * strlen(azerty[i]))/2;
        for (j = 0; j < strlen(azerty[i]); j++)
        {
            if (s.x == j && s.y == i)
                fillScreenRect(CYAN, x + width * j, y + height * i, width, height);
            else
                fillScreenRect(BLACK, x + width * j, y + height * i, width, height);

            drawRectScreen(x + width * j, y + height * i, width, height, WHITE);

            if (s.up)
                intraFontPrintf(k->font, x + width * j + width/2 - offset, y + height * i + height/2 + offset, "%c", toupper(azerty[i][j]));
            else
                intraFontPrintf(k->font, x + width * j + width/2 - offset, y + height * i + height/2 + offset, "%c", azerty[i][j]);
        }
    }
}

char* Start_Keyboard(Keyboard *k, int *prunning)
{
    int write_enable = 1, i = 0, j = 0, l = 0, x = 6, y = 18;

    SceCtrlData pad, oldpad;
    sceCtrlReadBufferPositive(&oldpad, 1);

    Select s;
    s.x = 0;
    s.y = 0;
    s.up = 0;


    for (l = 0; l < strlen(k->start_text); l++)
    {
        if (k->start_text[l] == '\0')
            break;
    }

    i = l;

    while (write_enable & *prunning)
    {
        if (l > NB_C-1)
            l = NB_C - 1;
        else if (l < 0)
            l = 0;

        clearScreen(WHITE);
        guStart();

        sceCtrlReadBufferPositive(&pad, 1);

        k->start_text[l] = '\0';

        intraFontSetStyle(k->font, 1.0f, BLACK, DARKGRAY, 0.0f, 0);
        intraFontPrintf(k->font, x, y, "%s", k->start_text);
        intraFontPrintf(k->font, 8, y + 40, "pos. = %d", i);

        Draw_Keyboard(k, s);

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();

		if ((pad.Buttons & PSP_CTRL_START) && !(oldpad.Buttons & PSP_CTRL_START))
            write_enable = 0;
        else if ((strlen(k->start_text) < NB_C-1) && (pad.Buttons & PSP_CTRL_CROSS) && !(oldpad.Buttons & PSP_CTRL_CROSS))
        {
            if (i > l)
                i = l;
            if (s.up)
                k->start_text[i] = toupper(azerty[s.y][s.x]);
            else
                k->start_text[i] = azerty[s.y][s.x];
            i++;
            l++;
        }
        else if ((pad.Buttons & PSP_CTRL_SQUARE) && !(oldpad.Buttons & PSP_CTRL_SQUARE))
        {
            for (j = i; j < l; j++)
                k->start_text[j] = k->start_text[j+1];
            --i;
            --l;

            if (i < 0)
                i = 0;
        }
        else if ((pad.Buttons & PSP_CTRL_TRIANGLE) && !(oldpad.Buttons & PSP_CTRL_TRIANGLE))
            s.up = !s.up;
        else if ((pad.Buttons & PSP_CTRL_LTRIGGER) && !(pad.Buttons & PSP_CTRL_CIRCLE) && !(oldpad.Buttons & PSP_CTRL_LTRIGGER))
        {
            --i;

            if (i < 0)
                i = 0;
        }
        else if ((pad.Buttons & PSP_CTRL_RTRIGGER) && !(pad.Buttons & PSP_CTRL_CIRCLE) && !(oldpad.Buttons & PSP_CTRL_RTRIGGER))
        {
            ++i;

            if (i > l)
                i = l;
        }
        else if (s.y > 0 && (pad.Buttons & PSP_CTRL_UP) && !(oldpad.Buttons & PSP_CTRL_UP))
            --s.y;
        else if (s.y < 3 && (pad.Buttons & PSP_CTRL_DOWN) && !(oldpad.Buttons & PSP_CTRL_DOWN))
            ++s.y;
        else if (s.x > 0 && (pad.Buttons & PSP_CTRL_LEFT) && !(oldpad.Buttons & PSP_CTRL_LEFT))
            --s.x;
        else if (s.x < (strlen(azerty[s.y]) - 1) && (pad.Buttons & PSP_CTRL_RIGHT) && !(oldpad.Buttons & PSP_CTRL_RIGHT))
            ++s.x;
        else if ((x > -145) && (pad.Buttons & PSP_CTRL_LTRIGGER) && (pad.Buttons & PSP_CTRL_CIRCLE))
            x -= 3;
        else if ((x < 15) && (pad.Buttons & PSP_CTRL_RTRIGGER) && (pad.Buttons & PSP_CTRL_CIRCLE))
            x += 3;
        else if ((pad.Buttons & PSP_CTRL_SELECT) && (pad.Buttons & PSP_CTRL_CIRCLE))
            x = 6;

        oldpad = pad;
    }

    return k->start_text;
}
