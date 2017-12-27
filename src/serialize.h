#include <pwd.h>
#include <grp.h>
#include <shadow.h>

#include "minos.pb.h"

namespace minos {

void serialize(const struct passwd& in, Passwd* out);
void serialize(const struct group& in, Group* out);
void serialize(const struct spwd& in, Shadow* out);

} // namespace minos
