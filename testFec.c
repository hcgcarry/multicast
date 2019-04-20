#include <stdio.h>
#include <math.h>
#include "testFec.h"

//這個事證明我的fec機制有用的
int main(void){
	
   char array[4];
   char tmp;
   int tmpArray[16];
   //testChar 是要給他encode 再 decode看看有沒有一樣的字元
   char testChar='a';
   array[0]=testChar;

   printf("test char:%c \n",array[0]);
   printf("test char bin:\n");
   //print char的2進位
   printChar2Bin(array[0],8);
   puts("############");
   char head,tail;
   for(int i=0;i<12;i++){
      array[0]=testChar;
	  //加密
      hamming8BitEncode(array);
	  //將array[0]和[1]的char轉乘binary的int array
      char2Bin(array[0],tmpArray,8);
      char2Bin(array[1],tmpArray+8,8);
      printf("出錯前:\n");
	  //將出錯前的bit print出來
      for(int i=0;i<12;i++){
         printf("%d",tmpArray[i]);
      }
      puts("");
	  //我們故意將第i個bit調整錯誤 看看最後結果有沒有修正回來
      tmpArray[i]=!tmpArray[i];
	  //轉回char放回array
      array[0]=binIntArray2BinChar(tmpArray,0,7);
      array[1]=binIntArray2BinChar(tmpArray,8,15);
      printf("調整讓第%d個bit出錯:\n",i);
      for(int i=0;i<12;i++){
         printf("%d",tmpArray[i]);
      }
      puts("");

	  //decode array
      tmp=hamming8BitDecode(array);
	  //如果執行的話可以發現最後的結果都跟一開始的一樣
      printf("decode:%c \n",tmp);
      printf("decode bin:");
      printChar2Bin(tmp,8);
      puts("$$$$$$$$$$$$$$$$$$$$$$$$while");
      puts("");
   }
   
   return 0;

}