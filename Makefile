CFLAGS=-std=c99 -Wall -Werror -Wextra

all: libportnotify.so

export LIBRARY_PATH=.

example: LDFLAGS+=-lportnotify
example: example.o

libportnotify.so: CFLAGS+=-fPIC
libportnotify.so: portnotify.o
	$(CC) $(CFLAGS) -shared -o $@ $^

clean:
	-rm example.o
	-rm portnotify.o
	-rm libportnotify.so
	-rm example

