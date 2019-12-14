#include <stdio.h>
#include <stdlib.h>
#include "linker.h"
#include <gtk/gtk.h>

static GtkWidget *ptr;

static void csoon(GtkWindow *widget)
{
	GtkWidget *okno, *tekst, *miejsce;
	GtkDialogFlags flaga;
	flaga=GTK_DIALOG_DESTROY_WITH_PARENT;
	okno=gtk_dialog_new_with_buttons("",(GtkWindow *)ptr,flaga,("OK"),GTK_RESPONSE_NONE,NULL);
	miejsce=gtk_dialog_get_content_area(GTK_DIALOG(okno));
	tekst=gtk_label_new("COMING SOON!");
	g_signal_connect_swapped(okno,"response",G_CALLBACK(gtk_widget_destroy),okno);
	gtk_container_add(GTK_CONTAINER(miejsce),tekst);
	gtk_widget_show_all(okno);
}

int main(int argc, char *argv[]){
	gtk_init(&argc,&argv);
	
	GtkWidget *main=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	ptr=main;
	g_signal_connect(G_OBJECT(main), "destroy",G_CALLBACK(gtk_main_quit), NULL);
	
	gtk_window_set_title(GTK_WINDOW(main),"PROJEKT");
	gtk_window_set_position(GTK_WINDOW(main),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(main), 10);
	
	GtkWidget *menu=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_container_add(GTK_CONTAINER(main),menu);
	
	GtkWidget *grid=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
	gtk_box_pack_start(GTK_BOX(menu), grid, TRUE, TRUE, 0);


	GtkWidget *powitanie=gtk_button_new_with_label("WELCOME!\nCHOOSE A GAME YOU WANT TO PLAY");
	gtk_grid_attach(GTK_GRID(grid),powitanie,0,0,1,1);
	

	GtkWidget *sneka=gtk_button_new_with_label("SNAKE");
	g_signal_connect(G_OBJECT(sneka),"clicked",G_CALLBACK(snake),NULL);
	gtk_grid_attach(GTK_GRID(grid),sneka,0,1,1,1);
	GtkWidget *td=gtk_button_new_with_label("TOWER DEFENCE");
	g_signal_connect(G_OBJECT(td),"clicked",G_CALLBACK(csoon),NULL);
	gtk_grid_attach(GTK_GRID(grid),td,0,2,1,1);
	GtkWidget *math=gtk_button_new_with_label("MATH");
	g_signal_connect(G_OBJECT(math),"clicked",G_CALLBACK(csoon),NULL);
	gtk_grid_attach(GTK_GRID(grid),math,0,3,1,1);
	GtkWidget *lands=gtk_button_new_with_label("LADDERS AND SNAKES");
	g_signal_connect(G_OBJECT(lands),"clicked",G_CALLBACK(csoon),NULL);
	gtk_grid_attach(GTK_GRID(grid),lands,0,4,1,1);
	GtkWidget *dam=gtk_button_new_with_label("DAM");
	g_signal_connect(G_OBJECT(dam),"clicked",G_CALLBACK(csoon),NULL);
	gtk_grid_attach(GTK_GRID(grid),dam,0,5,1,1);
	GtkWidget *exit=gtk_button_new_with_label("EXIT");
	g_signal_connect(G_OBJECT(exit),"clicked",G_CALLBACK(gtk_main_quit),NULL);
	gtk_grid_attach(GTK_GRID(grid),exit,0,6,1,1);

	gtk_widget_show_all(main);
	gtk_main();
	
	
	/*
	intro();
	while(true)
		if(kbhit()){
			char c;
			scanf("%c",&c);
			if(c=='a')//dac switch case
				snake();	
			printf("WOULD YOU LIKE TO PLAY ANOTHER GAME?\ny-yes n-no\n");
			while(true)
				if(kbhit){
					char x=getchar();
					if(x=='n'){
						printf("THANK YOU FOR PLAYING, SEE YOU NEXT TIME!\n");
						return 0;
					}
					else
						break;
				}
		}
	*/
}

