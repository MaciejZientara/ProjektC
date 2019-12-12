compile:
	@gcc -o projekt main.c intro.c snake.c kbhit.c `pkg-config gtk+-3.0 --cflags --libs`
