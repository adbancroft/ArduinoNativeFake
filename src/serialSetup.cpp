#include <USBAPI.h>
#include <map>

namespace ArduinoNativeFake
{
    std::map<Serial_*, unsigned long> baudRates;
}