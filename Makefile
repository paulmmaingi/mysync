# Compiler options
CC = cc
CFLAGS = -std=c11 -Wall -Werror

# Source files
SRCS = $(wildcard *.c)

# Object files
OBJS = $(SRCS:.c=.o)

# Executable
EXEC = mysync

# Targets
all: $(EXEC)
	rm -f $(OBJS)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

full_clean: clean
	rm -f $(EXEC)