#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

void atomic_update_file(const char* path, const char* buf, size_t len, mode_t mode) {
    size_t pathlen = strlen(path);
    char tname[pathlen + 7];
    memcpy(tname, path, pathlen);
    memset(tname + pathlen, 'X', 6);
    tname[pathlen + 6] = 0;
    int tfd = mkstemp(tname);
    if (-1 == tfd) {
        perror("mkstemp");
        return;
    }
    int do_rename = 1;
    if (0 != fchmod(tfd, mode)) {
        perror("fchmod");
        do_rename = 0;
    }
    if (len != write(tfd, buf, len)) {
        perror("write");
        do_rename = 0;
    }
    // Not using fsync because in the case of a crash, the file will be
    // recovered once minos-client is up anyway.
    // Here we only want to ensure that as long as minos-client is running,
    // applications calling _nss_mions_get* will get the correct results.
    close(tfd);
    if (do_rename) {
        if (0 != rename(tname, path)) {
            perror("rename");
            unlink(tname);
        }
    } else {
        unlink(tname);
    }
}
