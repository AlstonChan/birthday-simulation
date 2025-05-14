CC = gcc
CFLAGS = -Iinclude -DNCURSES_STATIC
LIBS = -lmenuw -lncursesw
SRC_DIR = src
BUILD_DIR = build
TARGET = birthday_simulation.exe

# Find all source files recursively
SOURCES = $(shell find $(SRC_DIR) -name '*.c')

# Map each source file to a corresponding object file
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)