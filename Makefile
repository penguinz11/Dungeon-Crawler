CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lncurses

# Targets and Sources
TARGET = dungeon
SRCS = main.c player.c map.c enemy.c cam.c combat.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

# This rule compiles .c files into .o (object) files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

# To run the game quickly: make run
run: all
	./$(TARGET)