const
  path = require('path'),
  fs = require('fs');

let scannedFiles = 0;
const startTime = Date.now();

function getstats(file) {
  let stats;

  scannedFiles++;

  try {
    stats = fs.lstatSync(file);
  } catch (err) {
    console.error(err.message);
    return 0;
  }

  if (stats.isDirectory()) {
    return scandir(file);
  }

  return stats.size;
}

function scandir(dir) {
  let files;

  try {
    files = fs.readdirSync(dir);
  } catch (err) {
    console.error(err.message);
    return 0;
  }

  return files
    .map(f => getstats(dir + path.sep + f))
    .reduce((acc, size) => acc + size, 0);
}

function sizeFormat(size) {
  const units = ['B', 'K', 'M', 'G', 'T'];
  let
    i,
    formatted = size;

  for (i = 0; i < units.length && formatted >= 1024; i++) {
    formatted /= 1024;
  }

  return String(Math.round(formatted)) + units[i];
}

for(let i = 2; i < process.argv.length; i++) {
  const size = getstats(process.argv[i]);
  console.log(`${sizeFormat(size).padStart(10)} ${process.argv[i]}`);
}

const duration = Date.now() - startTime;
console.log(`Scanned ${scannedFiles} files in ${duration/1000} seconds (${Number.parseInt(scannedFiles / duration * 1000)} files/s)`);
