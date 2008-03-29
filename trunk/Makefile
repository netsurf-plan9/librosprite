AR = ar
CC = gcc
LD = gcc
DOXYGEN = doxygen
INSTALL = install
SED = sed
MKDIR = mkdir

ARFLAGS = -cru
CFLAGS = -Wall -Wextra -Wundef -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wstrict-prototypes \
	-Wnested-externs -Werror -pedantic -std=c99 \
	-Wno-format-zero-length -Wformat-security -Wstrict-aliasing=2 \
	-Wmissing-format-attribute -Wunused -Wunreachable-code \
	-Wformat=2 -Werror-implicit-function-declaration \
	-Wmissing-declarations -Wmissing-prototypes
LDFLAGS = -L./

# Installation prefix, if not already defined (e.g. on command line)
PREFIX ?= /usr/local

.PHONY: all clean docs install

# Installation prefix, if not already defined (e.g. on command line)
PREFIX ?= /usr/local

.PHONY: all clean docs install

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
	rm -f $(wildcard *.o) $(wildcard *.a) example palette2c librosprite.pc
	rm -rf doc

install: librosprite.a
	$(MKDIR) -p $(PREFIX)/lib/pkgconfig
	$(MKDIR) -p $(PREFIX)/lib
	$(MKDIR) -p $(PREFIX)/include
	$(INSTALL) --mode=644 -t $(PREFIX)/lib librosprite.a
	$(INSTALL) --mode=644 -t $(PREFIX)/include librosprite.h
	$(SED) -e 's#PREFIX#$(PREFIX)#' librosprite.pc.in > librosprite.pc
	$(INSTALL) --mode=644 -t $(PREFIX)/lib/pkgconfig librosprite.pc

