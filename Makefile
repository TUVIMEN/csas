VERSION	=	2.0
CC	=	gcc -std=c99
LINK	=	-lncursesw -ltinfow
CFLAGS	=	-O3 -pipe -march=native -Wall -Wextra -DVERSION=\"${VERSION}\"
TARGET	=	csas

MANDIR	=	/usr/share/man/man1
BINDIR	=	/usr/bin

SRC	=	src/main.c src/load.c src/csas.c src/useful.c src/flexarr.c src/sort.c src/functions.c src/draw.c src/console.c src/calc.c src/preview.c src/expand.c
OBJ 	= 	${SRC:.c=.o}

all: ${OBJ}
	${CC} ${LINK} ${CFLAGS} $^ -o ${TARGET}
	strip --discard-all ${TARGET}

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

install: all
	mkdir -p ${BINDIR}
	cp -f ${TARGET} ${BINDIR}
	chmod 755 ${BINDIR}/${TARGET}
	sed "s/VERSION/${VERSION}/g" ${TARGET}.1 > ${MANDIR}/${TARGET}.1
	chmod 644 ${MANDIR}/${TARGET}.1
	cp csasrc /etc/

clean:
	find . -name "*.o" -exec rm "{}" \;
	rm ${TARGET}

uninstall:
	rm ${BINDIR}/${TARGET}
	rm ${MANDIR}/${TARGET}.1
