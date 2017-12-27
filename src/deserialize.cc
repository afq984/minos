#include "deserialize.h"

namespace minos {

size_t deserialize_required_buffer(const Passwd& in) {
    return (
        5
        + in.pw_name().length()
        + in.pw_passwd().length()
        + in.pw_gecos().length()
        + in.pw_dir().length()
        + in.pw_shell().length()
    );
}

size_t deserialize_required_buffer(const Group& in) {
    size_t result = (
        2
        + in.gr_name().length()
        + in.gr_passwd().length()
        + in.gr_mem().size() * (1 + sizeof(char*)) + sizeof(char*)
    );
    for (const auto& str: in.gr_mem()) {
        result += str.length();
    }
    return result;
}

size_t deserialize_required_buffer(const Shadow& in) {
    return (
        2
        + in.sp_namp().length()
        + in.sp_pwdp().length()
    );
}

char* copy_string_to_buf(const std::string& str, char*& buf) {
    char* pos = buf;
    buf += str.length() + 1;
    str.copy(pos, str.length());
    pos[str.length()] = 0;
    return pos;
}

void deserialize(const Passwd& in, struct passwd* out, char* buf) {
    out->pw_name = copy_string_to_buf(in.pw_name(), buf);
    out->pw_passwd = copy_string_to_buf(in.pw_passwd(), buf);
    out->pw_uid = in.pw_uid();
    out->pw_gid = in.pw_gid();
    out->pw_gecos = copy_string_to_buf(in.pw_gecos(), buf);
    out->pw_dir = copy_string_to_buf(in.pw_dir(), buf);
    out->pw_shell = copy_string_to_buf(in.pw_shell(), buf);
}

void deserialize(const Group& in, struct group* out, char* buf) {
    char** memp = out->gr_mem = (char**)buf;
    buf += (in.gr_mem().size() + 1) * sizeof(char*);
    out->gr_name = copy_string_to_buf(in.gr_name(), buf);
    out->gr_passwd = copy_string_to_buf(in.gr_passwd(), buf);
    out->gr_gid = in.gr_gid();
    for (const auto& mem: in.gr_mem()) {
        *memp = copy_string_to_buf(mem, buf);
        ++ memp;
    }
    *memp = 0;
}

void deserialize(const Shadow& in, struct spwd* out, char* buf) {
    out->sp_namp = copy_string_to_buf(in.sp_namp(), buf);
    out->sp_pwdp = copy_string_to_buf(in.sp_pwdp(), buf);
    out->sp_lstchg = in.sp_lstchg();
    out->sp_min = in.sp_min();
    out->sp_max = in.sp_max();
    out->sp_warn = in.sp_warn();
    out->sp_inact = in.sp_inact();
    out->sp_expire = in.sp_expire();
    out->sp_flag = in.sp_flag();
}

} // namespace minos
