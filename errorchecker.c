//將char array裡面的char相加
//用來製造判斷錯誤的識別碼
int errorChecker(char *databuf,int len){
    unsigned int sum=0;
	int thersold=10000;
	for(int i=0;i<len;i++){
		sum+=databuf[i];
		//超過範圍的話就給他取餘數
		if(sum > thersold){
			sum=sum%thersold;
		}
	}
	return (int)sum;
}