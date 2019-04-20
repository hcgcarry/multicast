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

		if(numBytes <0){
			perror("receive bytes <0");
			numBytesLessThanZeroCount++;
			
			continue;
		}
		for(int i=0;i<databufferSize;i++){
			//將經過fec encode的data decode ，如果有bit錯誤的話會修正
			databuf[i]=hamming8BitDecode(fecDataBuf+i*2);
			//將其decode成沒有經過修正的data ，用來比對有經過fec和沒有經過fec的封包錯誤率
			originDataBuf[i]=decode2OriginChar(fecDataBuf+i*2);
		}
		//因為data有經過decode所以資料長度要做修正
		numBytes=(numBytes-headerPadding*2)/2;

		
		//將放在header的封包序號擷取下來
		curSeqNum=Char2Dec(databuf,0,3);

		//將放在header的錯誤判別碼擷取下來，這個是server給的用來派段client接受到的data有沒有錯誤
		serverGiveErrorChecker=Char2Dec(databuf,4,7);
		//client 計算經過fec的data的errorchecker 就是將每一個data的char相加
		clientCheckErrorChecker=errorChecker(databuf+headerPadding,numBytes);
		//client計算沒經過fec的data的errorchecker 用來跟有經過fec的封包錯誤率做比對
		originDataClientErrorChecker=errorChecker(originDataBuf+headerPadding,\
		numBytes);
		//計算經過fec的data錯誤的次數
		if(clientCheckErrorChecker!=serverGiveErrorChecker){
			puts("@@@with fec data has different with server send data");
			withFecDataCorrputCount++;
		}
		//經過fec和沒有經過fec的data不一樣的次數
		if(clientCheckErrorChecker!=originDataClientErrorChecker){
			puts("$$$difference between fec and without fec");
			withFecAndWithoutFecDifCount++;
		}
		//沒有經過fec的data錯誤次數
		if(originDataClientErrorChecker!=serverGiveErrorChecker){
			puts("%%%without fec data has difference with server send data");
			withoutFecDataCorrputCount++;
		}
		printf("clientErrorChecker:%d originDataErrorChecker:%d serverErrorChecker:%d\n",\
		clientCheckErrorChecker,originDataClientErrorChecker, serverGiveErrorChecker);
		//因為封包序號在0~10000的範圍內，如果超過10000就會重製成0所以會變得比previous的封包序號小
		//而我們是透過current的封包序號減掉previous的封包序號來計算丟失的封包的 所以如果重製的話要做修正
		if(curSeqNum<preSeqNum){
			preSeqNum=preSeqNum-9999;
		}
		//計算這一次read計算到的丟失封包數
		curNotReceivePackageNum=curSeqNum-preSeqNum-1;
		//目前總共丟失的封包數
		totalNotReceivePackageNum+=curNotReceivePackageNum;
		if(curNotReceivePackageNum>0){
			puts("!!!!!!!!!!!!!!!!!!!!!!!!packages loss!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		}
			printf("preSeqNum:%d curSeqNum:%d curNotReceivePackageNum:%d totalNotReceivePackageNum:%d \n"\
			,preSeqNum,curSeqNum,curNotReceivePackageNum,totalNotReceivePackageNum);
		preSeqNum=curSeqNum;
		//printBuffer(databuf+headerPadding,numBytes);
		//如果寫入的numBytes比資料長度常就會寫入亂碼
		//將資料寫道file中 databuf+headerPadding是因為我們將前面幾個bit當作header存放seqnum和errorchecker
		numBytes=fwrite(databuf+headerPadding,sizeof(char),numBytes,fp);
		//判斷是否結尾
		if(numBytes < databufferSize-headerPadding){
			puts("&&&&&&&&&&&&&&&&&&&&&&");
			puts("receive complete");
			printf("withFecAndWithoutFecDifCount:%d withoutFecDataCorrputCount:%d contentCorrupt:%d\n",\
			withFecAndWithoutFecDifCount, withoutFecDataCorrputCount ,contentCorrupt);
			break;
		}




	}
	//這邊就是將資料顯示出來
	printf("with fec package content error rate:%d\n",\
	withFecDataCorrputCount/totalReceivePackageNum);
	printf("without fec package content error rate:%d\n",\
	withoutFecDataCorrputCount/totalReceivePackageNum);
	printf("with fec loss rate:%d\n",\
	(totalNotReceivePackageNum+withFecDataCorrputCount)/\
	(totalNotReceivePackageNum+ totalReceivePackageNum));
	printf("without fec loss rate:%d\n",\
	(totalNotReceivePackageNum+withoutFecDataCorrputCount)/\
	(totalNotReceivePackageNum+ totalReceivePackageNum));
	

	
	
}

