//�r��}�C���[�k �W�L�d��|�۰ʭ��s
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