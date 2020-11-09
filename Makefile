SHELL	=	/bin/bash
CC		=	gcc -std=c11
LINK	=	-lpthread -lncursesw -ltinfow
CFLAGS	=	-O2 -pipe -march=native -Wall -Wextra
TARGET	=	csas

MANDIR	=	/usr/local/man/man1
BINDIR	=	/usr/bin

OBJECTS = $(patsubst %.c, %.o, $(wildcard src/*.c))

all: $(OBJECTS)
	$(CC) $(LINK) $(CFLAGS) $^ -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	cp -f ${TARGET} ${BINDIR}
	chmod 755 ${BINDIR}/${TARGET}
	cp -f csas.desktop /usr/share/applications
	cp -f csas.png /usr/share/icons/hicolor/64x64/apps
	cp -f ${TARGET}.1 ${MANDIR}
	chmod 644 ${MANDIR}/${TARGET}.1

clean:
	find . -name "*.o" -exec rm "{}" \;
	rm ${TARGET}

uninstall:
	rm ${BINDIR}/${TARGET}
	rm ${MANDIR}/${TARGET}.1
