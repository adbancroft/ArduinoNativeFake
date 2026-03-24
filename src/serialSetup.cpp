#include <map>
#include "serialSetup.h"

using namespace fakeit;

void setupNativeFake(fakeit::Mock<Serial_> &mock, std::ostream &oStream, std::istream &iStream)
{
    static std::map<Serial_*, unsigned long> baudRates;
    When(OverloadedMethod(mock, begin, void(unsigned long))).AlwaysDo([&mock](unsigned long rate){
        baudRates[&mock.get()] = rate;
    });
    When(OverloadedMethod(mock, begin, void(unsigned long, uint8_t))).AlwaysDo([&mock](unsigned long rate, uint8_t config){
        baudRates[&mock.get()] = rate;
    });

    When(Method(mock, end)).AlwaysDo([&mock]() {
        mock.get().flush();
    });

    When(Method(mock, availableForWrite)).AlwaysDo([&oStream]() {
        std::ostream::pos_type original = oStream.tellp();
        oStream.seekp(0, std::ios::end);
        std::ostream::pos_type available = oStream.tellp();
        oStream.seekp(original);
        return (int)available;
    });
    
    When(Method(mock, available)).AlwaysDo([&iStream]() {
        std::istream::pos_type original = iStream.tellg();
        iStream.seekg(0, std::ios::end);
        std::istream::pos_type available = iStream.tellg();
        iStream.seekg(original);
        return (int)available;
    });
    When(Method(mock, peek)).AlwaysDo([&iStream]() {
        return (int)iStream.peek();
    });
    When(Method(mock, read)).AlwaysDo([&iStream]() {
        return (int)iStream.get();
    });
    When(Method(mock, flush)).AlwaysDo([&oStream]() {
        oStream.flush();
    });
    When(OverloadedMethod(mock, write, size_t(uint8_t))).AlwaysDo([&oStream](uint8_t c){
        oStream.put(c);
        return oStream.bad() ? 0U : 1U;
    });
    When(OverloadedMethod(mock, write, size_t(const uint8_t*, size_t))).AlwaysDo([&oStream](const uint8_t* buffer, size_t length){
        oStream.write((const char *)buffer, length);
        return oStream.bad() ? 0U : length;
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