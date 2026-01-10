CC     := gcc
SRC    := main.c
TARGET := main
CFLAGS := -Wall -Wextra -g

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f main

.PHONY: clean
