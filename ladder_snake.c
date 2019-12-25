#include <stdio.h>
#include <gtk/gtk.h>
#include "linker.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define N 10

GtkWidget *LS;
GtkWidget *TabButton[N+2][N+2];
GtkWidget *plansza;
GtkWidget *box;
GtkWidget *IleGraczy;

int PozGracz[3][2];
int tura=0,ILE;

static struct parametry{
	char *nr;
	bool isWall;
	bool gracz[3];
	int ladder;
	int snake;
}tab[N+2][N+2];

static void update(){
	for(int i=0; i<(N+2); i++)
		for(int j=0; j<(N+2); j++)
			if(tab[i][j].isWall==0){
				char a, b, c;
				if(tab[i][j].gracz[0])
					a='X';
				else
					a=' ';
				if(tab[i][j].gracz[1])
					b='Y';
				else
					b=' ';
				if(tab[i][j].gracz[2])
					c='Z';
				else
					c=' ';
				int tmpp;
				if(i%2==1)
					tmpp=N*(N-i)+(N-j+1);
				else	
					tmpp=N*(N-i)+j;
				char *tmp=g_strdup_printf("%d\n%c%c%c",tmpp,a,b,c);
				//printf("%s\n",tmp);
				gtk_button_set_label((GtkButton*)TabButton[i][j],tmp);
			}
}

static void ichi(){
	gtk_widget_destroy(IleGraczy);
	gtk_container_add(GTK_CONTAINER(LS),box);
	gtk_widget_show_all(box);
	for(int i=0; i<1; i++){
		PozGracz[i][0]=N;
		PozGracz[i][1]=1;
		tab[N][1].gracz[i]=true;
	}
	update();
	ILE=1;
}

static void ni(){
	gtk_widget_destroy(IleGraczy);
	gtk_container_add(GTK_CONTAINER(LS),box);
	gtk_widget_show_all(box);
	for(int i=0; i<2; i++){
		PozGracz[i][0]=N;
		PozGracz[i][1]=1;
		tab[N][1].gracz[i]=true;
	}
	update();
	ILE=2;
}

static void san(){
	gtk_widget_destroy(IleGraczy);
	gtk_container_add(GTK_CONTAINER(LS),box);
	gtk_widget_show_all(box);
	for(int i=0; i<3; i++){
		PozGracz[i][0]=N;
		PozGracz[i][1]=1;
		tab[N][1].gracz[i]=true;
	}
	update();
	ILE=3;
}

static void InitPlansza(){
	for(int i=0; i<(N+2); i++)
		for(int j=0; j<(N+2); j++){
			if(i==0 || j==0 || i==N+1 || j==N+1)
				tab[i][j].isWall=true;
			else{
				if(i%2==1)
					tab[i][j].nr=g_strdup_printf("%d",N*(N-i)+(N-j+1));
				else	
					tab[i][j].nr=g_strdup_printf("%d",N*(N-i)+j);
			}
		}
/* //debug
	for(int i=0; i<(N+2); i++){
		for(int j=0; j<(N+2); j++){
			if(tab[i][j].isWall)
				printf("@ ");
			else{
				if(i%2==1)
					printf("%s ",tab[i][j].nr);
				else	
					printf("%s ",tab[i][j].nr);
			}
		}
	printf("\n");
	}
*/
	for(int i=0; i<(N+2); i++)
		for(int j=0; j<(N+2); j++){
			if(tab[i][j].isWall)
				TabButton[i][j]=gtk_button_new_with_label("@");
			else{
				char a, b, c;
				if(tab[i][j].gracz[0])
					a='X';
				else
					a=' ';
				if(tab[i][j].gracz[1])
					b='Y';
				else
					b=' ';
				if(tab[i][j].gracz[2])
					c='Z';
				else
					c=' ';
				char *tmp=g_strdup_printf("%s\n%c%c%c",tab[i][j].nr,a,b,c);
				TabButton[i][j]=gtk_button_new_with_label(tmp);
			}
			gtk_grid_attach(GTK_GRID(plansza),TabButton[i][j],j,i,1,1);
		}
}

