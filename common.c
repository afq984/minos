#include "common.h"

int parse_args(struct Args* args, int argc, char** argv) {
    memset(args, 0, sizeof(struct Args));
    int c;
    extern char* optarg;
    extern int optopt;
    while (-1 != (c = getopt(argc, argv, "C:"))) {
        switch (c) {
            case 'C':
                args->config_file = optarg;
                break;
            case '?':
            default:
                return 1;
        }
    }
    if (args->config_file == NULL) {
        fprintf(stderr, "-C config is required\n");
        return 1;
    }
    return 0;
}

void error_reading_config(const GError* error) {
    fprintf(stderr, "Error reading config: %s\n", error->message);
}

int parse_net_options(struct NetOptions* options, GKeyFile* key_file) {
    g_autoptr(GError) error = NULL;
    options->address = g_key_file_get_string(key_file, "Minos", "Address", &error);
    if (error != NULL) {
        error_reading_config(error);
        return 1;
    }
    return 0;
}

int check_int64_error_set_default(int64_t* out, int64_t def, GError* error) {
    if (error == NULL) {
        return 0;
    }
    if (g_error_matches(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND)) {
        *out = def;
        return 0;
    }
        error_reading_config(error);
    return 1;
}

int check_string_list_error(GError* error) {
    if (error == NULL) {
        return 0;
    }
    if (g_error_matches(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND)) {
        return 0;
    }
        error_reading_config(error);
    return 1;
}

int parse_ent_options(struct EntOptions* options, GKeyFile* key_file, const char* group_name) {
    g_autoptr(GError) error = NULL;
    options->min_id = g_key_file_get_int64(key_file, group_name, "MinID", &error);
    if (check_int64_error_set_default(&options->min_id, INT64_MIN, error)) {
        return 1;
    }
    error = NULL;
    options->max_id = g_key_file_get_int64(key_file, group_name, "MaxID", &error);
    if (check_int64_error_set_default(&options->max_id, INT64_MAX, error)) {
        return 1;
    }
    error = NULL;
    options->blacklist = g_key_file_get_string_list(
        key_file, group_name, "Blacklist", &options->blacklist_len, &error);
    if (check_string_list_error(error)) {
        return 1;
    }
    error = NULL;
    options->whitelist = g_key_file_get_string_list(
        key_file, group_name, "Whitelist", &options->whitelist_len, &error);
    if (check_string_list_error(error)) {
        return 1;
    }
    return 0;
}
