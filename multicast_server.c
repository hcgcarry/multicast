/* Send Multicast Datagram code example. */
#include "multicast_server.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define databufferSize 10000
#define errorCheckLen 4
#define seqNumLen 4
//注意下面這個需要括號起來不然會出錯
#define headerPadding (seqNumLen+errorCheckLen)
//in_addr 用來表示32位元的ipv4位址
struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
char databuf[databufferSize] = "Multicast test message.";
int datalen = sizeof(databufferSize);
 
void dec2DecCharArray(int dec,char *charArray,int len);
int main (int argc, char *argv[ ])
{
/* Create a datagram socket on which to send. */
	int ipAddress=inet_addr(argv[1]);
	char *filename=argv[2];
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
	  perror("Opening datagram socket error");
	  exit(1);
	}
	else
	  printf("Opening the datagram socket...OK.\n");
	 
	/* Initialize the group sockaddr structure with a */
	/* group address of 225.1.1.1 and port 5555. */
	memset((char *) &groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
	groupSock.sin_port = htons(4321);
	 
	/* Disable loopback so you do not receive your own datagrams.
	{
	char loopch = 0;
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
	{
	perror("Setting IP_MULTICAST_LOOP error");
	close(sd);
	exit(1);
	}
	else
	printf("Disabling the loopback...OK.\n");
	}
	*/
	 
	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */
	//設定傳送資料到multicast ip 的 local ip
	localInterface.s_addr = ipAddress;
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
	{
	  perror("Setting local interface error");
	  exit(1);
	}
	else
	  printf("Setting the local interface...OK\n");
	/* Send a message to the multicast group specified by the*/
	/* groupSock sockaddr structure. */
	/*int datalen = 1024;*/
	if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
	{
		perror("Sending datagram message error");
	}
	else
	  printf("Sending datagram message...OK\n");
	 
	/* Try the re-read from the socket if the loopback is not disable
	好像沒用
	if(read(sd, databuf, datalen) < 0)
	{
	perror("Reading datagram message error\n");
	close(sd);
	exit(1);
	}
	else
	{
	printf("Reading datagram message from client...OK\n");
	printf("The message is: %s\n", databuf);
	}
	*/

	if(argc<3){
		fprintf(stderr,"there is no enough paremeter");
	}
	
	char seqNum[seqNumLen+1]={'0','0','0','0'};
	char errorCheckArray[errorCheckLen+1];
	memset(errorCheckArray,'0',sizeof(char)*(errorCheckLen+1));
	FILE *fp;
	int realBytes=0;
	int freadBytes=0;
	if((fp=fopen(filename,"rb"))==NULL){
		perror("send file open error");
	}
	int errorCheck=0;
	char fecDataBuf[2*databufferSize];
	while(!feof(fp)){
		//重製用來暫存錯誤辨別碼的陣列 
		for(int i=0;i<4;i++){
			errorCheckArray[i]='0';
		}
		puts("######################new");
		//將包的序列加一 超過10000就回重製回0
		addSeqNum(seqNum,seqNumLen);
		//將這次封包的序列放到要傳陣列的前面
		for(int i=0;i<4;i++){
			databuf[i]=seqNum[i];
		}
		
		//重file裡面讀取資料
		freadBytes=fread(databuf+headerPadding,sizeof(char),databufferSize-headerPadding,fp);
		//製造用來給client判斷資料有沒有錯誤的辨別代碼
		errorCheck=errorChecker(databuf+headerPadding,freadBytes);
		//printf("freadBytes:%d errorcheck :%d\n",freadBytes,errorCheck);
		//這個是把int的errorcheck轉換成array的形式好讓我們可以把他一個數字一個數字放到databuf的陣列裡面
		dec2DecCharArray(errorCheck,errorCheckArray,errorCheckLen);
		//put errorcheck to databuf
		for(int i=0;i<4;i++){
			//printf("%c",errorCheckArray[i]);
			databuf[seqNumLen+i]=errorCheckArray[i];
		}
		//printBuffer(databuf,freadBytes+headerPadding);
		

		//encode
		//fecDataBuf有兩倍原始資料的長度，因為加密的一個char會變成兩個char
		for(int i=0;i<databufferSize;i++){
			fecDataBuf[2*i]= databuf[i];
			//加密
			hamming8BitEncode(fecDataBuf+2*i);
		}
		//printBuffer(fecDataBuf,2*(freadBytes+headerPadding));

		//傳送到client端
		realBytes= sendto(sd, fecDataBuf, 2*(headerPadding+freadBytes), 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
	
		printf("sending sequence NO:%s errorchecker:%d sendBytes:%d\n",seqNum,errorCheck,realBytes);
	}
	printf("send complete");

	
	
}


//int 轉成一個digit一個digit的char陣列
void dec2DecCharArray(int dec,char *charArray,int len){
	int i=len-1;
	while(dec >0){
		charArray[i]=dec %10+'0';
		//printf("chararray i:%d value:%d\n",i,charArray[i]);
		dec=(int)(dec/10);
		i--;

	}

}

