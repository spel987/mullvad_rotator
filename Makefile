CC=gcc
SRC=main.c cli/cli.c commands/commands.c relay_data/relay_data.c utils/utils.c
TARGET=mullvad_rotator

CFLAGS=-Wall -Wextra -g
DEBUG_FLAGS=-fsanitize=address

ifeq ($(OS),Windows_NT)
    LDFLAGS=-lpcre2-posix
	EXTENSION=.exe
	RM=del /Q /F
else
    LDFLAGS=
	EXTENSION=
endif

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

debug:
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(SRC) -o $(TARGET)_debug $(LDFLAGS)

clean:
	$(RM) $(TARGET)$(EXTENSION) $(TARGET)_debug$(EXTENSION)