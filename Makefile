SOURCE_DIR := src
SOURCE := $(shell find $(SOURCE_DIR) -name "*.c")
HEADER := $(shell find $(SOURCE_DIR) -name "*.h")

FLAGS := -std=c18 -Wall -Werror -pedantic -fmax-errors=5

.PHONY: build
build: $(SOURCE) $(HEADER)
	@gcc $(FLAGS) $(SOURCE) -o .bin/main

.PHONY: release
release: $(SOURCE) $(HEADER)
	@gcc -O3 $(FLAGS) $(SOURCE) -o .bin/release


.PHONY: debug
debug: $(SOURCE) $(HEADER)
	@gcc -g $(FLAGS) $(SOURCE) -o .bin/debug

EMCC_EXPORT="_M_Server_init", "_M_Server_quit", "_M_Server_parse", "_M_Server_color_HTML", "_M_Server_get_errors", "_M_Server_print_tree"
EMCC_FLAGS=-s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' -s ALLOW_MEMORY_GROWTH=1

.PHONY: web
web: $(SOURCE) $(HEADER)
	@emcc $(SOURCE) -o editor/compiler.js -s EXPORTED_FUNCTIONS='[$(EMCC_EXPORT)]' $(EMCC_FLAGS)