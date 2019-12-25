#include <stdio.h>
#include "linker.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <gtk/gtk.h>
//#include <conio.h>

#define N 10
#define HEAD tab[Hx][Hy]
#define TAIL tab[Tx][Ty]

static int counter;
static int Hx,Hy,Tx,Ty;
static bool czyZjedzone;

GtkWidget *Snake;
GtkWidget *plansza;

GtkWidget *TabButton[N+2][N+2];

static struct parametry{
	bool head;
	bool isSnake;
	bool tail;
	bool fruit;
	bool isWall;
	bool nextU;
	bool nextD;
	bool nextL;
	bool nextR;

}tab[N+2][N+2];

static void cl(int n, int m){
	tab[n][m].head=false;
	tab[n][m].isSnake=false;
	tab[n][m].tail=false;
	tab[n][m].fruit=false;
	tab[n][m].isWall=false;
	tab[n][m].nextU=false;
	tab[n][m].nextD=false;
	tab[n][m].nextL=false;
	tab[n][m].nextR=false;	
}

static bool koniec(){
	if(tab[Hx-1][Hy].isWall||tab[Hx-1][Hy].isSnake)
		if(tab[Hx+1][Hy].isWall||tab[Hx+1][Hy].isSnake)
			if(tab[Hx][Hy-1].isWall||tab[Hx][Hy-1].isSnake)
				if(tab[Hx][Hy+1].isWall||tab[Hx][Hy+1].isSnake)
					return true;
	return false;
}

static int wynik(){
	if(counter==N*N-1)
		return 1;//wygrana
	if(koniec())
		return 2;//przegrana
	return 0;//neutralne
}

static void spawn(){	
	for(int i=0; i<N+2; i++)
		for(int j=0; j<N+2; j++)
			if(i==0 || j==0 || i==N+1 || j==N+1)
				tab[i][j].isWall=true;
	srand(time(0));
	int tmp=rand()%(N*N);
	int n=tmp%N+1,m=(tmp/N)%N+1;
	tab[n][m].head=true;
	tab[n][m].isSnake=true;
	Hx=n,Hy=m;
	if(tab[n-1][m].isWall==false){
		tab[n-1][m].nextD=true;
		tab[n-1][m].tail=true;
		tab[n-1][m].isSnake=true;
		Tx=n-1,Ty=m;
		return;
	}
	if(tab[n+1][m].isWall==false){
		tab[n+1][m].nextU=true;
		tab[n+1][m].tail=true;
		tab[n+1][m].isSnake=true;
		Tx=n+1,Ty=m;
		return;
	}
	if(tab[n][m-1].isWall==false){
		tab[n][m-1].nextR=true;
		tab[n][m-1].tail=true;
		tab[n][m-1].isSnake=true;
		Tx=n,Ty=m-1;
		return;
	}
	if(tab[n][m+1].isWall==false){
		tab[n][m+1].nextL=true;
		tab[n][m+1].tail=true;
		tab[n][m+1].isSnake=true;
		Tx=n,Ty=m+1;
		return;
	}
}

static void fruit(){
	if(counter==N*N)
		return;
	srand(time(0));
	while(true){
		int tmp=rand()%(N*N);
		int n=tmp%N+1,m=(tmp/N)%N+1;
		if(!tab[n][m].isSnake){
			tab[n][m].fruit=true;
			return;
		}
	}
}

static void zjedzenie(){
	counter++;
	czyZjedzone=true;
}

static void wyswietl(){
	for(int i=0; i<N+2; i++){
		for(int j=0; j<N+2; j++){
			if(tab[i][j].isWall){
				printf("@");
				continue;
			}
			if(tab[i][j].head){
				printf("H");
				continue;
			}
			if(tab[i][j].tail){
				printf("T");
				continue;
			}
			if(tab[i][j].isSnake){
				printf("B");
				continue;
			}
			if(tab[i][j].fruit){
				printf("*");
				continue;
			}
			printf(" ");
		}	
		if(i==0)printf("\t\t\t\t\t\t\tscore=%d",counter);
		if(i==1)printf("\t\t\t\t\t\t\tmove using wsad");
		if(i==2)printf("\t\t\t\t\t\t\tsave with 1, load with 2, exit with 3");
		printf("\n");
	}

}

