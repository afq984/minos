#define _GNU_SOURCE
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <shadow.h>

#include "common.h"

int read_system_pwd_spwd(FILE* out, FILE* sout, const struct EntOptions* user_conf) {
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
            perror("warning unexpected error in getspnam()");
        } else {
            if (-1 == putspent(shadow, sout)) {
                perror("unexpected error in putspent()");
                return 1;
            }
        }
    }
    return 0;
}

int read_system_grp(FILE* out, const struct EntOptions* group_conf) {
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

    char* pwdbuf = NULL;
    size_t pwdbufsize;
    FILE* pwdmem = open_memstream(&pwdbuf, &pwdbufsize);
    if (pwdmem == NULL) {
        perror("unexpected error during open_memstream");
        return 1;
    }
    char* spwdbuf = NULL;
    size_t spwdbufsize;
    FILE* spwdmem = open_memstream(&spwdbuf, &spwdbufsize);
    if (spwdmem == NULL) {
        perror("unexpected error during open_memstream");
        return 1;
    }
    if (read_system_pwd_spwd(pwdmem, spwdmem, &user_conf)) {
        return 1;
    }
    if (0 != fclose(pwdmem)) {
        perror("unexpected error during fclose");
        return 1;
    }
    if (0 != fclose(spwdmem)) {
        perror("unexpected error during fclose");
        return 1;
    }
    printf("%s", pwdbuf);
    printf("%s", spwdbuf);

    char* grpbuf = NULL;
    size_t grpbufsize;
    FILE* grpmem = open_memstream(&grpbuf, &grpbufsize);
    if (grpmem == NULL) {
        perror("unexpected error during open_memstream");
        return 1;
    }
    if (read_system_grp(grpmem, &group_conf)) {
        return 1;
    }
    fclose(grpmem);
    printf("%s", grpbuf);
}
