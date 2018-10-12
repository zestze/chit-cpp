//
// Created by zeke on 9/21/18.
//

#include <gtest/gtest.h>
#include <chitter.h>

using namespace chitter;
using namespace std;

//@TODO: make test cases for other functions
TEST(chitterTest, testUserExists) {
    EXPECT_TRUE(checkUserExists("zest"));
    EXPECT_FALSE(checkUserExists("zest12342452345"));
}

TEST(chitterTest, testVerifyPassword) {
    EXPECT_TRUE(verifyPassword("zest", ""));
    EXPECT_FALSE(verifyPassword("zest", "asdf"));
    EXPECT_TRUE(verifyPassword("testPass", "testPass"));
}

TEST(chitterTest, testGrabBio) {
    EXPECT_EQ(getBio("blah"), "wwhatup");
}

TEST(chitterTest, testGetStatus) {
    //tcp::endpoint end (asio::ip::address_v4::from_string("127.0.0.1"),
    //                   3372);
    //insertLogin("testPass", end);

    //cout << getStatusString(Status::Admin) << endl;

    auto[statusEnum, displayName] = getServerRoles("khalid", "server3003");
    EXPECT_TRUE(statusEnum == Status::Admin);
    EXPECT_EQ(displayName, "khalid");

    EXPECT_TRUE(getChannelRoles("zest", "123", "blah") == Status::Nonexistent);
    EXPECT_TRUE(getChannelRoles("blah", "#fudge", "server2646") == Status::Admin);
}

TEST(chitterTest, testGetFriends) {
    vector<string> expectedFriends = {"testPass"};
    vector<string> actualFriends = fetchFriends("test1");
    for (int i = 0; i < expectedFriends.size(); i++)
        EXPECT_EQ(expectedFriends[i], actualFriends[i]);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
