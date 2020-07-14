SOURCE_DIR := src
SOURCE := $(shell find $(SOURCE_DIR) -name "*.c")
HEADER := $(shell find $(SOURCE_DIR) -name "*.h")

FLAGS := -std=c18 -Wall -Werror -pedantic -fmax-errors=5

.PHONY: build
build: $(SOURCE) $(HEADER)
	@gcc $(FLAGS) $(SOURCE) -o .bin/main

.PHONY: debug
debug: $(SOURCE) $(HEADER)
	@gcc -g $(FLAGS) $(SOURCE) -o .bin/debug