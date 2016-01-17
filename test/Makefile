CC = clang++

DEFAULT_FLAGS = -std=c++14

WARNINGS = -Wall -Wextra -pedantic -Wcast-align -Wconversion -Wformat=2 -Wformat-nonliteral \
		   -Wmissing-format-attribute -Wnon-virtual-dtor -Wsign-conversion

DEBUG = -g -fno-optimize-sibling-calls -fno-omit-frame-pointer \
		   -fno-strict-aliasing -fstack-protector-all -fwrapv -fPIC

FLAGS = $(DEFAULT_FLAGS) $(WARNINGS) $(DEBUG)

EXE = test
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LINKER_OPTIONS) -o $(EXE)

%.o: %.cpp
	$(CC) -c $(FLAGS) $< -o $@

clean:
	rm -f $(EXE) $(OBJECTS)
