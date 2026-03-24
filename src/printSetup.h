#include <SimpleArduinoFake.h>

namespace ArduinoNativeFake
{

namespace PrintDetail
{
void setupWriteMethod(fakeit::Mock<Print> &mock, std::ostream &outputStream);
void setupPrintMethod(fakeit::Mock<Print> &mock, std::ostream &outputStream);
void setupPrintlnMethod(fakeit::Mock<Print> &mock, std::ostream &outputStream);
}

static inline void setupNativeFake(fakeit::Mock<Print> &mock, std::ostream &outputStream) {
    PrintDetail::setupWriteMethod(mock, outputStream);
    PrintDetail::setupPrintMethod(mock, outputStream);
    PrintDetail::setupPrintlnMethod(mock, outputStream);
}
}