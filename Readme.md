# Arduino Native Fake

[![Unit Tests](https://github.com/adbancroft/ArduinoNativeFake/actions/workflows/unit-tests.yml/badge.svg)](https://github.com/adbancroft/ArduinoNativeFake/actions/workflows/unit-tests.yml)

## What is this?

It's an extension to [ArduinoFake](https://github.com/FabioBatSilva/ArduinoFake) that mocks out enough functionality to run the [Speeduino](https://github.com/speeduino/speeduino) unit tests using the [Platform IO 'native' platform](https://docs.platformio.org/en/latest/platforms/native.html).

It mocks all methods of
* Print
* Stream
* Serial
* Function
* EEPROM

It is missing:
* Wire
* Client
* SPI

## Rationale (Why?)
When FakeIt attempts to execute an unmocked method, the program fails with a cryptic error. So it was easier to fully mock each object rather than trial and error on each method for the Speeduino unit test suite.

It might be useful to others, hence a separate project.