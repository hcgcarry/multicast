//�Nchar array�̭���char�ۥ[
//�Ψӻs�y�P�_���~���ѧO�X
int errorChecker(char *databuf,int len){
    unsigned int sum=0;
	int thersold=10000;
	for(int i=0;i<len;i++){
		sum+=databuf[i];
		//�W�L�d�򪺸ܴN���L���l��
		if(sum > thersold){
			sum=sum%thersold;
		}
	}
	return (int)sum;
}