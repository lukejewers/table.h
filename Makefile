CC     := gcc
SRC    := example.c
TARGET := example
CFLAGS := -Wall -Wextra -g

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f example

.PHONY: clean
