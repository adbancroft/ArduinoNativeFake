#include <random>
#include <iostream>
#include <ArduinoFake.h>
#include <unity.h>
#include "unity_filename_helper.h"
#include "stream.h"
#include "stringstream_helpers.h"

using namespace fakeit;

static void test_timeout_property(void)
{
    Stream *pStream = ArduinoFakeMock(Stream);
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Stream), stream);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long> distrib(0L, UINT32_MAX);

    unsigned long timeOut = distrib(gen);
    pStream->setTimeout(timeOut);
    TEST_ASSERT_EQUAL_UINT32(timeOut, pStream->getTimeout());
}

static void test_find(void)
{
    Stream *pStream = ArduinoFakeMock(Stream);
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Stream), stream);

    TEST_ASSERT_FALSE(pStream->find('c'));
    reset(stream, "aaaaaaaaa");
    TEST_ASSERT_FALSE(pStream->find('c'));
    reset(stream, "c");
    TEST_ASSERT_TRUE(pStream->find('c'));
    reset(stream, "abc");
    TEST_ASSERT_TRUE(pStream->find('c'));

    reset(stream, "");
    TEST_ASSERT_FALSE(pStream->find("xxxx"));
    reset(stream, "");
    TEST_ASSERT_FALSE(pStream->find(""));
    reset(stream, "aaaaaaaaa");
    TEST_ASSERT_FALSE(pStream->find("xxxx"));
    reset(stream, "xx");
    TEST_ASSERT_FALSE(pStream->find("xxxx"));
    reset(stream, "xxxx");
    TEST_ASSERT_TRUE(pStream->find("xxxx"));
    reset(stream, "abcxxxxabc");
    TEST_ASSERT_TRUE(pStream->find("xxxx"));

    reset(stream, "");
    TEST_ASSERT_FALSE(pStream->find("xxxx", 1));
    reset(stream, "");
    TEST_ASSERT_FALSE(pStream->find("xxxx", 0));
    reset(stream, "aaaaaaaaa");
    TEST_ASSERT_FALSE(pStream->find("xxxx", 2));
    reset(stream, "xx");
    TEST_ASSERT_FALSE(pStream->find("xxxx", 3));
    reset(stream, "xxxx");
    TEST_ASSERT_TRUE(pStream->find("xxxx", 3));
    reset(stream, "abcxxxxabc");
    TEST_ASSERT_TRUE(pStream->find("xxxx", 2));
}
 
static void test_findUntil(void)
{
    Stream *pStream = ArduinoFakeMock(Stream);
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Stream), stream);

    reset(stream, "");
    TEST_ASSERT_FALSE(pStream->findUntil("xxxx", "a"));
    reset(stream, "a");
    TEST_ASSERT_TRUE(pStream->findUntil("xxxx", "a"));
    reset(stream, "xxxa");
    TEST_ASSERT_TRUE(pStream->findUntil("xxxx", "a"));
    reset(stream, "xxxaxxx");
    TEST_ASSERT_TRUE(pStream->findUntil("xxxx", "a"));

    reset(stream, "");
    TEST_ASSERT_FALSE(pStream->findUntil("xxxx", 1, "a", 0));
    reset(stream, "");
    TEST_ASSERT_FALSE(pStream->findUntil("xxxx", 1, "a", 1));
    reset(stream, "a");
    TEST_ASSERT_TRUE(pStream->findUntil("xxxx", 2, "a", 1));
    reset(stream, "xxxa");
    TEST_ASSERT_TRUE(pStream->findUntil("xxxx", 3, "aa", 2));
    reset(stream, "xxxaxxx");
    TEST_ASSERT_TRUE(pStream->findUntil("xxxx", "a"));

    // Overlapping search term & terminator
    // Too complex - not working for now
    // reset(stream, "xxaaxaa");
    // TEST_ASSERT_TRUE(pStream->findUntil("xxaax", "aa"));
    // TEST_ASSERT_EQUAL(4, stream.tellg());
}

