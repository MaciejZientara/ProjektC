#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include "linker.h"

#define N 20

GtkWidget *TD,*zakup;
GtkWidget *gridLevel;
GtkWidget *LevelButton[10];
GtkWidget *UpgradeButton[7];
GtkWidget *IleStar;
GtkWidget *AchievementButton[7];
GtkWidget *TabButton[N+2][N+2];//tylko raz init planszy i przyciskow, potem update
GtkWidget *LevelInfo[3];//gold,life,round
GtkWidget *NextRound;
GtkWidget *plansza;//przy okazji zloto, zycie i goldMult//pamietaj o signal connect na zakup
GtkWidget *TabImage[5];
GtkWidget *Box[4];//0-world,1-upgrade,2-achivement,3-level//,4-zakup wiez

int unlocked[10];
int upgrade[3];

static void save(){//cos nie dziala, psuje plik zapisu albo przy wczytaniu?
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
	save();
	gtk_main_quit();
}

struct tower{
	int x,y;
	int type;//w zaleznosci od tego upgrade, range, damage, ...
	int range;
	int *cover;
	int cost;
	int damage;
	int level;
};

struct parametry{
	int nr;//do drogi
	bool isWall;
	bool road;
	bool land;
};

struct RoadNr{
	int nr;
	int x;
	int y;
	int enemy;
};

struct level{
	int Sx,Sy,Fx,Fy;
	int ileEnemy;
	int *enemyPattern;
	int ileTower;
	struct tower *tow;
	struct RoadNr *ROAD;//dfs od S do F po R i kolejne pole to kolejny numer drogi
	struct parametry tab[N+2][N+2];
}poziomy[10];

int Q;

static void Tsingle(){



}

static void Tmass(){


}

GtkWidget *closewin;
static void deswin(){
	gtk_widget_destroy(closewin);
}

static void clickZakup(int x, int y){//blokada na runde, kopia z pyt o res progress
//	GtkWidget *okno=gtk_window_new();
//	gtk_window_set_title(GTK_WINDOW(okno),"BUY TOWER")i;
//	Box[4]


}

static void clickUpgrade(int x, int y){


}

static void czyTower(GtkWidget *button, gpointer user_date){	
	int x,y;
	for(int i=0; i<N+2; i++)
		for(int j=0; j<N+2; j++)
			if(button==gtk_grid_get_child_at((GtkGrid*)plansza,j,i)){
				x=i,y=j;
				break;
			}
	bool czy=false;
	for(int i=0; i<poziomy[Q].ileTower; i++)
		if(poziomy[Q].tow[i].x==x && poziomy[Q].tow[i].y==y)
			czy=true;
	
	if(czy)
		clickUpgrade(x,y);
	else
		clickZakup(x,y);
	return;
	//a potem nie update?
}



int gold;
int life;
//int GoldMult;
int roundnr;

static void updatePlansza(){//zastapic label na image! (tam gdzie trzeba...)
	gtk_button_set_label((GtkButton*)LevelInfo[0],g_strdup_printf("GOLD: %d",gold));
	gtk_button_set_label((GtkButton*)LevelInfo[1],g_strdup_printf("LIFE: %d",life));
	gtk_button_set_label((GtkButton*)LevelInfo[2],g_strdup_printf("ROUND: %d/15",roundnr));

	for(int i=0; i<N+2; i++)
		for(int j=0; j<N+2; j++){
			if(poziomy[Q].tab[i][j].isWall){
				continue;
			}
			if(poziomy[Q].tab[i][j].road){
				gtk_button_set_label((GtkButton*)TabButton[i][j],"R");
				continue;
			}
			if(poziomy[Q].tab[i][j].land){
				gtk_button_set_label((GtkButton*)TabButton[i][j],"L");
				continue;
			}
		}

	//odzielnie tower i enemy
}

