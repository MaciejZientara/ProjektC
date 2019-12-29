#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include "linker.h"

#define N 20

GtkWidget *TD,*zakup;
GtkWidget *LevelButton[10];
GtkWidget *UpgradeButton[7];
GtkWidget *IleStar;
GtkWidget *AchivementButton[7];
GtkWidget *TabButton[N+2][N+2];//tylko raz init planszy i przyciskow, potem update
GtkWidget *plansza;//przy okazji zloto, zycie i goldMult//pamietaj o signal connect na zakup
GtkWidget *TabImage[5];
GtkWidget *Box[5];//0-world,1-upgrade,2-achivement,3-level,4-zakup wiez

int unlocked[10];
int upgrade[3];

static void save(){
	FILE *sv=fopen("TDsave.txt","w");
	for(int i=0; i<10; i++)
		fprintf(sv,"%d",unlocked[i]);
	for(int i=0; i<3; i++)
		fprintf(sv,"%d",upgrade[i]);
	fclose(sv);
}

static void reset(){
	unlocked[0]=0;
	for(int i=1; i<10; i++)
		unlocked[i]=7;
	for(int i=0; i<3; i++)
		upgrade[i]=0;
	save();
}

static void wq(){
	//save();			//!!!!!
	gtk_main_quit();
}

struct tower{
	//int type;
	int range;
	int *cover;
	int cost;
	int damage;
	int level;
};

struct enemy{
	int life;
	int count;
};

struct parametry{
	int nr;//do drogi
	bool S,F;
	bool isWall;
	bool road;
	bool land;
	int ileB;
	int lifeB;
	struct tower tow;
	struct enemy ene;
};

struct RoadNr{
	int nr;
	int x;
	int y;
	bool enemy;
};

struct level{
	int Sx,Sy,Fx,Fy;
	int ileEnemy;
	struct enemy *ene;//dynamiczna tab, jak -1 to koniec tury?
	struct RoadNr *ROAD;//dfs od S do F po R i kolejne pole to kolejny numer drogi
	struct parametry tab[N+2][N+2];
}poziomy[10];

int Q,X,Y;//!!!

static void Tsingle(){



}

static void Tmass(){


}

static void clickZakup(GtkWidget *button, gpointer user_data){
	//int current_pos = (gint) gtk_object_get_data (GTK_OBJECT(button), "current_pos");
//	X=(current_pos-1)/(N+2)+1;
//	Y=current_pos%(N+2);
	//open window, let choose
}

static void upgradeZakup(){


}





//int gold;
//int life;
//int GoldMult;





static void DFS(int q, int x, int y){
	int nr=1;
	while(poziomy[q].tab[x][y].F==false){
		poziomy[q].tab[x][y].nr=nr;
		poziomy[q].ROAD[nr-1].nr=nr;
		poziomy[q].ROAD[nr-1].x=x;
		poziomy[q].ROAD[nr-1].y=y;
		nr++;
		if(poziomy[q].tab[x-1][y].road && poziomy[q].tab[x][y].nr==0){x--;continue;}
		if(poziomy[q].tab[x+1][y].road && poziomy[q].tab[x][y].nr==0){x++;continue;}
		if(poziomy[q].tab[x][y-1].road && poziomy[q].tab[x][y].nr==0){y--;continue;}
		if(poziomy[q].tab[x][y+1].road && poziomy[q].tab[x][y].nr==0){y++;continue;}
	}
	poziomy[q].tab[x][y].nr=nr;
	poziomy[q].ROAD[nr-1].nr=nr;
	poziomy[q].ROAD[nr-1].x=x;
	poziomy[q].ROAD[nr-1].y=y;
	nr++;
	//realloc(poziomy[q].ROAD,nr*sizeof(struct RoadNr));//?
}

