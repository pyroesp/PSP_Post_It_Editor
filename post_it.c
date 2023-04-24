#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspdebug.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "post_it.h"
#include "graphics.h"



extern void waitOnPress(void);



PostIt* post_initPostIt(void){
	PostIt *post = NULL;
	post = (PostIt*)malloc(sizeof(PostIt));
	memset(post, 0, sizeof(PostIt));
	return post;
}


void post_readJson(PostIt *post, const char *filepath){
	if (!post || !filepath)
		return;
	
	// cJSON stuff
	int size = 0;
	char *v = NULL;
	FILE *fp = NULL;
	cJSON *json = NULL;
	
	fp = fopen(filepath, "r");
	if (fp){
		while(fgetc(fp) != EOF);
		size = ftell(fp) + 1;
		v = (char*)malloc(sizeof(char) * size);
		if (v){
			memset(v, 0, size);
			rewind(fp);
			fread(v, sizeof(char), size, fp);
			
			cJSON_InitHooks(NULL);
			json = cJSON_Parse(v);
			if (!json)
				return;
			post->json = json;
			free(v);
		}
		fclose(fp);
	}
}

void post_convertJsonToPostIt(PostIt *post){
	if (!post)
		return;
	post_freeEvent(post);

	cJSON *child = post->json->child;
	if (!child)
		return;
	
	post->size = cJSON_GetArraySize(child);
	post->event = (PEvent*)malloc(sizeof(PEvent) * post->size);	
	if (!post->event)
		return;
	
	memset(post->event, 0, sizeof(PEvent) * post->size);
	
	cJSON *temp = NULL;
	for (int i = 0; i < post->size; i++){
		temp = cJSON_GetObjectItem(cJSON_GetArrayItem(child, i), "event");
		if (temp){
			post->event[i].msg = temp->valuestring;
		}
		
		temp = cJSON_GetObjectItem(cJSON_GetArrayItem(child, i), "datetime");
		if (temp){
			post->event[i].datetime = temp->valuestring;
			sscanf(temp->valuestring, "%hu%*c%hu%*c%hu %hu%*c%hu", 
				&post->event[i].dt.year, &post->event[i].dt.month, &post->event[i].dt.day,
				&post->event[i].dt.hour, &post->event[i].dt.minutes
			);
			
			post_convertToTick(&post->event[i].dt, &post->event[i].tick);
		}
		
		temp = cJSON_GetObjectItem(cJSON_GetArrayItem(child, i), "repeat");
		if (temp)
			post->event[i].repeat = temp->valueint;
		
		char *datepart = NULL;
		temp = cJSON_GetObjectItem(cJSON_GetArrayItem(child, i), "datepart");
		if (temp){
			datepart = temp->valuestring;
			if (!strcmp(datepart, "hour"))
				post->event[i].part = HOUR;
			else if (!strcmp(datepart, "minute"))
				post->event[i].part = MINUTE;
			else if (!strcmp(datepart, "year"))
				post->event[i].part = YEAR;
			else if (!strcmp(datepart, "month"))
				post->event[i].part = MONTH;
			else if (!strcmp(datepart, "day"))
				post->event[i].part = DAY;
		}
	}
}


int post_convertToTick(const pspTime *t, u64 *tick){
	if (!t || !tick)
		return -1;
	
	return sceRtcGetTick(t, tick);
}

int post_convertToTimeStruct(const u64 *tick, pspTime *t){
	if (!t || !tick)
		return -1;
	
	return sceRtcSetTick(t, tick);
}

int post_removeEvent(PostIt *post, int event_index){
	if (post){
		if (post->json){
			int size = cJSON_GetArraySize(post->json->child);
			if (event_index < size){
				cJSON_DeleteItemFromArray(post->json->child, event_index);
				return 1;
			}
		}
	}
	return 0;
}


int post_addEvent(PostIt *post){
	if (!post)
		return 0;
	
	cJSON *new = NULL;
	cJSON *child = post->json->child;
	new = cJSON_CreateObject();
	if (new){
		if(cJSON_AddItemToArray(child, new))
			return 1;
		else
			cJSON_Delete(new);
	}
	return 0;
}


void post_addMessage(PostIt *post, char *msg){
	if (!post || !msg)
		return;
	
	int size = cJSON_GetArraySize(post->json->child);
	cJSON *j = cJSON_GetArrayItem(post->json->child, size - 1);
	if (j){
		cJSON *temp = NULL;
		char *buffer = (char*)malloc(sizeof(char) * (strlen(msg)+1));
		strcpy(buffer, msg);
		temp = cJSON_CreateString(buffer);
		if (temp){
			if (!cJSON_AddItemToObject(j, POST_IT_JSON_MESSAGE, temp))
				cJSON_Delete(temp);
		}
	}
}

