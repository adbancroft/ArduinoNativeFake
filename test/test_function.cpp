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
    analogReference(11);
    Verify(Method(ArduinoFake(Function), analogReference)).AtLeastOnce();
    analogReadResolution(11);
    Verify(Method(ArduinoFake(Function), analogReadResolution)).AtLeastOnce();
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
    TEST_ASSERT_EQUAL(0U, *portOutputRegister(pin));
    TEST_ASSERT_EQUAL(portOutputRegister(pin), portOutputRegister(pin));

    TEST_ASSERT_NOT_EQUAL(nullptr, portInputRegister(pin));
    TEST_ASSERT_EQUAL(0U, *portInputRegister(pin));
    TEST_ASSERT_EQUAL(portInputRegister(pin), portInputRegister(pin));

    TEST_ASSERT_NOT_EQUAL(nullptr, portModeRegister(pin));
    TEST_ASSERT_EQUAL(0U, *portModeRegister(pin));
    TEST_ASSERT_EQUAL(portModeRegister(pin), portModeRegister(pin));
}

static void test_pin_readwrite(void)
{
    setupNativeFake(ArduinoFake(Function));

    TEST_ASSERT_EQUAL(LOW, digitalRead(3));
    Verify(Method(ArduinoFake(Function), digitalRead)).AtLeastOnce();
    TEST_ASSERT_EQUAL(LOW, analogRead(3));
    Verify(Method(ArduinoFake(Function), analogRead)).AtLeastOnce();

    // Read returns last write
    digitalWrite(7, 99);
    Verify(Method(ArduinoFake(Function), digitalWrite)).AtLeastOnce();
    TEST_ASSERT_EQUAL(99, digitalRead(7));
    analogWrite(11, 123);
    Verify(Method(ArduinoFake(Function), analogWrite)).AtLeastOnce();
    TEST_ASSERT_EQUAL(123, analogRead(11));
}

#define UNKNOWN_PIN 0xFF

#if !defined(NOT_A_PIN)
#define NOT_A_PIN 0
#endif

static uint8_t getPinMode(uint8_t pin)
{
  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);

  // I don't see an option for mega to return this, but whatever...
  if (NOT_A_PIN == port) return UNKNOWN_PIN;
  // Is there a bit we can check?
  if (0 == bit) return UNKNOWN_PIN;

  // Is there only a single bit set?
  if (bit & (bit - 1)) return UNKNOWN_PIN;

  volatile uint8_t *reg, *out;
  reg = portModeRegister(port);
  out = portOutputRegister(port);

  if (*reg & bit)
    return OUTPUT;
  else if (*out & bit)
    return INPUT_PULLUP;
  else
    return INPUT;
}

