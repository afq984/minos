#include <stddef.h>
#include <sys/stat.h>

void atomic_update_file(const char* path, const char* buf, size_t len, mode_t mode);
