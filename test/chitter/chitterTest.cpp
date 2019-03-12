//
// Created by zeke on 9/21/18.
//

#include <gtest/gtest.h>
#include <chitter/chitter.h>

using namespace chitter;
using namespace std;

//@TODO: make test cases for other functions
//@TODO: split into multiple test files, and also create a phony test db with hardcoded vals, so not
//@TODO: making edits in 'prod' database

//@TODO: could also mock the entire thing...
TEST(chitterTest, testUserExists) {
    EXPECT_TRUE(checkUserExists("zest"));
    EXPECT_FALSE(checkUserExists("zest12342452345"));
}

TEST(chitterTest, testVerifyPassword) {
    EXPECT_TRUE(verifyPassword("zest", ""));
    EXPECT_FALSE(verifyPassword("zest", "asdf"));
    EXPECT_TRUE(verifyPassword("testPass", "testPass"));
    //@TODO: add more edge cases
}

TEST(chitterTest, testGrabBio) {
    EXPECT_EQ(getBio("blah"), "wwhatup");
    //@TODO: add more edge cases
}

TEST(chitterTest, testGetStatus) {
    auto[statusEnum, displayName] = getServerRoles("jack", "testServer");
    EXPECT_TRUE(statusEnum == Status::Admin);
    EXPECT_EQ(displayName, "nicholson");
}

TEST(chitterTest, testGetFriends) {
    /*
    vector<string> expectedFriends = {"testPass"};
    vector<string> actualFriends = fetchFriends("test1");
    for (int i = 0; i < expectedFriends.size(); i++)
        EXPECT_EQ(expectedFriends[i], actualFriends[i]);
        */

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
