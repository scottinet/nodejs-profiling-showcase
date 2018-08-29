const
  path = require('path'),
  fs = require('fs');

let scannedFiles = 0;
const startTime = Date.now();

function getstats(file, callback) {
  fs.lstat(file, (err, stats) => {
    scannedFiles++;

    if (err) {
      console.error(err.message);
      return setImmediate(callback, 0);
    }

    if (stats.isDirectory()) {
      return setImmediate(scandir, file, callback);
    }

    setImmediate(callback, stats.size);
  });
}

function scandir(dir, callback) {
  fs.readdir(dir, (err, files) => {
    if (err) {
      console.error(err.message);
      return setImmediate(callback, 0);
    }

    if (!files.length) {
      return setImmediate(callback, 0);
    }

    let
      total = 0,
      done = 0;
    const getstatscb = size => {
      total += size;
      done++;
      if (done === files.length) {
        setImmediate(callback, total);
      }
    };

    for (let i = 0; i < files.length; i++) {
      getstats(dir + path.sep + files[i], getstatscb);
    }
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

let taskDone = 0;
const
  taskCount = process.argv.length - 2,
  taskcb = (entry, size) => {
    console.log(`${sizeFormat(size).padStart(10)} ${entry}`);

    taskDone++;
    if (taskDone === taskCount) {
      const duration = Date.now() - startTime;
      console.log(`Scanned ${scannedFiles} files in ${duration/1000} seconds (${Number.parseInt(scannedFiles / duration * 1000)} files/s)`);
    }
  };

for(let i = 2; i < process.argv.length; i++) {
  getstats(process.argv[i], size => taskcb(process.argv[i], size));
}
