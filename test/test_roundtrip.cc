#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>

#include "serialize.h"
#include "deserialize.h"

using google::protobuf::util::MessageDifferencer;
using namespace minos;

void compare(const struct passwd& a, const struct passwd& b) {
    EXPECT_STREQ(a.pw_name, b.pw_name);
    EXPECT_STREQ(a.pw_passwd, b.pw_passwd);
    EXPECT_EQ(a.pw_uid, b.pw_uid);
    EXPECT_EQ(a.pw_gid, b.pw_gid);
    EXPECT_STREQ(a.pw_gecos, b.pw_gecos);
    EXPECT_STREQ(a.pw_shell, b.pw_shell);
}

void compare(const struct group& a, const struct group& b) {
    EXPECT_STREQ(a.gr_name, b.gr_name);
    EXPECT_STREQ(b.gr_name, b.gr_name);
    EXPECT_EQ(a.gr_gid, b.gr_gid);
}

void compare(const struct spwd& a, const struct spwd& b) {
    EXPECT_STREQ(a.sp_namp, b.sp_namp);
    EXPECT_STREQ(a.sp_pwdp, b.sp_pwdp);
    EXPECT_EQ(a.sp_lstchg, b.sp_lstchg);
    EXPECT_EQ(a.sp_min, b.sp_min);
    EXPECT_EQ(a.sp_max, b.sp_max);
    EXPECT_EQ(a.sp_warn, b.sp_warn);
    EXPECT_EQ(a.sp_inact, b.sp_inact);
    EXPECT_EQ(a.sp_expire, b.sp_expire);
    EXPECT_EQ(a.sp_flag, b.sp_flag);
}

template <class Message, class Entry>
void test_roundtrip(
    const Entry& original
) {
    Message serialized;
    Entry deserialized;
    Message reserialized;
    serialize(original, &serialized);
    auto buf = new char[deserialize_required_buffer(serialized)];
    deserialize(serialized, &deserialized, buf);
    compare(original, deserialized);

    serialize(deserialized, &reserialized);
    EXPECT_TRUE(MessageDifferencer::Equals(serialized, reserialized));
    EXPECT_EQ(deserialize_required_buffer(serialized), deserialize_required_buffer(reserialized));

    delete[] buf;
}

TEST(RoundTrip, Passwd) {
    struct passwd pwd;
    pwd.pw_name = strdup("username");
    pwd.pw_passwd = strdup("user password");
    pwd.pw_uid = 1024;
    pwd.pw_gid = 2048;
    pwd.pw_gecos = strdup("user information");
    pwd.pw_dir = strdup("/home/username");
    pwd.pw_shell = strdup("/bin/afg");
    test_roundtrip<Passwd>(pwd);
    free(pwd.pw_name);
    free(pwd.pw_passwd);
    free(pwd.pw_gecos);
    free(pwd.pw_dir);
    free(pwd.pw_shell);
}

TEST(RoundTrip, Group) {
    struct group grp;
    grp.gr_name = strdup("pp");
    grp.gr_passwd = strdup("y");
    grp.gr_gid = 6666;
    grp.gr_mem = new char*[7];
    grp.gr_mem[0] = strdup("afg");
    grp.gr_mem[1] = strdup("zlsh");
    grp.gr_mem[2] = strdup("scott");
    grp.gr_mem[3] = strdup("ron");
    grp.gr_mem[4] = strdup("katrina");
    grp.gr_mem[5] = strdup("githubber");
    grp.gr_mem[6] = 0;
    test_roundtrip<Group>(grp);
    free(grp.gr_name);
    free(grp.gr_passwd);
    for (int i = 0; i < 6; ++ i) {
        free(grp.gr_mem[i]);
    }
    delete[] grp.gr_mem;
}

TEST(RoundTrip, Shadow) {
    struct spwd shadow;
    shadow.sp_namp = strdup("afg");
    shadow.sp_pwdp = strdup("@ c0mp1ex pa55vv0rd");
    shadow.sp_lstchg = 123;
    shadow.sp_min = 456;
    shadow.sp_max = 789;
    shadow.sp_warn = 9487;
    shadow.sp_inact = 1312;
    shadow.sp_expire = 9301;
    shadow.sp_flag = 2390;
    test_roundtrip<Shadow>(shadow);
    free(shadow.sp_namp);
    free(shadow.sp_pwdp);
}

template <class Message, class Entry>
void test_roundtrip_local(
    void (&setXXent)(void),
    Entry* (&getXXent)(void),
    void (&endXXent)(void),
    bool must_have_entry = true
) {
    setXXent();
    Entry* original;
    int count = 0;
    for (original = getXXent(); original != NULL; original = getXXent()) {
        ++ count;
        test_roundtrip<Message>(*original);
    }
    if (must_have_entry) {
        ASSERT_GT(count, 0);
    }
    endXXent();
}

TEST(RoundTripLocal, PasswdBasic) {
    setpwent();
    struct passwd* original;
    Passwd serialized;
    struct passwd deserialized;
    Passwd reserialized;
    int count = 0;
    for (original = getpwent(); original != NULL; original = getpwent()) {
        ++ count;
        serialize(*original, &serialized);
        auto buf = new char[deserialize_required_buffer(serialized)];
        deserialize(serialized, &deserialized, buf);
        EXPECT_STREQ(original->pw_name, deserialized.pw_name);
        EXPECT_STREQ(original->pw_passwd, deserialized.pw_passwd);
        EXPECT_EQ(original->pw_uid, deserialized.pw_uid);
        EXPECT_EQ(original->pw_gid, deserialized.pw_gid);
        EXPECT_STREQ(original->pw_gecos, deserialized.pw_gecos);
        EXPECT_STREQ(original->pw_shell, deserialized.pw_shell);

        serialize(deserialized, &reserialized);
        EXPECT_TRUE(MessageDifferencer::Equals(serialized, reserialized));

        delete[] buf;
    }
    ASSERT_GT(count, 0);
    endpwent();
}

TEST(RoundTripLocal, Passwd) {
    SCOPED_TRACE("passwd roundtrip");
    test_roundtrip_local<Passwd>(
        setpwent,
        getpwent,
        endpwent
    );
}

TEST(RoundTripLocal, Group) {
    SCOPED_TRACE("group roundtrip");
    test_roundtrip_local<Group>(
        setgrent,
        getgrent,
        endgrent
    );
}

TEST(RoundTripLocal, Shadow) {
    SCOPED_TRACE("shadow roundtrip");
    test_roundtrip_local<Shadow>(
        setspent,
        getspent,
        endspent,
        false
    );
}
