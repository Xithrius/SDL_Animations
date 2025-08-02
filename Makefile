# Build configuration
CXX = g++
CXXFLAGS = -std=c++20 -Wall
SDL3_FLAGS := $(shell pkg-config --cflags --libs sdl3)

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files (auto-discover all .cpp files)
SOURCES := $(shell find $(SRC_DIR) -name "*.cpp" -type f)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = animations

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(SDL3_FLAGS) -lm

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Release build (optimized)
release: CXXFLAGS += -O3
release: $(TARGET)

# Debug build
debug: CXXFLAGS += -g
debug: $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  release  - Build with optimization"
	@echo "  debug    - Build with debug symbols"
	@echo "  run      - Build and run the program"
	@echo "  clean    - Remove build artifacts"
	@echo "  install-deps - Install SDL3 dependencies"
	@echo "  help     - Show this help message"

.PHONY: all release debug run clean install-deps help
