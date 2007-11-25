AR = ar
CC = gcc
LD = gcc
ARFLAGS = -cru
CFLAGS = -Wall -Wextra -Wundef -Wpointer-arith -Wcast-align \
	-Wwrite-strings \
	 -Wnested-externs -Werror -pedantic -std=c99
LDFLAGS = -L./
#-Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes

example: libsprite.a example.o
	${LD} -g -o $@ example.o ${LDFLAGS} -lsprite -lSDL

libsprite.a: libsprite.o
	${AR} ${ARFLAGS} libsprite.a libsprite.o

%.o: %.c
	${CC} -c -g ${CFLAGS} -o $@ $<