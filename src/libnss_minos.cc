#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <nss.h>
#include <string.h>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include "deserialize.h"
#include "minos.grpc.pb.h"

using namespace minos;

namespace minos {

static std::string server_address = "10.12.25.1:2047";

void set_server_address(const std::string& a) {
    server_address = a;
}

class Client {
public:
    std::unique_ptr<Minos::StubInterface> stub;
    Client(): Client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())) {}
    Client(std::shared_ptr<grpc::Channel> channel): stub(Minos::NewStub(channel)) {
    }
    template <class Request, class Reply, class Entry>
    enum nss_status getXXbyYY(
        grpc::Status(Minos::StubInterface::*method)(grpc::ClientContext*, const Request&, Reply*),
        const Request& request,
        Entry* entry,
        char* buf,
        size_t buflen,
        int* errnop
    ) {
        grpc::ClientContext context;
        Reply reply;
        grpc::Status status = ((*stub).*method)(&context, request, &reply);
        if (status.ok()) {
            if (deserialize_required_buffer(reply) <= buflen) {
                deserialize(reply, entry, buf);
                return NSS_STATUS_SUCCESS;
            } else {
                *errnop = ERANGE;
                return NSS_STATUS_TRYAGAIN;
            }
        } else if (status.error_code() == grpc::NOT_FOUND) {
            *errnop = ENOENT;
            return NSS_STATUS_NOTFOUND;
        } else {
            *errnop = EAGAIN;
            return NSS_STATUS_TRYAGAIN;
        }
    }
    template <class Reply>
    enum nss_status setXXent(
        grpc::Status(Minos::StubInterface::*method)(grpc::ClientContext*, const Empty&, Reply*),
        Reply* reply
    ) {
        grpc::ClientContext context;
        Empty request;
        grpc::Status status = ((*stub).*method)(&context, request, reply);
        if (status.ok()) {
            return NSS_STATUS_SUCCESS;
        } else {
            return NSS_STATUS_TRYAGAIN;
        }
    }
};

template <class Entry, class Reply>
enum nss_status getXXent(
    const Reply& reply,
    ssize_t* index,
    Entry* entry,
    char* buf,
    size_t buflen,
    int* errnop
) {
    const auto& entries = reply.entries();
    if (*index >= entries.size()) {
        return NSS_STATUS_NOTFOUND;
    }
    if (deserialize_required_buffer(entries[*index]) > buflen) {
        *errnop = ERANGE;
        return NSS_STATUS_TRYAGAIN;
    }
    deserialize(entries[*index], entry, buf);
    ++ *index;
    return NSS_STATUS_SUCCESS;
}

static PasswdList passwd_list;
static GroupList group_list;
static ShadowList shadow_list;
static ssize_t passwd_index = 0;
static ssize_t group_index = 0;
static ssize_t shadow_index = 0;

} // namespace minos

extern "C" {

enum nss_status _nss_minos_setpwent() {
    Client client;
    passwd_index = 0;
    return client.setXXent(&Minos::StubInterface::GetPasswdAll, &passwd_list);
}

enum nss_status _nss_minos_setgrent() {
    Client client;
    group_index = 0;
    return client.setXXent(&Minos::StubInterface::GetGroupAll, &group_list);
}

enum nss_status _nss_minos_setspent() {
    Client client;
    shadow_index = 0;
    return client.setXXent(&Minos::StubInterface::GetShadowAll, &shadow_list);
}

enum nss_status _nss_minos_getpwent_r(struct passwd* pwd, char* buf, size_t buflen, int* errnop) {
    return getXXent(passwd_list, &passwd_index, pwd, buf, buflen, errnop);
}

enum nss_status _nss_minos_getgrent_r(struct group* grp, char* buf, size_t buflen, int* errnop) {
    return getXXent(group_list, &group_index, grp, buf, buflen, errnop);
}

enum nss_status _nss_minos_getspent_r(struct spwd* shadow, char* buf, size_t buflen, int* errnop) {
    return getXXent(shadow_list, &shadow_index, shadow, buf, buflen, errnop);
}

enum nss_status _nss_minos_endpwent() {
    return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_minos_endgrent() {
    return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_minos_endspent() {
    return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_minos_getpwnam_r(
    const char* name, struct passwd* pwd, char* buf, size_t buflen, int* errnop
) {
    Client client;
    ByName request;
    request.set_name(name);
    return client.getXXbyYY(&Minos::StubInterface::GetPasswdByName, request, pwd, buf, buflen, errnop);
}

enum nss_status _nss_minos_getpwuid_r(
    uid_t uid, struct passwd* pwd, char* buf, size_t buflen, int* errnop
) {
    Client client;
    ByID request;
    request.set_id(uid);
    return client.getXXbyYY(&Minos::StubInterface::GetPasswdByUid, request, pwd, buf, buflen, errnop);
}

enum nss_status _nss_minos_getgrnam_r(
    const char* name, struct group* grp, char* buf, size_t buflen, int* errnop
) {
    Client client;
    ByName request;
    request.set_name(name);
    return client.getXXbyYY(&Minos::StubInterface::GetGroupByName, request, grp, buf, buflen, errnop);
}

enum nss_status _nss_minos_getgrgid_r(
    gid_t gid, struct group* grp, char* buf, size_t buflen, int* errnop
) {
    Client client;
    ByID request;
    request.set_id(gid);
    return client.getXXbyYY(&Minos::StubInterface::GetGroupByGid, request, grp, buf, buflen, errnop);
}

enum nss_status _nss_minos_getspnam_r(
    const char* name, struct spwd* shadow, char* buf, size_t buflen, int* errnop
) {
    Client client;
    ByName request;
    request.set_name(name);
    return client.getXXbyYY(&Minos::StubInterface::GetShadowByName, request, shadow, buf, buflen, errnop);
}

} // extern "C"
