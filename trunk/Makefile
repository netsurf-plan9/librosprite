AR = ar
CC = gcc
LD = gcc
ARFLAGS = -cru
CFLAGS = -Wall -Wextra -Wundef -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wstrict-prototypes \
	-Wnested-externs -Werror -pedantic -std=c99 \
	-Wno-format-zero-length -Wformat-security -Wstrict-aliasing=2 \
	-Wmissing-format-attribute \
	-Wformat=2 -Werror-implicit-function-declaration
LDFLAGS = -L./
#-Wmissing-declarations -Wmissing-prototypes 

example: librosprite.a example.o
	${LD} -g -o $@ example.o ${LDFLAGS} -lrosprite -lSDL

palette2c: librosprite.a palette2c.o
	${LD} -g -o $@ palette2c.o ${LDFLAGS} -lrosprite

librosprite.a: librosprite.o
	${AR} ${ARFLAGS} librosprite.a librosprite.o

%.o: %.c
	${CC} -c -g ${CFLAGS} -o $@ $<

clean:
	rm -f $(wildcard *.o) $(wildcard *.a) example palette