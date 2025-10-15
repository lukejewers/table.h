.PHONY: run

run: table
	./table

table: table.c
	gcc -g -Wall -Wextra -o table table.c

clean:
	rm -f table
