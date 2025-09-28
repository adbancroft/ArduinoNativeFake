#include <ArduinoFake.h>

void setupWriteMethod(fakeit::Mock<PrintFake> &mock, std::ostream &outputStream);
void setupPrintMethod(fakeit::Mock<PrintFake> &mock, std::ostream &outputStream);
void setupPrintlnMethod(fakeit::Mock<PrintFake> &mock, std::ostream &outputStream);

static inline void setupPrintObject(fakeit::Mock<PrintFake> &mock, std::ostream &outputStream) {
    setupWriteMethod(mock, outputStream);
    setupPrintMethod(mock, outputStream);
    setupPrintlnMethod(mock, outputStream);
}