static void init(){
//tower,enemy,level,achivement,grapic
	

	FILE *level=fopen("level","r");
	for(int q=0;q<1;q++){//q<1->10!!!
		int ileRoad=0;
		for(int i=0; i<N+2; i++)
			for(int j=0; j<N+2; j++){
				char c;
				fscanf(level,"%c",&c);
				if(c=='\n')
					fscanf(level,"%c",&c);
				if(c=='@')
					poziomy[q].tab[i][j].isWall=true;
				else{
					if(c=='L')
						poziomy[q].tab[i][j].land=true;
					else{
						ileRoad++;
						poziomy[q].tab[i][j].road=true;
						if(c=='S'){
							poziomy[q].tab[i][j].S=true;
							poziomy[q].Sx=i,poziomy[q].Sy=j;
						}
						if(c=='F'){
							poziomy[q].tab[i][j].F=true;
							poziomy[q].Fx=i,poziomy[q].Fy=j;
						}
					}
				}
			}
		poziomy[q].ROAD=calloc(ileRoad*sizeof(struct RoadNr),1);
		fscanf(level,"%d",&poziomy[q].ileEnemy);
		poziomy[q].ene=malloc(poziomy[q].ileEnemy*sizeof(struct enemy));
		for(int i=0; i<poziomy[q].ileEnemy; i++)
			fscanf(level,"%d %d",&poziomy[q].ene->life, &poziomy[q].ene->count);
	
		DFS(q,poziomy[q].Sx,poziomy[q].Sy);
	
	//debug	
		for(int i=0; i<N+2; i++){
			for(int j=0; j<N+2; j++){
				if(poziomy[q].tab[i][j].isWall){putchar('@');continue;}
				if(poziomy[q].tab[i][j].road){putchar('R');continue;}
				if(poziomy[q].tab[i][j].land){putchar('L');continue;}
			}
			printf("\n");
		}
	
	}
	fclose(level);
	
	FILE *sv=fopen("TDsave.txt","r");
	if(sv==NULL)
		reset();
	else{
		for(int i=0; i<10; i++)
			fscanf(sv,"%d",&unlocked[i]);
		for(int i=0; i<3; i++)
			fscanf(sv,"%d",&upgrade[i]);
		fclose(sv);
	}

	//graphic init
}

static void update(){//trzeba podzielic update na poszczegolne boxy
	
	
}


static void hideup(){
	g_object_ref(Box[1]);
	gtk_container_remove(GTK_CONTAINER(TD),Box[1]);
	gtk_container_add(GTK_CONTAINER(TD),Box[0]);
	gtk_widget_show_all(Box[0]);
}//1->0

static void hideach(){
	g_object_ref(Box[2]);
	gtk_container_remove(GTK_CONTAINER(TD),Box[2]);
	gtk_container_add(GTK_CONTAINER(TD),Box[0]);
	gtk_widget_show_all(Box[0]);
}//2->0

static void showup(){
	g_object_ref(Box[0]);
	gtk_container_remove(GTK_CONTAINER(TD),Box[0]);
	gtk_container_add(GTK_CONTAINER(TD),Box[1]);
	gtk_widget_show_all(Box[1]);
}//0->1

static void showach(){
	g_object_ref(Box[0]);
	gtk_container_remove(GTK_CONTAINER(TD),Box[0]);
	gtk_container_add(GTK_CONTAINER(TD),Box[2]);
	gtk_widget_show_all(Box[2]);
}//0->2

