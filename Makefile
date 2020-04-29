.PHONY: clean

C_FILES = $(shell find . -name "*.c")
HEADER_FILES = $(shell find . -name "*.h")
CC = gcc
CC_FLAGS = -std=c99

run: $(C_FILES)
	@$(CC) $(CC_FLAGS) $(C_FILES) -O2 -o proj
	@echo "---BUILD COMPLETE---"
	@./proj

clean:
	@rm proj