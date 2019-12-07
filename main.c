#include <stdio.h>
#include <stdlib.h>
#include "linker.h"

int main(){
	intro();
	while(true)
		if(kbhit()){
			char c;
			scanf("%c",&c);
			if(c=='a')
				snake();	
		}
}

