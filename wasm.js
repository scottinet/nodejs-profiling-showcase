console.log('Loading module...');
const WasmModule = require('./wasm/du');

console.log('Instantiating...');

WasmModule.onRuntimeInitialized = () => {
  console.log('Running...');
  const result = WasmModule.ccall('scan', 'number', ['string'], ['/home/scottinet']);
  // const result = WasmModule._scan('/home/scottinet');
console.log('====> result pointer: ', result);
  const
    size = WasmModule.getValue(result, 'i64'),
    count = WasmModule.getValue(result + 4, 'i64');

  console.log(`Size: ${size}, Files scanned: ${count}`);
};

// const
//   path = require('path'),
//   fs = require('fs');

// const wasmModule = path.join(__dirname, 'wasm/du.wasm');

// console.log('Loading WASM...');
// const wasmRaw = fs.readFileSync(wasmModule);

// WebAssembly.instantiate(wasmRaw)
//   .then(module => {
//     console.log('WASM Module loaded.');
//   })
//   .catch(err => console.error(err));
