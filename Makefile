# Compiler selection.
CC = clang++

# Compiler flags.
CFLAGS = -w -std=c++14

# LLVM linkage flags.
LLVM_FLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs core`

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

	@$(CC) $(CFLAGS) $(LLVM_FLAGS) -o $(OBJS)/ast.o -c $(SRCS)/ast.cpp
	@echo "  [+] Compiled $(OBJS)/ast.o"

	@$(CC) $(CFLAGS) $(LLVM_FLAGS) -o $(OBJS)/lexer.o -c $(SRCS)/lexer.cpp
	@echo "  [+] Compiled $(OBJS)/lexer.o"

	@$(CC) $(CFLAGS) $(LLVM_FLAGS) -o $(OBJS)/parser.o -c $(SRCS)/parser.cpp
	@echo "  [+] Compiled $(OBJS)/parser.o"

	@echo "done"

link: setup compile
	@echo "Linking binaries..."

	@$(CC) $(CFLAGS) $(LLVM_FLAGS) -o $(BINS)/kaleidoscope $(SRCS)/main.cpp $(OBJS)/*.o
	@echo "  [+] Linked $(BINS)/kaleidoscope"

	@echo "done"

clean:
	@echo "Cleaning..."

	@$(RM) -rf $(BINS)
	@echo "  [-] Cleaned ./$(BINS)"

	@$(RM) -rf $(OBJS)
	@echo "  [-] Cleaned ./$(OBJS)"

	@echo "done"
