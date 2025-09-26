#include <ArduinoFake.h>
#include <iterator>

 /**
  * @brief Setup an the EEPROM mock with a memory backing store
  * 
  * @tparam Iterator (usually auto-deduced)
  * @param mock The EEPROM mock object 
  * @param begin Iterator to the start of the address space (address 0)
  * @param end Iterator to the end of the address space
  */
template<class Iterator>
static inline void setupEEPROMObject(   fakeit::Mock<EEPROMFake> &mock, 
                                        Iterator begin, 
                                        Iterator end) {
    static_assert(
        std::is_same<std::random_access_iterator_tag,
                        typename std::iterator_traits<Iterator>::iterator_category>::value,
        "The setupEEPROMObject() function only accepts random access iterators\n");

    using namespace fakeit;

    When(Method(mock, read)).AlwaysDo(
            [begin](int address) -> uint8_t { return *(begin + address); }
    );
    When(Method(mock, write)).AlwaysDo(
            [begin](int address, uint8_t value) { *(begin + address) = value; }
    );
    When(Method(mock, update)).AlwaysDo(
            [begin](int address, uint8_t value) {
            if (*(begin + address) != value) {
                *(begin + address) = value;
            }
        }
    );
    When(Method(mock, length)).AlwaysDo(
            [begin, end]() -> uint16_t { return end - begin; }
    );
}