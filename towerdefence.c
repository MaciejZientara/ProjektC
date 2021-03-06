#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include "linker.h"
#include <gdk/gdkwayland.h>
#include <time.h>

#define N 20

GtkWidget *TD,*zakup;
GtkWidget *gridLevel;
GtkWidget *LevelButton[10];
GtkWidget *UpgradeButton[8];
GtkWidget *IleStar;
GtkWidget *AchievementButton[7];
GtkWidget *TabButton[N+2][N+2];
GtkWidget *LevelInfo[3];//gold,life,round
GtkWidget *NextRound;
GtkWidget *plansza;
//GtkWidget *TabImage[12];
GtkWidget *Box[4];//0-world,1-upgrade,2-achivement,3-level

int unlocked[10];
int upgrade[3];

int ileEnemy;
int *enemyPattern;
//czy dodac typy enemy (regen,...)

int gold;
int life;
int roundnr;
double GoldMult;
int addDamage;
double costred;

int HARD=1;
bool ENDLESS;

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
	save();
	gtk_main_quit();
}

struct tower{
	int x,y;
	int type;//w zaleznosci od tego upgrade, range, damage, ...
	int covsize;
	int *cover;
	int level;
	int mode;//1-first,2-last,3-strong
	struct tower *next;
	int dmg;
	int iletur;
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
	int enemy;//life
};

struct level{
	int Sx,Sy,Fx,Fy;
	struct tower *tow;
	int ileR;
	struct RoadNr *ROAD;
	struct parametry tab[N+2][N+2];
}poziomy[10];

int Q,X,Y;

GtkWidget *closewin;
static void deswin(){
	gtk_widget_destroy(closewin);
}


static void updatePlansza(){
	gtk_button_set_label((GtkButton*)LevelInfo[0],g_strdup_printf("GOLD: %d",gold));
	gtk_button_set_label((GtkButton*)LevelInfo[1],g_strdup_printf("LIFE: %d",life));
	gtk_button_set_label((GtkButton*)LevelInfo[2],g_strdup_printf("ROUND: %d/15",roundnr));
	
	//plansza
	for(int i=0; i<N+2; i++)
		for(int j=0; j<N+2; j++){
			if(poziomy[Q].tab[i][j].isWall){
				gtk_button_set_label((GtkButton*)TabButton[i][j],"#");
				gtk_widget_set_name(TabButton[i][j],"wall");
				continue;
			}
			if(poziomy[Q].tab[i][j].road){
				gtk_widget_set_name(TabButton[i][j],"road");
				int life=poziomy[Q].ROAD[poziomy[Q].tab[i][j].nr-1].enemy;

				if(life==0)
					gtk_button_set_label((GtkButton*)TabButton[i][j],"_");
				else{
					gtk_button_set_label((GtkButton*)TabButton[i][j],g_strdup_printf("%d",life));	
				}
				continue;
			}
			if(poziomy[Q].tab[i][j].land){
				gtk_button_set_label((GtkButton*)TabButton[i][j],".");
				gtk_widget_set_name(TabButton[i][j],"land");
				continue;
			}
		}
	
	//tower
	struct tower *t=poziomy[Q].tow;
	while(t!=NULL){
		if(t->type==1)
		gtk_button_set_label((GtkButton*)TabButton[t->x][t->y],g_strdup_printf("B%d",t->level));
		if(t->type==2)
		gtk_button_set_label((GtkButton*)TabButton[t->x][t->y],g_strdup_printf("C%d",t->level));
		if(t->type==3)
		gtk_button_set_label((GtkButton*)TabButton[t->x][t->y],g_strdup_printf("S%d",t->level));
		if(t->type==4)
		gtk_button_set_label((GtkButton*)TabButton[t->x][t->y],g_strdup_printf("V%d",t->level));
		if(t->type==5)
		gtk_button_set_label((GtkButton*)TabButton[t->x][t->y],g_strdup_printf("D%d",t->level));
		t=t->next;
	}
}

static void updateAchievement(){
	for(int i=0; i<5; i++)
		if(unlocked[i*2] && unlocked[i*2+1] && unlocked[i*2]<7 && unlocked[i*2+1]<7)
			gtk_button_set_label((GtkButton*)AchievementButton[i],g_strdup_printf("STAGE %d FINISHED",i+1));
		else
			gtk_button_set_label((GtkButton*)AchievementButton[i],"X");

	//dodaj hard mode na buuton[4]
	//dodaj endless level
	
	bool ALLSTAR=true;
	for(int i=0; i<10; i++)
		if(unlocked[i]!=3)
			ALLSTAR=false;
	if(ALLSTAR)
		gtk_button_set_label((GtkButton*)AchievementButton[5],"FULL GAME CLEAR");
	else
		gtk_button_set_label((GtkButton*)AchievementButton[5],"X");
		
}

