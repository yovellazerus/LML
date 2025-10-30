# ==========================================
# Makefile for forth.c project
# ==========================================

# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 -O2
LDFLAGS := 

# Source and output
SRC     := main.c
OBJ     := $(SRC:.c=.o)
OUT     := editor.exe

# Default target
all: $(OUT)

# Link step
$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)
	del $(OBJ)

# Compile step
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Debug build
debug: CFLAGS += -g -O0
debug: clean all

# Run
run: $(OUT)
	.\$(OUT)

# Clean
clean:
	del $(OUT)

# Phony targets
.PHONY: all clean debug run
