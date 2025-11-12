#!/bin/bash

CC=gcc
CFLAGS=""
SRC=main.c
TARGET=shell

# Compile
$CC $SRC -o $TARGET $CFLAGS

# Check if compilation succeeded
if [ $? -eq 0 ]; then
    echo "Build succeeded. Running $TARGET..."
    ./$TARGET
else
    echo "Build failed."
fi
