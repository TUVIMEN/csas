VERSION = 2.0
CC = gcc -std=c99
CFLAGS = -O3 -pipe -march=native -Wall -Wextra -DVERSION=\"${VERSION}\"
LDFLAGS = -lncursesw -ltinfow
TARGET = csas

PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man
BINDIR = ${DESTDIR}${PREFIX}/bin
MANDIR = $(DESTDIR)${MANPREFIX}/man1

SRC = src/main.c src/load.c src/csas.c src/useful.c src/flexarr.c src/sort.c src/functions.c src/draw.c src/console.c src/calc.c src/preview.c src/expand.c
OBJ = ${SRC:.c=.o}

all: options csas

options:
	@echo ${TARGET} build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

csas: ${OBJ}
	${CC} ${CFLAGS} ${LDFLAGS} $^ -o ${TARGET}
	strip --discard-all ${TARGET}

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

dist: clean
	mkdir -p ${TARGET}-${VERSION}
	cp -r LICENSE Makefile README.md src csas.1 csasrc ${TARGET}-${VERSION}
	tar -c ${TARGET}-${VERSION} | xz -e9 > ${TARGET}-${VERSION}.tar.xz
	rm -rf ${TARGET}-${VERSION}

clean:
	rm -f ${TARGET} ${OBJ} ${TARGET}-${VERSION}.tar.xz

install: all
	mkdir -p ${BINDIR}
	cp -f ${TARGET} ${BINDIR}
	chmod 755 ${BINDIR}/${TARGET}
	mkdir -p ${MANDIR}
	sed "s/VERSION/${VERSION}/g" ${TARGET}.1 | bzip2 -9 > ${MANDIR}/${TARGET}.1.bz2
	chmod 644 ${MANDIR}/${TARGET}.1.bz2

uninstall:
	rm ${BINDIR}/${TARGET}\
		${MANDIR}/${TARGET}.1.bz2
