#include <math.h>
//char*={'1','0','1','0'} => int=1010
int Char2Dec(char *str,int start ,int end){
    int result=0;
    int len=end-start+1;
    int count=0;
	//���W������10���X����a��result
    for(int i=end;i>=start;i--){
        result+=(str[i]-'0')*pow(10,count);
        count++;
    }
    return result;
}