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

static void wyswietlGTK(){
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
static void save(){}				//!!!
static void load(){}				//!!!

void snake(){
	printf("1 NEW GAME\n2 LOAD\n3 EXIT\n");
	while(true){
		if(kbhit){
			char c=getchar();
			if(c=='3')
				return;
			if(c=='1'){
				newGame();
				break;
			}
			if(c=='2'){
				load();
				break;
			}
		}
	}
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
	}
}
