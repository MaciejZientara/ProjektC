#include <stdio.h>
#include <stdlib.h>
#include "linker.h"

int main(){
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
}

