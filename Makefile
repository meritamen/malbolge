CC = clang
CFLAGS = -Wall -O3
TARGET = malbolge
SRC_DIR = src
SRC = malbolge.c
TEST_DIR = test

all: $(TARGET)

$(TARGET):
		$(CC) -o $(TARGET) $(CFLAGS) $(SRC_DIR)/$(SRC)

clean:
		rm -f $(TARGET)
