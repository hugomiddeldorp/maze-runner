FILE = main.c
OUT = main

all : $(FILE)
	gcc $(FILE) -w -lSDL2 -lSDL2_image -o $(OUT)
	./$(OUT)
