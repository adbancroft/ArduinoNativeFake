#pragma once
#include <map>
#include <type_traits>
#include "streamSetupDetail.hpp"

namespace ArduinoNativeFake
{
    
template <class TFake>
static inline void setupStreamFake(fakeit::Mock<TFake> &mock, std::istream &stream)
{
    static_assert(std::is_base_of<Stream, TFake>::value, "Needs to be a class derived from Stream");

    // These are pure virtual in Stream, so don't implement here (they'll be implemented in fakes derived from Stream)
    // virtual int available() = 0;
    // virtual int read() = 0;
    // virtual int peek() = 0;

    using namespace fakeit;

    // This can't be declared as a static variable here, since this is a template.
    extern std::map<Stream*, unsigned long> timeOuts;
    When(Method(mock, setTimeout)).AlwaysDo([&mock](unsigned long timeOut) {
        timeOuts[&mock.get()] = timeOut;
    });
    When(Method(mock, getTimeout)).AlwaysDo([&mock](void) {
        return timeOuts[&mock.get()];
    });

    When(OverloadedMethod(mock, find, bool(char))).AlwaysDo([&mock](char target){
        return mock.get().find(&target, 1U);
    });
    When(OverloadedMethod(mock, find, bool(const char*))).AlwaysDo([&mock](const char *target){
        return mock.get().find(target, strlen(target));
    });
    When(OverloadedMethod(mock, find, bool(const char*, size_t))).AlwaysDo([&mock](const char *target, size_t length){
        return mock.get().findUntil(target, length, nullptr, 0);
    });
    When(OverloadedMethod(mock, findUntil, bool(const char*, const char *))).AlwaysDo([&mock](const char *target, const char *terminator){
        return mock.get().findUntil(target, strlen(target), terminator, strlen(terminator));
    });
    // reads data from the stream until the target string of the given length is found
    // search terminated if the terminator string is found
    // returns true if target string is found, false if terminated or timed out
    When(OverloadedMethod(mock, findUntil, bool(const char*, size_t, const char*, size_t))).AlwaysDo([&stream](const char *target, size_t targetLen, const char *terminate, size_t termLen){
        std::string strTarget(target, StreamDetail::max_strlen(target, targetLen));
        std::string strTerminate(terminate, StreamDetail::max_strlen(terminate, termLen));

        std::string found;
        if (!strTarget.empty() && !strTerminate.empty())
        {
            std::array<std::string, 2> searchTargets = { strTarget, strTerminate };
            found = StreamDetail::search(searchTargets.begin(), searchTargets.end(), stream);
        }
        else if (!strTarget.empty()) {
            found = StreamDetail::search(strTarget, stream);
        }
        else if (!strTerminate.empty()) {
            found = StreamDetail::search(strTerminate, stream);
        } else {
            // Deliberate - do nothing
        }
        return !found.empty();
    });
    
    When(OverloadedMethod(mock, parseInt, long(LookaheadMode, char))).AlwaysDo([&stream](LookaheadMode mode, char ignore){
        // Create a locale with the custom ignore facet and
        // imbue the stringstream with the custom locale
        auto oldLocale = stream.imbue(std::locale(std::locale::classic(), new StreamDetail::ignore_char_int_t(mode, ignore)));
        std::string i;
        stream >> i;
        (void)stream.imbue(oldLocale);
        try { 
            return std::stol(StreamDetail::removeIgnoreChar(i, ignore));
        }
        catch (std::invalid_argument &e)
        {
            return 0L;
        }
    });

    When(OverloadedMethod(mock, parseFloat, float(LookaheadMode, char))).AlwaysDo([&stream](LookaheadMode mode, char ignore){
        // Create a locale with the custom ignore facet and
        // imbue the stringstream with the custom locale
        auto oldLocale = stream.imbue(std::locale(std::locale::classic(), new StreamDetail::ignore_char_float_t(mode, ignore)));
        std::string i;
        stream >> i;
        (void)stream.imbue(oldLocale);
        try { 
            return std::stof(StreamDetail::removeIgnoreChar(i, ignore));
        }
        catch (std::invalid_argument &e)
        {
            return 0.0f;
        }
    });

    When(OverloadedMethod(mock, readBytes, size_t(char*, size_t))).AlwaysDo([&stream](char *buffer, size_t length){
        (void)stream.read(buffer, length);
        return stream.gcount();
    });

    When(OverloadedMethod(mock, readBytesUntil, size_t(char, char*, size_t))).AlwaysDo([&stream](char terminator, char *buffer, size_t length){
        char c = stream.get();
        size_t index = 0;
        while ((index < length)
            && (c!=std::char_traits<char>::eof())
            && (c!=terminator)) {
            *buffer++ = c;
            ++index;
            c = stream.get();
        }
        return index; // return number of characters, not including null terminator
    });

    When(Method(mock, readString)).AlwaysDo([&stream](){
        std::string s;
        stream >> s;
        return String(s.c_str());
    });

    When(Method(mock, readStringUntil)).AlwaysDo([&stream](char terminator){
        std::string s;
        char c = stream.get();
        while ((c!=std::char_traits<char>::eof())
            && (c!=terminator)) {
            s += c;
            c = stream.get();
        }
        return String(s.c_str());
    });
}

}