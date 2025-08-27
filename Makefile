CC = gcc
CFLAGS = -g -Wall -Wextra -O3 -Iinclude
DFLAGS = -g -O0 -Iinclude

TARGET = lpf
DTARGET = lpf_debug

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
DOBJ = $(SRC:.c=.d.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

debug: $(DTARGET)

$(DTARGET): $(DOBJ)
	$(CC) $(DFLAGS) -o $@ $(DOBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

src/%.d.o: src/%.c
	$(CC) $(DFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(DTARGET) $(OBJ) $(DOBJ)