static void test_pinMode(void)
{
    setupNativeFake(ArduinoFake(Function));
    static constexpr uint8_t PA = 1;
    static constexpr uint8_t PB = 2;
    static constexpr uint8_t PC = 3;
    static constexpr uint8_t PD = 4;
    static constexpr uint8_t PE = 5;
    static constexpr uint8_t PF = 6;
    static constexpr uint8_t PG = 7;
    static constexpr uint8_t PH = 8;
    static constexpr uint8_t PJ = 10;
    static constexpr uint8_t PK = 11;
    static constexpr uint8_t PL = 12;

    static constexpr uint8_t digital_pin_to_port[] = {
        // PORTLIST		
        // -------------------------------------------		
        PE	, // PE 0 ** 0 ** USART0_RX	
        PE	, // PE 1 ** 1 ** USART0_TX	
        PE	, // PE 4 ** 2 ** PWM2	
        PE	, // PE 5 ** 3 ** PWM3	
        PG	, // PG 5 ** 4 ** PWM4	
        PE	, // PE 3 ** 5 ** PWM5	
        PH	, // PH 3 ** 6 ** PWM6	
        PH	, // PH 4 ** 7 ** PWM7	
        PH	, // PH 5 ** 8 ** PWM8	
        PH	, // PH 6 ** 9 ** PWM9	
        PB	, // PB 4 ** 10 ** PWM10	
        PB	, // PB 5 ** 11 ** PWM11	
        PB	, // PB 6 ** 12 ** PWM12	
        PB	, // PB 7 ** 13 ** PWM13	
        PJ	, // PJ 1 ** 14 ** USART3_TX	
        PJ	, // PJ 0 ** 15 ** USART3_RX	
        PH	, // PH 1 ** 16 ** USART2_TX	
        PH	, // PH 0 ** 17 ** USART2_RX	
        PD	, // PD 3 ** 18 ** USART1_TX	
        PD	, // PD 2 ** 19 ** USART1_RX	
        PD	, // PD 1 ** 20 ** I2C_SDA	
        PD	, // PD 0 ** 21 ** I2C_SCL	
        PA	, // PA 0 ** 22 ** D22	
        PA	, // PA 1 ** 23 ** D23	
        PA	, // PA 2 ** 24 ** D24	
        PA	, // PA 3 ** 25 ** D25	
        PA	, // PA 4 ** 26 ** D26	
        PA	, // PA 5 ** 27 ** D27	
        PA	, // PA 6 ** 28 ** D28	
        PA	, // PA 7 ** 29 ** D29	
        PC	, // PC 7 ** 30 ** D30	
        PC	, // PC 6 ** 31 ** D31	
        PC	, // PC 5 ** 32 ** D32	
        PC	, // PC 4 ** 33 ** D33	
        PC	, // PC 3 ** 34 ** D34	
        PC	, // PC 2 ** 35 ** D35	
        PC	, // PC 1 ** 36 ** D36	
        PC	, // PC 0 ** 37 ** D37	
        PD	, // PD 7 ** 38 ** D38	
        PG	, // PG 2 ** 39 ** D39	
        PG	, // PG 1 ** 40 ** D40	
        PG	, // PG 0 ** 41 ** D41	
        PL	, // PL 7 ** 42 ** D42	
        PL	, // PL 6 ** 43 ** D43	
        PL	, // PL 5 ** 44 ** D44	
        PL	, // PL 4 ** 45 ** D45	
        PL	, // PL 3 ** 46 ** D46	
        PL	, // PL 2 ** 47 ** D47	
        PL	, // PL 1 ** 48 ** D48	
        PL	, // PL 0 ** 49 ** D49	
        PB	, // PB 3 ** 50 ** SPI_MISO	
        PB	, // PB 2 ** 51 ** SPI_MOSI	
        PB	, // PB 1 ** 52 ** SPI_SCK	
        PB	, // PB 0 ** 53 ** SPI_SS	
        PF	, // PF 0 ** 54 ** A0	
        PF	, // PF 1 ** 55 ** A1	
        PF	, // PF 2 ** 56 ** A2	
        PF	, // PF 3 ** 57 ** A3	
        PF	, // PF 4 ** 58 ** A4	
        PF	, // PF 5 ** 59 ** A5	
        PF	, // PF 6 ** 60 ** A6	
        PF	, // PF 7 ** 61 ** A7	
        PK	, // PK 0 ** 62 ** A8	
        PK	, // PK 1 ** 63 ** A9	
        PK	, // PK 2 ** 64 ** A10	
        PK	, // PK 3 ** 65 ** A11	
        PK	, // PK 4 ** 66 ** A12	
        PK	, // PK 5 ** 67 ** A13	
        PK	, // PK 6 ** 68 ** A14	
        PK	, // PK 7 ** 69 ** A15	
    };

    #define _BV(bit) (1 << (bit))
    static constexpr uint8_t digital_pin_to_bit_mask[] = {
        // PIN IN PORT		
        // -------------------------------------------		
        _BV( 0 )	, // PE 0 ** 0 ** USART0_RX	
        _BV( 1 )	, // PE 1 ** 1 ** USART0_TX	
        _BV( 4 )	, // PE 4 ** 2 ** PWM2	
        _BV( 5 )	, // PE 5 ** 3 ** PWM3	
        _BV( 5 )	, // PG 5 ** 4 ** PWM4	
        _BV( 3 )	, // PE 3 ** 5 ** PWM5	
        _BV( 3 )	, // PH 3 ** 6 ** PWM6	
        _BV( 4 )	, // PH 4 ** 7 ** PWM7	
        _BV( 5 )	, // PH 5 ** 8 ** PWM8	
        _BV( 6 )	, // PH 6 ** 9 ** PWM9	
        _BV( 4 )	, // PB 4 ** 10 ** PWM10	
        _BV( 5 )	, // PB 5 ** 11 ** PWM11	
        _BV( 6 )	, // PB 6 ** 12 ** PWM12	
        _BV( 7 )	, // PB 7 ** 13 ** PWM13	
        _BV( 1 )	, // PJ 1 ** 14 ** USART3_TX	
        _BV( 0 )	, // PJ 0 ** 15 ** USART3_RX	
        _BV( 1 )	, // PH 1 ** 16 ** USART2_TX	
        _BV( 0 )	, // PH 0 ** 17 ** USART2_RX	
        _BV( 3 )	, // PD 3 ** 18 ** USART1_TX	
        _BV( 2 )	, // PD 2 ** 19 ** USART1_RX	
        _BV( 1 )	, // PD 1 ** 20 ** I2C_SDA	
        _BV( 0 )	, // PD 0 ** 21 ** I2C_SCL	
        _BV( 0 )	, // PA 0 ** 22 ** D22	
        _BV( 1 )	, // PA 1 ** 23 ** D23	
        _BV( 2 )	, // PA 2 ** 24 ** D24	
        _BV( 3 )	, // PA 3 ** 25 ** D25	
        _BV( 4 )	, // PA 4 ** 26 ** D26	
        _BV( 5 )	, // PA 5 ** 27 ** D27	
        _BV( 6 )	, // PA 6 ** 28 ** D28	
        _BV( 7 )	, // PA 7 ** 29 ** D29	
        _BV( 7 )	, // PC 7 ** 30 ** D30	
        _BV( 6 )	, // PC 6 ** 31 ** D31	
        _BV( 5 )	, // PC 5 ** 32 ** D32	
        _BV( 4 )	, // PC 4 ** 33 ** D33	
        _BV( 3 )	, // PC 3 ** 34 ** D34	
        _BV( 2 )	, // PC 2 ** 35 ** D35	
        _BV( 1 )	, // PC 1 ** 36 ** D36	
        _BV( 0 )	, // PC 0 ** 37 ** D37	
        _BV( 7 )	, // PD 7 ** 38 ** D38	
        _BV( 2 )	, // PG 2 ** 39 ** D39	
        _BV( 1 )	, // PG 1 ** 40 ** D40	
        _BV( 0 )	, // PG 0 ** 41 ** D41	
        _BV( 7 )	, // PL 7 ** 42 ** D42	
        _BV( 6 )	, // PL 6 ** 43 ** D43	
        _BV( 5 )	, // PL 5 ** 44 ** D44	
        _BV( 4 )	, // PL 4 ** 45 ** D45	
        _BV( 3 )	, // PL 3 ** 46 ** D46	
        _BV( 2 )	, // PL 2 ** 47 ** D47	
        _BV( 1 )	, // PL 1 ** 48 ** D48	
        _BV( 0 )	, // PL 0 ** 49 ** D49	
        _BV( 3 )	, // PB 3 ** 50 ** SPI_MISO	
        _BV( 2 )	, // PB 2 ** 51 ** SPI_MOSI	
        _BV( 1 )	, // PB 1 ** 52 ** SPI_SCK	
        _BV( 0 )	, // PB 0 ** 53 ** SPI_SS	
        _BV( 0 )	, // PF 0 ** 54 ** A0	
        _BV( 1 )	, // PF 1 ** 55 ** A1	
        _BV( 2 )	, // PF 2 ** 56 ** A2	
        _BV( 3 )	, // PF 3 ** 57 ** A3	
        _BV( 4 )	, // PF 4 ** 58 ** A4	
        _BV( 5 )	, // PF 5 ** 59 ** A5	
        _BV( 6 )	, // PF 6 ** 60 ** A6	
        _BV( 7 )	, // PF 7 ** 61 ** A7	
        _BV( 0 )	, // PK 0 ** 62 ** A8	
        _BV( 1 )	, // PK 1 ** 63 ** A9	
        _BV( 2 )	, // PK 2 ** 64 ** A10	
        _BV( 3 )	, // PK 3 ** 65 ** A11	
        _BV( 4 )	, // PK 4 ** 66 ** A12	
        _BV( 5 )	, // PK 5 ** 67 ** A13	
        _BV( 6 )	, // PK 6 ** 68 ** A14	
        _BV( 7 )	, // PK 7 ** 69 ** A15	
    };

    When(Method(ArduinoFake(Function), digitalPinToPort)).AlwaysDo([](uint8_t pin) -> uint8_t { return digital_pin_to_port[pin]; });
    When(Method(ArduinoFake(Function), digitalPinToBitMask)).AlwaysDo([](uint8_t pin) -> uint8_t { return digital_pin_to_bit_mask[pin]; });

    pinMode(8, INPUT_PULLUP);
    Verify(Method(ArduinoFake(Function), pinMode)).AtLeastOnce();
    TEST_ASSERT_EQUAL(INPUT_PULLUP, getPinMode(8));

    pinMode(11, INPUT);
    TEST_ASSERT_EQUAL(INPUT, getPinMode(11));

    pinMode(11, OUTPUT);
    TEST_ASSERT_EQUAL(OUTPUT, getPinMode(11));
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
    RUN_TEST(test_pin_readwrite);
    RUN_TEST(test_pinMode);
}
