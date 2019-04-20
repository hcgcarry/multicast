#include <stdio.h>
//ex:int*={0,1,1,1,0,1,0,0} => char='\b01110100'
char binIntArray2BinChar(int *binIntArray,int start,int stop){
   int tmp=0;
   char result;
   int binIntArrayLen=stop-start+1;
   if(binIntArrayLen > 8){
       fprintf(stderr,"binIntArray2BinChar input int array must less than 8");
       exit(1);
   }
   //重開頭乘到尾 不需要用到pow
   for(int i=start;i<=stop;i++){
	   
       tmp=tmp*2+binIntArray[i];
   } 
   result=(char)tmp;
   return result;
}