TARGET = PSP_Post_It_Editor
OBJS = main.o graphics.o framebuffer.o post_it.o osk.o gui.o ./cJSON/cJSON.o

INCDIR = ./cJSON
CFLAGS = -O2 -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ./cJSON
LDFLAGS = 
LIBS = -lintrafont -lpspgum -lpspgu -lpng -lz -lm -lpspctrl

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSP Post It - Editor
PSP_EBOOT_ICON = PPi_Icon.png
PSP_EBOOT_PIC1 = PPi_background.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

