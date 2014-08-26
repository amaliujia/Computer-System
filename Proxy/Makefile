CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: proxy

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

proxy.o: proxy.c csapp.h
	$(CC) $(CFLAGS) -c proxy.c

sd.o: sd.c sd.h
	$(CC) $(CFLAGS) -c sd.c

cache.o: cache.c cache.h
	$(CC) $(CFLAGS) -c cache.c

sem.o: sem.h sem.c
	$(CC) $(CFLAGS) -c sem.c

wrapper.o: wrapper.h wrapper.c
	$(CC) $(CFLAGS) -c wrapper.c

proxy: proxy.o csapp.o sd.o cache.o sem.o wrapper.o

clean:
	rm -f *~ *.o proxy core *.tar *.zip *.gzip *.bzip *.gz

