SHELL	=	/bin/bash
CC	=	gcc -std=gnu17
LINK	=	-lpthread -lncurses
CFLAGS	=	-O2 -march=native -Wall -Wextra
TARGET	=	csas

OBJECTS = $(patsubst %.c, %.o, $(wildcard src/*.c))

all: $(OBJECTS)
	$(CC) $(LINK) $(CFLAGS) $^ -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	find . -name "*.o" -exec rm "{}" \;
	rm ${TARGET}
