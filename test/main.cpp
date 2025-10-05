// clang-format off
#include <ArduinoFake.h>
#include <unity.h>

void setUp(void)
{
    ArduinoFakeReset();
}

void tearDown(void) // Not called, but needed for unity
{
    // clean stuff up here
}

int main(int argc, char **argv)
{
    extern void run_print_tests(void);
    extern void run_eeprom_tests(void);
    extern void run_function_tests(void);
    extern void run_stream_tests(void);
    extern void run_serial_tests(void);

    UNITY_BEGIN();

    run_print_tests();
    run_eeprom_tests();
    run_function_tests();
    run_stream_tests();
    run_serial_tests();
    
    return UNITY_END();
}