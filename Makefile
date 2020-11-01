SHELL	=	/bin/bash
CC	=	gcc -std=gnu17
LINK	=	-lpthread -lncurses -ltinfo
CFLAGS	=	-O2 -pipe -march=native -Wall -Wextra
TARGET	=	csas

MANDIR	=	/usr/local/man/man1
BINDIR	=	/usr/bin

OBJECTS = $(patsubst %.c, %.o, $(wildcard src/*.c))

all: $(OBJECTS)
	$(CC) $(LINK) $(CFLAGS) $^ -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	cp -f ${TARGET} ${BINDIR}
	chmod 755 ${BINDIR}/${TARGET}
	cp -f ${TARGET}.1 ${MANDIR}
	chmod 644 ${MANDIR}/${TARGET}.1

clean:
	find . -name "*.o" -exec rm "{}" \;
	rm ${TARGET}
