enum minos_status {
	MINOS_SUCCESS,
	MINOS_NOTFOUND,
	MINOS_SERVER_ERROR
};
struct minos_passwd {
	string pw_name<>;
	string pw_passwd<>;
	uint32_t pw_uid;
	uint32_t pw_gid;
	string pw_gecos<>;
	string pw_dir<>;
	string pw_shell<>;
};
struct minos_passwd_result {
	minos_status status;
	minos_passwd entry;
};
typedef minos_passwd minos_passwd_list<>;
struct minos_passwd_list_result {
	minos_status status;
	minos_passwd list<>;
};
typedef string minos_group_member<>;
struct minos_group {
	string gr_name<>;
	string gr_passwd<>;
	uint32_t gr_gid;
	minos_group_member gr_mem<>;
};
struct minos_group_result {
	minos_status status;
	minos_group entry;
};
struct minos_group_list_result {
	minos_status status;
	minos_group list<>;
};
struct minos_shadow {
	string sp_namp<>;
	string sp_pwdp<>;
	long int sp_lstchg;
	long int sp_min;
	long int sp_max;
	long int sp_warn;
	long int sp_inact;
	long int sp_expire;
	unsigned long int sp_flag;
};
struct minos_shadow_result {
	minos_status status;
	minos_shadow entry;
};
typedef minos_shadow minos_shadow_list<>;
struct minos_shadow_list_result {
	minos_status status;
	minos_shadow list<>;
};
program MINOS {
	version MINOSVERSION {
		minos_passwd_result MINOS_GETPWNAM(string name) = 1;
		minos_passwd_result MINOS_GETPWUID(uint32_t uid) = 2;
		minos_passwd_list_result MINOS_GETPWALL() = 3;
		minos_group_result MINOS_GETGRNAM(string name) = 4;
		minos_group_result MINOS_GETGRGID(uint32_t gid) = 5;
		minos_group_list_result MINOS_GETGRALL() = 6;
		minos_shadow_result MINOS_GETSPNAM(string name) = 7;
		minos_shadow_list_result MINOS_GETSPALL() = 9;
	} = 1;
} = 0x2da38bc;
