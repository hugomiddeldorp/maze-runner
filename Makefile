FILE = main.c
OUT = main

all : $(FILE)
	gcc $(FILE) -w -lSDL2 -lSDL2_image -lSDL2_ttf -o $(OUT)
	./$(OUT)
