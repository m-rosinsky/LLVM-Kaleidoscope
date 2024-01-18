# Compiler selection.
CC = clang++

# Compiler flags.
CFLAGS = -Wall -Wextra -pedantic -std=c++14

# Object dir.
OBJS = objs

# Binary dir.
BINS = bins

# Source dir.
SRCS = src

# Rules.
all: setup compile link

setup:
	@echo "Creating subdirectories..."

	@mkdir -p $(OBJS)
	@echo "  [+] Created ./$(OBJS)"

	@mkdir -p $(BINS)
	@echo "  [+] Created ./$(BINS)"

	@mkdir -p $(SRCS)
	@echo "  [+] Created ./$(SRCS)"

	@echo "done"

compile: setup
	@echo "Compiling sources..."

	@$(CC) $(CFLAGS) -o $(OBJS)/lexer.o -c $(SRCS)/lexer.cpp
	@echo "  [+] Compiled $(OBJS)/lexer.o"

	@$(CC) $(CFLAGS) -o $(OBJS)/parser.o -c $(SRCS)/parser.cpp
	@echo "  [+] Compiled $(OBJS)/parser.o"

	@echo "done"

link: setup compile
	@echo "Linking binaries..."

	@$(CC) $(CFLAGS) -o $(BINS)/kaleidoscope $(SRCS)/main.cpp $(OBJS)/*.o
	@echo "  [+] Linked $(BINS)/kaleidoscope"

	@echo "done"

clean:
	@echo "Cleaning..."

	@$(RM) -rf $(BINS)
	@echo "  [-] Cleaned ./$(BINS)"

	@$(RM) -rf $(OBJS)
	@echo "  [-] Cleaned ./$(OBJS)"

	@echo "done"