static void updateUpgrade(){
//UpgradeButton
//GoldMult
//costred
//addDamage
//upgarde[3]
	int ile=0;
	for(int i=0; i<10; i++)
		if(unlocked[i]<7)
			ile+=unlocked[i];
	gtk_button_set_label((GtkButton*)IleStar,g_strdup_printf("%d/30",ile));

	int tmp1, tmp2, tmp3;

	//upgrade koszt za 1,2,3; goldmult 1,2,3; damage wiez 6,12
	if(upgrade[0]==0){
		GoldMult=1.0;
		tmp1=1;
	}
	if(upgrade[0]==1){
		GoldMult=1.2;
		tmp1=2;
	}
	if(upgrade[0]==2){
		GoldMult=1.5;
		tmp1=3;
	}
	if(upgrade[0]==3){
		GoldMult=2.0;
		tmp1=-1;
	}

	if(upgrade[1]==0){
		costred=1.0;
		tmp2=1;
	}
	if(upgrade[1]==1){
		costred=0.9;
		tmp2=2;
	}
	if(upgrade[1]==2){
		costred=0.8;
		tmp2=3;
	}
	if(upgrade[1]==3){
		costred=0.5;
		tmp2=-1;
	}

	if(upgrade[2]==0){
		addDamage=0;
		tmp3=6;
	}
	if(upgrade[2]==1){
		addDamage=1;
		tmp3=12;
	}
	if(upgrade[2]==2){
		addDamage=3;
		tmp3=-1;
	}

	gtk_button_set_label((GtkButton*)UpgradeButton[0],g_strdup_printf("+GOLD MULT\nLEVEL:%d",upgrade[0]));
	gtk_button_set_label((GtkButton*)UpgradeButton[1],g_strdup_printf("-TOWER COST\nLEVEL:%d",upgrade[1]));
	gtk_button_set_label((GtkButton*)UpgradeButton[2],g_strdup_printf("+DAMAGE\nLEVEL:%d",upgrade[2]));

	if(tmp1!=-1)
		gtk_button_set_label((GtkButton*)UpgradeButton[3],g_strdup_printf("BUY FOR %d",tmp1));
	else
		gtk_button_set_label((GtkButton*)UpgradeButton[3],"MAX LEVEL");	
	if(tmp2!=-1)
		gtk_button_set_label((GtkButton*)UpgradeButton[4],g_strdup_printf("BUY FOR %d",tmp2));	
	else
		gtk_button_set_label((GtkButton*)UpgradeButton[4],"MAX LEVEL");
	if(tmp3!=-1)
		gtk_button_set_label((GtkButton*)UpgradeButton[5],g_strdup_printf("BUY FOR %d",tmp3));
	else
		gtk_button_set_label((GtkButton*)UpgradeButton[5],"MAX LEVEL");

}

static void updateWorld(){
	for(int q=0; q<10; q++)
		if(unlocked[q]!=7)
			gtk_button_set_label((GtkButton*)LevelButton[q],g_strdup_printf("%d.%d",q,unlocked[q]));
		else
			gtk_button_set_label((GtkButton*)LevelButton[q],g_strdup_printf("%d.X",q));	
}

static void update(){
	updateAchievement();
	updateUpgrade();
	updateWorld();
}

static void starup(GtkWidget *button, gpointer user_date){
	int ile=0;
	for(int i=0; i<10; i++)
		if(unlocked[i]<7)
			ile+=unlocked[i];
	int wydane=0;

	int starcost[3][4]={{1,2,3,0},{1,2,3,0},{6,12,0,0}};

	for(int i=0; i<3; i++)
		for(int j=0; j<upgrade[i]; j++)
			wydane+=starcost[i][j];

	for(int q=0; q<3; q++)
		if(UpgradeButton[3+q]==button)
			if(starcost[q][upgrade[q]]){
				if(ile-wydane-starcost[q][upgrade[q]+1]>=0){
					upgrade[q]++;
					update();
				}
				else{
					free(P);
					P=calloc(sizeof(struct pairGS),1);
					P->G=TD;
					strcpy(P->S,g_strdup_printf("NOT ENOUGH STARS!"));
					dialog();
				}
			}
}

static void resetupgrade(){
	for(int i=0; i<3; i++)
		upgrade[i]=0;
	updateUpgrade();
}

