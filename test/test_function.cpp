#include <ArduinoFake.h>
#include <unity.h>
#include "unity_filename_helper.h"
#include "function.h"

using namespace fakeit;

static void nullInterrupt(void) {}

static void test_stubs(void)
{
    setupNativeFake(ArduinoFake(Function));

    // These are all stubbed to do nothing
    init();
    Verify(Method(ArduinoFake(Function), init)).AtLeastOnce();
    pinMode(8, INPUT_PULLDOWN);
    Verify(Method(ArduinoFake(Function), pinMode)).AtLeastOnce();
    digitalWrite(8, LOW);
    Verify(Method(ArduinoFake(Function), digitalWrite)).AtLeastOnce();
    digitalRead(8);
    Verify(Method(ArduinoFake(Function), digitalRead)).AtLeastOnce();
    analogRead(8);
    Verify(Method(ArduinoFake(Function), analogRead)).AtLeastOnce();
    analogReference(11);
    Verify(Method(ArduinoFake(Function), analogReference)).AtLeastOnce();
    analogReadResolution(11);
    Verify(Method(ArduinoFake(Function), analogReadResolution)).AtLeastOnce();
    analogWrite(8, LOW);
    Verify(Method(ArduinoFake(Function), analogWrite)).AtLeastOnce();
    pulseIn(8, 100, 100000);
    Verify(Method(ArduinoFake(Function), pulseIn)).AtLeastOnce();
    pulseInLong(8, 100, 100000);
    Verify(Method(ArduinoFake(Function), pulseInLong)).AtLeastOnce();
    detachInterrupt(11);
    Verify(Method(ArduinoFake(Function), detachInterrupt)).AtLeastOnce();
    attachInterrupt(11, nullInterrupt, INPUT);
    Verify(Method(ArduinoFake(Function), attachInterrupt)).AtLeastOnce();
    sei();
    Verify(Method(ArduinoFake(Function), sei)).AtLeastOnce();
    cli();
    Verify(Method(ArduinoFake(Function), cli)).AtLeastOnce();
    tone(11, 5000, 300);
    Verify(Method(ArduinoFake(Function), tone)).AtLeastOnce();
    noTone(11);
    Verify(Method(ArduinoFake(Function), noTone)).AtLeastOnce();
    yield();
    Verify(Method(ArduinoFake(Function), yield)).AtLeastOnce();
}

static void test_millis(void)
{
    setupNativeFake(ArduinoFake(Function));

    unsigned long initialValue = millis();
    unsigned long lastValue = initialValue;
    for (int loop=0; loop<10000; ++loop)
    {
        TEST_ASSERT_GREATER_OR_EQUAL_UINT32(lastValue, millis());
        lastValue = millis();
    }
    TEST_ASSERT_GREATER_THAN_UINT32(initialValue, lastValue);
}

static void test_micros(void)
{
    setupNativeFake(ArduinoFake(Function));

    unsigned long initialValue = micros();
    unsigned long lastValue = initialValue;
    for (int loop=0; loop<10000; ++loop)
    {
        TEST_ASSERT_GREATER_OR_EQUAL_UINT32(lastValue, micros());
        lastValue = micros();
    }
    TEST_ASSERT_GREATER_THAN_UINT32(initialValue, lastValue);
}

static void test_delay(void)
{
    setupNativeFake(ArduinoFake(Function));

    unsigned long initialValue = millis();
    delay(100);
    TEST_ASSERT_GREATER_THAN_UINT32(initialValue+99, millis());
}

static void test_delayMicroseconds(void)
{
    setupNativeFake(ArduinoFake(Function));

    unsigned long initialValue = micros();
    delayMicroseconds(750);
    TEST_ASSERT_GREATER_THAN_UINT32(initialValue+750, micros());
}

static void test_shiftOut(void)
{
    setupNativeFake(ArduinoFake(Function));

    shiftOut(8, 11, LSBFIRST, 127);
}

static void test_shiftIn(void)
{
    setupNativeFake(ArduinoFake(Function));

    TEST_ASSERT_EQUAL(0, shiftIn(8, 11, LSBFIRST));
}

static void test_random(void)
{
    setupNativeFake(ArduinoFake(Function));

    TEST_ASSERT_EQUAL(1L, random(1L));
    TEST_ASSERT_EQUAL(1L, random(0L, 1L));
    TEST_ASSERT_EQUAL(100L, random(100L, 100L));

    randomSeed(1234);
    std::array<long, 101> generated;
    for (long &item: generated)
    {
        item = random(9999);
    }
    randomSeed(1234); // Reset seed to same value - should generate same sequence
    for (long item: generated)
    {
        TEST_ASSERT_EQUAL(item, random(9999));
    }
}

static void test_map(void)
{
    setupNativeFake(ArduinoFake(Function));

    TEST_ASSERT_EQUAL(0, map(0, 0, 1, 0, 1));
    TEST_ASSERT_EQUAL(1, map(1, 0, 1, 0, 1));
    TEST_ASSERT_EQUAL(33, map(1, 0, 3, 0, 100));
}

static void test_pinport(void)
{
    setupNativeFake(ArduinoFake(Function));

    uint8_t pin = (uint8_t)random(UINT8_MAX);
    TEST_ASSERT_EQUAL(pin, digitalPinToPort(pin));
    TEST_ASSERT_EQUAL(pin, digitalPinToBitMask(pin));
    TEST_ASSERT_EQUAL(pin, digitalPinToTimer(pin));

    TEST_ASSERT_NOT_EQUAL(nullptr, portOutputRegister(pin));
    TEST_ASSERT_EQUAL(portOutputRegister(pin), portOutputRegister(pin));

    TEST_ASSERT_NOT_EQUAL(nullptr, portInputRegister(pin));
    TEST_ASSERT_EQUAL(portInputRegister(pin), portInputRegister(pin));

    TEST_ASSERT_NOT_EQUAL(nullptr, portModeRegister(pin));
    TEST_ASSERT_EQUAL(portModeRegister(pin), portModeRegister(pin));
}

void run_function_tests()
{
    unity_filename_helper_t _ufname_helper(__FILE__);

    RUN_TEST(test_stubs);
    RUN_TEST(test_millis);
    RUN_TEST(test_micros);
    RUN_TEST(test_delay);
    RUN_TEST(test_delayMicroseconds);
    RUN_TEST(test_shiftOut);
    RUN_TEST(test_shiftIn);
    RUN_TEST(test_random);
    RUN_TEST(test_map);
    RUN_TEST(test_pinport);
}
