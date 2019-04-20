all:server client testFec


cc=gcc

#main:main.o hamming.o dec2Bin.o char2Bin.o binIntArray2BinChar.o intBinArray2Dec.o printDec2Bin.o\
	${cc}  -g -o main intBinArray2Dec.o main.o \
	hamming.o dec2Bin.o char2Bin.o binIntArray2BinChar.o  printDec2Bin.o -lm
#intBinArray2Dec.o: intBinArray2Dec.c\
	gcc -g -c $< -o $@ -lm
clean:
	rm *.o

server:multicast_server.o addSeqNum.o errorchecker.o hamming.o\
char2Bin.o dec2Bin.o binIntArray2BinChar.o printDec2Bin.o printBuffer.o
	${cc}  -g -o server multicast_server.o addSeqNum.o errorchecker.o hamming.o \
char2Bin.o dec2Bin.o binIntArray2BinChar.o printDec2Bin.o printBuffer.o


client:multicast_client.o Char2Dec.o errorchecker.o hamming.o\
char2Bin.o dec2Bin.o binIntArray2BinChar.o printDec2Bin.o printBuffer.o
	${cc}  -g -o client multicast_client.o Char2Dec.o errorchecker.o hamming.o \
char2Bin.o dec2Bin.o binIntArray2BinChar.o printDec2Bin.o  printBuffer.o -lm

Char2Dec.o:Char2Dec.c
	${cc}  -g -c $< -o $@ 

testFec:testFec.o \
char2Bin.o dec2Bin.o binIntArray2BinChar.o printDec2Bin.o hamming.o
	${cc}  -g -o testFec testFec.o \
char2Bin.o dec2Bin.o binIntArray2BinChar.o printDec2Bin.o hamming.o

