# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread -fPIC
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG
INCLUDES = -I./lib -I./tests
LDFLAGS = -ldl

# Make spdlog optional - use simple logging if not available
SPDLOG_EXISTS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --exists spdlog 2>/dev/null && echo yes || echo no)
ifeq ($(SPDLOG_EXISTS),yes)
    CXXFLAGS += -DUSE_SPDLOG $(shell pkg-config --cflags spdlog)
    LDFLAGS += $(shell pkg-config --libs spdlog)
else
    CXXFLAGS += -DUSE_SIMPLE_LOGGING
endif

# Targets
TARGET = oroto-kernel
MAIN_TARGET = main
TEST_TARGET = tests/test_runner

# Source files
MAIN_SOURCES = main.cpp cmd_parser.cpp system_calls.cpp device_interface.cpp
LIB_SOURCES = $(wildcard lib/*.cpp)
TOOL_SOURCES = $(wildcard tools/*.cpp)
DISPLAY_SOURCES = $(wildcard display/*.cpp)
TEST_SOURCES = $(wildcard tests/*.cpp)

SOURCES = $(MAIN_SOURCES) $(LIB_SOURCES) $(TOOL_SOURCES) $(DISPLAY_SOURCES)
OBJECTS = $(SOURCES:.cpp=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

MAIN_OBJECTS = $(MAIN_SOURCES:.cpp=.o) $(LIB_SOURCES:.cpp=.o) $(TOOL_SOURCES:.cpp=.o) $(DISPLAY_SOURCES:.cpp=.o)

# Default target
all: $(TARGET) $(MAIN_TARGET)

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)

# Release build
release: CXXFLAGS += $(RELEASE_FLAGS)
release: clean $(TARGET)

# Main executable (for basic testing)
main: $(MAIN_OBJECTS)
	@echo "Linking main..."
	$(CXX) $(CXXFLAGS) -o $@ $(MAIN_OBJECTS) $(LDFLAGS)

# Oroto kernel executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Test executable
$(TEST_TARGET): $(TEST_OBJECTS)
	@echo "Linking $(TEST_TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run tests
test: $(TEST_TARGET)
	@echo "Running tests..."
	./$(TEST_TARGET)

# Run with memory checking (if valgrind is available)
memcheck: $(TARGET)
	@echo "Running memory check..."
	@if command -v valgrind >/dev/null 2>&1; then \
		valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(TARGET); \
	else \
		echo "Valgrind not available, running normally..."; \
		./$(TARGET); \
	fi

# Code formatting (if clang-format is available)
format:
	@echo "Formatting code..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i; \
		echo "Code formatted successfully"; \
	else \
		echo "clang-format not available"; \
	fi

# Static analysis (if cppcheck is available)
analyze:
	@echo "Running static analysis..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c++17 --suppress=missingIncludeSystem $(SOURCES); \
	else \
		echo "cppcheck not available"; \
	fi

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(TARGET) $(MAIN_TARGET) $(TEST_TARGET)
	rm -f *.log tests/*.log
	find . -name "*.o" -delete

# Install (placeholder)
install: $(TARGET)
	@echo "Installing $(TARGET)..."
	mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin/
	mkdir -p /usr/local/lib/oroto/plugins

# Run the kernel
run: $(TARGET)
	./$(TARGET)

# Create plugin template
plugin-template:
	@echo "Creating plugin template..."
	@mkdir -p plugins/example
	@echo '#include "../../lib/plugin_manager.h"' > plugins/example/example_plugin.cpp
	@echo '#include <iostream>' >> plugins/example/example_plugin.cpp
	@echo '' >> plugins/example/example_plugin.cpp
	@echo 'class ExamplePlugin : public Oroto::IPlugin {' >> plugins/example/example_plugin.cpp
	@echo 'public:' >> plugins/example/example_plugin.cpp
	@echo '    bool initialize() override {' >> plugins/example/example_plugin.cpp
	@echo '        std::cout << "Example plugin initialized\\n";' >> plugins/example/example_plugin.cpp
	@echo '        return true;' >> plugins/example/example_plugin.cpp
	@echo '    }' >> plugins/example/example_plugin.cpp
	@echo '' >> plugins/example/example_plugin.cpp
	@echo '    void shutdown() override {' >> plugins/example/example_plugin.cpp
	@echo '        std::cout << "Example plugin shutdown\\n";' >> plugins/example/example_plugin.cpp
	@echo '    }' >> plugins/example/example_plugin.cpp
	@echo '' >> plugins/example/example_plugin.cpp
	@echo '    std::string getName() const override {' >> plugins/example/example_plugin.cpp
	@echo '        return "ExamplePlugin";' >> plugins/example/example_plugin.cpp
	@echo '    }' >> plugins/example/example_plugin.cpp
	@echo '' >> plugins/example/example_plugin.cpp
	@echo '    std::string getVersion() const override {' >> plugins/example/example_plugin.cpp
	@echo '        return "1.0.0";' >> plugins/example/example_plugin.cpp
	@echo '    }' >> plugins/example/example_plugin.cpp
	@echo '' >> plugins/example/example_plugin.cpp
	@echo '    std::string getDescription() const override {' >> plugins/example/example_plugin.cpp
	@echo '        return "An example plugin for Oroto";' >> plugins/example/example_plugin.cpp
	@echo '    }' >> plugins/example/example_plugin.cpp
	@echo '};' >> plugins/example/example_plugin.cpp
	@echo '' >> plugins/example/example_plugin.cpp
	@echo 'DECLARE_PLUGIN(ExamplePlugin)' >> plugins/example/example_plugin.cpp

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build main targets"
	@echo "  debug     - Build with debug flags"
	@echo "  release   - Build with release optimization"
	@echo "  test      - Build and run tests"
	@echo "  memcheck  - Run with memory checking"
	@echo "  format    - Format code with clang-format"
	@echo "  analyze   - Run static analysis with cppcheck"
	@echo "  clean     - Clean build files"
	@echo "  install   - Install the application"
	@echo "  run       - Run the kernel"
	@echo "  plugin-template - Create example plugin"

.PHONY: all debug release test memcheck format analyze clean install run plugin-template help