static void MakePlansza(){	
	for(int i=0; i<N+2; i++)
		for(int j=0; j<N+2; j++){
			if(tab[i][j].isWall){
				TabButton[i][j]=gtk_button_new_with_label("@");
				gtk_grid_attach(GTK_GRID(plansza),TabButton[i][j],j,i,1,1);
				continue;
			}
			if(tab[i][j].head){
				TabButton[i][j]=gtk_button_new_with_label("H");
				gtk_grid_attach(GTK_GRID(plansza),TabButton[i][j],j,i,1,1);
				continue;
			}
			if(tab[i][j].tail){
				TabButton[i][j]=gtk_button_new_with_label("T");
				gtk_grid_attach(GTK_GRID(plansza),TabButton[i][j],j,i,1,1);
				continue;
			}
			if(tab[i][j].isSnake){
				TabButton[i][j]=gtk_button_new_with_label("B");
				gtk_grid_attach(GTK_GRID(plansza),TabButton[i][j],j,i,1,1);
				continue;
			}
			if(tab[i][j].fruit){
				TabButton[i][j]=gtk_button_new_with_label("*");
				gtk_grid_attach(GTK_GRID(plansza),TabButton[i][j],j,i,1,1);
				continue;
			}
			TabButton[i][j]=gtk_button_new_with_label("");
			gtk_grid_attach(GTK_GRID(plansza),TabButton[i][j],j,i,1,1);
		}
}

static void wyswietlGTK(){	
	for(int i=0; i<N+2; i++)
		for(int j=0; j<N+2; j++){
			if(tab[i][j].isWall){
				gtk_button_set_label((GtkButton*)TabButton[i][j],"@");
				continue;
			}
			if(tab[i][j].head){
				gtk_button_set_label((GtkButton*)TabButton[i][j],"H");
				continue;
			}
			if(tab[i][j].tail){
				gtk_button_set_label((GtkButton*)TabButton[i][j],"T");
				continue;
			}
			if(tab[i][j].isSnake){
				gtk_button_set_label((GtkButton*)TabButton[i][j],"B");
				continue;
			}
			if(tab[i][j].fruit){
				gtk_button_set_label((GtkButton*)TabButton[i][j],"*");
				continue;
			}
			gtk_button_set_label((GtkButton*)TabButton[i][j]," ");
		}
}

static void ruch(char c){
	if(c=='w')
		if(!tab[Hx-1][Hy].isWall&&!tab[Hx-1][Hy].isSnake){
			HEAD.nextU=true;
			HEAD.head=false;
			Hx--;
		}
	if(c=='a')
		if(!tab[Hx][Hy-1].isWall&&!tab[Hx][Hy-1].isSnake){
			HEAD.nextL=true;
			HEAD.head=false;
			Hy--;
		}
	if(c=='s')
		if(!tab[Hx+1][Hy].isWall&&!tab[Hx+1][Hy].isSnake){
			HEAD.nextD=true;
			HEAD.head=false;
			Hx++;
		}
	if(c=='d')
		if(!tab[Hx][Hy+1].isWall&&!tab[Hx][Hy+1].isSnake){
			HEAD.nextR=true;
			HEAD.head=false;
			Hy++;
		}
	HEAD.isSnake=true;
	HEAD.head=true;
	
	if(czyZjedzone)
		czyZjedzone=false;
	else{
		if(TAIL.nextU){
			cl(Tx,Ty);
			Tx--;
			TAIL.tail=true;
			return;
		}
		if(TAIL.nextL){
			cl(Tx,Ty);
			Ty--;
			TAIL.tail=true;
			return;
		}
		if(TAIL.nextD){
			cl(Tx,Ty);
			Tx++;
			TAIL.tail=true;
			return;
		}
		if(TAIL.nextR){
			cl(Tx,Ty);
			Ty++;
			TAIL.tail=true;
			return;
		}
	}
	
}
 
static void newGame(){
	spawn();
	fruit();
	counter=1;
}

static void ruchGTK(char c){
	if(c=='w')
		if(tab[Hx-1][Hy].isWall||tab[Hx-1][Hy].isSnake)
			return;
	if(c=='a')
		if(tab[Hx][Hy-1].isWall||tab[Hx][Hy-1].isSnake)
			return;
	if(c=='s')
		if(tab[Hx+1][Hy].isWall||tab[Hx+1][Hy].isSnake)
			return;
	if(c=='d')
		if(tab[Hx][Hy+1].isWall||tab[Hx][Hy+1].isSnake)
			return;
	ruch(c);
	
	if(HEAD.fruit){
		HEAD.fruit=false;
		fruit();
		zjedzenie();
	}

	wyswietlGTK();	
	
	int tmp=wynik();
	if(tmp){
		//printf("%d\n",tmp);
		if(tmp==1){
			free(P);
			P=calloc(sizeof(struct pairGS),1);
			P->G=Snake;
			strcpy(P->S,"YOU WIN!");
			dialog();
			return;
		}
		else{
			free(P);
			P=calloc(sizeof(struct pairGS),1);
			P->G=Snake;
			strcpy(P->S,"YOU LOSE!");
			dialog();
//			gtk_main_quit();//???
			//printf("YOUR SCORE = %d\n",counter);
		}
		return;					//i co dalej?
	}
	return;
}

