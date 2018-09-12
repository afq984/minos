#include "common.h"


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
    struct NetOptions net_conf = {0};
    if (parse_net_options(&net_conf, key_file)) {
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
}
