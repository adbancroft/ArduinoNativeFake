#pragma once
#include <type_traits>
#include "printSetupDetail.hpp"

namespace ArduinoNativeFake
{
template <class TFake>
static inline void setupPrintFake(fakeit::Mock<TFake> &mock, std::ostream &outputStream) {
    static_assert(std::is_base_of<Print, TFake>::value, "Needs to be a class derived from Print");
    PrintDetail::setupWriteMethod(mock, outputStream);
    PrintDetail::setupPrintMethod(mock, outputStream);
    PrintDetail::setupPrintlnMethod(mock, outputStream);
}
}