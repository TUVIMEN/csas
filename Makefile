SHELL	=	/bin/bash
CC	=	gcc -std=gnu17
LINK	=	-lpthread -lncurses
CFLAGS	=	-Wall -g -march=native -Wextra
TARGET	=	csas

OBJECTS = $(patsubst %.c, %.o, $(wildcard src/*.c))

all: $(OBJECTS)
	$(CC) $(LINK) $(CFLAGS) $^ -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	find . -name "*.o" -exec rm "{}" \;
	rm ${TARGET}
