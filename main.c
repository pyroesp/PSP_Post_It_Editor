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
#include <pspctrl.h>
#include <pspdebug.h>

#include <intraFont.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "graphics.h"
#include "PPi_ED.h"
#include "keyboard.h"

PSP_MODULE_INFO("PPi_Editor", 0, 1, 1);

static int running = 1;

int exit_callback(int arg1, int arg2, void *common) {
	running = 0;
	return 0;
}

int CallbackThread(SceSize args, void *argp) {
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

int SetupCallbacks(void) {
	int thid = sceKernelCreateThread("CallbackThread", CallbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
	if (thid >= 0) sceKernelStartThread(thid, 0, 0);
	return thid;
}

extern void waitForPress(void);

int main ()
{
    pspDebugScreenInit();
    SetupCallbacks();

    if (!intraFontInit())
        sceKernelExitGame();

    intraFont *ltn[2];

    char path[] = "ms0:/seplugins/PPi.txt";
    char font[40];
    int c = 0, i = 0, j = 0, m = 0, r_note = 0, e_note;

    for (i = 0; i < 2; ++i)
    {
        sprintf(font, "flash0:/font/ltn%d.pgf", i+7);
        ltn[i] = intraFontLoad(font, 0);

        if (!ltn[i])
            sceKernelExitGame();
    }

	char bf[NB_L * NB_C + OFFSET];
	memset(bf, 0, sizeof(bf));

    FILE *file = NULL;
    file = fopen(path, "r");

	if (!file){
		pspDebugScreenSetXY(15, 2);
		pspDebugScreenPrintf("                    ");
		pspDebugScreenSetXY(15, 2);
		pspDebugScreenPrintf("PPi file not found. ");
		sceKernelDelayThread(1000 * 5);
		waitForPress();
        return 0;
	}

    do
    {
        c = fgetc(file);
        bf[m++] = c;
    }while (c != EOF);

    m = 0;

    fclose(file);

    char fl[NB_L][NB_C];
    int nb_l_f = Number_Lines_File(bf);

    for (i = 0; i <= nb_l_f; i++)
    {
        for (j = 0; j < NB_C; j++)
        {
            fl[i][j] = bf[m];

            if ((bf[m] == '\n') | (bf[m] == '\0'))
                break;
            else
                m++;
        }

        fl[i][j] = '\0';

        m--;
        m = Search_Endline(bf, m);
    }

    initGraphics();

    Keyboard k;

    k.font = ltn[1];
    k.start_text[0] = '\0';

    int option = Menu(ltn, &running);

    while (running)
    {
        switch (option)
        {
            case 0:
                if (nb_l_f > -1)
                    View_PPi(nb_l_f, fl, ltn[1], &running);
                else
                    Error_VNote(&running, ltn[1]);

                option = 6;
                break;
            case 1:
                if (nb_l_f > -1)
                {
                    r_note = Choose_Note(ltn, nb_l_f, &running);
                    fl[r_note][0] = '\0';

                    for (i = r_note; i < nb_l_f; i++)
                    {
                        for (j = 0; j < NB_C; j++)
                            fl[i][j] = fl[i+1][j];
                    }

                    fl[nb_l_f][0] = '\0';

                    nb_l_f--;

                    if (nb_l_f < -1)
                        nb_l_f = -1;
                }
                else
                    Error_RNote(&running, ltn[1]);

                option = 6;
                break;
            case 2:
                if (nb_l_f >= NB_L - 1)
                    Error_ANote(&running, ltn[1]);
                else
                {
                    char *recov = Start_Keyboard(&k, &running);
                    nb_l_f++;

                    for (i = 0; i < NB_C; i++)
                        fl[nb_l_f][i] = recov[i];
                }

                option = 6;
                break;
            case 3:
                if (nb_l_f > -1)
                {  
                    e_note = Choose_Note(ltn, nb_l_f, &running); 

                    for(i = 0; i < NB_C; i++)
                        k.start_text[i] = fl[e_note][i]; 

                    char* recov = Start_Keyboard(&k, &running);

                    for (i = 0; i < NB_C; i++)
                        fl[e_note][i] = recov[i];

                    k.start_text[0] = '\0';
                }
                else
                    Error_ENote(&running, ltn[1]);

                option = 6;
                break;
            case 4:
                running = 0;
                break;
            case 5:
                View_Credit(&running, ltn[1]);
                option = 6;
                break;
            case 6:
                option = Menu(ltn, &running);
                break;
        }
    }

    char lettre[76] = {'1','2','3','4','5','6','7','8','9','0','(',')','a','z','e','r','t','y','u','i','o','p','A','Z','E','R','T','Y','U','I','O','P','!','?','q','s','d','f','g','h','j','k','l','m','Q','S','D','F','G','H','J','K','L','M',';','/','w','x','c','v','b','n','W','X','C','V','B','N','.',',',':',' ','-','_','\n','\0'};

    file = fopen(path, "w+");

    for (i = 0; i <= nb_l_f; i++)
    {
        for (j = 0; j < strlen(fl[i]); j++)
        {
            for (m = 0; m < 76; m++)
            {
                if (fl[i][j] == lettre[m])
                        fputc(fl[i][j], file);
            }
        }

        if (i != nb_l_f)
            fputc('\n', file);
        else
            fputc('\0', file);
    }

    fclose(file);
	disableGraphics();

    for (i = 0; i < 2; ++i){
        intraFontUnload(ltn[i]);
	}
	
	intraFontShutdown();
	sceKernelExitGame();

	return 0;
}
