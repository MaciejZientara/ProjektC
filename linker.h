#include <stdio.h>
#include <stdbool.h>
#include <gtk/gtk.h>

GtkWidget *Main;

struct pairGS{
	GtkWidget *G;
	char S[20];
}*P;

void TowDef();
void LandS();
void snake();
bool kbhit();
void dialog();
