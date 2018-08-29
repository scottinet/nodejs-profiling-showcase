#!/bin/bash

emcc -s WASM=1 du.c -o du.js -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "getValue"]' -s EXPORTED_FUNCTIONS='["_scan"]' -s FORCE_FILESYSTEM=1
