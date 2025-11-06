# Compiler and assembler
NASM     = nasm
CC       = gcc
LD       = ld
AR       = ar

# Flags
NASM_FLAGS = -f elf64 -g -F dwarf
# Enable AVX-512 for any C files that might need it (e.g., interface)
# -O2 for optimization, -Wall for warnings, -std=c11 for C standard
CC_FLAGS   = -O2 -Wall -std=c11 -g -I$(INC_DIR)
LD_FLAGS   = -m elf_x86_64

# Directories
SRC_DIR    = src
OBJ_DIR    = obj
BIN_DIR    = bin
INC_DIR    = include

# Source files
# Find all .asm and .c files in the src directory and its subdirectories
ASM_SOURCES := $(wildcard $(SRC_DIR)/**/*.asm) $(wildcard $(SRC_DIR)/*.asm)
C_SOURCES   := $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c)

# Object files
# Convert source file paths to object file paths
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.o, $(ASM_SOURCES))
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES))

# Target library
LIB_TARGET = $(BIN_DIR)/libultrasort.a

# Test files
TEST_UTILITY_SRC = tests/test_utilities.c
TEST_UTILITY_BIN = $(BIN_DIR)/test_utilities

TEST_INSERT_SRC = tests/test_insertion_sort.c
TEST_INSERT_BIN = $(BIN_DIR)/test_insertion_sort

# Default target
all: $(LIB_TARGET)

# Rule to build the static library
$(LIB_TARGET): $(ASM_OBJECTS) $(C_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(AR) rcs $@ $^

# --- Test Targets ---

# Test target for utility functions
test: $(LIB_TARGET) $(TEST_UTILITY_SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CC_FLAGS) -o $(TEST_UTILITY_BIN) $(TEST_UTILITY_SRC) $(LIB_TARGET)

# NEW: Test target for insertion sort
insert_test: $(LIB_TARGET) $(TEST_INSERT_SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CC_FLAGS) -o $(TEST_INSERT_BIN) $(TEST_INSERT_SRC) $(LIB_TARGET)

# --- Generic Build Rules ---

# Rule to compile .asm files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(NASM) $(NASM_FLAGS) $< -o $@

# Rule to compile .c files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean test insert_test
