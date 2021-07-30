CC	=	gcc -std=c99
LINK	=	-lpthread -lncursesw -ltinfow
CFLAGS	=	-g -pipe -march=native -Wall -Wextra
TARGET	=	csas

MANDIR	=	/usr/local/man/man1
BINDIR	=	/usr/bin

SRC=src/console.c  src/draw.c  src/expand.c  src/functions.c  src/inits.c  src/load.c  src/loading.c  src/main.c  src/preview.c  src/sort.c  src/useful.c
OBJ = ${SRC:.c=.o}

all: ${OBJ}
	${CC} ${LINK} ${CFLAGS} $^ -o ${TARGET}
	#strip --discard-all ${TARGET}

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

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
