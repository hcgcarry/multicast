//字串陣列的加法 超過範圍會自動重製
void addSeqNum(char *seqNum, int len)
{
    for (int i = len - 1; i >= 0; i--)
    {
        if (seqNum[i] == '9')
        {
            seqNum[i] = '0';
        }
        else
        {
            seqNum[i]++;
            break;
        }
    }
}