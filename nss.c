#include <nss.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <shadow.h>

#ifndef ETC
#define ETC "/etc"
#endif

struct MinosState {
    const char* pathname;
    FILE* file;
};

#define PASSWD_FILE (ETC "/passwd.minos")
#define GROUP_FILE (ETC "/group.minos")
#define SHADOW_FILE (ETC "/shadow.minos")
static struct MinosState passwd_state = {.pathname = PASSWD_FILE, .file = NULL};
static struct MinosState group_state = {.pathname = GROUP_FILE, .file = NULL};
static struct MinosState shadow_state = {.pathname = SHADOW_FILE, .file = NULL};

static enum nss_status minos_open(struct MinosState* st, int* errnop) {
    int errno_backup = errno;
    if (st->file == NULL) {
        st->file = fopen(st->pathname, "r");
    } else {
        st->file = freopen(st->pathname, "r", st->file);
    }
    enum nss_status status = NSS_STATUS_SUCCESS;
    if (st->file == NULL) {
        if (errno == EACCES || errno == ERANGE) {
            status = NSS_STATUS_UNAVAIL;
        } else {
            status = NSS_STATUS_TRYAGAIN;
        }
        *errnop = errno;
    }
    errno = errno_backup;
    return status;
}

static enum nss_status minos_setent(struct MinosState* st) {
    int errno_discard;
    return minos_open(st, &errno_discard);
}

static enum nss_status minos_endent(struct MinosState* st) {
    int errno_backup = errno;
    if (st->file != NULL) {
        fclose(st->file);
    }
    st->file = NULL;
    errno = errno_backup;
    return NSS_STATUS_SUCCESS;
}

#define GETXXENT(RESULT_TYPE, STATE, FGETXXENT_R)                              \
    {                                                                          \
        if (STATE.file == NULL) {                                              \
            enum nss_status status = minos_open(&STATE, errnop);               \
            if (status != NSS_STATUS_SUCCESS) {                                \
                return status;                                                 \
            }                                                                  \
        }                                                                      \
        RESULT_TYPE* resultp;                                                  \
        int error = FGETXXENT_R(STATE.file, result, buffer, buflen, &resultp); \
        if (error == 0) {                                                      \
            return NSS_STATUS_SUCCESS;                                         \
        }                                                                      \
        *errnop = error;                                                       \
        if (error == ENOENT) {                                                 \
            return NSS_STATUS_NOTFOUND;                                        \
        }                                                                      \
        return NSS_STATUS_TRYAGAIN;                                            \
    }

#define GETXXBYYY(PATHNAME, RESULT_TYPE, FGETXXENT_R, BREAK_CONDITION)                     \
    {                                                                                      \
        struct MinosState state = {.pathname = PATHNAME, .file = NULL};                    \
        enum nss_status status = minos_open(&state, errnop);                               \
        if (status != NSS_STATUS_SUCCESS) {                                                \
            return status;                                                                 \
        }                                                                                  \
        RESULT_TYPE* resultp;                                                              \
        int error = 0;                                                                     \
        while (0 == (error = FGETXXENT_R(state.file, result, buffer, buflen, &resultp))) { \
            if (BREAK_CONDITION) {                                                         \
                break;                                                                     \
            }                                                                              \
        }                                                                                  \
        if (state.file != NULL) {                                                          \
            fclose(state.file);                                                            \
        }                                                                                  \
        if (error) {                                                                       \
            *errnop = error;                                                               \
            if (error == ENOENT) {                                                         \
                return NSS_STATUS_NOTFOUND;                                                \
            }                                                                              \
            return NSS_STATUS_TRYAGAIN;                                                    \
        }                                                                                  \
        return NSS_STATUS_SUCCESS;                                                         \
    }

enum nss_status _nss_minos_setpwent(void) { return minos_setent(&passwd_state); }

enum nss_status _nss_minos_endpwent(void) { return minos_endent(&passwd_state); }

enum nss_status _nss_minos_getpwent_r(
    struct passwd* result, char* buffer, size_t buflen, int* errnop) {
    GETXXENT(struct passwd, passwd_state, fgetpwent_r);
}

enum nss_status _nss_minos_getpwnam_r(
    const char* name, struct passwd* result, char* buffer, size_t buflen, int* errnop) {
    GETXXBYYY(PASSWD_FILE, struct passwd, fgetpwent_r, 0 == strcmp(name, result->pw_name));
}

enum nss_status _nss_minos_getpwuid_r(
    uid_t uid, struct passwd* result, char* buffer, size_t buflen, int* errnop) {
    GETXXBYYY(PASSWD_FILE, struct passwd, fgetpwent_r, uid == result->pw_uid);
}

enum nss_status _nss_minos_setgrent(void) { return minos_setent(&group_state); }

enum nss_status _nss_minos_endgrent(void) { return minos_endent(&group_state); }

enum nss_status _nss_minos_getgrent_r(
    struct group* result, char* buffer, size_t buflen, int* errnop) {
    GETXXENT(struct group, group_state, fgetgrent_r);
}

enum nss_status _nss_minos_getgrnam_r(
    const char* name, struct group* result, char* buffer, size_t buflen, int* errnop) {
    GETXXBYYY(GROUP_FILE, struct group, fgetgrent_r, 0 == strcmp(name, result->gr_name));
}

enum nss_status _nss_minos_getgrgid_r(
    gid_t gid, struct group* result, char* buffer, size_t buflen, int* errnop) {
    GETXXBYYY(GROUP_FILE, struct group, fgetgrent_r, gid == result->gr_gid);
}

enum nss_status _nss_minos_setspent(void) { return minos_setent(&shadow_state); }

enum nss_status _nss_minos_endspent(void) { return minos_endent(&shadow_state); }

enum nss_status _nss_minos_getspent_r(
    struct spwd* result, char* buffer, size_t buflen, int* errnop) {
    GETXXENT(struct spwd, shadow_state, fgetspent_r);
}

enum nss_status _nss_minos_getspnam_r(
    const char* name, struct spwd* result, char* buffer, size_t buflen, int* errnop) {
    GETXXBYYY(SHADOW_FILE, struct spwd, fgetspent_r, 0 == strcmp(name, result->sp_namp));
}
