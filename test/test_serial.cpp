#include <iostream>
#include <ArduinoFake.h>
#include <unity.h>
#include "unity_filename_helper.h"
#include "stringstream_helpers.h"
#include "serial.h"

using namespace fakeit;

static void test_stubs(void)
{
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Serial), stream, stream);

    Serial.begin(9600);
    TEST_ASSERT_EQUAL(9600, Serial.baud());
    Verify(OverloadedMethod(ArduinoFake(Serial), begin, void(unsigned long))).AtLeastOnce();
    Serial.begin(9600, 2U);
    Verify(OverloadedMethod(ArduinoFake(Serial), begin, void(unsigned long, uint8_t))).AtLeastOnce();
    Serial.end();
    Verify(Method(ArduinoFake(Serial), end)).AtLeastOnce();
    Serial.flush();
    Verify(Method(ArduinoFake(Serial), flush)).AtLeastOnce();
    TEST_ASSERT_EQUAL(0, Serial.readBreak());
    Verify(Method(ArduinoFake(Serial), readBreak)).AtLeastOnce();

    TEST_ASSERT_EQUAL(Serial_::ONE_STOP_BIT, Serial.stopbits());
    Verify(Method(ArduinoFake(Serial), stopbits)).AtLeastOnce();

    TEST_ASSERT_EQUAL(Serial_::EVEN_PARITY, Serial.paritytype());
    Verify(Method(ArduinoFake(Serial), paritytype)).AtLeastOnce();

    TEST_ASSERT_EQUAL(8, Serial.numbits());
    Verify(Method(ArduinoFake(Serial), numbits)).AtLeastOnce();

    TEST_ASSERT_EQUAL(true, Serial.dtr());
    Verify(Method(ArduinoFake(Serial), dtr)).AtLeastOnce();

    TEST_ASSERT_EQUAL(true, Serial.rts());
    Verify(Method(ArduinoFake(Serial), rts)).AtLeastOnce();    
}

static void test_availableForWrite(void)
{
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Serial), stream, stream);

    TEST_ASSERT_EQUAL(0, Serial.availableForWrite());

    reset(stream, "123456789");
    TEST_ASSERT_EQUAL(9, Serial.availableForWrite());
}

static void test_available(void)
{
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Serial), stream, stream);

    TEST_ASSERT_EQUAL(0, Serial.available());

    reset(stream, "123456789");
    TEST_ASSERT_EQUAL(9, Serial.available());
}

static void test_peek(void)
{
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Serial), stream, stream);

    TEST_ASSERT_EQUAL(-1, Serial.peek());

    reset(stream, "123456789");
    TEST_ASSERT_EQUAL('1', Serial.peek());
    TEST_ASSERT_EQUAL('1', Serial.peek());
    TEST_ASSERT_EQUAL('1', Serial.peek());
}

static void test_read(void)
{
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Serial), stream, stream);

    TEST_ASSERT_EQUAL(-1, Serial.read());

    reset(stream, "123456789");
    TEST_ASSERT_EQUAL('1', Serial.read());
    TEST_ASSERT_EQUAL('2', Serial.read());
    TEST_ASSERT_EQUAL('3', Serial.read());
}

static void test_write(void)
{
    std::stringstream stream;
    setupNativeFake(ArduinoFake(Serial), stream, stream);

    TEST_ASSERT_EQUAL(1, Serial.write('a'));
    TEST_ASSERT_EQUAL('a', stream.str()[0]);

    TEST_ASSERT_EQUAL(5, Serial.write("bcdef"));
    TEST_ASSERT_EQUAL_STRING("abcdef", stream.str().c_str());
}

void run_serial_tests()
{
    unity_filename_helper_t _ufname_helper(__FILE__);

    RUN_TEST(test_stubs);
    RUN_TEST(test_availableForWrite);
    RUN_TEST(test_available);
    RUN_TEST(test_peek);
    RUN_TEST(test_read);
    RUN_TEST(test_write);
}