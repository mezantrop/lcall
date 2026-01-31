PREFIX	?= /usr/local
CC 		= cc
CFLAGS	+= -O3 -Wall
LDFLAGS	+=
LDLIBS	+=
OBJS 	= lcall.o funcs.o

.PHONY:	all clean install uninstall

all: lcall

lcall: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f lcall *.o

install:
	install -d $(PREFIX)/bin/
	install -m 755 lcall $(PREFIX)/bin/
	install -d $(PREFIX)/share/man/man1
	install -m 644 lcall.1 $(PREFIX)/share/man/man1/

deinstall: uninstall
	rm -f $(PREFIX)/bin/lcall

lcall.o: lcall.h
funcs.o: funcs.h
