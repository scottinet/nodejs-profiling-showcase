#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <linux/limits.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

off_t getstats(const char *path, unsigned long long * scanned);

off_t walk(const char *path, unsigned long long * scanned) {
  DIR *dir = opendir(path);
  off_t size = 0;

  if (dir == NULL) {
    perror(path);
    return 0;
  }

  char buffer[PATH_MAX];
  strcpy(buffer, path);
  buffer[strlen(path)] = '/';
  char * ptr = buffer + strlen(path) + 1;

  struct dirent *entry;
  errno = 0;

  while ((entry = readdir(dir)) != NULL) {
    const size_t len = strlen(entry->d_name);

    if (len > 2 || (entry->d_name[0] != '.' && entry->d_name[1] != '.')) {
      memcpy(ptr, entry->d_name, len + 1);
      size += getstats(buffer, scanned);
    }

    errno = 0;
  }

  if (errno) {
    perror(path);
  }

  closedir(dir);

  return size;
}

off_t getstats(const char *path, unsigned long long * scanned) {
  struct stat result;

  (*scanned)++;
  if (lstat(path, &result) != 0) {
    perror(path);
    return 0;
  }

  if ((result.st_mode & S_IFMT) == S_IFDIR) {
    return walk(path, scanned);
  }

  return result.st_size;
}

char * sizeformat(off_t size) {
  const char * units = "BKMGT";
  unsigned i;

  for (i = 0; i < strlen(units) && size >= 1024; i++) {
    size /= 1024;
  }

  char formatted[256];
  snprintf(formatted, 256, "%ld%c", size, units[i]);
  return strdup(formatted);
}

unsigned long long *scan(const char *path) {
  unsigned long long *result = calloc(2, sizeof(long long));

  result[0] = getstats(path, &result[1]);

  return result;
}

int main(int argc, const char **argv) {
  struct timeval start, end;
  unsigned long long scanned = 0;

  gettimeofday(&start, NULL);

  for(int i = 1; i < argc; i++) {
    unsigned long long *result = scan(argv[i]);
    char * formatted = sizeformat(result[0]);
    printf("%s %s\n", formatted, argv[i]);
    free(formatted);
    scanned += result[1];
    free(result);
  }

  gettimeofday(&end, NULL);
  double elapsed = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);
  unsigned long long rate = elapsed > 0 ? (unsigned long long)(scanned / elapsed) : 0;
  printf("Scanned %llu files in %.3f seconds (%llu files/s)\n", scanned, elapsed, rate);
  return 0;
}

#ifdef __cplusplus
}
#endif
