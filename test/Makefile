CC = g++

DEFAULT_FLAGS  = -std=c++14 -MMD -MP -coverage -ftest-coverage

LINKER_OPTIONS = -lgcov --coverage

INCLUDE        = -I../include

WARNINGS       = -Wall -Wextra -pedantic -Wcast-align -Wconversion -Wformat=2 -Wformat-nonliteral \
		         -Wmissing-format-attribute -Wnon-virtual-dtor -Wsign-conversion
DEBUG          = -g -fno-optimize-sibling-calls -fno-omit-frame-pointer \
		         -fno-strict-aliasing -fstack-protector-all -fwrapv -fPIC
ASAN           = -fsanitize=address -fsanitize=null -fsanitize=undefined

FLAGS          = $(DEFAULT_FLAGS) $(INCLUDE) $(WARNINGS) $(DEBUG) $(ASAN)


BUILD_DIR = .build
SOURCES   = $(wildcard *.cpp)
OBJECTS   = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
DEPS      = $(OBJECTS:.o=.d)
EXE	      = test

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) $(ASAN) $(LINKER_OPTIONS) -o $(EXE)

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CC) -c $(FLAGS) $< -o $@

$(BUILD_DIR):
	@mkdir $@

-include $(DEPS)

clean:
	rm -rf $(EXE) $(BUILD_DIR)
