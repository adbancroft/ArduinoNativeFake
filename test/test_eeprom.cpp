#include <SimpleArduinoFake.h>
#include <unity.h>
#include <array>
#include "unity_filename_helper.h"
#include "eepromSetup.h"

static void test_length(void)
{
    EEPROMClass* eeprom(SimpleArduinoFake::getContext()._EEPROM.getFake());

    {
        std::array<unsigned char, 0> buffer;
        setupNativeFake(SimpleArduinoFake::getContext()._EEPROM, buffer.begin(), buffer.end());
        TEST_ASSERT_EQUAL(0, eeprom->length());
    }

    {
        std::array<unsigned char, 64> buffer;
        setupNativeFake(SimpleArduinoFake::getContext()._EEPROM, buffer.begin(), buffer.end());
        TEST_ASSERT_EQUAL(64, eeprom->length());
    }
}

static void test_read(void)
{
    EEPROMClass* eeprom(SimpleArduinoFake::getContext()._EEPROM.getFake());

    std::array<unsigned char, 5> buffer = {'t', 'e', 's', 't', '1'};
    setupNativeFake(SimpleArduinoFake::getContext()._EEPROM, buffer.begin(), buffer.end());

    TEST_ASSERT_EQUAL('t', eeprom->read(0U));
    TEST_ASSERT_EQUAL('e', eeprom->read(1U));
    TEST_ASSERT_EQUAL('s', eeprom->read(2U));
    TEST_ASSERT_EQUAL('t', eeprom->read(3U));
    TEST_ASSERT_EQUAL('1', eeprom->read(4U));
}
    
static void test_write(void)
{
    EEPROMClass* eeprom(SimpleArduinoFake::getContext()._EEPROM.getFake());

    std::array<unsigned char, 256> buffer;
    setupNativeFake(SimpleArduinoFake::getContext()._EEPROM, buffer.begin(), buffer.end());

    eeprom->write(121, 'x');
    TEST_ASSERT_EQUAL('x', buffer[121]);

    eeprom->write(0, 'y');
    TEST_ASSERT_EQUAL('y', buffer[0]);

    eeprom->write(sizeof(buffer)-1, 'z');
    TEST_ASSERT_EQUAL('z', buffer[sizeof(buffer)-1]);
}

static void test_update(void)
{
    EEPROMClass* eeprom(SimpleArduinoFake::getContext()._EEPROM.getFake());

    std::array<unsigned char, 256> buffer;
    setupNativeFake(SimpleArduinoFake::getContext()._EEPROM, buffer.begin(), buffer.end());

    eeprom->update(121, 'x');
    TEST_ASSERT_EQUAL('x', buffer[121]);
    eeprom->update(121, 'x');
    TEST_ASSERT_EQUAL('x', buffer[121]);

    eeprom->update(0, 'y');
    TEST_ASSERT_EQUAL('y', buffer[0]);

    eeprom->update(sizeof(buffer)-1, 'z');
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