static void DiceRoll(){
	srand(time(0));
	int tmp=rand()%6+1;
	free(P);
	P=calloc(sizeof(struct pairGS),1);
	P->G=LS;
	if(tura==0)
	strcpy(P->S,g_strdup_printf("X GOT %d!",tmp));
	if(tura==1)
	strcpy(P->S,g_strdup_printf("Y GOT %d!",tmp));
	if(tura==2)
	strcpy(P->S,g_strdup_printf("Z GOT %d!",tmp));
	dialog();
	
	for(int q=0; q<tmp; q++){
		int i=PozGracz[tura][0];
		int j=PozGracz[tura][1];
		if(i%2==0){
			if(j==N){
				tab[i][j].gracz[tura]=false;
				tab[i-1][j].gracz[tura]=true;
				PozGracz[tura][0]--;
			}
			else{	
				tab[i][j].gracz[tura]=false;
				tab[i][j+1].gracz[tura]=true;
				PozGracz[tura][1]++;
			}	
		}
		else{
			if(j==1){
				tab[i][j].gracz[tura]=false;
				tab[i-1][j].gracz[tura]=true;
				PozGracz[tura][0]--;
			}
			else{	
				tab[i][j].gracz[tura]=false;
				tab[i][j-1].gracz[tura]=true;
				PozGracz[tura][1]--;
			}	
		}
		update();
	}
	//waz/drabina
	//losowanie waz/drabina N razy	


	//if(win)...
	tura=(tura+1)%ILE;
	return;
}

static GtkWidget* gra(){
	LS=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(LS),"Ladders and Snakes");
	gtk_window_set_position(GTK_WINDOW(LS),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(LS),10);
	g_signal_connect(G_OBJECT(LS),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	//pseudoLS=LS;
	//GtkWidget *background=gtk_layout_new(NULL,NULL);

	box=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);


	plansza=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(plansza), 0);
	gtk_grid_set_row_homogeneous(GTK_GRID(plansza), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(plansza), 0);
	gtk_grid_set_column_homogeneous(GTK_GRID(plansza), TRUE);

	gtk_box_pack_start((GtkBox*)box,plansza,TRUE,FALSE,0);

	InitPlansza();
	
	GtkWidget *roll=gtk_button_new_with_label("DICE\nROLL");
	g_signal_connect(G_OBJECT(roll),"clicked",G_CALLBACK(DiceRoll),NULL);
	gtk_box_pack_start((GtkBox*)box,roll,TRUE,FALSE,0);

//	pop=gtk_dialog_new_with_buttons("HOW MANY PLAYERS?",(GtkWindow*)LS,GTK_DIALOG_DESTROY_WITH_PARENT,"1",G_CALLBACK(ichi),"2",ni,"3",san,NULL);
//	gtk_widget_show_all(pop);


	IleGraczy=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
//	gtk_grid_set_row_spacing(GTK_GRID(IleGraczy), 5);
//	gtk_grid_set_row_homogeneous(GTK_GRID(IleGraczy), TRUE);
//	gtk_grid_set_column_spacing(GTK_GRID(IleGraczy), 5);
//	gtk_grid_set_column_homogeneous(GTK_GRID(IleGraczy), TRUE);
	
	gtk_container_add(GTK_CONTAINER(LS),IleGraczy);

	GtkWidget *ile=gtk_button_new_with_label("How many players?");
	//gtk_grid_attach(GTK_GRID(LS),ile,0,0,1,1);
	gtk_box_pack_start((GtkBox*)IleGraczy,ile,TRUE,FALSE,0);
	GtkWidget *jeden=gtk_button_new_with_label("1");
	g_signal_connect(G_OBJECT(jeden),"clicked",G_CALLBACK(ichi),NULL);
	//gtk_grid_attach(GTK_GRID(LS),jeden,0,1,1,1);
	gtk_box_pack_start((GtkBox*)IleGraczy,jeden,TRUE,FALSE,0);
	GtkWidget *dwa=gtk_button_new_with_label("2");
	g_signal_connect(G_OBJECT(dwa),"clicked",G_CALLBACK(ni),NULL);
	//gtk_grid_attach(GTK_GRID(LS),dwa,0,2,1,1);
	gtk_box_pack_start((GtkBox*)IleGraczy,dwa,TRUE,FALSE,0);
	GtkWidget *trzy=gtk_button_new_with_label("3");
	g_signal_connect(G_OBJECT(trzy),"clicked",G_CALLBACK(san),NULL);
	//gtk_grid_attach(GTK_GRID(LS),trzy,0,3,1,1);	
	gtk_box_pack_start((GtkBox*)IleGraczy,trzy,TRUE,FALSE,0);
	

	//grid na box, potem widget_hide, object_ref, container_remove i add nowy

	//update, dialog, update, no box
	
	//wybor ile graczy

	//wyswietl plansze i roll button

	//dialog na ile oczek na kostce

	//jak plansza???????


	return LS;
}

void LandS(){
	gtk_widget_hide(Main);
	GtkWidget *LSd=gra();
	gtk_widget_show_all(LSd);
	return;
}
