#include <map>
#include "serial.h"

using namespace fakeit;

void setupSerialObject(fakeit::Mock<SerialFake> &mock, std::iostream &stream)
{
    static std::map<SerialFake*, unsigned long> baudRates;
    When(OverloadedMethod(mock, begin, void(unsigned long))).AlwaysDo([&mock](unsigned long rate){
        baudRates[&mock.get()] = rate;
    });
    When(OverloadedMethod(mock, begin, void(unsigned long, uint8_t))).AlwaysDo([&mock](unsigned long rate, uint8_t config){
        baudRates[&mock.get()] = rate;
    });

    When(Method(mock, end)).AlwaysDo([&mock]() {
        mock.get().flush();
    });

    When(Method(mock, availableForWrite)).AlwaysDo([&stream]() {
        std::iostream::pos_type original = stream.tellp();
        stream.seekp(0, std::ios::end);
        std::iostream::pos_type available = stream.tellp();
        stream.seekp(original);
        return (int)available;
    });
    
    When(Method(mock, available)).AlwaysDo([&stream]() {
        std::iostream::pos_type original = stream.tellg();
        stream.seekg(0, std::ios::end);
        std::iostream::pos_type available = stream.tellg();
        stream.seekg(original);
        return (int)available;
    });
    When(Method(mock, peek)).AlwaysDo([&stream]() {
        return (int)stream.peek();
    });
    When(Method(mock, read)).AlwaysDo([&stream]() {
        return (int)stream.get();
    });
    When(Method(mock, flush)).AlwaysDo([&stream]() {
        stream.flush();
    });
    When(OverloadedMethod(mock, write, size_t(uint8_t))).AlwaysDo([&stream](uint8_t c){
        stream.put(c);
        return stream.bad() ? 0U : 1U;
    });
    When(OverloadedMethod(mock, write, size_t(const uint8_t*, size_t))).AlwaysDo([&stream](const uint8_t* buffer, size_t length){
        stream.write((const char *)buffer, length);
        return stream.bad() ? 0U : length;
    });

    When(Method(mock, readBreak)).AlwaysReturn(0U);

    When(Method(mock, baud)).AlwaysDo([&mock](void){
        return baudRates[&mock.get()];
    });
    When(Method(mock, stopbits)).AlwaysReturn(Serial_::ONE_STOP_BIT);
    When(Method(mock, paritytype)).AlwaysReturn(Serial_::EVEN_PARITY);
    When(Method(mock, numbits)).AlwaysReturn(8);

    When(Method(mock, dtr)).AlwaysReturn(true);
    When(Method(mock, rts)).AlwaysReturn(true);
}