CC=gcc
CFLAGS=-g -Wall -Werror

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

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fullclean: clean
	rm -f $(EXEC)

lines:
	@echo '   total:' $$(cat $(SRC_DIR)/*.c | wc -l)
	@echo stripped: $$(cat $(SRC_DIR)/*.c | sed 's,//.*,,' | grep -v '^$$' | \
		grep -v '^\s*[{}]\s*' | wc -l)

functions:
	@cat $(SRC_DIR)/*.c | grep '^[a-z].*(.*)' | sed 's/;//' | sort -u

.PHONY: check-leaks

DIRS ?= test1 test2 test2/empty	test2/empty

OPTS ?= -a -r

check-leaks: $(EXEC)
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(EXEC) $(DIRS) $(OPTS)