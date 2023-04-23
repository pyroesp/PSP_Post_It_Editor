#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <psputility.h>
#include <psputility_osk.h>


#define OSK_STRING_SIZE 128


typedef struct{
	SceUtilityOskData *data;
	int size;
	
	SceUtilityOskParams params;
	
	int started;
}Osk;



void osk_init(Osk *kb, unsigned short *desc, unsigned short *intext, unsigned short *outtext);
void osk_updateOskParam(Osk *kb, unsigned short *desc, unsigned short *intext, unsigned short *outtext);
void osk_start(Osk *kb);
int osk_showOsk(Osk *kb);
void osk_stop(Osk *kb);
void osk_free(Osk *kb);

void osk_convertCharToUnsignedShort(char *c, unsigned short *us);
void osk_convertUnsignedShortToChar(unsigned short *us, char *c);


#endif