OBJECTS = cs_main.o
HEADERS = cs_data.h
CC = gcc
OUTPUT = 
DEBUG = -Wall
LINKING_OPTIONS = 
CFLAGS = $(shell allegro-config --cflags)
LDFLAGS = $(shell allegro-config --libs)
WIN_CC = wine c:\\MinGW\\bin\\gcc.exe
WIN_OUTPUT = princess.exe

princess: $(OBJECTS)
	 $(CC) $(DEBUG) $(LINKING_OPTIONS) -o princess $(OBJECTS) $(LDFLAGS)

cs_main.o: cs_main.c $(HEADERS)
	 $(CC) $(DEBUG) -c cs_main.c $(CFLAGS)

clean:
	 rm -f $(OBJECTS)

win:
	 $(WIN_CC) $(DEBUG) *.c -o $(WIN_OUTPUT) -lalleg

