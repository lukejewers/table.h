.PHONY: run

run: main
	./main

main: main.c
	gcc -g -Wall -Wextra -o main main.c

clean:
	rm -f main