static void setTower(int typ){
	double koszt;
	int ran,dmg,ile=1;
	if(typ==1){
		koszt=100;
		ran=3;
		dmg=1;
	}
	if(typ==2){
		koszt=200;
		ran=2;
		dmg=2;
	}
	if(typ==3){
		koszt=250;
		ran=1;
		dmg=10;
	}
	if(typ==4){
		koszt=300;
		ran=1;
		dmg=5;
	}
	koszt*=costred;
	dmg+=addDamage;

	struct tower *t=poziomy[Q].tow;
	while(t!=NULL){
		if(abs(X-t->x)<=2 && abs(Y-t->y)<=2){
			if(t->type==5){
				//if(t->level>=1)koszt*=0.9;
				//if(t->level>=2)koszt*=0.9;
				if(t->level>=3)dmg+=1;
				if(t->level>=4)ile+=1;
			}
		}
		t=t->next;
	}

	if(gold<(int)koszt){
		dialog();
		return;
	}
	gold-=(int)koszt;

	struct tower *tmp=(struct tower*)malloc(sizeof(struct tower));
	tmp->next=poziomy[Q].tow;
	poziomy[Q].tow=tmp;
	
	poziomy[Q].tow->x=X;
	poziomy[Q].tow->y=Y;
	poziomy[Q].tow->type=typ;
	poziomy[Q].tow->mode=1;
	poziomy[Q].tow->level=1;
	poziomy[Q].tow->dmg=dmg;
	poziomy[Q].tow->iletur=ile;

	//wylicz cover
	int RoadRange=0;
	for(int i=0; i<1+2*ran; i++)
		for(int j=0; j<1+2*ran; j++)
			if(X+i-ran>=0 && Y+j-ran>=0 && X+i-ran<N+2 && Y+j-ran<N+2)
				if(poziomy[Q].tab[X+i-ran][Y+j-ran].road)
					RoadRange++;
	
	poziomy[Q].tow->covsize=RoadRange;
	poziomy[Q].tow->cover=(int*)calloc(sizeof(int)*RoadRange,1);
	
	int poz=0;
	for(int i=0; i<1+2*ran; i++)
		for(int j=0; j<1+2*ran; j++)
			if(X+i-ran>=0 && Y+j-ran>=0 && X+i-ran<N+2 && Y+j-ran<N+2)
				if(poziomy[Q].tab[X+i-ran][Y+j-ran].road){
					poziomy[Q].tow->cover[poz++]=poziomy[Q].tab[X+i-ran][Y+j-ran].nr;
				}

	for(int i=0; i<RoadRange; i++){
		int mini=666,poz=i;
		for(int j=i; j<RoadRange; j++)
			if(poziomy[Q].tow->cover[j]<mini){
				mini=poziomy[Q].tow->cover[j];
				poz=j;
			}
		int tmp=poziomy[Q].tow->cover[i];
		poziomy[Q].tow->cover[i]=poziomy[Q].tow->cover[poz];
		poziomy[Q].tow->cover[poz]=tmp;
	}

	deswin();
	updatePlansza();
}

static void balista(){
	setTower(1);
}

static void catapult(){
	setTower(2);
}

static void shydragon(){
	setTower(3);
}

static void vulcan(){
	setTower(4);
}

static void druid(){
	double koszt=400;
	koszt*=costred;

	if(gold<(int)koszt){
		dialog();
		return;
	}
	gold-=(int)koszt;

	struct tower *tmp=(struct tower*)malloc(sizeof(struct tower));
	tmp->next=poziomy[Q].tow;
	poziomy[Q].tow=tmp;
	
	poziomy[Q].tow->x=X;
	poziomy[Q].tow->y=Y;
	poziomy[Q].tow->type=5;
	poziomy[Q].tow->level=1;

	deswin();
	updatePlansza();
//dodaj dmg i iletur do wiez w zasiegu?
}

static void clickZakup(){
	if(GTK_IS_WIDGET(closewin))
		deswin();
	GtkWidget *okno=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(okno),"BUY TOWER");
	gtk_window_set_position(GTK_WINDOW(okno),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(okno),10);
	closewin=okno;
	g_signal_connect(G_OBJECT(okno),"destroy",G_CALLBACK(deswin),NULL);

	GtkWidget *boxo=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_container_add(GTK_CONTAINER(okno),boxo);

	GtkWidget *grido=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grido), 5);
	gtk_grid_set_row_homogeneous(GTK_GRID(grido), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grido), 5);
	gtk_grid_set_column_homogeneous(GTK_GRID(grido), TRUE);

	gtk_box_pack_start((GtkBox*)boxo,grido,TRUE,FALSE,0);

	free(P);
	P=calloc(sizeof(struct pairGS),1);
	P->G=okno;
	strcpy(P->S,g_strdup_printf("NOT ENOUGH GOLD!"));

	GtkWidget *opis=gtk_button_new_with_label("WHICH TOWER WOULD YOU LIKE?");
	gtk_grid_attach(GTK_GRID(grido),opis,1,0,1,1);
	
	GtkWidget *t1=gtk_button_new_with_label(g_strdup_printf("BALISTA\n%d GOLD",(int)(100*costred)));
	g_signal_connect(G_OBJECT(t1),"clicked",balista,NULL);
	gtk_grid_attach(GTK_GRID(grido),t1,1,1,1,1);

	GtkWidget *t2=gtk_button_new_with_label(g_strdup_printf("CATAPULT\n%d GOLD",(int)(200*costred)));
	g_signal_connect(G_OBJECT(t2),"clicked",catapult,NULL);
	gtk_grid_attach(GTK_GRID(grido),t2,1,2,1,1);
	
	GtkWidget *t4=gtk_button_new_with_label(g_strdup_printf("SHY DRAGON\n%d GOLD",(int)(250*costred)));
	g_signal_connect(G_OBJECT(t4),"clicked",shydragon,NULL);
	gtk_grid_attach(GTK_GRID(grido),t4,1,3,1,1);
	
	GtkWidget *t5=gtk_button_new_with_label(g_strdup_printf("VULCAN\n%d GOLD",(int)(300*costred)));
	g_signal_connect(G_OBJECT(t5),"clicked",vulcan,NULL);
	gtk_grid_attach(GTK_GRID(grido),t5,1,4,1,1);

	GtkWidget *t6=gtk_button_new_with_label(g_strdup_printf("DRUID\n%d GOLD",(int)(400*costred)));
	g_signal_connect(G_OBJECT(t6),"clicked",druid,NULL);
	gtk_grid_attach(GTK_GRID(grido),t6,1,5,1,1);
	
	GtkWidget *exit=gtk_button_new_with_label("CANCEL");
	g_signal_connect(G_OBJECT(exit),"clicked",G_CALLBACK(deswin),NULL);
	gtk_grid_attach(GTK_GRID(grido),exit,1,6,1,1);

	gtk_widget_show_all(okno);
}

