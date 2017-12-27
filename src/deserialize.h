#include <pwd.h>
#include <grp.h>
#include <shadow.h>

#include "minos.pb.h"

namespace minos {

size_t deserialize_required_buffer(const Passwd& in);
size_t deserialize_required_buffer(const Group& in);
size_t deserialize_required_buffer(const Shadow& in);
void deserialize(const Passwd& in, struct passwd* out, char* buf);
void deserialize(const Group& in, struct group* out, char* buf);
void deserialize(const Shadow& in, struct spwd* out, char* buf);

} // namespace minos
