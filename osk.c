#include <stdlib.h>
#include <string.h>


#include "osk.h"


void osk_init(Osk *kb, unsigned short *desc, unsigned short *intext, unsigned short *outtext){
	kb->size = 1;
	kb->started = 0;
	
	kb->data = (SceUtilityOskData*)malloc(sizeof(SceUtilityOskData) * kb->size);
	memset(kb->data, 0, sizeof(SceUtilityOskData));
	kb->data->language = PSP_UTILITY_OSK_LANGUAGE_ENGLISH; // Use system default for text input
	kb->data->lines = 1;
	kb->data->unk_24 = 1;
	kb->data->inputtype = PSP_UTILITY_OSK_INPUTTYPE_ALL; // Allow all input types
	kb->data->desc = desc;
	kb->data->intext = intext;
	kb->data->outtextlength = OSK_STRING_SIZE;
	kb->data->outtextlimit = 32; // Limit input to 32 characters
	kb->data->outtext = outtext;
	
	
	memset(&kb->params, 0, sizeof(SceUtilityOskParams));
	kb->params.base.size = sizeof(SceUtilityOskParams);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &kb->params.base.language);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &kb->params.base.buttonSwap);
	kb->params.base.graphicsThread = 17;
	kb->params.base.accessThread = 19;
	kb->params.base.fontThread = 18;
	kb->params.base.soundThread = 16;
	kb->params.datacount = kb->size;
	kb->params.data = kb->data;
}


void osk_start(Osk *kb){
	if (!kb->started)
		if (sceUtilityOskInitStart(&kb->params))
			kb->started = 1;
}



void osk_updateOskParam(Osk *kb, unsigned short *desc, unsigned short *intext, unsigned short *outtext){
	if (!kb->started){
		if (desc)
			memcpy(kb->data->desc, desc, sizeof(unsigned short) * OSK_STRING_SIZE);
		else
			memset(kb->data->desc, 0, sizeof(unsigned short) * OSK_STRING_SIZE);
		
		if (intext)
			memcpy(kb->data->intext, intext, sizeof(unsigned short) * OSK_STRING_SIZE);
		else
			memset(kb->data->intext, 0, sizeof(unsigned short) * OSK_STRING_SIZE);
		
		if (outtext)
			memcpy(kb->data->outtext, outtext, sizeof(unsigned short) * OSK_STRING_SIZE);	
		else
			memset(kb->data->outtext, 0, sizeof(unsigned short) * OSK_STRING_SIZE);
	}
}


int osk_showOsk(Osk *kb){
	int done = 0;
	if (kb->started){
		switch(sceUtilityOskGetStatus())
		{
			case PSP_UTILITY_DIALOG_INIT:
				break;
			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilityOskUpdate(1);
				break;
			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilityOskShutdownStart();
				break;
			case PSP_UTILITY_DIALOG_FINISHED:
				break;
			case PSP_UTILITY_DIALOG_NONE:
				done = 1;
				break;
			default :
				break;
		}
	}
	
	return done;
}


void osk_stop(Osk *kb){
	if (kb->started){
		sceUtilityOskShutdownStart();
		kb->started = 0;
	}
}

void osk_free(Osk *kb){
	if (kb){
		if (kb->data){
			if (kb->data->desc)
				free(kb->data->desc);
			if (kb->data->intext)
				free(kb->data->intext);
			if (kb->data->outtext)
				free(kb->data->outtext);
			free(kb->data);
		}
	}
}


void osk_convertCharToUnsignedShort(char *c, unsigned short *us){
	int i;
	for (i = 0; c[i] != 0 && i < OSK_STRING_SIZE; i++)
		us[i] = c[i];
	us[i] = 0;
}


void osk_convertUnsignedShortToChar(unsigned short *us, char *c){
	int i;
	for (i = 0; us[i] != 0 && i < OSK_STRING_SIZE; i++)
		c[i] = us[i];
	c[i] = 0;
}