static struct tower* findwieza(){
	struct tower *t=poziomy[Q].tow;
	while(t!=NULL){
		if(t->x==X && t->y==Y)
			break;
		t=t->next;
	}
	return t;
}

static void zamkupg(){
	//gtk_button_set_label() albo styl do normy
	updatePlansza();
	deswin();
}

static void upp(){
	struct tower *t=findwieza();

	double koszt=t->level;
	if(koszt==3)koszt*=3;//?
	if(t->type==1)koszt+=100;
	if(t->type==2)koszt+=200;
	if(t->type==3)koszt+=250;
	if(t->type==4)koszt+=300;
	if(t->type==5)koszt+=400;

	if(t->type!=5){
		struct tower *f=poziomy[Q].tow;
		while(f!=NULL){
			if(abs(X-f->x)<=2 && abs(Y-f->y)<=2){
				if(f->type==5){
					if(f->level>=1)koszt*=0.9;
					if(f->level>=2)koszt*=0.9;
				}
			}
			f=f->next;
		}
	}
	
	if(koszt>gold){
		dialog();
		return;
	}
	gold-=(int)koszt;
	
	t->level++;
	if(t->type!=5)
		t->dmg++;
	else{
		struct tower *f=poziomy[Q].tow;
		while(f!=NULL){
			if(abs(X-f->x)<=2 && abs(Y-f->y)<=2){
				if(f->type!=5){
					if(t->level==3)f->dmg+=1;
					if(t->level==4)f->iletur=1;
				}
			}
			f=f->next;
		}
	}
	updatePlansza();
	zamkupg();
}

static void modechange(GtkWidget *button, gpointer user_date){
	struct tower *t=findwieza();
	t->mode=t->mode%3+1;
	if(t->mode==1)
		gtk_button_set_label((GtkButton*)button,"mode:FIRST");
	if(t->mode==2)
		gtk_button_set_label((GtkButton*)button,"mode:LAST");
	if(t->mode==3)
		gtk_button_set_label((GtkButton*)button,"mode:STRONG");
}

static void sello(){	
	struct tower *t=poziomy[Q].tow;
	
	if(t->x==X && t->y==Y){
		double koszt=t->level;
		if(t->type==1)koszt+=100;
		if(t->type==2)koszt+=200;
		if(t->type==3)koszt+=250;
		if(t->type==4)koszt+=300;
		if(t->type==5)koszt+=400;
		gold+=t->level*0.9*koszt;
	
	if(t->type==5){
		struct tower *f=poziomy[Q].tow;
		while(f!=NULL){
			if(abs(X-f->x)<=2 && abs(Y-f->y)<=2){
				if(f->type!=5){
					if(t->level>=3)f->dmg-=1;
					if(t->level==4)f->iletur-=1;
				}
			}
			f=f->next;
		}
	}
	//druid

		struct tower *tmp=t->next;
		free(t);
		poziomy[Q].tow=tmp;
		updatePlansza();
		zamkupg();
		return;
	}
	while(t->next->x!=X || t->next->y!=Y)
		t=t->next;
	
	double koszt=t->next->level;
	if(t->next->type==1)koszt+=100;
	if(t->next->type==2)koszt+=200;
	if(t->next->type==3)koszt+=250;
	if(t->next->type==4)koszt+=300;
	if(t->next->type==5)koszt+=400;
	gold+=t->next->level*0.9*koszt;
	
	if(t->next->type==5){
		struct tower *f=poziomy[Q].tow;
		while(f!=NULL){
			if(abs(X-f->x)<=2 && abs(Y-f->y)<=2){
				if(f->type!=5){
					if(t->next->level>=3)f->dmg-=1;
					if(t->next->level==4)f->iletur-=1;
				}
			}
			f=f->next;
		}
	}
	//druid

	struct tower *tmp=t->next->next;
	free(t->next);
	t->next=tmp;
	updatePlansza();
	zamkupg();
}

