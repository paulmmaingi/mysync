CC=gcc
CFLAGS=-g3 -Wall -Werror -Wextra -O2 -fsanitize=address -fno-omit-frame-pointer

SRC_DIR = src
OBJ_DIR = obj

# c src files
SRCS = $(wildcard $(SRC_DIR)/*.c)
# c obj files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

HEADERS = $(wildcard $(SRC_DIR)/headers/*.h)

# executable
EXEC = mysync

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJS)
	rmdir $(OBJ_DIR)

fullclean: clean
	rm -f $(EXEC)

lines:
	@echo '   total:' $$(cat $(SRC_DIR)/*.c | wc -l)
	@echo stripped: $$(cat $(SRC_DIR)/*.c | sed 's,//.*,,' | grep -v '^$$' | \
		grep -v '^\s*[{}]\s*' | wc -l)

functions:
	@cat $(SRC_DIR)/*.c | grep '^[a-z].*(.*)' | sed 's/;//' | sort -u

.PHONY: check-leaks

DIRS ?= test3 test2 test1 test4
# DIRS ?= test6 test5

OPTS ?= -vpar -i "yo"

check-leaks: $(EXEC)
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(EXEC) $(OPTS) $(DIRS)