static void test_parseInt(void)
{
    Stream *pStream = ArduinoFakeMock(Stream);
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Stream), stream);

    // Default
    reset(stream, "");
    TEST_ASSERT_EQUAL(0L, pStream->parseInt());

    // No prefix
    reset(stream, "0");
    TEST_ASSERT_EQUAL(0L, pStream->parseInt());
    reset(stream, "000");
    TEST_ASSERT_EQUAL(0L, pStream->parseInt());
    reset(stream, "199");
    TEST_ASSERT_EQUAL(199L, pStream->parseInt());

    // Whitespace prefix
    reset(stream, "-199");
    TEST_ASSERT_EQUAL(-199L, pStream->parseInt());
    reset(stream, "-199");
    TEST_ASSERT_EQUAL(-199L, pStream->parseInt(LookaheadMode::SKIP_WHITESPACE));
    reset(stream, " -199");
    TEST_ASSERT_EQUAL(-199L, pStream->parseInt());
    reset(stream, " -199");
    TEST_ASSERT_EQUAL(-199L, pStream->parseInt(LookaheadMode::SKIP_WHITESPACE));
    reset(stream, "\t-199");
    TEST_ASSERT_EQUAL(-199L, pStream->parseInt());
    reset(stream, "\t-199");
    TEST_ASSERT_EQUAL(-199L, pStream->parseInt(LookaheadMode::SKIP_WHITESPACE));

    // Non-ws prefix
    reset(stream, "abc-199");
    TEST_ASSERT_EQUAL(-199L, pStream->parseInt(LookaheadMode::SKIP_ALL));
    reset(stream, "abc-199");
    TEST_ASSERT_EQUAL(0L, pStream->parseInt(LookaheadMode::SKIP_WHITESPACE));
    reset(stream, "abc-199");
    TEST_ASSERT_EQUAL(0L, pStream->parseInt(LookaheadMode::SKIP_NONE));

    // Ignore chars
    reset(stream, "aaa199");
    TEST_ASSERT_EQUAL(199L, pStream->parseInt(LookaheadMode::SKIP_NONE, 'a'));
    reset(stream, "-19,999");
    TEST_ASSERT_EQUAL(-19999L, pStream->parseInt(LookaheadMode::SKIP_NONE, ','));
    reset(stream, "ccccccccccccc-19,999");
    TEST_ASSERT_EQUAL(-19999L, pStream->parseInt(LookaheadMode::SKIP_ALL, ','));
}


static void test_parseFloat(void)
{
    Stream *pStream = ArduinoFakeMock(Stream);
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Stream), stream);

    // Default
    reset(stream, "");
    TEST_ASSERT_EQUAL(0.0f, pStream->parseFloat());

    // No prefix
    reset(stream, "0.0a");
    TEST_ASSERT_EQUAL(0.0f, pStream->parseFloat());
    reset(stream, "0.00");
    TEST_ASSERT_EQUAL(0.0f, pStream->parseFloat());
    reset(stream, "199.123");
    TEST_ASSERT_EQUAL(199.123f, pStream->parseFloat());

    // Whitespace prefix
    reset(stream, "-199.123");
    TEST_ASSERT_EQUAL(-199.123f, pStream->parseFloat());
    reset(stream, "-199.123");
    TEST_ASSERT_EQUAL(-199.123f, pStream->parseFloat(LookaheadMode::SKIP_WHITESPACE));
    reset(stream, " -199.123");
    TEST_ASSERT_EQUAL(-199.123f, pStream->parseFloat());
    reset(stream, " -199.123");
    TEST_ASSERT_EQUAL(-199.123f, pStream->parseFloat(LookaheadMode::SKIP_WHITESPACE));
    reset(stream, "\t-199.123");
    TEST_ASSERT_EQUAL(-199.123f, pStream->parseFloat());
    reset(stream, "\t-199.123");
    TEST_ASSERT_EQUAL(-199.123f, pStream->parseFloat(LookaheadMode::SKIP_WHITESPACE));

    // Non-ws prefix
    reset(stream, "abc-199.123");
    TEST_ASSERT_EQUAL(-199.123f, pStream->parseFloat(LookaheadMode::SKIP_ALL));
    reset(stream, "abc-199.123");
    TEST_ASSERT_EQUAL(0.0f, pStream->parseFloat(LookaheadMode::SKIP_WHITESPACE));
    reset(stream, "abc-199.123");
    TEST_ASSERT_EQUAL(0.0f, pStream->parseFloat(LookaheadMode::SKIP_NONE));

    // Ignore chars
    reset(stream, "aaa199.123");
    TEST_ASSERT_EQUAL(199.0f, pStream->parseFloat(LookaheadMode::SKIP_NONE, 'a'));
    reset(stream, "-19,999.123");
    TEST_ASSERT_EQUAL(-19999.123f, pStream->parseFloat(LookaheadMode::SKIP_NONE, ','));
    reset(stream, "ccccccccccccc-19,999.123f");
    TEST_ASSERT_EQUAL(-19999.123f, pStream->parseFloat(LookaheadMode::SKIP_ALL, ','));
}

