#include <pwd.h>
#include <grp.h>
#include <shadow.h>

#include <minos.h>

namespace minos {

void serialize(const struct passwd& in, minos_passwd& out);
void serialize(const struct group& in, minos_group& out);
void serialize(const struct spwd& in, minos_shadow& out);
void serialize_copy(const struct passwd& in, minos_passwd& out);
void serialize_copy(const struct group& in, minos_group& out);
void serialize_copy(const struct spwd& in, minos_shadow& out);
void free_copy(const minos_passwd& in);
void free_copy(const minos_group& in);
void free_copy(const minos_shadow& in);
size_t deserialize_required_buffer(const minos_passwd& in);
size_t deserialize_required_buffer(const minos_group& in);
size_t deserialize_required_buffer(const minos_shadow& in);
void deserialize(const minos_passwd& in, struct passwd& out, char* buf);
void deserialize(const minos_group& in, struct group& out, char* buf);
void deserialize(const minos_shadow& in, struct spwd& out, char* buf);

}
