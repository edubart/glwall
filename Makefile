CC=gcc
CFLAGS=-Wall -O3 -fomit-frame-pointer
LDFLAGS=-s -lglfw -lX11 -lGL
prefix=/usr/local

.PHONY: all clean distclean install
all: glwall

glwall: glwall.o
	${CC} ${LDFLAGS} -o glwall $^

${OBJS}: %.o: %.c *.h
	${CC} ${CFLAGS} -o $@ -c $<

clean:
	rm -f *.o glwall

distclean: clean

install:
	mkdir -p $(prefix)/bin
	install -m 755 glwall $(prefix)/bin/
