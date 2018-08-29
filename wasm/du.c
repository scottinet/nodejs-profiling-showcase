#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
  #define OFFTPRINTF "%d"
#else
  #define OFFTPRINTF "%lld"
#endif


off_t getstats(const char *path, long long * scanned);

off_t walk(const char *path, long long * scanned) {
  struct dirent **list;
  int n = scandir(path, &list, NULL, alphasort);

  if (n == -1) {
    char msg[255];
    snprintf(msg, 255, "%s:%d '%s'", __FILE__, __LINE__, path);
    perror(msg);
    return 0;
  }

  off_t size = 0;

  while (n--) {
    if (strcmp(list[n]->d_name, ".") && strcmp(list[n]->d_name, "..")) {
      const size_t len = strlen(path) + strlen(list[n]->d_name) + 3;
      char *target = malloc(len);

      snprintf(target, len, "%s/%s", path, list[n]->d_name);
      size += getstats(target, scanned);

      free(target);
    }

    free(list[n]);
  }

  free(list);

  return size;
}

off_t getstats(const char *path, long long * scanned) {
  struct stat result;

  (*scanned)++;

  if (lstat(path, &result) != 0) {
    char msg[255];
    snprintf(msg, 255, "%s:%d '%s'", __FILE__, __LINE__, path);
    perror(msg);
    abort();
  }

  if ((result.st_mode & S_IFMT) == S_IFDIR) {
    return walk(path, scanned);
  }

  return result.st_size;
}

char * sizeformat(off_t size) {
  const char * units = "BKMGT";
  int i;

  for (i = 0; i < strlen(units) && size >= 1024; i++) {
    size /= 1024;
  }

  char formatted[16];
  snprintf(formatted, 16, OFFTPRINTF"%c", size, units[i]);
  return strdup(formatted);
}

long long *scan(const char *path) {
  long long *result = calloc(2, sizeof(long long));

#ifdef __EMSCRIPTEN__
  EM_ASM_({
    const path = Pointer_stringify($0);
    console.log('PATH = ', path);
    FS.mkdir(path);
    FS.mount(NODEFS, { root: path }, path);
    console.log('Mounted.');
  }, path);
#endif

  result[0] = getstats(path, &result[1]);

  return result;
}

int main(int argc, const char **argv) {
  struct timeval start, end;

  gettimeofday(&start, NULL);

  // for(int i = 0; i < argc - 1; i++) {
    long long *result = scan("/home/scottinet");
    char * formatted = sizeformat(result[0]);
    printf("%s %s\n", formatted, "/home/scottinet");
    free(formatted);
  // }

  gettimeofday(&end, NULL);
  double elapsed = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);
  long rate = elapsed > 0 ? (long)(result[1] / elapsed) : 0;
  printf("Scanned %lld files in %.3f seconds (%ld files/s)\n", result[1], elapsed, rate);
  free(result);
  exit(0);
}
