# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -O2

# Target executable
TARGET = shell.exe

# Source files
SRCS = main.c

# Default rule
all: $(TARGET)

# Rule to build the target
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Clean build files
clean:
	del $(TARGET) *.o

# Phony targets
.PHONY: all clean
