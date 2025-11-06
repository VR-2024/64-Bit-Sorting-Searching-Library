# Compiler and assembler
NASM     = nasm
CC       = gcc
LD       = ld
AR       = ar

# Flags
NASM_FLAGS = -f elf64 -g -F dwarf
# -O2 for optimization, -Wall for warnings, -std=c11 for C standard
# -no-pie flag added to fix linker error (PIE vs .bss relocation)
# -mavx512f flag has been removed for compatibility
CC_FLAGS   = -O2 -Wall -std=c11 -g -I$(INC_DIR) -no-pie
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

TEST_RADIX_SRC = tests/test_radix_sort.c
TEST_RADIX_BIN = $(BIN_DIR)/test_radix_sort

TEST_MAIN_SRC = tests/test_main_sort.c
TEST_MAIN_BIN = $(BIN_DIR)/test_main_sort

# NEW: Benchmark files
BENCHMARK_SRC = benchmarks/benchmark_main.c
BENCHMARK_BIN = $(BIN_DIR)/benchmark_main

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

# Test target for insertion sort
insert_test: $(LIB_TARGET) $(TEST_INSERT_SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CC_FLAGS) -o $(TEST_INSERT_BIN) $(TEST_INSERT_SRC) $(LIB_TARGET)

# Test target for radix sort
radix_test: $(LIB_TARGET) $(TEST_RADIX_SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CC_FLAGS) -o $(TEST_RADIX_BIN) $(TEST_RADIX_SRC) $(LIB_TARGET)

# Test target for the main adaptive sorter
main_test: $(LIB_TARGET) $(TEST_MAIN_SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CC_FLAGS) -o $(TEST_MAIN_BIN) $(TEST_MAIN_SRC) $(LIB_TARGET)

# --- NEW: Benchmark Target ---
benchmark: $(LIB_TARGET) $(BENCHMARK_SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CC_FLAGS) -o $(BENCHMARK_BIN) $(BENCHMARK_SRC) $(LIB_TARGET)

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

.PHONY: all clean test insert_test radix_test main_test benchmark
