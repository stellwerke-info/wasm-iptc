DEPS = $(wildcard src/*.h)
OBJ = $(patsubst %.c,%.o,$(wildcard src/*.c))
OUTPUT = dist/iptc.wasm

$(OUTPUT): $(OBJ) Makefile external_symbols
	wasm-ld \
		-o $(OUTPUT) \
		--strip-all \
		--allow-undefined-file=external_symbols \
		--initial-memory=131072 \
		--compress-relocations \
		--lto-O3 \
		-O3 \
		--gc-sections \
		$(OBJ) \
		--no-entry

%.o: %.c $(DEPS) Makefile
	clang \
		-c \
		-Wall \
		--target=wasm32 \
		-Os \
		-fvisibility=hidden \
		-std=c11 \
		-ffunction-sections \
		-fdata-sections \
		-DPRINTF_DISABLE_SUPPORT_FLOAT=1 \
		-DPRINTF_DISABLE_SUPPORT_LONG_LONG=1 \
		-DPRINTF_DISABLE_SUPPORT_PTRDIFF_T=1 \
		-mbulk-memory \
		-o $@ \
		$<

clean:
	rm -f $(OBJ)
