#include <stdio.h>
//'\b00101100' => int[8]={0,0,1,0,1,1,0,0}
void char2Bin(char chr,int *result,int charBitSize){
   for(int i=charBitSize-1;i>=0;i--){
	   //利用跟1做and來判斷，如果該位置是1那麼結果就是1如果該位置是0那麼結果就是0
       result[charBitSize-1-i]= (chr >> i) & 1;
   } 
}


//print char 的2進位 
//debug用
void printChar2Bin(char chr,int charBitSize){
    int intArray[charBitSize];
	//把她轉乘int array在一個一個digit print出來
    char2Bin(chr,intArray,charBitSize);
    for(int i=0;i<charBitSize;i++){
        printf("%d",intArray[i]);
    }
}