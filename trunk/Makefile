AR = ar
CC = gcc
LD = gcc
DOXYGEN = doxygen
ARFLAGS = -cru
CFLAGS = -Wall -Wextra -Wundef -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wstrict-prototypes \
	-Wnested-externs -Werror -pedantic -std=c99 \
	-Wno-format-zero-length -Wformat-security -Wstrict-aliasing=2 \
	-Wmissing-format-attribute -Wunused -Wunreachable-code \
	-Wformat=2 -Werror-implicit-function-declaration \
	-Wmissing-declarations -Wmissing-prototypes
LDFLAGS = -L./

all: example palette2c

example: librosprite.a example.o
	${LD} -g -o $@ example.o ${LDFLAGS} -lrosprite -lSDL

palette2c: librosprite.a palette2c.o
	${LD} -g -o $@ palette2c.o ${LDFLAGS} -lrosprite

librosprite.a: librosprite.o
	${AR} ${ARFLAGS} librosprite.a librosprite.o

%.o: %.c
	${CC} -c -g ${CFLAGS} -o $@ $<

docs:
	${DOXYGEN}

clean:
	rm -f $(wildcard *.o) $(wildcard *.a) example palette
	rm -rf doc