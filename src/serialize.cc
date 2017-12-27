#include "serialize.h"

namespace minos {

void serialize(const struct passwd& in, Passwd* out) {
    out->set_pw_name(in.pw_name);
    out->set_pw_passwd(in.pw_passwd);
    out->set_pw_uid(in.pw_uid);
    out->set_pw_gid(in.pw_gid);
    out->set_pw_gecos(in.pw_gecos);
    out->set_pw_dir(in.pw_dir);
    out->set_pw_shell(in.pw_shell);
}

void serialize(const struct group& in, Group* out) {
    out->set_gr_name(in.gr_name);
    out->set_gr_passwd(in.gr_passwd);
    out->set_gr_gid(in.gr_gid);
    for (char** strp = in.gr_mem; *strp; ++ strp) {
        out->add_gr_mem(*strp);
    }
}

void serialize(const struct spwd& in, Shadow* out) {
    out->set_sp_namp(in.sp_namp);
    out->set_sp_pwdp(in.sp_pwdp);
    out->set_sp_lstchg(in.sp_lstchg);
    out->set_sp_min(in.sp_min);
    out->set_sp_max(in.sp_max);
    out->set_sp_warn(in.sp_warn);
    out->set_sp_inact(in.sp_inact);
    out->set_sp_expire(in.sp_expire);
    out->set_sp_flag(in.sp_flag);
}

} // namespace minos
