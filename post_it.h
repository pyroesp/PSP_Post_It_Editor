#ifndef POST_IT_H
#define POST_IT_H

#include <intraFont.h>

#include "cJSON/cJSON.h"


#define POST_IT_PATH "ms0:/PSP/GAME/PSP Post It - Editor/"
#define POST_IT_FILE "post it.json"


typedef enum{
	NONE,
	HOUR,
	MINUTE,
	YEAR,
	MONTH,
	DAY
}DatePart;

typedef struct{
	int hour;
	int minute;
	
	int year;
	int month;
	int day;
}DateTime;

typedef struct{
	char *msg;
	char *date;
	
	DateTime dt;
	
	int repeat;
	DatePart part;
}PostIt_Event;


typedef struct{
	PostIt_Event *event;
	int size;
}PostIt;


PostIt* post_readPostIt(cJSON *json);

void post_displayEvents(int x, int y, PostIt* p, intraFont* font);

void post_free(PostIt *post);


#endif