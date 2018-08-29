# Node.js profiling showcase

I use this repository for live demonstrations about how to profile Node.js code.

Don't mind the webassembly versions (wasm.js and the wasm directory): the example I chose is exactly what cannot be done efficiently with webassembly, since this example uses the filesystem heavily... and webassembly, for security reasons, virtualizes the FS with a very slow layer. Dang.