static void updateAchievement(){
//AchievementButton[7]
	for(int i=0; i<5; i++)
		if(unlocked[i*2] && unlocked[i*2+1] && unlocked[i*2]<7 && unlocked[i*2+1<7])
			gtk_button_set_label((GtkButton*)AchievementButton[i],g_strdup_printf("STAGE %d FINISHED",i+1));
	
	bool ALLSTAR=true;
	for(int i=0; i<10; i++)
		if(unlocked[i]!=3)
			ALLSTAR=false;
	if(ALLSTAR)
		gtk_button_set_label((GtkButton*)AchievementButton[5],"FULL GAME CLEAR");
}

static void updateUpgrade(){
//UpgradeButton

}

static void updateWorld(){
	for(int q=0; q<10; q++)
		if(unlocked[q]!=7)
			gtk_button_set_label((GtkButton*)LevelButton[q],g_strdup_printf("%d.%d",q,unlocked[q]));
}

static void update(){
	updateAchievement();
	updateUpgrade();
	updateWorld();
}

static void resup(){
	reset();
	update();
	deswin();
}

static void respro(){
	GtkWidget *okno=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(okno),"RESET PROGRESS");
	gtk_window_set_position(GTK_WINDOW(okno),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(okno),10);
	closewin=okno;
	g_signal_connect(G_OBJECT(okno),"destroy",deswin,NULL);

	GtkWidget *boxo=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_container_add(GTK_CONTAINER(okno),boxo);

	GtkWidget *grido=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grido), 5);
	gtk_grid_set_row_homogeneous(GTK_GRID(grido), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grido), 5);
	gtk_grid_set_column_homogeneous(GTK_GRID(grido), TRUE);

	gtk_box_pack_start((GtkBox*)boxo,grido,TRUE,FALSE,0);

	GtkWidget *opis=gtk_button_new_with_label("ARE YOU SURE YOU WANT TO RESET YOUR PROGRESS?");
	gtk_grid_attach(GTK_GRID(grido),opis,0,0,3,1);
	GtkWidget *yes=gtk_button_new_with_label("YES");
	g_signal_connect(G_OBJECT(yes),"clicked",resup,NULL);
	gtk_grid_attach(GTK_GRID(grido),yes,0,1,1,1);
	GtkWidget *no=gtk_button_new_with_label("NO");
	g_signal_connect(G_OBJECT(no),"clicked",deswin,NULL);
	gtk_grid_attach(GTK_GRID(grido),no,2,1,1,1);

	gtk_widget_show_all(okno);
}

