# CS4DTRP — Cubic Simultaneous 4-Day Time Rotation Protocol

CC       ?= gcc
CFLAGS   ?= -std=c11 -Wall -Wextra -Wpedantic
CPPFLAGS += -Iinclude

LIB_SRC  = src/cs4dtrp.c
LIB_OBJ  = $(LIB_SRC:.c=.o)
LIB      = libcs4dtrp.a

TEST_SRC = tests/test_cs4dtrp.c
TEST_BIN = tests/test_cs4dtrp

.PHONY: all lib test clean

all: lib test

lib: $(LIB)

$(LIB): $(LIB_OBJ)
	$(AR) rcs $@ $^

src/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -L. -lcs4dtrp -o $@

clean:
	rm -f $(LIB_OBJ) $(LIB) $(TEST_BIN)
