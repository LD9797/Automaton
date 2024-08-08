#include "../Headers/util.h"

char* get_executable_path() {
  static char path[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
  if (count != -1) {
    path[count] = '\0';
    return dirname(path);
  }
  return NULL;
}