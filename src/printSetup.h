#include <SimpleArduinoFake.h>

void setupWriteMethod(fakeit::Mock<Print> &mock, std::ostream &outputStream);
void setupPrintMethod(fakeit::Mock<Print> &mock, std::ostream &outputStream);
void setupPrintlnMethod(fakeit::Mock<Print> &mock, std::ostream &outputStream);

static inline void setupNativeFake(fakeit::Mock<Print> &mock, std::ostream &outputStream) {
    setupWriteMethod(mock, outputStream);
    setupPrintMethod(mock, outputStream);
    setupPrintlnMethod(mock, outputStream);
}