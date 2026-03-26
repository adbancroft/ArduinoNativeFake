#include <iostream>
#include <SimpleArduinoFake.h>
#include <unity.h>
#include "unity_filename_helper.h"
#include "printSetup.h"
#include "stringstream_helpers.h"

using namespace fakeit;

static void assert_string_size(size_t actualSize, const char *expectedStr, std::ostringstream &actualStr)
{
    TEST_ASSERT_EQUAL_STRING(expectedStr, actualStr.str().c_str());
    actualStr.str("");
    actualStr.clear();
    actualStr.seekp(0, std::ios::beg);    
}

static void test_write(void)
{
    auto &printFake = SimpleArduinoFake::getContext()._Print;
    Print* print(printFake.getFake());
    std::ostringstream stream;
    ArduinoNativeFake::setupPrintFake(printFake, stream);

    TEST_ASSERT_EQUAL(0, print->availableForWrite());

    assert_string_size(print->write('a'), "a", stream);

    const uint8_t buffer[] = { 'a', 'b', 'c'};
    assert_string_size(print->write(buffer, sizeof(buffer)), "abc", stream);
}

static void test_availableForWrite(void)
{
    auto &printFake = SimpleArduinoFake::getContext()._Print;
    Print* print(printFake.getFake());
    std::stringstream stream;
    ArduinoNativeFake::setupPrintFake(printFake, stream);

    TEST_ASSERT_EQUAL(0, print->availableForWrite());

    reset(stream, "123456789");
    TEST_ASSERT_EQUAL(9, print->availableForWrite());
}

static std::string leftPad(std::string str, size_t targetLength, char paddingChar) {
    if (str.length() < targetLength) {
        size_t paddingNeeded = targetLength - str.length();
        str.insert(0, paddingNeeded, paddingChar);
    }
    return str;
}

static std::string padToLongWidth(const char *toPad) {
    return leftPad(toPad, sizeof(long)*CHAR_BIT, '0');
}

template <typename T>
static void test_print_bases(Print* print, std::ostringstream &stream)
{
    T t = 1234;

    assert_string_size(print->print(t, HEX), "4D2", stream);
    assert_string_size(print->print(t, OCT), "2322", stream);
    assert_string_size(print->print(t, BIN), padToLongWidth("10011010010").c_str(), stream);
    assert_string_size(print->print(t, DEC), "1234", stream);
}

static void test_print(void)
{
    auto &printFake = SimpleArduinoFake::getContext()._Print;
    Print* print(printFake.getFake());
    std::ostringstream stream;
    ArduinoNativeFake::setupPrintFake(printFake, stream);

    assert_string_size(print->print(F("test")), "test", stream);
    assert_string_size(print->print('a'), "a", stream);
    assert_string_size(print->print(String("abc")), "abc", stream);
    assert_string_size(print->print("abc"), "abc", stream);

    test_print_bases<int>(print, stream);
    test_print_bases<unsigned int>(print, stream);
    test_print_bases<long>(print, stream);
    test_print_bases<unsigned long>(print, stream);

    assert_string_size(print->print(12.34567), "12.35", stream);
    assert_string_size(print->print(12.34567, 10), "12.3456700000", stream);

    IPAddress printable(192, 168, 1, 1);
    assert_string_size(print->print(printable), "192.168.1.1", stream);
}

template <typename T>
static void test_println_bases(Print* print, std::ostringstream &stream)
{
    T t = 1234;

    assert_string_size(print->println(t, HEX), "4D2\n", stream);
    assert_string_size(print->println(t, OCT), "2322\n", stream);
    assert_string_size(print->println(t, BIN), (padToLongWidth("10011010010")+"\n").c_str(), stream);
    assert_string_size(print->println(t, DEC), "1234\n", stream);
}

static void test_println(void)
{
    auto &printFake = SimpleArduinoFake::getContext()._Print;
    Print* print(printFake.getFake());
    std::ostringstream stream;
    ArduinoNativeFake::setupPrintFake(printFake, stream);

    assert_string_size(print->println(F("test")), "test\n", stream);
    assert_string_size(print->println('a'), "a\n", stream);
    assert_string_size(print->println(String("abc")), "abc\n", stream);
    assert_string_size(print->println("abc"), "abc\n", stream);

    test_println_bases<int>(print, stream);
    test_println_bases<unsigned int>(print, stream);
    test_println_bases<long>(print, stream);
    test_println_bases<unsigned long>(print, stream);

    assert_string_size(print->println(12.34567), "12.35\n", stream);
    assert_string_size(print->println(12.34567, 10), "12.3456700000\n", stream);

    IPAddress printable(192, 168, 1, 1);
    assert_string_size(print->println(printable), "192.168.1.1\n", stream);
}

void run_print_tests()
{
    unity_filename_helper_t _ufname_helper(__FILE__);

    RUN_TEST(test_write);
    RUN_TEST(test_availableForWrite);
    RUN_TEST(test_print);
    RUN_TEST(test_println);
}
