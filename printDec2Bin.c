#include <stdio.h>
//debug¥Î
void printDec2Bin(int bin,int decLen)
{
    for (int i = decLen-1; i >= 0; i--)
    {

        if ((bin >> i) & 1)
            printf("1");
        else
        {
            printf("0");
        }
    }
    puts("");
}