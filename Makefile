CC = gcc
CFLAGS = -Iinclude -DNCURSES_STATIC
LIBS = -lmenuw -lncursesw
SRC_DIR = src
BUILD_DIR = build
TARGET = birthday_simulation.exe
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)