static void clickUpgrade(){
	if(GTK_IS_WIDGET(closewin))
		deswin();
	
	struct tower *t=findwieza();
	
	int ran=0;
	if(t->type==1)ran=3;
	if(t->type==2)ran=2;
	if(t->type==3)ran=1;
	if(t->type==4)ran=1;
	if(t->type==5)ran=2;
	
	for(int i=0; i<1+2*ran; i++)
		for(int j=0; j<1+2*ran; j++)
			if(X+i-ran>0 && Y+j-ran>0 && X+i-ran<N+1 && Y+j-ran<N+1)
				gtk_widget_set_name(TabButton[X+i-ran][Y+j-ran],"towran");
	gtk_widget_set_name(TabButton[X][Y],"seltow");

	GtkWidget *okno=gtk_window_new(GTK_WINDOW_TOPLEVEL);

	if(t->type==1)
		gtk_window_set_title(GTK_WINDOW(okno),g_strdup_printf("BALISTA:%d",t->level));
	if(t->type==2)
		gtk_window_set_title(GTK_WINDOW(okno),g_strdup_printf("CATAPULT:%d",t->level));
	if(t->type==3)
		gtk_window_set_title(GTK_WINDOW(okno),g_strdup_printf("SHY DRAGON:%d",t->level));
	if(t->type==4)
		gtk_window_set_title(GTK_WINDOW(okno),g_strdup_printf("VULCAN:%d",t->level));
	if(t->type==5)
		gtk_window_set_title(GTK_WINDOW(okno),g_strdup_printf("DRUID:%d",t->level));

	gtk_window_set_position(GTK_WINDOW(okno),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(okno),10);
	closewin=okno;
	g_signal_connect(G_OBJECT(okno),"destroy",G_CALLBACK(zamkupg),NULL);

	GtkWidget *boxo=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_container_add(GTK_CONTAINER(okno),boxo);

	GtkWidget *grido=gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grido), 5);
	gtk_grid_set_row_homogeneous(GTK_GRID(grido), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grido), 5);
	gtk_grid_set_column_homogeneous(GTK_GRID(grido), TRUE);

	gtk_box_pack_start((GtkBox*)boxo,grido,TRUE,FALSE,0);

	free(P);
	P=calloc(sizeof(struct pairGS),1);
	P->G=okno;
	strcpy(P->S,g_strdup_printf("NOT ENOUGH GOLD!"));

	GtkWidget *opis=gtk_button_new_with_label("TOWER SETTINGS");
	gtk_grid_attach(GTK_GRID(grido),opis,0,0,1,1);
//dodac koszt upgrade do label!!!
	GtkWidget *t1=gtk_button_new();
	if(t->level<4){
		gtk_button_set_label((GtkButton*)t1,g_strdup_printf("UPGRADE %d->%d",t->level,t->level+1));
		g_signal_connect(G_OBJECT(t1),"clicked",G_CALLBACK(upp),NULL);
	}
	else
		gtk_button_set_label((GtkButton*)t1,"MAX LEVEL");

	gtk_grid_attach(GTK_GRID(grido),t1,0,1,1,1);
	GtkWidget *t2=gtk_button_new();
	if(t->mode==1)
		gtk_button_set_label((GtkButton*)t2,"mode:FIRST");
	if(t->mode==2)
		gtk_button_set_label((GtkButton*)t2,"mode:LAST");
	if(t->mode==3)
		gtk_button_set_label((GtkButton*)t2,"mode:STRONG");
	g_signal_connect(G_OBJECT(t2),"clicked",G_CALLBACK(modechange),NULL);
	gtk_grid_attach(GTK_GRID(grido),t2,0,2,1,1);
	GtkWidget *sell=gtk_button_new_with_label("SELL");
	g_signal_connect(G_OBJECT(sell),"clicked",G_CALLBACK(sello),NULL);
	gtk_grid_attach(GTK_GRID(grido),sell,0,3,1,1);
	GtkWidget *exit=gtk_button_new_with_label("CANCEL");
	g_signal_connect(G_OBJECT(exit),"clicked",G_CALLBACK(zamkupg),NULL);
	gtk_grid_attach(GTK_GRID(grido),exit,0,4,1,1);
	
	gtk_widget_show_all(okno);

//sell - usuniecie z tow z przepieciem ptr, dla druida usuniecie efektow z wiez
}

