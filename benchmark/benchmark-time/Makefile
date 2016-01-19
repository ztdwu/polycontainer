CC = clang++

DEFAULT_FLAGS = -std=c++14 -O3

LIBRARY = dependencies/google-benchmark-framework/build

LINKER_OPTIONS = -lpthread $(LIBRARY)/lib/libbenchmark.a -flto -fuse-ld=gold -O3

INCLUDE = -I$(LIBRARY)/include -I../ -I../../

WARNINGS = -Wall -Wextra -pedantic -Wcast-align -Wconversion -Wformat=2 -Wformat-nonliteral \
		   -Wmissing-format-attribute -Wnon-virtual-dtor -Wsign-conversion

FLAGS = $(DEFAULT_FLAGS) $(WARNINGS) $(INCLUDE)

EXE = benchmark
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LINKER_OPTIONS) -o $(EXE)

%.o: %.cpp
	$(CC) -c $(FLAGS) $< -o $@

clean:
	rm -f $(EXE) $(OBJECTS)
