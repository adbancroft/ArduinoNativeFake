#include <SimpleArduinoFake.h>

namespace ArduinoNativeFake
{

void setupNativeFake(fakeit::Mock<Serial_> &mock, std::ostream &oStream, std::istream &iStream);

}