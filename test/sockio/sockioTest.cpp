//
// Created by zeke on 10/10/18.
//

#include <gtest/gtest.h>
#include <sockio/sockio.h>

using namespace sockio;
using namespace std;

TEST(sockioTest, testSock) {
    //@TODO: this is a candidate for 'mocking'
    //tcp::socket socket;
}

TEST(sockioTest, testSplit) {
    string fullMsg = "zeke\r\n"
                     "zest\r\n"
                     "zigg\r\n";
    deque<string> splitMsgs = split(fullMsg, "\r\n");
    EXPECT_EQ(splitMsgs[0], "zeke");
    EXPECT_EQ(splitMsgs[1], "zest");
    EXPECT_EQ(splitMsgs[2], "zigg");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