void post_addDateTime(PostIt *post, pspTime datetime){
	if (!post)
		return;
	
	int size = cJSON_GetArraySize(post->json->child);
	cJSON *j = cJSON_GetArrayItem(post->json->child, size - 1);
	if (j){
		char *buffer = (char*)malloc(sizeof(char) * POST_IT_SIZE_DATETIME);
		if (buffer){
			sprintf(buffer, "%04hu-%02hu-%02hu %02hu:%02hu",
				datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minutes
			);
			cJSON *temp = NULL;
			temp = cJSON_CreateString(buffer);
			if (temp){
				if (!cJSON_AddItemToObject(j, POST_IT_JSON_DATETIME, temp))
					cJSON_Delete(temp);
			}
		}
	}
}

void post_addDatePart(PostIt *post, DatePart part){
	if (!post)
		return;
	
	int size = cJSON_GetArraySize(post->json->child);
	cJSON *j = cJSON_GetArrayItem(post->json->child, size - 1);
	if (j){
		char *buffer = (char*)malloc(sizeof(char) * POST_IT_SIZE_DATEPART);
		if (buffer){
			sprintf(buffer, "%s", 
				part == YEAR ? "year" : part == MONTH ? "month" : part == DAY ? "day" : 
				part == HOUR ? "hour" : part == MINUTE ? "minute" : "none"
			);
			cJSON *temp = NULL;
			temp = cJSON_CreateString(buffer);
			if (temp){
				if (!cJSON_AddItemToObject(j, POST_IT_JSON_DATEPART, temp))
					cJSON_Delete(temp);
			}
		}
	}
}

void post_addRepeat(PostIt *post, int repeat){
	if (!post)
		return;
	
	int size = cJSON_GetArraySize(post->json->child);
	cJSON *j = cJSON_GetArrayItem(post->json->child, size - 1);
	if (j){
		cJSON *temp = NULL;
		temp = cJSON_CreateNumber((double)repeat);
		if (temp){
			if (!cJSON_AddItemToObject(j, POST_IT_JSON_REPEAT, temp))
				cJSON_Delete(temp);
		}
	}
}


void post_displayEvents(int x, int y, PostIt* p, intraFont* font){
	if (!p || !font)
		return;
	
	int i;
	intraFontSetStyle(font, 0.9f, BLACK, DARKGRAY, 0.0f, 0);
	for (i = 0; i < p->size; i++){
		intraFontPrintf(font, x, y + 45*i, 
			"Event (%d): %s\n    Datetime: %04hu-%02hu-%02hu %02hu:%02hu\n    Datepart: %s\n    Repeat: %d\n",
			i, p->event[i].msg,
			p->event[i].dt.year, p->event[i].dt.month, p->event[i].dt.day, p->event[i].dt.hour, p->event[i].dt.minutes,
			!p->event[i].part ? "none" : 
				p->event[i].part == YEAR ? "year" : p->event[i].part == MONTH ? "month" : p->event[i].part == DAY ? "day" : 
				p->event[i].part == HOUR ? "hour" : p->event[i].part == MINUTE ? "minute" : "unkn",
			!p->event[i].repeat ?  0 : p->event[i].repeat
		);
	}
}

void post_displayJSON(int x, int y, PostIt* p, intraFont* font){
	if (!p || !font)
		return;
	
	int reset_x = x;
	char *buffer = cJSON_Print(p->json);
	intraFontSetStyle(font, 0.9f, BLACK, DARKGRAY, 0.0f, 0);
	for (int i = 0; i < strlen(buffer); i++){
		x = intraFontPrintf(font, x, y, "%c", buffer[i]);
		if (buffer[i] == '\n'){
			y += font->advancey * font->size / 4.0;
			x = reset_x;
		}else if (buffer[i] == '\t'){
			x += 4;
		}
	}
	free(buffer);
}


void post_freeAll(PostIt *post){
	if (post){
		post_freeJson(post);
		post_freeEvent(post);
		post_freePostIt(post);
	}
}

void post_freeJson(PostIt *post){
	if (post)
		if (post->json)
			cJSON_Delete(post->json);
}

void post_freeEvent(PostIt *post){
	if (post)
		if (post->event)
			free(post->event);
}

void post_freePostIt(PostIt *post){
	if (post)
		free(post);
}