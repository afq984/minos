#define _DEFAULT_SOURCE

#include <string>
#include <memory>
#include <cassert>
#include <mutex>

#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <unistd.h>

#include <grpc++/grpc++.h>

#include "server.h"
#include "serialize.h"
#include "minos.grpc.pb.h"

using grpc::ServerContext;
using grpc::Status;

namespace minos {

template <class Entry, class Param>
class _GetXXByYY {
private:
    void* buffer;
    size_t buflen;
public:
    Entry entry;
    grpc::Status status;
    bool ok;
    _GetXXByYY(
        int (&function)(const Param, Entry*, char*, size_t, Entry**),
        const Param& param
    ):
        buffer(malloc(1024)),
        buflen(1024)
    {
        Entry* result;
        retry:
        int ret = function(param, &entry, (char*)buffer, buflen, &result);
        if (result == NULL) {
            switch (ret) {
                case 0:
                case ENOENT:
                case ESRCH:
                case EBADF:
                case EPERM:
                    status = Status(grpc::NOT_FOUND, "No such entry matching query");
                    ok = false;
                    return;
                case ERANGE:
                    buflen *= 2;
                    if (buflen <= 16384) {
                        buffer = realloc(buffer, buflen);
                        goto retry;
                    }
                    break;
            }
            status = Status(grpc::ABORTED, std::string("Unexpected error: ") + std::strerror(ret));
            ok = false;
            return;
        }
        status = Status::OK;
        ok = true;
    }
    ~_GetXXByYY() {
        free(buffer);
    }
};

template <class Entry, class Param>
_GetXXByYY<Entry, Param> getXXbyYY(
    int (&function)(const Param, Entry*, char*, size_t, Entry**),
    const Param& param
) {
    return _GetXXByYY<Entry, Param>(function, param);
}

template <class Entry>
class GetXXAll {
    static std::mutex entry_mutex;
    std::lock_guard<std::mutex> entry_guard;
    Entry* (&getXXent)();
    void (&endXXent)();
    bool ok;
public:
    Entry* entryp;
    grpc::Status status;
    GetXXAll(
        Entry* (&getXXent)(),
        void (&setXXent)(),
        void (&endXXent)()
    ):
        entry_guard(entry_mutex),
        getXXent(getXXent),
        endXXent(endXXent),
        ok(true)
    {
        setXXent();
    }
    ~GetXXAll() {
        endXXent();
    }
    bool next() {
        if (not ok) {
            return false;
        }
        errno = 0;
        entryp = getXXent();
        if (entryp == NULL) {
            ok = false;
            printf("%d\n", errno);
            switch (errno) {
            case 0:
                status = Status::OK;
                break;
            default:
                status = Status(grpc::ABORTED, std::string("Unexpected error"));
            }
        } else {
            ok = true;
        }
        return ok;
    }
};
template <class Entry> std::mutex GetXXAll<Entry>::entry_mutex;

class MinosImpl final: public Minos::Service {
    std::mutex passwdLock;
    std::mutex groupLock;
    std::mutex shadowLock;
    Status GetPasswdByName(ServerContext* context, const ByName* request, Passwd* reply) override {
        auto handle = getXXbyYY(getpwnam_r, request->name().c_str());
        if (handle.ok) {
            serialize(handle.entry, reply);
        }
        return handle.status;
    }
    Status GetPasswdByUid(ServerContext* context, const ByID* request, Passwd* reply) override {
        auto handle = getXXbyYY(getpwuid_r, request->id());
        if (handle.ok) {
            serialize(handle.entry, reply);
        }
        return handle.status;
    }
    Status GetPasswdAll(ServerContext* context, const Empty*, PasswdList* reply) override {
        GetXXAll<passwd> handle(getpwent, setpwent, endpwent);
        while (handle.next()) {
            serialize(*handle.entryp, reply->add_entries());
        }
        return handle.status;
    }
    Status GetGroupByName(ServerContext* context, const ByName* request, Group* reply) override {
        auto handle = getXXbyYY(getgrnam_r, request->name().c_str());
        if (handle.ok) {
            serialize(handle.entry, reply);
        }
        return handle.status;
    }
    Status GetGroupByGid(ServerContext* context, const ByID* request, Group* reply) override {
        auto handle = getXXbyYY(getgrgid_r, request->id());
        if (handle.ok) {
            serialize(handle.entry, reply);
        }
        return handle.status;
    }
    Status GetGroupAll(ServerContext* context, const Empty*, GroupList* reply) override {
        GetXXAll<group> handle(getgrent, setgrent, endgrent);
        while (handle.next()) {
            serialize(*handle.entryp, reply->add_entries());
        }
        return handle.status;
    }
    Status GetShadowByName(ServerContext* context, const ByName* request, Shadow* reply) override {
        auto handle = getXXbyYY(getspnam_r, request->name().c_str());
        if (handle.ok) {
            serialize(handle.entry, reply);
        }
        return handle.status;
    }
    Status GetShadowAll(ServerContext* context, const Empty* request, ShadowList* reply) override {
        GetXXAll<spwd> handle(getspent, setspent, endspent);
        while (handle.next()) {
            serialize(*handle.entryp, reply->add_entries());
        }
        return handle.status;
    }
};

Server::Server(const std::string& address):
    service(new MinosImpl)
{

    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials(), &port);
    builder.RegisterService(service.get());

    grpc_server = builder.BuildAndStart();
}

} // namespace minos
