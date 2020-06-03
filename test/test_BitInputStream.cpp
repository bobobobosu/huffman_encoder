#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "BitInputStream.hpp"

using namespace std;
using namespace testing;

TEST(BitInputStreamTests, SIMPLE_TEST) {
    string byteStr = "10000000";
    char byte = (char)stoi(byteStr, nullptr, 2);
    string byteStr2 = "01010101";
    char byte2 = (char)stoi(byteStr2, nullptr, 2);

    stringstream ss;
    ss.write(&byte, 1);
    ss.write(&byte2, 1);
    BitInputStream bis(ss, 1);

    ASSERT_EQ(1, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
    ASSERT_EQ(1, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
    ASSERT_EQ(1, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
}