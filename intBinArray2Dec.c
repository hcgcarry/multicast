#include <stdio.h>
#include <math.h>
//好像用不到
//將int array轉換成int
int intBinArray2Dec(int *intBinArray,int intBinArrayLen){
    int sum=0;
    for(int i=0;i<intBinArrayLen;i++){
       sum+=pow(2,(intBinArrayLen-1-i))*intBinArray[i];
    }
    return sum;
}