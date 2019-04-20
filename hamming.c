#include "hamming.h"
#include <stdio.h>
//雖然輸入的事一個string但是只取前練兩個char使用
void hamming8BitEncode(char *input){
    //puts("hamming8BitEncode start #################");
    char head=input[0],tail;
    int headArray[8];
    int resultArray[16];
    int ham=0;
    memset(resultArray,0,16*sizeof(int));
	//head一開始的時候是存放要encode的char
	//將他轉成array的形式我們才能操作bit
    char2Bin(head,headArray,8);

	//hamming code的bit的放法
    resultArray[2]=headArray[0];
    resultArray[4]=headArray[1];
    resultArray[5]=headArray[2];
    resultArray[6]=headArray[3];
    resultArray[8]=headArray[4];
    resultArray[9]=headArray[5];
    resultArray[10]=headArray[6];
    resultArray[11]=headArray[7];
    int init=0;
    //for loop完會得到hamming的用來調教的那幾個bit，我們令他為ham
	//這邊就是照著hamming algo去寫
    for(int i=0;i<12;i++){
		//如果還沒有ham的值
        if(init==0 && resultArray[i]==1){
            ham=i+1;
            init=1;
            //printf("ham start hamindex:%d\n",i);
            //printDec2Bin(i+1,8);
            //printDec2Bin(ham,8);

        }
        else if(resultArray[i]==1){
			//作補數
            ham=ham^(i+1);
            //printf("index:%d\n",i);
            //printDec2Bin(i+1,8);
            //printDec2Bin(ham,8);
        }
    }
    //printf("ham %d",ham);
    int hamArrayLen=32;
    int hamArray[hamArrayLen];
	//將ham寫弄陣列
    dec2Bin(ham,hamArray);
    //puts("");
    //printf("hamarray:");
    for(int i=0;i<32;i++){
        //printf("%d",hamArray[i]);
    }
	//將ham放到最終的結果裡面
    resultArray[0]=hamArray[hamArrayLen-1];
    resultArray[1]=hamArray[hamArrayLen-2];
    resultArray[3]=hamArray[hamArrayLen-3];
    resultArray[7]=hamArray[hamArrayLen-4];
    //puts("");
    
	//將array轉回char
    head=binIntArray2BinChar(resultArray,0,7);
    tail=binIntArray2BinChar(resultArray,8,15);
    //puts("head:");
    //printChar2Bin(head,8);
    //puts("tail:");
    //printChar2Bin(tail,8);

	//放回去陣列裡面，更新了前面兩個值

    input[0]=head;
    input[1]=tail;
    //puts("hamming8BitEncode end #################");

}


//解碼
char hamming8BitDecode(char *inputStr){
    char head=inputStr[0],tail=inputStr[1];
    int headArray[8],tailArray[8];
	//char轉成binary的int array 讓我們可以操作bit
    char2Bin(head,headArray,8);
    char2Bin(tail,tailArray,8);
    int combinArray[16];
    int hamCode=0;
    int init=0;
    for(int i=0;i<8;i++){
        combinArray[i]= headArray[i];
    }
    for(int i=0;i<8;i++){
        combinArray[i+8]= tailArray[i];
    }
	//這邊是hamming的algo
    for(int i=0;i<12;i++){
		//因為hamcode是要做補數用的不能給她隨邊初始化一個值
		//所以等待有bit是一個情況再付值給他
        if(init==0 && combinArray[i]!=0){
            //printf(" ham start hamindex:%d\n",i);
            //printDec2Bin(hamCode,8);
            hamCode=i+1;
            init=1;
            //printDec2Bin(i+1,8);
            //printDec2Bin(hamCode,8);
            }
		//作補數
        else if(combinArray[i]!=0){
                //printf("index:%d\n",i);
                //printDec2Bin(hamCode,8);
                hamCode=hamCode^(i+1);
                //printDec2Bin(i+1,8);
                //printDec2Bin(hamCode,8);
        }
    }
            
	//如果hamcode是0的話就代表沒有錯誤不用修正 如果友值得話錯誤的就是hamcode的那個位置 將它做補數就能修正了
    if(hamCode!=0){
        combinArray[hamCode-1]=!(combinArray[hamCode-1]);
    }
	//將修正完的code放回array
    int resultArray[8];
    resultArray[0]=combinArray[2];
    resultArray[1]=combinArray[4];
    resultArray[2]=combinArray[5];
    resultArray[3]=combinArray[6];
    resultArray[4]=combinArray[8];
    resultArray[5]=combinArray[9];
    resultArray[6]=combinArray[10];
    resultArray[7]=combinArray[11];
	//將binary int array轉回char
    return binIntArray2BinChar(resultArray,0,7);
}


//這邊單純的就是將encode的data轉換回沒有encode的data 沒有修正的功能
char decode2OriginChar(char *str){
    char head=str[0];
    char tail=str[1];
    int combinArray[16];
    char2Bin(head,combinArray,8);
    char2Bin(tail,combinArray+8,8);
    int resultArray[8];
    resultArray[0]=combinArray[2];
    resultArray[1]=combinArray[4];
    resultArray[2]=combinArray[5];
    resultArray[3]=combinArray[6];
    resultArray[4]=combinArray[8];
    resultArray[5]=combinArray[9];
    resultArray[6]=combinArray[10];
    resultArray[7]=combinArray[11];
    return binIntArray2BinChar(resultArray,0,7);
}