CFLAGS= -O3 -flto -Wall -I/usr/local/include
LDFLAGS=-L/usr/local/lib -lSDL2

.PHONY: clean

life: life.o
	$(CC) $(CFLAGS) -o $@ life.o $(LDFLAGS)

.c.o:
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	-rm life life.o
