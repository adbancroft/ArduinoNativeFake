#include <chrono>
#include <thread>
#include <random>
#include <map>
#include "functionSetup.h"

using namespace fakeit;

void setupNativeFake(fakeit::Mock<SimpleArduinoFake::details::FunctionFake> &mock)
{
    // Do nothing - stub to prevent crashes
    When(Method(mock, init)).AlwaysReturn();

    // Mimic the AVR core.
    When(Method(mock, pinMode)).AlwaysDo([](uint8_t pin, uint8_t mode){
    	uint8_t port = digitalPinToPort(pin);
    	if (port == NOT_A_PIN) return;

        volatile uint8_t *reg = portModeRegister(port);
        volatile uint8_t *out = portOutputRegister(port);

        uint8_t bit = digitalPinToBitMask(pin);
        if (mode == INPUT) { 
            *reg &= ~bit;
            *out &= ~bit;
        } else if (mode == INPUT_PULLUP) {
            *reg &= ~bit;
            *out |= bit;
        } else {
            *reg |= bit;
        }
    });

    // Should be overridden per test - but stub to prevent crashes
    // Just return the last value set.
    using map_uint8_uint8_t = std::map<SimpleArduinoFake::details::FunctionFake*, std::map<uint8_t, uint8_t>>;
    static map_uint8_uint8_t pinValues;
    When(Method(mock, digitalWrite)).AlwaysDo([&mock](uint8_t pin, uint8_t value) {
        pinValues[&mock.get()][pin] = value;
    });
    When(Method(mock, digitalRead)).AlwaysDo([&mock](uint8_t pin){
        auto result = pinValues.insert(std::make_pair(&mock.get(), map_uint8_uint8_t::mapped_type()));
        if (result.first->second.find(pin)==result.first->second.end()) {
            return LOW;
        }
        return (int)result.first->second[pin];
    });
    When(Method(mock, analogWrite)).AlwaysDo([](uint8_t pin, uint8_t value) {
        digitalWrite(pin, value);
    });
    When(Method(mock, analogRead)).AlwaysDo([](uint8_t pin){
        return digitalRead(pin);
    });

    // Do nothing - stub to prevent crashes
    When(Method(mock, analogReference)).AlwaysReturn();
    When(Method(mock, analogReadResolution)).AlwaysReturn();

    When(Method(mock, millis)).AlwaysDo([]() {
        return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count(); 
    });
    When(Method(mock, micros)).AlwaysDo([]() {
        return std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count(); 
    });

    When(Method(mock, delay)).AlwaysDo([](unsigned long ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    });
    When(Method(mock, delayMicroseconds)).AlwaysDo([](unsigned long us) {
        std::this_thread::sleep_for(std::chrono::milliseconds(us));
    });

    // Do nothing - stub to prevent crashes
    When(Method(mock, pulseIn)).AlwaysReturn();
    When(Method(mock, pulseInLong)).AlwaysReturn();

    When(Method(mock, shiftOut)).AlwaysDo([](uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
        for (int i = 0; i < 8; i++)  {
            if (bitOrder == LSBFIRST) {
                digitalWrite(dataPin, val & 1);
                val >>= 1;
            } else {	
                digitalWrite(dataPin, (val & 128) != 0);
                val <<= 1;
            }
                
            digitalWrite(clockPin, HIGH);
            digitalWrite(clockPin, LOW);		
        }    
    });
    When(Method(mock, shiftIn)).AlwaysDo([](uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
        uint8_t value = 0;
        for (int i = 0; i < 8; ++i) {
            digitalWrite(clockPin, HIGH);
            if (bitOrder == LSBFIRST)
                value |= digitalRead(dataPin) << i;
            else
                value |= digitalRead(dataPin) << (7 - i);
            digitalWrite(clockPin, LOW);
        }
        return value;
    });

    // Do nothing - stub to prevent crashes
    When(Method(mock, detachInterrupt)).AlwaysReturn();
    When(Method(mock, attachInterrupt)).AlwaysReturn();
    When(Method(mock, cli)).AlwaysReturn();
    When(Method(mock, sei)).AlwaysReturn();

    When(Method(mock, tone)).AlwaysReturn();
    When(Method(mock, noTone)).AlwaysReturn();

    static std::map<SimpleArduinoFake::details::FunctionFake*, std::mt19937> mockToRandom;
    When(OverloadedMethod(mock, random, long(long))).AlwaysDo([](long howBig) {
        return random(0, howBig);
    });
    When(OverloadedMethod(mock, random, long(long, long))).AlwaysDo([&mock](long howSmall, long howBig) {
        if (mockToRandom.find(&mock.get())==mockToRandom.end())
        {
            std::random_device rd;
            mockToRandom[&mock.get()] = std::mt19937(rd());
        }
        std::uniform_int_distribution<long> distrib(howSmall, howBig);
        return distrib(mockToRandom[&mock.get()]);
    });
    When(Method(mock, randomSeed)).AlwaysDo([&mock](long newSeed){
        mockToRandom[&mock.get()] = std::mt19937(newSeed);
    });

    When(Method(mock, map)).AlwaysDo([](long x, long in_min, long in_max, long out_min, long out_max){
        // Avoid division by zero error, since that's what AVR does
        long outRange = (in_max - in_min); 
        if (outRange==0L) {
            outRange = 1L;
        }
        return (x - in_min) * (out_max - out_min) / outRange + out_min;
    });

    When(Method(mock, yield)).AlwaysReturn();

    When(Method(mock, digitalPinToPort)).AlwaysDo([](uint8_t pin) -> uint8_t { return pin; });
    When(Method(mock, digitalPinToBitMask)).AlwaysDo([](uint8_t pin) -> uint8_t { return pin; });
    When(Method(mock, digitalPinToTimer)).AlwaysDo([](uint8_t pin) -> uint8_t { return pin; });

    // These functions are idempotent, so we have to deal with lifetime issues since returning pointers
    static map_uint8_uint8_t mockToOutputRegister;
    When(Method(mock, portOutputRegister)).AlwaysDo([&mock](uint8_t pin) { 
        auto result = mockToOutputRegister.insert(std::make_pair(&mock.get(), map_uint8_uint8_t::mapped_type()));
        if (result.first->second.find(pin)==result.first->second.end())
        {
            result.first->second[pin] = 0U;
        }
        return &result.first->second[pin]; 
    });
    static map_uint8_uint8_t mockToInputRegister;
    When(Method(mock, portInputRegister)).AlwaysDo([&mock](uint8_t pin) { 
        auto result = mockToInputRegister.insert(std::make_pair(&mock.get(), map_uint8_uint8_t::mapped_type()));
        if (result.first->second.find(pin)==result.first->second.end())
        {
            result.first->second[pin] = 0U;
        }
        return &result.first->second[pin]; 
    });
    static map_uint8_uint8_t mockToModeRegister;
    When(Method(mock, portModeRegister)).AlwaysDo([&mock](uint8_t pin) { 
        auto result = mockToModeRegister.insert(std::make_pair(&mock.get(), map_uint8_uint8_t::mapped_type()));
        if (result.first->second.find(pin)==result.first->second.end())
        {
            result.first->second[pin] = 0U;
        }
        return &result.first->second[pin]; 
    });
}