static void test_readBytes(void)
{
    Stream *pStream = ArduinoFakeMock(Stream);
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Stream), stream);

    char buffer[256];
    TEST_ASSERT_EQUAL(0U, pStream->readBytes(buffer, sizeof(buffer)));
    reset(stream, "xxxxxxx");
    TEST_ASSERT_EQUAL(0U, pStream->readBytes(buffer, 0));

    reset(stream, "xxxxxxx");
    TEST_ASSERT_EQUAL(7U, pStream->readBytes(buffer, sizeof(buffer)));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("xxxxxxx", buffer, 7);

    reset(stream, "yyyyyyyyy");
    TEST_ASSERT_EQUAL(2U, pStream->readBytes(buffer, 2));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("yy", buffer, 2);
}

static void test_readBytesUntil(void)
{
    Stream *pStream = ArduinoFakeMock(Stream);
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Stream), stream);

    char buffer[256];
    TEST_ASSERT_EQUAL(0U, pStream->readBytesUntil('a', buffer, sizeof(buffer)));
    reset(stream, "xxxxxxx");
    TEST_ASSERT_EQUAL(0U, pStream->readBytesUntil('a', buffer, 0));

    reset(stream, "xxxyxxx");
    TEST_ASSERT_EQUAL(3U, pStream->readBytesUntil('y', buffer, sizeof(buffer)));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("xxx", buffer, 3);

    reset(stream, "yyyyyyyya");
    TEST_ASSERT_EQUAL(2U, pStream->readBytesUntil('a', buffer, 2));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("yy", buffer, 2);
}

static void test_readString(void)
{
    Stream *pStream = ArduinoFakeMock(Stream);
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Stream), stream);

    {
        String s = pStream->readString();
        TEST_ASSERT_EQUAL_STRING("", s.c_str());
    }

    {
        reset(stream, "yyyyyyyya");
        String s = pStream->readString();
        TEST_ASSERT_EQUAL_STRING("yyyyyyyya", s.c_str());
    }

    {
        reset(stream, "yyyyy yyya");
        String s = pStream->readString();
        TEST_ASSERT_EQUAL_STRING("yyyyy", s.c_str());
    }    
}

static void test_readStringUntil(void)
{
    Stream *pStream = ArduinoFakeMock(Stream);
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Stream), stream);

    {
        String s = pStream->readStringUntil(' ');
        TEST_ASSERT_EQUAL_STRING("", s.c_str());
    }

    {
        reset(stream, "yyyyyyyya");
        String s = pStream->readStringUntil('a');
        TEST_ASSERT_EQUAL_STRING("yyyyyyyy", s.c_str());
    }

    {
        reset(stream, "yyyyy yyya");
        String s = pStream->readStringUntil('a');
        TEST_ASSERT_EQUAL_STRING("yyyyy yyy", s.c_str());
    }    
}


void run_stream_tests()
{
    unity_filename_helper_t _ufname_helper(__FILE__);

    RUN_TEST(test_timeout_property);
    RUN_TEST(test_find);
    RUN_TEST(test_findUntil);
    RUN_TEST(test_parseInt);
    RUN_TEST(test_parseFloat);
    RUN_TEST(test_readBytes);
    RUN_TEST(test_readBytesUntil);
    RUN_TEST(test_readString);
    RUN_TEST(test_readStringUntil);
}