static GtkWidget* gra(){
	TD=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(TD),"TOWER DEFENCE");
	gtk_window_set_position(GTK_WINDOW(TD),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(TD),10);
	g_signal_connect(G_OBJECT(TD),"destroy",G_CALLBACK(wq),NULL);

	//init();!!!

	Box[0]=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);//world//do init X a jak odbl to nr i ile *
	Box[1]=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);//upgrade
	Box[2]=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);//achivement
	Box[3]=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);//level
	Box[4]=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);//zakup wiez

	//plansza init
	plansza=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(plansza),0);
	gtk_grid_set_row_homogeneous(GTK_GRID(plansza),TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(plansza),0);
	gtk_grid_set_column_homogeneous(GTK_GRID(plansza),TRUE);
	
	for(int i=0; i<N+2; i++)
		for(int j=0; j<N+2; j++){
			TabButton[i][j]=gtk_button_new();
		//	gtk_object_set_data (GTK_OBJECT(TabButton[i][j]), "current_pos", (gpointer)(i*(N+2)+j));
			gtk_grid_attach(GTK_GRID(plansza),TabButton[i][j],j,i,1,1);
		}
	//world init
	GtkWidget *RESET=gtk_button_new_with_label("RESET\nPROGRESS");
	g_signal_connect(G_OBJECT(RESET),"clicked",G_CALLBACK(reset),NULL);
	gtk_box_pack_start((GtkBox*)Box[0],RESET,FALSE,FALSE,0);
	
	GtkWidget *gridLevel=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(gridLevel),5);
	gtk_grid_set_row_homogeneous(GTK_GRID(gridLevel),TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(gridLevel),5);
	gtk_grid_set_column_homogeneous(GTK_GRID(gridLevel),TRUE);
	for(int i=0; i<10; i++){
		LevelButton[i]=gtk_button_new_with_label("X");
		//gtk_object_set_data(GTK_OBJECT(LevelButton[i]),"wybrany_poziom",(gpointer)i);
		gtk_grid_attach(GTK_GRID(gridLevel),LevelButton[i],i%5,i/5,1,1);
	}
	gtk_box_pack_start((GtkBox*)Box[0],gridLevel,TRUE,FALSE,0);

	GtkWidget *UPGRADE=gtk_button_new_with_label("UPGRADE");
	g_signal_connect(G_OBJECT(UPGRADE),"clicked",G_CALLBACK(showup),NULL);
	gtk_box_pack_start((GtkBox*)Box[0],UPGRADE,TRUE,FALSE,0);	
	GtkWidget *ACHIVEMENT=gtk_button_new_with_label("ACHIVEMENT");
	g_signal_connect(G_OBJECT(ACHIVEMENT),"clicked",G_CALLBACK(showach),NULL);
	gtk_box_pack_start((GtkBox*)Box[0],ACHIVEMENT,TRUE,FALSE,0);

	gtk_container_add(GTK_CONTAINER(TD),Box[0]);

	//upgrade init
	IleStar=gtk_button_new_with_label("0/30");
	gtk_box_pack_start((GtkBox*)Box[1],IleStar,TRUE,FALSE,0);

	GtkWidget *UPLevel=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(UPLevel),5);
	gtk_grid_set_row_homogeneous(GTK_GRID(UPLevel),TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(UPLevel),5);
	gtk_grid_set_column_homogeneous(GTK_GRID(UPLevel),TRUE);
	gtk_box_pack_start((GtkBox*)Box[1],UPLevel,TRUE,FALSE,0);

	UpgradeButton[0]=gtk_button_new_with_label("+GOLD MULT.\nLEVEL:0");
	gtk_grid_attach(GTK_GRID(UPLevel),UpgradeButton[0],0,0,1,1);
	UpgradeButton[1]=gtk_button_new_with_label("-TOWER COST\nLEVEL:0");
	gtk_grid_attach(GTK_GRID(UPLevel),UpgradeButton[1],1,0,1,1);
	UpgradeButton[2]=gtk_button_new_with_label("+DAMAGE\nLEVEL:0");
	gtk_grid_attach(GTK_GRID(UPLevel),UpgradeButton[2],2,0,1,1);
	
	UpgradeButton[3]=gtk_button_new_with_label("BUY");
	
	gtk_grid_attach(GTK_GRID(UPLevel),UpgradeButton[3],0,1,1,1);
	UpgradeButton[4]=gtk_button_new_with_label("BUY");
	
	gtk_grid_attach(GTK_GRID(UPLevel),UpgradeButton[4],1,1,1,1);
	UpgradeButton[5]=gtk_button_new_with_label("BUY");
	
	gtk_grid_attach(GTK_GRID(UPLevel),UpgradeButton[5],2,1,1,1);
	UpgradeButton[6]=gtk_button_new_with_label("RETURN");
	g_signal_connect(G_OBJECT(UpgradeButton[6]),"clicked",G_CALLBACK(hideup),NULL);
	gtk_box_pack_start((GtkBox*)Box[1],UpgradeButton[6],TRUE,FALSE,0);



	//achivement init
	for(int i=0; i<6; i++){
		AchivementButton[i]=gtk_button_new_with_label("X");
		gtk_box_pack_start((GtkBox*)Box[2],AchivementButton[i],TRUE,FALSE,0);
	}
	AchivementButton[6]=gtk_button_new_with_label("RETURN");
	g_signal_connect(G_OBJECT(AchivementButton[6]),"clicked",G_CALLBACK(hideach),NULL);
	gtk_box_pack_start((GtkBox*)Box[2],AchivementButton[6],TRUE,FALSE,0);
	

	//oddzielny box na zapisy szerokosc 5/1 i reset szerokosc 1/1(pytanie czy pewny), 
	//odzielny box na achivement
	//odzielny box na mapy
	//odzielny window na budowe wiez
	return TD;
}

void TowDef(){
	GtkWidget *wys=gra();
	gtk_widget_show_all(wys);
	return;
/*
	ile przeciwnikow na runde, losowanie
	moze daj n do rand i tworzyc po jednej turze i testowac
	
	box na wybor zapisu 3 tak jak ile graczy
	reset zapisu
	wszytsko w odzielnych plikach
	wszystkie poziomy w jednym pliku, jeden po drugim
	achivementy, upgrade i postep w pliku zapisu	

	upgrade koszt za 1,2,3; goldmult 1,2,3; damage wiez 6,12

	wieze poj range=2+level, damage=2*level+upgradeDamage
	wieze mass range=level, damage=level+ugradeDamage
	cost=X*upgradeCost

runds:
kazdy poziom to ilosc zycia balonow
po 15 tur, 1. to 5 pojedyn, kazda kolejna to kolejne 5
dla 1-5 pojedyncze balony, dla 6-10 podwojne, dla 11-15 potrojne



LFZ-popsicle (NCS)
*/
}
