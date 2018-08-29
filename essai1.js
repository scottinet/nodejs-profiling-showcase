const
  path = require('path'),
  fs = require('fs');

let scannedFiles = 0;
const startTime = Date.now();

function getstats(file) {
  return new Promise(resolve => {
    fs.lstat(file, (err, stats) => {
      scannedFiles++;

      if (err) {
        console.error(err.message);
        return resolve(0);
      }

      if (stats.isDirectory()) {
        return resolve(scandir(file));
      }

      resolve(stats.size);
    });
  });
}

function scandir(dir) {
  return new Promise(resolve => {
    fs.readdir(dir, (err, files) => {
      if (err) {
        console.error(err.message);
        return resolve(0);
      }

      const promises = files.map(f => getstats(dir + path.sep + f));

      Promise.all(promises)
        .then(sizes => resolve(sizes.reduce((acc, size) => acc + size, 0)));
    });
  });
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

const promises = [];

for(let i = 2; i < process.argv.length; i++) {
  const entry = process.argv[i];
  promises.push(getstats(entry)
    .then(total => console.log(`${sizeFormat(total).padStart(10)} ${entry}`)));
}

Promise.all(promises)
  .then(() => {
    const duration = Date.now() - startTime;
    console.log(`Scanned ${scannedFiles} files in ${duration/1000} seconds (${Number.parseInt(scannedFiles / duration * 1000)} files/s)`);
  });
