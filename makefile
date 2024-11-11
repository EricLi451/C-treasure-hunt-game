CC = gcc
CFLAGS = -ansi -pedantic -Wall -Wextra

TARGET = Shijun_Li_Assign01

SRCS = treasure_hunt.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all
