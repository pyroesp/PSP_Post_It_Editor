TARGET = PPiEditor
OBJS = main.o PPi_ED.o keyboard.o graphics.o framebuffer.o

INCDIR =
CFLAGS = -O2 -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS = 
LIBS = -lintrafont -lpspgum -lpspgu -lpng -lz -lm -lpspctrl

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PPi file editor
PSP_EBOOT_ICON = PPi_Icon.png
PSP_EBOOT_PIC1 = PPi_background.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

