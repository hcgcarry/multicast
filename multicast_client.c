/* Receiver/client multicast Datagram example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include "multicast_client.h"
#include <string.h>

#define seqNumLen 4
#define errorCheckLen 4
#define headerPadding 8
#define databufferSize 10000
#define fecDataBufferSize 2*10000
struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[databufferSize];
void printBuffer(char *buffer,int len);
 
int main(int argc, char *argv[])
{
	int ipAddress=inet_addr(argv[1]);
	char *filename=argv[2];
/* Create a datagram socket on which to receive. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
		perror("Opening datagram socket error");
		exit(1);
	}
	else
	printf("Opening datagram socket....OK.\n");
		 
	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	//SOL_SOCKET 式指名socket level的選項設定
	
	{
		int reuse = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("Setting SO_REUSEADDR error");
		close(sd);
		exit(1);
	}
	else
		printf("Setting SO_REUSEADDR...OK.\n");
	}
	 
	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(4321);
	//inaddr_any 代表本機分配到的任何ipv4的位址
	localSock.sin_addr.s_addr = INADDR_ANY;
	
	printf("local sock ip %s\n",inet_ntoa(localSock.sin_addr));
	if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
	{
		perror("Binding datagram socket error");
		close(sd);
		exit(1);
	}
	else
		printf("Binding datagram socket...OK.\n");
	 
	/* Join the multicast group 226.1.1.1 on the local 203.106.93.94 */
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
	//把這個interface(網卡) 榜到multicast群組裏面
	group.imr_interface.s_addr = ipAddress;
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
	{
		perror("Adding multicast group error");
		close(sd);
		exit(1);
	}
	else
		printf("Adding multicast group...OK.\n");
	 
	/* Read from the socket. */
	datalen = sizeof(databufferSize);
	if(read(sd, databuf, datalen) < 0)
	{
		perror("Reading datagram message error");
		close(sd);
		exit(1);
	}
	else
	{
		printf("Reading datagram message...OK.\n");
		printf("The message from multicast server is: \"%s\"\n", databuf);
	}
	
	if(argc<3){
		fprintf(stderr,"there is no enough parameter");
		exit(1);
	}
	FILE *fp;
	//打開要寫的檔案
	if((fp=fopen(filename,"wb"))==NULL){
		perror("there have error at open file");
		exit(1);
	}
	int numBytes;
	int corrupt=0;
	int curSeqNum=0,preSeqNum=0;
	int curLoss=0,totalLoss=0;
	int curNotReceivePackageNum=0;
	int totalNotReceivePackageNum=0;
	int totalSendPackNum=0;
	int serverGiveErrorChecker=0;
	int clientCheckErrorChecker=0;
	int originDataClientErrorChecker=0;
	char fecDataBuf[2*databufferSize];
	char originDataBuf[databufferSize];
	int contentCorrupt=0;	
	int withFecAndWithoutFecDifCount=0;
	int withoutFecDataCorrputCount=0;
	int withFecDataCorrputCount=0;
	int totalReceivePackageNum=0;
	int numBytesLessThanZeroCount=0;
	while(1){
		totalReceivePackageNum++;
		puts("#################new");

		numBytes=read(sd,fecDataBuf,2*databufferSize);
		//printf("fecdatabuf numBytes:%d \n",numBytes);

		//如果read出問題
		if(numBytes <0){
			perror("receive bytes <0");
			//計算出問題的個數
			numBytesLessThanZeroCount++;
			
			continue;
		}
		//因為fecDataBuf read到的事經過hamming encode的所以這邊要解碼
		for(int i=0;i<databufferSize;i++){
			//解碼的程式 可以修正錯誤
			databuf[i]=hamming8BitDecode(fecDataBuf+i*2);
			//解碼成沒有修正錯誤的 用來計算有解碼跟沒解碼的loss rate difference
			originDataBuf[i]=decode2OriginChar(fecDataBuf+i*2);
		}
		//因為decode 所以修正得到的資料量 headerpadding是 header: 包的序號長度加上錯誤碼的長度
		numBytes=(numBytes-headerPadding*2)/2;

		
		//將header的seqnum從字元轉成數字得到seqnum
		curSeqNum=Char2Dec(databuf,0,3);

		//將header的errorchecker從字元轉成數字 這個式server傳來給client用來判斷client data有沒錯誤的
		serverGiveErrorChecker=Char2Dec(databuf,4,7);
		//client 將char相加得到client端的errorchecker用來跟server給的errorchecker比對
		clientCheckErrorChecker=errorChecker(databuf+headerPadding,numBytes);
		//這個是計算原始沒有經過hamming修正的data的char相加得到errorchecker
		originDataClientErrorChecker=errorChecker(originDataBuf+headerPadding,\
		numBytes);
		//client errorchecker與server給的errorchecker不一樣的話就代表封包錯誤
		if(clientCheckErrorChecker!=serverGiveErrorChecker){
			puts("@@@with fec data has different with server send data");
			withFecDataCorrputCount++;
		}
		//比對有fec修正和沒有fec修正的errorchecker
		if(clientCheckErrorChecker!=originDataClientErrorChecker){
			puts("$$$difference between fec and without fec");
			withFecAndWithoutFecDifCount++;
		}
		//比對沒fec修正的data是否正確
		if(originDataClientErrorChecker!=serverGiveErrorChecker){
			puts("%%%without fec data has difference with server send data");
			withoutFecDataCorrputCount++;
		}
		printf("clientErrorChecker:%d originDataErrorChecker:%d serverErrorChecker:%d\n",\
		clientCheckErrorChecker,originDataClientErrorChecker, serverGiveErrorChecker);
		//因為我們的seqnum的範圍再0~10000所以如果seqnum重製的話就會比前一個seqnum小(重0開始)
		//而如果要計算沒接收到的封包個數的話就要對前一個seqnum做修正將其剪掉9999
		if(curSeqNum<preSeqNum){
			preSeqNum=preSeqNum-9999;
		}
		//這一次封包的序號跟上一次封包的序號比對可以找出丟失的封包個數
		curNotReceivePackageNum=curSeqNum-preSeqNum-1;
		//所有丟失的封包個數
		totalNotReceivePackageNum+=curNotReceivePackageNum;
		if(curNotReceivePackageNum>0){
			puts("!!!!!!!!!!!!!!!!!!!!!!!!packages loss!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		}
			printf("preSeqNum:%d curSeqNum:%d curNotReceivePackageNum:%d totalNotReceivePackageNum:%d \n"\
			,preSeqNum,curSeqNum,curNotReceivePackageNum,totalNotReceivePackageNum);
		preSeqNum=curSeqNum;
		//printBuffer(databuf+headerPadding,numBytes);
		//如果寫入的numBytes比資料長度常就會寫入亂碼
		numBytes=fwrite(databuf+headerPadding,sizeof(char),numBytes,fp);
		if(numBytes < databufferSize-headerPadding){
			puts("&&&&&&&&&&&&&&&&&&&&&&");
			puts("receive complete");
			printf("withFecAndWithoutFecDifCount:%d withoutFecDataCorrputCount:%d contentCorrupt:%d\n",\
			withFecAndWithoutFecDifCount, withoutFecDataCorrputCount ,contentCorrupt);
			break;
		}




	}
	printf("total package not receive:%d\n",totalNotReceivePackageNum);
	printf("with fec package content error rate:%f\%\n",\
	(float)100*withFecDataCorrputCount/totalReceivePackageNum);
	printf("without fec package content error rate:%f\%\n",\
	(float)100*withoutFecDataCorrputCount/totalReceivePackageNum);
	printf("with fec loss rate:%f\%\n",\
	(float)100*(totalNotReceivePackageNum+withFecDataCorrputCount)/\
	(totalNotReceivePackageNum+ totalReceivePackageNum));
	printf("without fec loss rate:%f\%\n",\
	(float)100*(totalNotReceivePackageNum+withoutFecDataCorrputCount)/\
	(totalNotReceivePackageNum+ totalReceivePackageNum));
	

	
	
}

