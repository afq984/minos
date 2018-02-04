#include <nss.h>
#include <errno.h>

#include <minos.h>
#include "serialize.h"


namespace {

template <class Result, class Entry, class Param>
enum nss_status getXXbyYY(
	Result* (&rpc)(Param, CLIENT*),
	bool_t (&xdrproc)(XDR*, Result*),
	Param param, Entry* entry, char* buf, size_t buflen, int* errnop
) {
	CLIENT* clnt = clnt_create("127.0.0.1", MINOS, MINOSVERSION, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror("127.0.0.1");
		return NSS_STATUS_TRYAGAIN;
	}
	Result* result = rpc(param, clnt);
	enum nss_status status;
	switch (result->status) {
		case MINOS_SUCCESS:
			if (minos::deserialize_required_buffer(result->entry) <= buflen) {
				status = NSS_STATUS_SUCCESS;
				minos::deserialize(result->entry, *entry, buf);
			} else {
				*errnop = ERANGE;
				status = NSS_STATUS_TRYAGAIN;
			}
			break;
		case MINOS_NOTFOUND:
			*errnop = ENOENT;
			status = NSS_STATUS_NOTFOUND;
			break;
		case MINOS_SERVER_ERROR:
			*errnop = EAGAIN;
			status = NSS_STATUS_TRYAGAIN;
			break;
	}
	clnt_freeres(clnt, (xdrproc_t)xdrproc, (char*)result);
	clnt_destroy(clnt);
	return status;
}

}

extern "C" {

enum nss_status _nss_minos_getpwnam_r(
	const char* name, struct passwd* pwd, char* buf, size_t buflen, int* errnop
) {
	return getXXbyYY(
		minos_getpwnam_1,
		xdr_minos_passwd_result,
		strdupa(name), pwd, buf, buflen, errnop);
}

enum nss_status _nss_minos_getpwuid_r(
	uid_t uid, struct passwd* pwd, char* buf, size_t buflen, int* errnop
) {
	return getXXbyYY(
		minos_getpwuid_1,
		xdr_minos_passwd_result,
		uid, pwd, buf, buflen, errnop);
}

enum nss_status _nss_minos_getgrnam_r(
	const char* name, struct group* grp, char* buf, size_t buflen, int* errnop
) {
	return getXXbyYY(
		minos_getgrnam_1,
		xdr_minos_group_result,
		strdupa(name), grp, buf, buflen, errnop);
}

enum nss_status _nss_minos_getgrgid_r(
	gid_t gid, struct group* grp, char* buf, size_t buflen, int* errnop
) {
	return getXXbyYY(
		minos_getgrgid_1,
		xdr_minos_group_result,
		gid, grp, buf, buflen, errnop);
}

enum nss_status _nss_minos_getspnam_r(
	const char* name, struct spwd* shadow, char* buf, size_t buflen, int* errnop
) {
	return getXXbyYY(
		minos_getspnam_1,
		xdr_minos_shadow_result,
		strdupa(name), shadow, buf, buflen, errnop);
}

}
