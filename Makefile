FILE = main.c
OUT = main

all : $(FILE)
	gcc-11 $(FILE) -w -lSDL2 -lSDL2_image -lSDL2_ttf -o $(OUT)
	./$(OUT)
