# Variable for the compiler
CC = gcc

# Compilation flags (-Wall shows warnings, -lncurses links the library)
CFLAGS = -Wall
LIBS = -lncurses

# The name of your final executable
TARGET = dungeon_crawler

# All source files
SRCS = main.c player.c map.c enemy.c

# The default rule
all:
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LIBS)

# Rule to clean up the folder
clean:
	rm -f $(TARGET)