.PHONY: clean

C_FILES = $(shell find . -name "*.c")
HEADER_FILES = $(shell find . -name "*.h")
CC = gcc

run: $(C_FILES)
	@$(CC) $(C_FILES) -O2 -o proj
	@echo "---BUILD COMPLETE---"
	@./proj

clean:
	@rm proj