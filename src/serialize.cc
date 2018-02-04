#include <string.h>
#include <type_traits>
#include "serialize.h"

namespace minos {

void serialize(const struct passwd& in, minos_passwd& out) {
    out.pw_name = in.pw_name;
    out.pw_passwd = in.pw_passwd;
    out.pw_uid = in.pw_uid;
    out.pw_gid = in.pw_gid;
    out.pw_gecos = in.pw_gecos;
    out.pw_dir = in.pw_dir;
    out.pw_shell = in.pw_shell;
}

void serialize(const struct group& in, minos_group& out) {
    out.gr_name = in.gr_name;
    out.gr_passwd = in.gr_passwd;
    out.gr_gid = in.gr_gid;
    int mem_count = 0;
    for (; in.gr_mem[mem_count]; ++ mem_count);
    out.gr_mem.gr_mem_len = mem_count;
    out.gr_mem.gr_mem_val = in.gr_mem;
}

void serialize(const struct spwd& in, minos_shadow& out) {
    out.sp_namp = in.sp_namp;
    out.sp_pwdp = in.sp_pwdp;
    out.sp_lstchg = in.sp_lstchg;
    out.sp_min = in.sp_min;
    out.sp_max = in.sp_max;
    out.sp_warn = in.sp_warn;
    out.sp_inact = in.sp_inact;
    out.sp_expire = in.sp_expire;
    out.sp_flag = in.sp_flag;
}

void serialize_copy(const struct passwd& in, minos_passwd& out) {
    serialize(in, out);
    out.pw_name = strdup(out.pw_name);
    out.pw_passwd = strdup(out.pw_passwd);
    out.pw_gecos = strdup(out.pw_gecos);
    out.pw_dir = strdup(out.pw_dir);
    out.pw_shell = strdup(out.pw_shell);
}

void serialize_copy(const struct group& in, minos_group& out) {
    serialize(in, out);
    out.gr_name = strdup(out.gr_name);
    out.gr_passwd = strdup(out.gr_passwd);
    out.gr_mem.gr_mem_val = new char*[out.gr_mem.gr_mem_len];
    for (unsigned i = 0; i < out.gr_mem.gr_mem_len; ++ i) {
        out.gr_mem.gr_mem_val[i] = strdup(in.gr_mem[i]);
    }
}

void serialize_copy(const struct spwd& in, minos_shadow& out) {
    serialize(in, out);
    out.sp_namp = strdup(out.sp_namp);
    out.sp_pwdp = strdup(out.sp_pwdp);
}

void free_copy(const minos_passwd& in) {
    free(in.pw_name);
    free(in.pw_passwd);
    free(in.pw_gecos);
    free(in.pw_dir);
    free(in.pw_shell);
}

void free_copy(const minos_group& in) {
    free(in.gr_name);
    free(in.gr_passwd);
    for (unsigned i = 0; i < in.gr_mem.gr_mem_len; ++ i) {
        free(in.gr_mem.gr_mem_val[i]);
    }
    delete[] in.gr_mem.gr_mem_val;
}

void free_copy(const minos_shadow& in) {
    free(in.sp_namp);
    free(in.sp_pwdp);
}

size_t deserialize_required_buffer(const minos_passwd& in) {
    return (
        5
        + strlen(in.pw_name)
        + strlen(in.pw_passwd)
        + strlen(in.pw_gecos)
        + strlen(in.pw_dir)
        + strlen(in.pw_shell)
    );
}

size_t deserialize_required_buffer(const minos_group& in) {
    size_t result = (
        2
        + strlen(in.gr_name)
        + strlen(in.gr_passwd)
        + in.gr_mem.gr_mem_len * (1 + sizeof(char*)) + sizeof(char*)
    );
    for (unsigned i = 0; i < in.gr_mem.gr_mem_len; ++ i) {
        result += strlen(in.gr_mem.gr_mem_val[i]);
    }
    return result;
}

size_t deserialize_required_buffer(const minos_shadow& in) {
    return (
        2
        + strlen(in.sp_namp)
        + strlen(in.sp_pwdp)
    );
}

char* strcpy_udbuf(const char* str, char*& buf) {
    char* pos = buf;
    strcpy(pos, str);
    buf += strlen(str) + 1;
    return pos;
}

void deserialize(const minos_passwd& in, struct passwd& out, char* buf) {
    out.pw_name = strcpy_udbuf(in.pw_name, buf);
    out.pw_passwd = strcpy_udbuf(in.pw_passwd, buf);
    out.pw_uid = in.pw_uid;
    out.pw_gid = in.pw_gid;
    out.pw_gecos = strcpy_udbuf(in.pw_gecos, buf);
    out.pw_dir = strcpy_udbuf(in.pw_dir, buf);
    out.pw_shell = strcpy_udbuf(in.pw_shell, buf);
}

void deserialize(const minos_group& in, struct group& out, char* buf) {
    char** memp = out.gr_mem = (char**)buf;
    buf += (in.gr_mem.gr_mem_len + 1) * sizeof(char*);
    out.gr_name = strcpy_udbuf(in.gr_name, buf);
    out.gr_passwd = strcpy_udbuf(in.gr_passwd, buf);
    out.gr_gid = in.gr_gid;
    for (unsigned i = 0; i < in.gr_mem.gr_mem_len; ++ i) {
        *memp = strcpy_udbuf(in.gr_mem.gr_mem_val[i], buf);
        ++ memp;
    }
    *memp = 0;
}

void deserialize(const minos_shadow& in, struct spwd& out, char* buf) {
    out.sp_namp = strcpy_udbuf(in.sp_namp, buf);
    out.sp_pwdp = strcpy_udbuf(in.sp_pwdp, buf);
    out.sp_lstchg = in.sp_lstchg;
    out.sp_min = in.sp_min;
    out.sp_max = in.sp_max;
    out.sp_warn = in.sp_warn;
    out.sp_inact = in.sp_inact;
    out.sp_expire = in.sp_expire;
    out.sp_flag = in.sp_flag;
}

static_assert(std::is_same<decltype(spwd::sp_flag), decltype(minos_shadow::sp_flag)>::value, "sp_flag");

}
