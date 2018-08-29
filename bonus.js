const
  du = require('bindings')('du'),
  bignum = require('bignum');

let scannedFiles = 0;
const startTime = Date.now();

function sizeFormat(size) {
  const units = ['B', 'K', 'M', 'G', 'T'];
  let
    i,
    formatted = bignum(size);

  for (i = 0; i < units.length && formatted >= 1024; i++) {
    formatted = formatted.div(1024);
  }

  return formatted + units[i];
}

for(let i = 2; i < process.argv.length; i++) {
  const res = du.scan(process.argv[i]);
  scannedFiles += res[1];
  console.log(`${sizeFormat(bignum(res[0])).padStart(10)} ${process.argv[i]}`);
}

const duration = Date.now() - startTime;
console.log(`Scanned ${scannedFiles} files in ${duration/1000} seconds (${Number.parseInt(scannedFiles / duration * 1000)} files/s)`);
