# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -O2

# Target executable
TARGET = test.exe

# Source files
SRCS = main.c Darray.c

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
