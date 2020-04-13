CFLAGS=-Wall -I/usr/local/include
LDFLAGS=-L/usr/local/lib -lSDL2

life: life.o
	$(CC) $(CFLAGS) -o $@ life.o $(LDFLAGS)

