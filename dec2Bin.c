#include <stdio.h>

//想法很簡單就是用bitwise去check那個位元是一就把1存到相同位置的陣列
//用來放答案的result 因該要有32長度


void dec2Bin(int dec,int *result){
    for(int i=31;i>=0;i--){
        if(dec>>i & 1 == 1){
           result[31-i]=1; 
        }
        else
        {
            result[31-i]=0;
        }
        
    }
}