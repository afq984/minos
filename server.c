#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <shadow.h>

#include <zmq.h>

#include "common.h"

struct Snapshot {
    char* buf;
    size_t len;
};

const int timeoutms = 4000;

int read_system_pwd_spwd(
    struct Snapshot* psnap, struct Snapshot* spsnap, const struct EntOptions* user_conf) {
    FILE* out = open_memstream(&psnap->buf, &psnap->len);
    if (out == NULL) {
        perror("unexpected error during open_memstream");
        return 1;
    }
    FILE* sout = open_memstream(&spsnap->buf, &spsnap->len);
    if (sout == NULL) {
        perror("unexpected error during open_memstream");
        return 1;
    }
    setpwent();
    while (1) {
        errno = 0;
        struct passwd* pwd = getpwent();
        if (pwd == NULL) {
            if (errno == 0) {
                break;
            }
            perror("warning: unexpected error in getpwent()");
            break;
        }
        if (should_skip_ent(user_conf, pwd->pw_uid, pwd->pw_name)) {
            continue;
        }
        if (-1 == putpwent(pwd, out)) {
            perror("unexpected error in putpwent()");
            return 1;
        }
        struct spwd* shadow = getspnam(pwd->pw_name);
        if (shadow == NULL) {
            perror("warning: unexpected error in getspnam()");
        } else {
            if (-1 == putspent(shadow, sout)) {
                perror("unexpected error in putspent()");
                return 1;
            }
        }
    }
    if (0 != fclose(out)) {
        perror("unexpected error during fclose");
        return 1;
    }
    if (0 != fclose(sout)) {
        perror("unexpected error during fclose");
        return 1;
    }
    return 0;
}

int read_system_grp(struct Snapshot* gsnap, const struct EntOptions* group_conf) {
    FILE* out = open_memstream(&gsnap->buf, &gsnap->len);
    if (out == NULL) {
        perror("unexpected error during open_memstream");
        return 1;
    }
    setgrent();
    while (1) {
        errno = 0;
        struct group* grp = getgrent();
        if (grp == NULL) {
            if (errno == 0) {
                break;
            }
            perror("warning: unexpected error in getgrent()");
            break;
        }
        if (should_skip_ent(group_conf, grp->gr_gid, grp->gr_name)) {
            continue;
        }
        if (-1 == putgrent(grp, out)) {
            perror("unexpected error putgrent()");
            return 1;
        }
    }
    if (0 != fclose(out)) {
        perror("unexpected error during fclose");
        return 1;
    }
    return 0;
}

int main(int argc, char** argv) {
    struct Args args;
    if (parse_args(&args, argc, argv)) {
        return 1;
    }
    printf("Config file: %s\n", args.config_file);
    g_autoptr(GError) error = NULL;
    g_autoptr(GKeyFile) key_file = g_key_file_new();
    if (!g_key_file_load_from_file(key_file, args.config_file, G_KEY_FILE_NONE, &error)) {
        fprintf(stderr, "Error loading config file %s: %s\n", args.config_file, error->message);
        return 1;
    }
    struct MinosOptions minos_conf = {0};
    if (parse_minos_options(&minos_conf, key_file)) {
        return 1;
    }
    struct EntOptions user_conf = {0};
    struct EntOptions group_conf = {0};
    if (parse_ent_options(&user_conf, key_file, "User")) {
        return 1;
    }
    if (parse_ent_options(&group_conf, key_file, "Group")) {
        return 1;
    }

    void* zctx = zmq_ctx_new();
    if (zctx == NULL) {
        perror("Cannot create 0MQ context");
        return 1;
    }
    void* zsock = zmq_socket(zctx, ZMQ_XPUB);
    if (zsock == NULL) {
        perror("Cannot create 0MQ socket");
        return 1;
    }
    if (0 != zmq_bind(zsock, minos_conf.address)) {
        perror("Cannot bind 0MQ socket");
        return 1;
    }
    int xpub_verbose = 1;
    if (0 != zmq_setsockopt(zsock, ZMQ_XPUB_VERBOSE, &xpub_verbose, sizeof xpub_verbose)) {
        fprintf(stderr, "zmq_setsockopt: %s\n", zmq_strerror(errno));
        return 1;
    }
    while (1) {
        zmq_pollitem_t items[] = {{zsock, 0, ZMQ_POLLIN, 0}};
        int poll = zmq_poll(items, 1, timeoutms);
        if (poll == -1) {
            fprintf(stderr, "internal error in zmq_poll: %s\n", strerror(errno));
            return 1;
        }
        int send = 0;
        if (poll == 0) {
            send = 1;
        }
        if (items[0].revents & ZMQ_POLLIN) {
            char buf[1];
            if (-1 == zmq_recv(zsock, buf, sizeof buf, 0)) {
                fprintf(stderr, "internal error in zmq_recv: %s\n", strerror(errno));
                return 1;
            }
            if (buf[0] == 1) {
                puts("Subscription");
                send = 1;
            } else {
                puts("Unsubscription");
                send = 1;
            }
        }
        if (send) {
            struct Snapshot psnap = {0};
            struct Snapshot spsnap = {0};
            if (read_system_pwd_spwd(&psnap, &spsnap, &user_conf)) {
                goto cleanup;
            }

            struct Snapshot gsnap = {0};
            if (read_system_grp(&gsnap, &group_conf)) {
                goto cleanup;
            }
            if (psnap.len != zmq_send(zsock, psnap.buf, psnap.len, ZMQ_SNDMORE)) {
                fprintf(stderr, "failed to send psnap: %s\n", zmq_strerror(errno));
                return 1;
            }
            if (gsnap.len != zmq_send(zsock, gsnap.buf, gsnap.len, ZMQ_SNDMORE)) {
                fprintf(stderr, "failed to send gsnap: %s\n", zmq_strerror(errno));
                return 1;
            }
            if (spsnap.len != zmq_send(zsock, spsnap.buf, spsnap.len, 0)) {
                fprintf(stderr, "failed to send spsnap: %s\n", zmq_strerror(errno));
                return 1;
            }
        cleanup:
            free(psnap.buf);
            free(gsnap.buf);
            free(spsnap.buf);
        }
    }
}
