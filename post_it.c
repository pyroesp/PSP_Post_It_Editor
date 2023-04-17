#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "post_it.h"


void waitOnPress(void){
	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);
	
	while(!(pad.Buttons & PSP_CTRL_CIRCLE)){
		sceCtrlReadBufferPositive(&pad, 1);
		sceKernelDelayThread(100000);
	}
	while(pad.Buttons & PSP_CTRL_CIRCLE){
		sceCtrlReadBufferPositive(&pad, 1);
		sceKernelDelayThread(100000);
	}
}


PostIt* post_readPostIt(cJSON *json){
	if (!json)
		return NULL;
	cJSON *child = json->child;
	if (!child)
		return NULL;
	
	PostIt *post = NULL;
	post = (PostIt*)malloc(sizeof(PostIt));
	if (!post)
		return NULL;
	
	post->size = cJSON_GetArraySize(child);
	post->event = (PostIt_Event*)malloc(sizeof(PostIt_Event) * post->size);
	
	if (!post->event)
		return NULL;
	
	cJSON *temp = NULL;
	
	for (int i = 0; i < post->size; i++){
		temp = cJSON_GetObjectItem(cJSON_GetArrayItem(child, i), "event");
		if (temp)
			post->event[i].string = temp->valuestring;
		
		temp = cJSON_GetObjectItem(cJSON_GetArrayItem(child, i), "datetime");
		if (temp){
			post->event[i].date = temp->valuestring;
			sscanf(temp->valuestring, "%d%*c%d%*c%d %d%*c%d", 
				&post->event[i].dt.year, &post->event[i].dt.month, &post->event[i].dt.day,
				&post->event[i].dt.hour, &post->event[i].dt.minute
			);
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
	
	return post;
}


void post_free(PostIt *post){
	free(post->event);
	free(post);
}