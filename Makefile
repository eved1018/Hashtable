# Define the C compiler and flags
CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=c11 -Wfloat-equal -Wswitch-default -Wswitch-enum -Wunreachable-code -Wconversion -Wshadow 

# Define directories for source, object files, and binaries
SRCDIR = .
OBJDIR = build
BINDIR = bin
HDRDIR = include

CFLAGS += -I$(HDRDIR)

# Find all .c source files
SOURCES := $(wildcard $(SRCDIR)/*.c)

# Generate corresponding object file names in the OBJDIR
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

# Define the executable name (you can change this)
EXECUTABLE = $(BINDIR)/hashtable


# Default target: build the executable
all: bear

build: $(EXECUTABLE)

# Rule to create the binary directory if it doesn't exist
$(BINDIR):
	mkdir -p $(BINDIR)

# Rule to create the object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Rule to link object files into the executable
$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

# Rule to compile .c files into .o files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files and directories
clean:
	rm -rf $(OBJDIR) $(BINDIR)

run:
	./$(EXECUTABLE) $(ARGS)

# Target to run bear for generating compile_commands.json
bear:
	bear -- make build
	@mv compile_commands.json $(OBJDIR)
