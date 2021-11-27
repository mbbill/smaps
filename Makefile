CFLAGS=-Wall -Werror -g -O0

smaps: smaps.o main.c
	$(CC) $(CFLAGS) $^ -o $@

smaps.o: smaps.c
	$(CC) $(CFLAGS) -O -c $^

clean:
	rm -v smaps smaps.o
