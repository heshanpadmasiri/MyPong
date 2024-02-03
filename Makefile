CC := clang++
CFLAGS := -Wall -Wextra -Wpedantic -std=c++11 $(shell pkg-config --cflags raylib) -fPIC -g
LDFLAGS := $(shell pkg-config --libs raylib)
LIBFLAGS := -shared

# Directories
SRC_DIR := src
LIB_DIR := lib
BUILD_DIR := build
BIN_DIR := bin

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
LIBS := $(wildcard $(LIB_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
LIB_OBJS := $(patsubst $(LIB_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(LIBS))
LIB := $(BUILD_DIR)/lib.so
TARGET := $(BIN_DIR)/my_pong

# Compile and link

all: $(TARGET)
lib: $(LIB)

$(TARGET): $(OBJS) $(LIB)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(LIB): $(LIB_OBJS)
	$(CC) $(LIBFLAGS) -o $@ $^ $(LDFLAGS)

# Compile lib files
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run the application
run: $(TARGET)
	$(TARGET)

# Phony targets
.PHONY: clean run all lib

.PRECIOUS: $(LIB) lib