static void DFS(int q, int x, int y){
	int nr=1;
	while(x!=poziomy[q].Fx || y!=poziomy[q].Fy){
		poziomy[q].tab[x][y].nr=nr;
		poziomy[q].ROAD[nr-1].nr=nr;
		poziomy[q].ROAD[nr-1].x=x;
		poziomy[q].ROAD[nr-1].y=y;
		nr++;
		if(poziomy[q].tab[x-1][y].road && poziomy[q].tab[x-1][y].nr==0){x--;continue;}
		if(poziomy[q].tab[x+1][y].road && poziomy[q].tab[x+1][y].nr==0){x++;continue;}
		if(poziomy[q].tab[x][y-1].road && poziomy[q].tab[x][y-1].nr==0){y--;continue;}
		if(poziomy[q].tab[x][y+1].road && poziomy[q].tab[x][y+1].nr==0){y++;continue;}
	}
	poziomy[q].tab[x][y].nr=nr;
	poziomy[q].ROAD[nr-1].nr=nr;
	poziomy[q].ROAD[nr-1].x=x;
	poziomy[q].ROAD[nr-1].y=y;
	nr++;
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

static void hidelev(){
	g_object_ref(Box[3]);
	gtk_container_remove(GTK_CONTAINER(TD),Box[3]);
	gtk_container_add(GTK_CONTAINER(TD),Box[0]);
	gtk_widget_show_all(Box[0]);
	deswin();
}//3->0

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

static void showlev(GtkWidget *button, gpointer user_date){
	for(int q=0; q<10; q++)
		if(gtk_grid_get_child_at(GTK_GRID(gridLevel),q%5,q/5)==button)
			if(unlocked[q]!=7){
				g_object_ref(Box[0]);
				gtk_container_remove(GTK_CONTAINER(TD),Box[0]);
				gtk_container_add(GTK_CONTAINER(TD),Box[3]);
				Q=q;
				//printf("q=%d\n",q);
				poziomy[q].ileTower=0;
				poziomy[q].tow=(struct tower*)malloc(0);//?
				updatePlansza();
				roundnr=0;
				life=15;
				gold=100;//?
				gtk_widget_show_all(Box[3]);
				break;
			}
}//0->3

static void ExLev(){
	GtkWidget *okno=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(okno),"RESET PROGRESS");
	gtk_window_set_position(GTK_WINDOW(okno),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(okno),10);
	closewin=okno;
	g_signal_connect(G_OBJECT(okno),"destroy",deswin,NULL);

	GtkWidget *boxo=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_container_add(GTK_CONTAINER(okno),boxo);

	GtkWidget *grido=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grido), 5);
	gtk_grid_set_row_homogeneous(GTK_GRID(grido), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grido), 5);
	gtk_grid_set_column_homogeneous(GTK_GRID(grido), TRUE);

	gtk_box_pack_start((GtkBox*)boxo,grido,TRUE,FALSE,0);

	GtkWidget *opis=gtk_button_new_with_label("ARE YOU SURE YOU WANT TO EXIT?");
	gtk_grid_attach(GTK_GRID(grido),opis,0,0,3,1);
	GtkWidget *yes=gtk_button_new_with_label("YES");
	g_signal_connect(G_OBJECT(yes),"clicked",hidelev,NULL);
	gtk_grid_attach(GTK_GRID(grido),yes,0,1,1,1);
	GtkWidget *no=gtk_button_new_with_label("NO");
	g_signal_connect(G_OBJECT(no),"clicked",deswin,NULL);
	gtk_grid_attach(GTK_GRID(grido),no,2,1,1,1);

	gtk_widget_show_all(okno);
}

static void ROUND(){

//updatePlansza();
}

