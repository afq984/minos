#pragma once

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <glib.h>

struct Args {
    char* config_file;
};

int parse_args(struct Args* args, int argc, char** argv);

void error_reading_config(const GError* error);

struct NetOptions {
    char* address;
};

int parse_net_options(struct NetOptions* options, GKeyFile* key_file);

struct EntOptions {
    int64_t min_id;
    int64_t max_id;
    size_t blacklist_len;
    char** blacklist;
    size_t whitelist_len;
    char** whitelist;
};

int check_int64_error_set_default(int64_t* out, int64_t def, GError* error);

int check_string_list_error(GError* error);

int parse_ent_options(struct EntOptions* options, GKeyFile* key_file, const char* group_name);
