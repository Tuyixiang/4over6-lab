.PHONY: clean

C_FILES = $(shell find . -name "*.c")
HEADER_FILES = $(shell find . -name "*.h")
CC = gcc
CC_FLAGS = -std=c99 -lrt -lpthread

run: $(C_FILES) $(HEADER_FILES)
	@$(CC) $(CC_FLAGS) $(C_FILES) -O2 -o proj
	@echo "---BUILD COMPLETE---"
	@./proj

test: $(C_FILES) $(HEADER_FILES)
	@$(CC) $(CC_FLAGS) $(C_FILES) -O2 -o test
	@echo "---BUILD COMPLETE---"

clean:
	@rm proj