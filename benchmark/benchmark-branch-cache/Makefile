CC = clang++

DEFAULT_FLAGS  = -std=c++14 -O3 -MMD -MP

LINKER_OPTIONS = -flto -fuse-ld=gold -O3

INCLUDE        = -I$(LIBRARY)/include -I../ -I../../include

WARNINGS       = -Wall -Wextra -pedantic -Wcast-align -Wconversion -Wformat=2 -Wformat-nonliteral \
		        -Wmissing-format-attribute -Wnon-virtual-dtor -Wsign-conversion

FLAGS          = $(DEFAULT_FLAGS) $(INCLUDE) $(WARNINGS) $(DEBUG) $(ASAN)


BUILD_DIR = .build
SOURCES   = $(wildcard *.cpp)
OBJECTS   = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
DEPS      = $(OBJECTS:.o=.d)
EXE       = benchmark

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) $(ASAN) $(LINKER_OPTIONS) -o $(EXE)

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CC) -c $(FLAGS) $< -o $@

$(BUILD_DIR):
	@mkdir $@

-include $(DEPS)

clean:
	rm -rf $(EXE) $(BUILD_DIR)
