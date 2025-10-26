CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = build/main
TEST_TARGET = build/test
TEST_FILE = tests/test.c

all: build

build: main.c
	@echo "--- Compiling main.c ---"
	$(CC) $(CFLAGS) main.c -o $(TARGET)

run: build
	@echo "--- Running $(TARGET) ---"
	./$(TARGET)

clean:
	@echo "--- Cleaning up executable ---"
	rm -f $(TARGET)

test: $(TEST_FILE)
	@echo "--- Compiling tests ---"
	$(CC) $(CFLAGS) $(TEST_FILE) -o $(TEST_TARGET)
	@echo "--- Running $(TEST_TARGET) ---"
	./$(TEST_TARGET)

.PHONY: all build run clean
