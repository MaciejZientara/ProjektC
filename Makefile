compile:
	@gcc -o projekt main.c snake.c kbhit.c gtk.c ladder_snake.c `pkg-config gtk+-3.0 --cflags --libs`