static void init(){
	FILE *level=fopen("level","r");
	for(int q=0;q<1;q++){								//q<1->10!!!
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
							poziomy[q].Sx=i,poziomy[q].Sy=j;
						}
						if(c=='F'){
							poziomy[q].Fx=i,poziomy[q].Fy=j;
						}
					}
				}
			}

		poziomy[q].ROAD=calloc(ileRoad*sizeof(struct RoadNr),1);
		fscanf(level,"%d",&poziomy[q].ileEnemy);
		poziomy[q].enemyPattern=(int*)malloc(poziomy[q].ileEnemy*sizeof(int));
		for(int i=0; i<poziomy[q].ileEnemy; i++)
			fscanf(level,"%d",&poziomy[q].enemyPattern[i]);
		DFS(q,poziomy[q].Sx,poziomy[q].Sy);
		poziomy[q].tow=(struct tower*)malloc(0);//?
	}
	fclose(level);
	
	FILE *sv=fopen("TDsave.txt","r");
	if(sv==NULL)
		reset();
	else{
		char c;
		for(int i=0; i<10; i++){
			fscanf(sv,"%c",&c);
			unlocked[i]=c-'0';
		}
		for(int i=0; i<3; i++){
			fscanf(sv,"%c",&c);
			upgrade[i]=c-'0';
		}
	}
	fclose(sv);
	
	//graphic init

	Box[0]=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);//world//do init X a jak odbl to nr lev i ile *
	Box[1]=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);//upgrade
	Box[2]=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);//achievement
	Box[3]=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);//level

	//plansza init
	
	GtkWidget *exit=gtk_button_new_with_label("EXIT LEVEL");
	g_signal_connect(G_OBJECT(exit),"clicked",G_CALLBACK(ExLev),NULL);
	gtk_box_pack_start((GtkBox*)Box[3],exit,TRUE,FALSE,0);

	GtkWidget *levelinfo=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(levelinfo),5);
	gtk_grid_set_row_homogeneous(GTK_GRID(levelinfo),TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(levelinfo),5);
	gtk_grid_set_column_homogeneous(GTK_GRID(levelinfo),TRUE);
	gtk_box_pack_start((GtkBox*)Box[3],levelinfo,TRUE,FALSE,0);

	for(int i=0; i<3; i++){
		LevelInfo[i]=gtk_button_new();
		gtk_grid_attach(GTK_GRID(levelinfo),LevelInfo[i],0,i,1,1);
	}

	NextRound=gtk_button_new_with_label("NEXT ROUND");
	g_signal_connect(G_OBJECT(NextRound),"clicked",G_CALLBACK(ROUND),NULL);
	gtk_box_pack_start((GtkBox*)Box[3],NextRound,TRUE,FALSE,0);

	plansza=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(plansza),0);
	gtk_grid_set_row_homogeneous(GTK_GRID(plansza),TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(plansza),0);
	gtk_grid_set_column_homogeneous(GTK_GRID(plansza),TRUE);

	gtk_box_pack_start((GtkBox*)Box[3],plansza,TRUE,FALSE,0);

	for(int i=0; i<N+2; i++)
		for(int j=0; j<N+2; j++){
			TabButton[i][j]=gtk_button_new();
			gtk_grid_attach(GTK_GRID(plansza),TabButton[i][j],j,i,1,1);
		}

	//world init
	GtkWidget *RESET=gtk_button_new_with_label("RESET\nPROGRESS");
	g_signal_connect(G_OBJECT(RESET),"clicked",G_CALLBACK(respro),NULL);
	gtk_box_pack_start((GtkBox*)Box[0],RESET,FALSE,FALSE,0);
	
	gridLevel=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(gridLevel),5);
	gtk_grid_set_row_homogeneous(GTK_GRID(gridLevel),TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(gridLevel),5);
	gtk_grid_set_column_homogeneous(GTK_GRID(gridLevel),TRUE);
	for(int i=0; i<10; i++){
		LevelButton[i]=gtk_button_new_with_label(g_strdup_printf("%d.X",i));
		g_signal_connect(G_OBJECT(LevelButton[i]),"clicked",G_CALLBACK(showlev),NULL);
		gtk_grid_attach(GTK_GRID(gridLevel),LevelButton[i],i%5,i/5,1,1);
	}
	gtk_box_pack_start((GtkBox*)Box[0],gridLevel,TRUE,FALSE,0);

	GtkWidget *UPGRADE=gtk_button_new_with_label("UPGRADE");
	g_signal_connect(G_OBJECT(UPGRADE),"clicked",G_CALLBACK(showup),NULL);
	gtk_box_pack_start((GtkBox*)Box[0],UPGRADE,TRUE,FALSE,0);	
	GtkWidget *ACHIEVEMENT=gtk_button_new_with_label("ACHIEVEMENT");
	g_signal_connect(G_OBJECT(ACHIEVEMENT),"clicked",G_CALLBACK(showach),NULL);
	gtk_box_pack_start((GtkBox*)Box[0],ACHIEVEMENT,TRUE,FALSE,0);

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

	//achievement init
	for(int i=0; i<6; i++){
		AchievementButton[i]=gtk_button_new_with_label("X");
		gtk_box_pack_start((GtkBox*)Box[2],AchievementButton[i],TRUE,FALSE,0);
	}
	AchievementButton[6]=gtk_button_new_with_label("RETURN");
	g_signal_connect(G_OBJECT(AchievementButton[6]),"clicked",G_CALLBACK(hideach),NULL);
	gtk_box_pack_start((GtkBox*)Box[2],AchievementButton[6],TRUE,FALSE,0);
}

static GtkWidget* gra(){
	TD=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(TD),"TOWER DEFENCE");
	gtk_window_set_position(GTK_WINDOW(TD),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(TD),10);
	g_signal_connect(G_OBJECT(TD),"destroy",G_CALLBACK(wq),NULL);

	init();

	//update();		//???

	//oddzielny box na zapisy szerokosc 5/1 i reset szerokosc 1/1(pytanie czy pewny), 
	//odzielny box na achivement
	//odzielny box na mapy
	//odzielny window na budowe wiez
	return TD;
}

void TowDef(){//main function
	gtk_widget_hide(Main);
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
