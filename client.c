#define _GNU_SOURCE

#include <stdio.h>
#include <assert.h>

#include <zmq.h>
#include <glib.h>

#include "common.h"
#include "atomic_update_file.h"

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

    void* zctx = zmq_ctx_new();
    if (zctx == NULL) {
        fprintf(stderr, "FATAL: Cannot create 0MQ context\n");
        return 1;
    }
    void* zsock = zmq_socket(zctx, ZMQ_SUB);
    if (zsock == NULL) {
        fprintf(stderr, "FATAL: Cannot create 0MQ socket\n");
        return 1;
    }
    int ret = zmq_connect(zsock, minos_conf.address);
    if (ret != 0) {
        fprintf(stderr, "FATAL: Cannot connect 0MQ socket\n");
        return 1;
    }
    ret = zmq_setsockopt(zsock, ZMQ_SUBSCRIBE, 0, 0);
    if (ret != 0) {
        fprintf(stderr, "FATAL: zmq_setsockopt: %s\n", zmq_strerror(errno));
        return 1;
    }

    const char* databases[] = {"passwd", "group", "shadow"};
    char* filenames[3];
    for (int i = 0; i < 3; i++) {
        assert(-1 != asprintf(&filenames[i], "%s/%s.minos", minos_conf.sysconfdir, databases[i]));
    }

    while (1) {
        int more;
        size_t moresize = sizeof more;

        zmq_msg_t parts[3];
        for (int i = 0; i < 3; i++) {
            zmq_msg_init(&parts[i]);
            assert(-1 != zmq_recvmsg(zsock, &parts[i], 0));
            assert(0 == zmq_getsockopt(zsock, ZMQ_RCVMORE, &more, &moresize));
            assert((i == 2) != more);
        }

        for (int i = 0; i < 3; i++) {
            atomic_update_file(filenames[i], zmq_msg_data(&parts[i]), zmq_msg_size(&parts[i]), 0644);
        }

        for (int i = 0; i < 3; i++) {
            assert(0 == zmq_msg_close(&parts[i]));
        }
    }
}
