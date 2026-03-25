#include <SimpleArduinoFake.h>
#include <unity.h>
#include <array>
#include "unity_filename_helper.h"
#include "eepromSetup.h"

static void test_length(void)
{
    {
        std::array<unsigned char, 0> buffer;
        ArduinoNativeFake::setupNativeFake(SimpleArduinoFake::getContext()._EEPROM, buffer.begin(), buffer.end());
        TEST_ASSERT_EQUAL(0, EEPROM.length());
    }

    {
        std::array<unsigned char, 64> buffer;
        ArduinoNativeFake::setupNativeFake(SimpleArduinoFake::getContext()._EEPROM, buffer.begin(), buffer.end());
        TEST_ASSERT_EQUAL(64, EEPROM.length());
    }
}

static void test_read(void)
{
    std::array<unsigned char, 5> buffer = {'t', 'e', 's', 't', '1'};
    ArduinoNativeFake::setupNativeFake(SimpleArduinoFake::getContext()._EEPROM, buffer.begin(), buffer.end());

    TEST_ASSERT_EQUAL('t', EEPROM.read(0U));
    TEST_ASSERT_EQUAL('e', EEPROM.read(1U));
    TEST_ASSERT_EQUAL('s', EEPROM.read(2U));
    TEST_ASSERT_EQUAL('t', EEPROM.read(3U));
    TEST_ASSERT_EQUAL('1', EEPROM.read(4U));
}
    
static void test_write(void)
{
    std::array<unsigned char, 256> buffer;
    ArduinoNativeFake::setupNativeFake(SimpleArduinoFake::getContext()._EEPROM, buffer.begin(), buffer.end());

    EEPROM.write(121, 'x');
    TEST_ASSERT_EQUAL('x', buffer[121]);

    EEPROM.write(0, 'y');
    TEST_ASSERT_EQUAL('y', buffer[0]);

    EEPROM.write(sizeof(buffer)-1, 'z');
    TEST_ASSERT_EQUAL('z', buffer[sizeof(buffer)-1]);
}

static void test_update(void)
{
    std::array<unsigned char, 256> buffer;
    ArduinoNativeFake::setupNativeFake(SimpleArduinoFake::getContext()._EEPROM, buffer.begin(), buffer.end());

    EEPROM.update(121, 'x');
    TEST_ASSERT_EQUAL('x', buffer[121]);
    EEPROM.update(121, 'x');
    TEST_ASSERT_EQUAL('x', buffer[121]);

    EEPROM.update(0, 'y');
    TEST_ASSERT_EQUAL('y', buffer[0]);

    EEPROM.update(sizeof(buffer)-1, 'z');
    TEST_ASSERT_EQUAL('z', buffer[sizeof(buffer)-1]);
}

void run_eeprom_tests()
{
    unity_filename_helper_t _ufname_helper(__FILE__);

    RUN_TEST(test_length);
    RUN_TEST(test_read);
    RUN_TEST(test_write);
    RUN_TEST(test_update);
}