static void rW(){ruchGTK('w');return;}
static void rA(){ruchGTK('a');return;}
static void rS(){ruchGTK('s');return;}
static void rD(){ruchGTK('d');return;}

static GtkWidget* gra(){
	newGame();
	GtkWidget *Snake=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(Snake),"SNAKE");
	gtk_window_set_position(GTK_WINDOW(Snake),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(Snake),10);
	g_signal_connect(G_OBJECT(Snake),"destroy",G_CALLBACK(gtk_main_quit),NULL);

	GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_container_add(GTK_CONTAINER(Snake),box);	

	plansza=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(plansza), 0);
	gtk_grid_set_row_homogeneous(GTK_GRID(plansza), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(plansza), 0);
	gtk_grid_set_column_homogeneous(GTK_GRID(plansza), TRUE);
	gtk_box_pack_start((GtkBox*)box,plansza,TRUE,FALSE,0);

	MakePlansza();

	GtkWidget *wsad=gtk_grid_new();	
	gtk_grid_set_row_spacing(GTK_GRID(wsad), 0);
	gtk_grid_set_row_homogeneous(GTK_GRID(wsad), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(wsad), 0);
	gtk_grid_set_column_homogeneous(GTK_GRID(wsad), TRUE);
	gtk_box_pack_start((GtkBox*)box,wsad,TRUE,FALSE,0);

	GtkWidget *W=gtk_button_new_with_label("W");
	g_signal_connect(G_OBJECT(W),"clicked",G_CALLBACK(rW),NULL);
	gtk_grid_attach(GTK_GRID(wsad),W,1,0,1,1);
	
	GtkWidget *A=gtk_button_new_with_label("A");
	g_signal_connect(G_OBJECT(A),"clicked",G_CALLBACK(rA),NULL);
	gtk_grid_attach(GTK_GRID(wsad),A,0,1,1,1);
	
	GtkWidget *S=gtk_button_new_with_label("S");
	g_signal_connect(G_OBJECT(S),"clicked",G_CALLBACK(rS),NULL);
	gtk_grid_attach(GTK_GRID(wsad),S,1,1,1,1);
	
	GtkWidget *D=gtk_button_new_with_label("D");
	g_signal_connect(G_OBJECT(D),"clicked",G_CALLBACK(rD),NULL);
	gtk_grid_attach(GTK_GRID(wsad),D,2,1,1,1);

	wyswietlGTK();
	return Snake;
}

void snake(){
	gtk_widget_hide(Main);
	//newGame();	
	Snake=gra();
	gtk_widget_show_all(Snake);
	return;
/*
	system("clear");

	while(true){
		int tmp=wynik();
		if(tmp){
			system("clear");
			if(tmp==1){
				printf("YOU WIN!");
			}
			else{
				printf("YOU LOSE!\n");
				printf("YOUR SCORE = %d\n",counter);
			}
			break;					//i co dalej?
		}
		
		system("clear");
		wyswietl();
		while(true){//petla do ruchu
			if(kbhit()){
				system("clear");
				wyswietl();
			//if(kbhit()){
				char c=getchar();
				if(c=='\n')
					continue;
				if(c=='3'){
					system("clear");
					return;
				}
				//potencjalne wyjscie, save, load, newGame dla 1,2,3,4
				if(c=='w')
					if(tab[Hx-1][Hy].isWall||tab[Hx-1][Hy].isSnake)
						continue;
				if(c=='a')
					if(tab[Hx][Hy-1].isWall||tab[Hx][Hy-1].isSnake)
						continue;
				if(c=='s')
					if(tab[Hx+1][Hy].isWall||tab[Hx+1][Hy].isSnake)
						continue;
				if(c=='d')
					if(tab[Hx][Hy+1].isWall||tab[Hx][Hy+1].isSnake)
						continue;
				if(c=='w'||c=='a'||c=='s'||c=='d'){
					ruch(c);
					break;
				}
			}
		}
		if(HEAD.fruit){
			HEAD.fruit=false;
			fruit();
			zjedzenie();
		}
	}*/
}
