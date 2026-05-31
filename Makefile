# --- Makefile ---
CC = gcc
TARGET = galaga_oyun
SRCS = klon2.c
OBJS = $(SRCS:.c=.o)

CFLAGS = -Wall -O2 `pkg-config --cflags sdl3 sdl3-ttf`
LDFLAGS = `pkg-config --libs sdl3 sdl3-ttf`

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
