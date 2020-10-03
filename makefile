CXX      := -c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror -std=c++20 -I.
LDFLAGS  := -L/usr/lib -lstdc++ -lm

BUILD    := ./build
OBJ_DIR  := $(BUILD)
BIN_DIR  := $(BUILD)/bin

SRC      := samples/lines.cpp # Change which sample you want to test here
HEADER   := pixel/pixel.hpp

TARGET   := sample

.PHONY: all build clean debug release

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(SRC) $(HEADER)
	$(CXX) $(CXXFLAGS) -o $(BUILD)/$(TARGET) $< $(LDFLAGS)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(BUILD)/*