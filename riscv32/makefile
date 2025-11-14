CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC = main.c
TARGET = riscv_vm.exe

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	del $(TARGET)
