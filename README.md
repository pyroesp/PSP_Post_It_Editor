# PSP Post It - Editor

## Compiling
Use the [pspdev/pspdev docker image](https://hub.docker.com/r/pspdev/pspdev) and add [libintrafont](https://github.com/pspdev/libintraFont) to it.  

Go to the source folder and run ```docker run -it --rm -v ${PWD}:/home/code pspdev/intrafont:intraFont```.  
Navigate to ```/home/code``` and run ```make``` in it to compile.  
Run ```make clean``` to clean the folder.

Note that ```pspdev/intrafont:intraFont``` is a modified pspdev/pspdev image I made with intraFont added to it.  

## Controls
Navigate the menu with the up/down buttons.  
Select an option with X, return with O.  

## Legacy
Legacy code from the original homebrew is in the legacy branch.  
I've made some changes so it compiles today.  
Legacy code crashes on exit, haven't looked at why.  

The legacy concept was to display a string from a text file, when a set date and time was reached.  

## New code rewrite
The concept for the new code rewrite is to use a json file to setup events with the possibility to repeat these events.  

See below for an example of the json format:
```
{
	"post it":[
		{
			"event": "Birthday of X",
			"datetime": "2023-04-17 14:00"
			"datepart":"year",
			"repeat": 1
		},
		{
			"event": "Take out trash",
			"datetime": "2023-04-23 08:00"
		}
	]
}
```  
The datetime is formatted as ```YYYY-MM-DD HH:MM```, with the time part in 24-hour notation.
The datepart can be the following: ```year```, ```month```, ```day```, ```hour```, ```minute```.  
Datepart and repeat are optional.  

## Plugin
This homebrew should be used in conjuction with the PSP Post It PRX.  
At the time of writing it still needs to be updated to work with json files.  

## Credits
kevinr for the graphics library.  
raytwo for all the images.  
Benhur for [intraFont](https://github.com/pspdev/libintraFont).  
DaveGamble for [cJSON](https://github.com/DaveGamble/cJSON), at the time of writing the library is at v1.7.15.  

## License
All code, except cJSON, is under Creative Commons [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).  
cJSON is under MIT license, more info on the [cJSON github](https://github.com/DaveGamble/cJSON) page.  