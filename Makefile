CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lncurses

#define target and its dependencies
TARGET = dungeon
SRCS = main.c player.c map.c enemy.c cam.c combat.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

#compiles .c files into .o
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

#make run to quickly run it 
run: all
	./$(TARGET)