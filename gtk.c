#include <gtk/gtk.h>
#include <stdio.h>
#include "linker.h"

void dialog(){
	GtkWidget *okno, *tekst, *miejsce;
	GtkDialogFlags flaga;
	flaga=GTK_DIALOG_DESTROY_WITH_PARENT;
	okno=gtk_dialog_new_with_buttons("",(GtkWindow*)P->G,flaga,("OK"),GTK_RESPONSE_NONE,NULL);
	miejsce=gtk_dialog_get_content_area(GTK_DIALOG(okno));
	tekst=gtk_label_new(P->S);
	g_signal_connect_swapped(okno,"response",G_CALLBACK(gtk_widget_destroy),okno);
	gtk_container_add(GTK_CONTAINER(miejsce),tekst);
	gtk_widget_show_all(okno);
}
