CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c99

INCLUDES = -I..

LIB_SRC = ../chacha20.c

TARGETS = test_block_function test_chacha20_cipher

.PHONY: all clean run

all: $(TARGETS)

test_block_function: test_block_function.c $(LIB_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

test_chacha20_cipher: test_chacha20_cipher.c $(LIB_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

run: all
	@echo "=========================================="
	@./test_block_function
	@echo "=========================================="
	@./test_chacha20_cipher
	@echo "=========================================="

clean:
	rm -f $(TARGETS)
