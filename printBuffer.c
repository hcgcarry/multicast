#include <stdio.h>
//debug¥Î
void printBuffer(char *buffer,int len){
	for(int i=0;i<len;i++){
        if(i%5==0)puts("");
		printChar2Bin(buffer[i],8);
		printf(" ");
	}
	puts("");
	for(int i=0;i<len;i++){
		printf("%c",buffer[i]);
	}
	puts("");
}