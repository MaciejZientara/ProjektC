#include <stdio.h>
#include <stdbool.h>
#include <gtk/gtk.h>

GtkWidget *Main;

struct pairGS{
	GtkWidget *G;
	char S[15];
}*P;

void snake();
bool kbhit();
void dialog();
