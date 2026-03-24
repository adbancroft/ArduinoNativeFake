#include <bitset>
#include <iomanip>
#include "printSetup.h"

namespace ArduinoNativeFake
{

namespace PrintDetail
{

using namespace fakeit;

void setupWriteMethod(Mock<Print> &mock, std::ostream &outputStream) {
    When(Method(mock, availableForWrite)).AlwaysReturn(0);

    When(OverloadedMethod(mock, write, size_t(uint8_t))).AlwaysDo([&outputStream](uint8_t c) {
        outputStream.put((char)c);
        return 1;
    });

    When(OverloadedMethod(mock, write, size_t(const uint8_t *, size_t))).AlwaysDo([&mock](const uint8_t *buffer, size_t size) {
        size_t n = 0;
        while (size--) {
            
            if (mock.get().write(*buffer++)) n++;
            else break;
        }
        return n;
    });
}


template<typename T>
std::string toString(const T& value, int base) {
    if (base == HEX) {
        std::ostringstream oss;
        oss << std::uppercase << std::hex << value;
        return oss.str();
    }
    else if (base == OCT) {
        std::ostringstream oss;
        oss << std::oct << value;
        return oss.str();
    }
    else if (base == BIN) {
        std::bitset<sizeof(value) * 8U> binaryRepresentation(value);
        return binaryRepresentation.to_string();
    } else { // DEC
        std::ostringstream oss;
        oss << std::dec << value;
        return oss.str();
    }
}

void setupPrintMethod(Mock<Print> &mock, std::ostream &outputStream) {
    When(OverloadedMethod(mock, print, size_t(const __FlashStringHelper *))).AlwaysDo([&mock](const __FlashStringHelper *ifsh) {
        return mock.get().print(reinterpret_cast<const char *>(ifsh));
    });
    When(OverloadedMethod(mock, print, size_t(const String &))).AlwaysDo([&mock](const String &str) {
        return mock.get().print(str.c_str());
    });
    When(OverloadedMethod(mock, print, size_t(const char[]))).AlwaysDo([&outputStream](const char *str) {
        outputStream << str;
        return strlen(str);
    });
    When(OverloadedMethod(mock, print, size_t(char))).AlwaysDo([&mock](char c) {
        return mock.get().write(c);
    });
    When(OverloadedMethod(mock, print, size_t(unsigned char, int))).AlwaysDo([&mock](unsigned char c, int base) {
        return mock.get().print((unsigned long)c, base);
    });
    When(OverloadedMethod(mock, print, size_t(int, int))).AlwaysDo([&mock](int n, int base) {
        return mock.get().print((long)n, base);
    });
    When(OverloadedMethod(mock, print, size_t(unsigned int, int))).AlwaysDo([&mock](unsigned int n, int base) {
        return mock.get().print((unsigned long)n, base);
    });
    When(OverloadedMethod(mock, print, size_t(long, int))).AlwaysDo([&outputStream](long n, int base) {
        std::string str = toString(n, base);
        outputStream << str;
        return str.length();
    });
    When(OverloadedMethod(mock, print, size_t(unsigned long, int))).AlwaysDo([&outputStream](unsigned long n, int base) {
        std::string str = toString(n, base);
        outputStream << str;
        return str.length();
    });
    When(OverloadedMethod(mock, print, size_t(double, int))).AlwaysDo([&outputStream](double n, int precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << n;
        std::string str = oss.str();
        outputStream << str;
        return str.length();
    });
    When(OverloadedMethod(mock, print, size_t(const Printable&))).AlwaysDo([&mock](const Printable& x){
        return x.printTo(*SimpleArduinoFake::getContext()._Print.getFake());
    });
}

void setupPrintlnMethod(Mock<Print> &mock, std::ostream &outputStream) {
    When(OverloadedMethod(mock, println, size_t(const __FlashStringHelper *))).AlwaysDo([&mock, &outputStream](const __FlashStringHelper *ifsh){
        return mock.get().print(ifsh) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(const String &s))).AlwaysDo([&mock, &outputStream](const String &s){
        return mock.get().print(s) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(const char*))).AlwaysDo([&mock, &outputStream](const char *s){
        return mock.get().print(s) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(char))).AlwaysDo([&mock, &outputStream](char c){
        return mock.get().print(c) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(unsigned char, int))).AlwaysDo([&mock, &outputStream](unsigned char c, int base){
        return mock.get().print(c, base) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(int, int))).AlwaysDo([&mock, &outputStream](int n, int base){
        return mock.get().print(n, base) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(unsigned int, int))).AlwaysDo([&mock, &outputStream](unsigned int n, int base){
        return mock.get().print(n, base) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(long, int))).AlwaysDo([&mock, &outputStream](long n, int base){
        return mock.get().print(n, base) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(unsigned long, int))).AlwaysDo([&mock, &outputStream](unsigned long n, int base){
        return mock.get().print(n, base) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(double, int))).AlwaysDo([&mock, &outputStream](double d, int decimals){
        return mock.get().print(d, decimals) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(const Printable&))).AlwaysDo([&mock, &outputStream](const Printable &x){
        return mock.get().print(x) + mock.get().println();
    });
    When(OverloadedMethod(mock, println, size_t(void))).AlwaysDo([&mock, &outputStream](void){
        outputStream << '\n';
        return 1U;
    });
}
}
}