//bool trwarunda ?
static void czyTower(GtkWidget *button, gpointer user_date){
	int x,y;//blokada na runde
	for(int i=0; i<N+2; i++)
		for(int j=0; j<N+2; j++)
			if(button==gtk_grid_get_child_at((GtkGrid*)plansza,j,i)){
				x=i,y=j;
				break;
			}
	if(!poziomy[Q].tab[x][y].land)
		return;
	bool czy=false;
	struct tower *t=poziomy[Q].tow;
	while(t!=NULL){	
		if(t->x==x && t->y==y)
			czy=true;
		t=t->next;
	}
	
	X=x,Y=y;

	if(czy)
		clickUpgrade();
	else
		clickZakup();
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
		poziomy[q].tab[x][y].nr=nr;//printf("nr=%d\n",poziomy[q].tab[x][y].nr);
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

static void rectowfree(struct tower *x){
	if(x->next!=NULL)
		rectowfree(x->next);
	free(x);
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
	deswin();//
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

				if(poziomy[Q].tow!=NULL)
					rectowfree(poziomy[Q].tow);
				poziomy[q].tow=NULL;
				
				roundnr=0;
				life=15;
				gold=250*GoldMult;
				updatePlansza();
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

static void fail(){
	if(GTK_IS_WIDGET(closewin))
		closewin=gtk_button_new();
	hidelev();
	free(P);
	P=calloc(sizeof(struct pairGS),1);
	P->G=TD;
	strcpy(P->S,"YOUR DEFENCE LOST!");
	dialog();
}

static void win(){
	if(GTK_IS_WIDGET(closewin))
		closewin=gtk_button_new();
	if(HARD==1){
	unlocked[Q]=(life-1)/5+1;
	if(Q<9)
		unlocked[Q+1]=0;
	}
	else
		unlocked[Q]=2*((life-1)/5+1);
	hidelev();
	update();
}

static void atakbalista(struct tower *t, int nr){
	if(t->level==4){
		srand(time(0));
		int tmp=rand()%20;
		if(!tmp){
			poziomy[Q].ROAD[nr].enemy=0;
			return;
		}	
	}
	if(0>poziomy[Q].ROAD[nr].enemy-t->dmg)
		poziomy[Q].ROAD[nr].enemy=0;
	else
		poziomy[Q].ROAD[nr].enemy=poziomy[Q].ROAD[nr].enemy-t->dmg;
	gtk_button_set_label((GtkButton*)TabButton[poziomy[Q].ROAD[nr].x][poziomy[Q].ROAD[nr].y],"!!");
}

static void atakcatapult(struct tower *t, int nr){
	int zycie[5];
	for(int i=0; i<5; i++)
		if(nr+i-2>=0 && nr+i-2<poziomy[Q].ileR)
			if(0>poziomy[Q].ROAD[nr+i-2].enemy-t->dmg)
				zycie[i]=0;
			else
				zycie[i]=poziomy[Q].ROAD[nr+i-2].enemy-t->dmg;
	
	if(t->level==4){
		if(nr-2>=0)
			poziomy[Q].ROAD[nr-2].enemy=zycie[0];
		if(nr+2<poziomy[Q].ileR)
			poziomy[Q].ROAD[nr+2].enemy=zycie[4];
	}
	if(nr-1>=0)
		poziomy[Q].ROAD[nr-1].enemy=zycie[1];
	
	poziomy[Q].ROAD[nr].enemy=zycie[2];
	
	if(nr+1<poziomy[Q].ileR)
		poziomy[Q].ROAD[nr+1].enemy=zycie[3];

}

static void atakdragon(struct tower *t){
	bool czy=false;
	if(t->level==4){
		for(int i=0; i<t->covsize; i++)
			if(poziomy[Q].ROAD[t->cover[i]].enemy){
				czy=true;
				break;
			}
	}
	else{
		czy=true;
		for(int i=0; i<t->covsize; i++){
			if(!poziomy[Q].ROAD[t->cover[i]].enemy){
				czy=false;
				break;
			}
		}
	}

	if(czy)
		for(int i=0; i<t->covsize; i++){
			if(0>poziomy[Q].ROAD[t->cover[i]].enemy-t->dmg)
				poziomy[Q].ROAD[t->cover[i]].enemy=0;
			else
				poziomy[Q].ROAD[t->cover[i]].enemy=poziomy[Q].ROAD[t->cover[i]].enemy-t->dmg;
		}
}

static void atakvulcan(struct tower *t){
	srand(time(0));
	int j;
	if(t->level>=1)
		j=1;
	if(t->level==3)
		j=2;
	if(t->level==4)
		j=4;
	for(int i=0; i<j; i++){
		int tmp=t->cover[rand()%t->covsize];
		if(0>poziomy[Q].ROAD[tmp].enemy-t->dmg)
			poziomy[Q].ROAD[tmp].enemy=0;
		else
			poziomy[Q].ROAD[tmp].enemy=poziomy[Q].ROAD[tmp].enemy-t->dmg;
	}
}

static void ataktower(struct tower *t,int ile){
	//if(t->iletur)
	//	t->iletur--;
	//else
	//	return false;
for(int qq=0; qq<ile; qq++){
	if(t->type==1 || t->type==2){//mode dependent
		int n=t->covsize;
		if(t->mode==1){
			for(int i=n-1; i>=0; i--)
				if(poziomy[Q].ROAD[t->cover[i]].enemy){
//printf("na %d enemy %d\n",t->cover[i],poziomy[Q].ROAD[t->cover[i]].enemy);
					if(t->type==1)
						atakbalista(t,t->cover[i]);
					else
						atakcatapult(t,t->cover[i]);
					break;
				}
		}	
		if(t->mode==2){
			for(int i=0; i<n; i++)
				if(poziomy[Q].ROAD[t->cover[i]].enemy){
					if(t->type==1)
						atakbalista(t,t->cover[i]);
					else
						atakcatapult(t,t->cover[i]);
					break;
				}
		}	
		if(t->mode==3){
			int tmp=0, pos;
			for(int i=0; i<n; i++)
				if(poziomy[Q].ROAD[t->cover[i]].enemy>tmp){
					tmp=poziomy[Q].ROAD[t->cover[i]].enemy;
					pos=t->cover[i];
				}
			if(tmp){
				if(t->type==1)
					atakbalista(t,pos);
				else
					atakcatapult(t,pos);
			}
		}	
	}
	else{
		if(t->type==3)
			atakdragon(t);
		else
			atakvulcan(t);
	}

	//1-balista,2-catapult,3-shy dragon,4-vulcan,5-druid
			//przy ataku life=max(life-attack,0)
			//...
			//ile razy atak, policz najpierw ile druid4 i for int i do tej wartosci
			//podzial na wieze, ile razy atakuje(druid), attack mode
	}
	return;
}


int iteri;
bool new=false;
static bool runda(){
//LevelInfo[2]
//for(int i=0; i<15; i++)
//	gtk_button_set_label((GtkButton*)LevelInfo[2],g_strdup_printf("%d/15",i));

//tower attack
	int ileR=poziomy[Q].ileR;
	//while(i<ileEnemy){
		if(!new){
			if(enemyPattern[iteri]==-1){
				roundnr++;
				new=true;
			}
		}
		else{
			bool czy=false;
			for(int f=0; f<ileR; f++)
				if(poziomy[Q].ROAD[f].enemy){
					czy=true;
					break;
				}
			if(!czy){
				new=false;
				gold+=100*(roundnr+1)*GoldMult;
				updatePlansza();
				if(!ENDLESS){
					if(roundnr==15)
						win();
				}
				else{
					if(!roundnr%15)
						iteri=0;
						new=false;
				}
				return false;
			}
		}
		if(life<=0)
			fail();
		
		life-=poziomy[Q].ROAD[ileR-1].enemy;
		
		for(int j=ileR-1; j>0; j--){
			poziomy[Q].ROAD[j].enemy=poziomy[Q].ROAD[j-1].enemy;
		}
		if(new){
			poziomy[Q].ROAD[0].enemy=0;
		}
		else{
			poziomy[Q].ROAD[0].enemy=enemyPattern[iteri++]*HARD*(Q/2+1)*(10*(roundnr/15)+1);
			//printf("pat=%d,h=%d,q=%d,r=%d\n",enemyPattern[iteri-1],HARD,Q/2+1,(10*(roundnr/15)+1));
		}
		struct tower *t=poziomy[Q].tow;
		while(t!=NULL){
			if(t->type==5){
				t=t->next;
				continue;
			}
			ataktower(t,t->iletur);
			t=t->next;
		}
	updatePlansza();
	//}
	return true;
}

static void ROUND(){
	int  i=0, roundcount=0;
	if(!ENDLESS){
		while(roundcount<roundnr){
			if(enemyPattern[i]==-1)
				roundcount++;
			i++;
		}
	}
	else{
		while(roundcount<roundnr%15){
			if(enemyPattern[i]==-1)
				roundcount++;
			i++;
		}
	}
	iteri=i;
	g_timeout_add(200,(GSourceFunc)runda,NULL);
}

static void hardmode(){
	bool czy=true;
	for(int i=0; i<10; i++)
		if(unlocked[i]==7)
			czy=false;

	if(!czy)
		return;
	
	if(HARD==1){
		HARD=2;
		free(P);
		P=calloc(sizeof(struct pairGS),1);
		P->G=TD;
		strcpy(P->S,"HARDMODE ON!");
		dialog();
	}
	else{
		HARD=1;
		free(P);
		P=calloc(sizeof(struct pairGS),1);
		P->G=TD;
		strcpy(P->S,"HARDMODE OFF!");
		dialog();
	}
}

static void endless(){
	bool ALLSTAR=true;
	for(int i=0; i<10; i++)
		if(unlocked[i]!=3)
			ALLSTAR=false;
	if(!ALLSTAR)
		return;

	if(!ENDLESS){
		ENDLESS=true;
		free(P);
		P=calloc(sizeof(struct pairGS),1);
		P->G=TD;
		strcpy(P->S,"ENDLESSMODE ON!");
		dialog();
	}
	else{
		ENDLESS=false;
		free(P);
		P=calloc(sizeof(struct pairGS),1);
		P->G=TD;
		strcpy(P->S,"ENDLESSMODE OFF!");
		dialog();
	}
}


static void init(){
	FILE *level=fopen("level","r");
	for(int q=0;q<10;q++){
		int ileRoad=0;
		char tmp;
		fscanf(level,"%c",&tmp);
		if(tmp=='\n')
			fscanf(level,"%c",&tmp);
		
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
		poziomy[q].ileR=ileRoad;
		poziomy[q].ROAD=calloc(ileRoad*sizeof(struct RoadNr),1);
		DFS(q,poziomy[q].Sx,poziomy[q].Sy);
		poziomy[q].tow=NULL;
	}
	fclose(level);

	FILE *en=fopen("Enemy.txt","r");
	int tmp,j=0;
	while(!feof(en)){
		fscanf(en,"%d",&tmp);
		j++;
	}
	ileEnemy=j;
	enemyPattern=(int*)calloc(sizeof(int)*j,1);
	fclose(en);
	en=fopen("Enemy.txt","r");
	j=0;
	while(!feof(en)){
		fscanf(en,"%d",&tmp);
		enemyPattern[j++]=tmp;
	}
	fclose(en);

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

//	TabImage[0]=gtk_image_new_from_file("land.png");//mniejsze obrazy i dokonczyc

	//box init

	Box[0]=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);//world
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
			g_signal_connect(G_OBJECT(TabButton[i][j]),"clicked",G_CALLBACK(czyTower),NULL);
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
	g_signal_connect(G_OBJECT(UpgradeButton[3]),"clicked",G_CALLBACK(starup),NULL);
	gtk_grid_attach(GTK_GRID(UPLevel),UpgradeButton[3],0,1,1,1);
	UpgradeButton[4]=gtk_button_new_with_label("BUY");
	g_signal_connect(G_OBJECT(UpgradeButton[4]),"clicked",G_CALLBACK(starup),NULL);
	gtk_grid_attach(GTK_GRID(UPLevel),UpgradeButton[4],1,1,1,1);
	UpgradeButton[5]=gtk_button_new_with_label("BUY");
	g_signal_connect(G_OBJECT(UpgradeButton[5]),"clicked",G_CALLBACK(starup),NULL);
	gtk_grid_attach(GTK_GRID(UPLevel),UpgradeButton[5],2,1,1,1);
	UpgradeButton[6]=gtk_button_new_with_label("RESET");
	g_signal_connect(G_OBJECT(UpgradeButton[6]),"clicked",G_CALLBACK(resetupgrade),NULL);
	gtk_box_pack_start((GtkBox*)Box[1],UpgradeButton[6],TRUE,FALSE,0);
	UpgradeButton[7]=gtk_button_new_with_label("RETURN");
	g_signal_connect(G_OBJECT(UpgradeButton[7]),"clicked",G_CALLBACK(hideup),NULL);
	gtk_box_pack_start((GtkBox*)Box[1],UpgradeButton[7],TRUE,FALSE,0);
/*
	gtk_widget_set_name(Box[1],"land");
	gtk_widget_set_name(IleStar,"road");
	gtk_widget_set_name(UpgradeButton[0],"road");
	gtk_widget_set_name(UpgradeButton[1],"road");
	gtk_widget_set_name(UpgradeButton[2],"road");
	gtk_widget_set_name(UpgradeButton[3],"road");
	gtk_widget_set_name(UpgradeButton[4],"road");
	gtk_widget_set_name(UpgradeButton[5],"road");
	gtk_widget_set_name(UpgradeButton[6],"road");
*/
	//achievement init
	for(int i=0; i<6; i++){
		AchievementButton[i]=gtk_button_new_with_label("X");
		gtk_box_pack_start((GtkBox*)Box[2],AchievementButton[i],TRUE,FALSE,0);
	}
	AchievementButton[6]=gtk_button_new_with_label("RETURN");
	g_signal_connect(G_OBJECT(AchievementButton[6]),"clicked",G_CALLBACK(hideach),NULL);
	gtk_box_pack_start((GtkBox*)Box[2],AchievementButton[6],TRUE,FALSE,0);

	g_signal_connect(G_OBJECT(AchievementButton[4]),"clicked",G_CALLBACK(hardmode),NULL);
	g_signal_connect(G_OBJECT(AchievementButton[5]),"clicked",G_CALLBACK(endless),NULL);

}

static GtkWidget* gra(){
	TD=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(TD),"TOWER DEFENCE");
	gtk_window_set_position(GTK_WINDOW(TD),GTK_WIN_POS_CENTER_ALWAYS);
	gtk_container_set_border_width(GTK_CONTAINER(TD),10);
	g_signal_connect(G_OBJECT(TD),"destroy",G_CALLBACK(wq),NULL);

	init();

	update();
	
	return TD;
}

void TowDef(){//main function
	gtk_widget_hide(Main);
	GtkWidget *wys=gra();
	gtk_widget_show_all(wys);
	return;
/*
	ile przeciwnikow na runde, losowanie
	tworzyc po jednej turze i testowac
	

	int rx,ry;
	gtk_window_get_default_size(TD,rx,ry);
	gtk_resize(TD,rx,ry);


	LFZ-popsicle (NCS)
*/
}
