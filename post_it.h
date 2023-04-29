#ifndef POST_IT_H
#define POST_IT_H

#include <sys/time.h>
#include <psprtc.h>

#include <intraFont.h>

#include "cJSON/cJSON.h"


#define POST_IT_PATH 		"ms0:/PSP/GAME/PSP Post It - Editor/"
#define POST_IT_PATH_REL 	"./"
#define POST_IT_FILE 		"post it.json"

#define POST_IT_JSON_MESSAGE	"event"
#define POST_IT_JSON_DATETIME 	"datetime"
#define POST_IT_JSON_DATEPART 	"datepart"
#define POST_IT_JSON_REPEAT 	"repeat"

#define POST_IT_DATEPART_OPTIONS "year, month, day, hour, minute"

// show 5 events on screen max
#define POST_IT_MAX_ON_SCREEN 5


#define POST_IT_SIZE_DATETIME 17
#define POST_IT_SIZE_DATEPART 7


typedef enum{
	State_None = 0,
	State_Message,
	State_DateTime,
	State_DatePart,
	State_Repeat,
	State_Done
}EventState;


typedef enum{
	NONE = 0,
	HOUR,
	MINUTE,
	YEAR,
	MONTH,
	DAY
}DatePart;


typedef struct{
	char *msg;
	char *datetime;
	
	pspTime dt;
	u64 tick; // time in µs
	
	int repeat;
	DatePart part;
}PEvent;


typedef struct{
	cJSON *json;
	PEvent *event;
	int size;
}PostIt;


PostIt* post_initPostIt(void);
void post_readJson(PostIt *post, const char *filepath);
void post_convertJsonToPostIt(PostIt *post);


int post_convertToTick(const pspTime *t, u64 *tick);
int post_convertToTimeStruct(const u64 *tick, pspTime *t);

int post_removeEvent(PostIt *post, int event_index);

int post_addEvent(PostIt *post);
void post_addMessage(PostIt *post, char *msg);
void post_addDateTime(PostIt *post, pspTime datetime);
void post_addDatePart(PostIt *post, DatePart part);
void post_addRepeat(PostIt *post, int repeat);

void post_editMessage(PostIt *post, int index, char *msg);
void post_editDateTime(PostIt *post, int index, pspTime datetime);
void post_editDatePart(PostIt *post, int index, DatePart part);
void post_editRepeat(PostIt *post, int index, int repeat);


void post_displayEvents(int x, int y, PostIt* p, intraFont* font, int start, int max);
void post_displayJSON(int x, int y, PostIt* p, intraFont* font);

void post_freeAll(PostIt *post);
void post_freeJson(PostIt *post);
void post_freeEvent(PostIt *post);
void post_freePostIt(PostIt *post);


#endif