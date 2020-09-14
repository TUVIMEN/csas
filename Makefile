CC=gcc -std=gnu17
CXX=g++ -std=c++17
LINK=-lpthread -lncurses
CFLAGS=-Wall -O2 -march=native #-fsanitize=address
CXXFLAGS=-Wall -O2 -pipe -march=native
TARGET=main

main: main.o Chars.o Bulk.o Usefull.o Load.o Sort.o Functions.o FastRun.o
	${CC} ${LINK} ${CFLAGS} Bulk.o Usefull.o Load.o Sort.o Functions.o Chars.o FastRun.o main.o -o ${TARGET}
FastRun.o:
	$(CC) $(LINK) $(CFLAGS) -c Sources/FastRun.c -o FastRun.o
Bulk.o:
	$(CC) $(LINK) $(CFLAGS) -c Sources/Bulk.c -o Bulk.o
Chars.o:
	$(CC) $(LINK) $(CFLAGS) -c Sources/Chars.c -o Chars.o
Usefull.o:
	$(CC) $(LINK) $(CFLAGS) -c Sources/Usefull.c -o Usefull.o
Load.o:
	$(CC) $(LINK) $(CFLAGS) -c Sources/Load.c -o Load.o
Sort.o:
	$(CC) $(LINK) $(CFLAGS) -c Sources/Sort.c -o Sort.o
Functions.o:
	$(CC) $(LINK) $(CFLAGS) -c Sources/Functions.c -o Functions.o
main.o:
	$(CC) $(LINK) $(CFLAGS) -c Sources/main.c -o main.o

clean:
	rm Chars.o
	rm Bulk.o
	rm Usefull.o
	rm Load.o
	rm Sort.o
	rm Functions.o
	rm FastRun.o
	rm main.o
	rm ${TARGET}
