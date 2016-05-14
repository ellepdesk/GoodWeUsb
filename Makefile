# Declaration of variables
C++ := clang++
C++_FLAGS := -Wall
SEARCH_PATH = "/usr/lib/x86_64-linux-gnu/"
LIBRARIES = -lhidapi-libusb

# File names
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

EXECUTABLE := $(current_dir)
SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all
all: $(EXECUTABLE)

# Main target
$(EXECUTABLE): $(OBJECTS) Makefile
	echo $(SOURCES)
	$(C++) $(C++_FLAGS) $(OBJECTS) $(LIBRARIES) -o $(EXECUTABLE)

# To obtain object files
%.o: %.cxx Makefile
	$(C++) -c $(C++_FLAGS) -I $(SEARCH_PATH) $< -o $@

# To remove generated files
.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

.PHONY: run
run: $(EXECUTABLE)
	./$(EXECUTABLE)
