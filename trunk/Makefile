AR = ar
CC = gcc
LD = gcc
DOXYGEN = doxygen
INSTALL = install
SED = sed
MKDIR = mkdir
PKG_CONFIG = pkg-config

ARFLAGS = -cru
CFLAGS = -g -Wall -Wextra -Wundef -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wstrict-prototypes \
	-Wnested-externs -Werror -pedantic -std=c99 \
	-Wno-format-zero-length -Wformat-security -Wstrict-aliasing=2 \
	-Wmissing-format-attribute -Wunused -Wunreachable-code \
	-Wformat=2 -Werror-implicit-function-declaration \
	-Wmissing-declarations -Wmissing-prototypes
LDFLAGS = -g -L./

# Installation prefix, if not already defined (e.g. on command line)
PREFIX ?= /usr/local
DESTDIR ?=

.PHONY: all clean docs install uninstall

all: librosprite.a

example: librosprite.a example.o
	${LD} -o $@ example.o ${LDFLAGS} \
		$(shell PKG_CONFIG_PATH=.:$(PKG_CONFIG_PATH) $(PKG_CONFIG) --cflags --libs sdl librosprite)

palette2c: librosprite.a palette2c.o
	${LD} -o $@ palette2c.o ${LDFLAGS} \
		$(shell PKG_CONFIG_PATH=.:$(PKG_CONFIG_PATH) $(PKG_CONFIG) --cflags --libs librosprite)

librosprite.a: librosprite.o librosprite.pc
	${AR} ${ARFLAGS} librosprite.a librosprite.o

librosprite.pc: librosprite.pc.in
	$(SED) -e 's#PREFIX#$(PREFIX)#' librosprite.pc.in > librosprite.pc

%.o: %.c
	${CC} -c ${CFLAGS} -o $@ $<

docs:
	${DOXYGEN}

clean:
	rm -f $(wildcard *.o) $(wildcard *.a) example palette2c librosprite.pc
	rm -rf doc

install: librosprite.a librosprite.pc
	$(MKDIR) -p $(DESTDIR)$(PREFIX)/lib/pkgconfig
	$(MKDIR) -p $(DESTDIR)$(PREFIX)/lib
	$(MKDIR) -p $(DESTDIR)$(PREFIX)/include
	$(INSTALL) --mode=644 -t $(DESTDIR)$(PREFIX)/lib librosprite.a
	$(INSTALL) --mode=644 -t $(DESTDIR)$(PREFIX)/include librosprite.h
	$(INSTALL) --mode=644 -t $(DESTDIR)$(PREFIX)/lib/pkgconfig librosprite.pc

uninstall:
	rm $(DESTDIR)$(PREFIX)/lib/librosprite.a
	rm $(DESTDIR)$(PREFIX)/include/librosprite.h
	rm $(DESTDIR)$(PREFIX)/lib/pkgconfig/librosprite.pc
