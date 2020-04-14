CFLAGS=-fsanitize=address -Wall -I/usr/local/include
LDFLAGS=-L/usr/local/lib -lSDL2

.PHONY: clean

life: life.o
	$(CC) $(CFLAGS) -o $@ life.o $(LDFLAGS)

clean:
	-rm life life.o
