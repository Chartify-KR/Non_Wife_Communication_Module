# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

# Directories
OBJS_DIR := objs/
DEPS_DIR := deps/

# Build targets
TARGET = NodeApp
SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp)
OBJS = $(SOURCES:%.cpp=$(OBJS_DIR)%.o)
DEPS = $(SOURCES:%.cpp=$(DEPS_DIR)%.d)

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# To obtain object files
$(OBJS_DIR)%.o: %.cpp
	@mkdir -p $(OBJS_DIR)
	@mkdir -p $(DEPS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -MMD -MP
	mv $(OBJS_DIR)$*.d $(DEPS_DIR)

# Include dependencies
-include $(DEPS)

# Clean
clean:
	rm -rf $(OBJS_DIR) $(DEPS_DIR) $(TARGET)

# Phony targets
.PHONY: all clean
