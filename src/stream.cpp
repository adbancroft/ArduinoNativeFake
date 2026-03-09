#include <map>
#include <algorithm>
#include "stream.h"
#include "print.h"

using namespace fakeit;

template <class Iterator>
static std::string find_begins_with(const Iterator begin, const Iterator end, const std::string &next)
{
    if(end!=std::find_if(begin, end, [&next](const std::string &target){
                    return target.rfind(next, 0)==0;
                })) 
    {
        return next;
    }
    return std::string();
}

template <class Iterator>
static std::string find_initial_match(const Iterator begin, const Iterator end, std::istream &input)
{
    std::string match;
    do {
        match = find_begins_with(begin, end, std::string(1U, input.get()));
    } while (!input.eof() && match.empty());
    return match;
}

template <class Iterator>
static std::string search(const Iterator begin, const Iterator end, std::istream &input)
{
    std::string match = find_initial_match(begin, end, input);
    while (!match.empty() && end==std::find(begin, end, match))
    {
        char c = input.get();
        match = find_begins_with(begin, end, match + c);
        // Matching failed but not eof, so restart the search
        if (match.empty() && !input.eof()) {
            input.putback(c);
            match = find_initial_match(begin, end, input);
        }
    }   
    return match;
}

static std::string search(const std::string &searchTerm, std::istream &input)
{
    std::array<std::string, 1> targets = { searchTerm };
    return search(targets.begin(), targets.end(), input);
}

static size_t max_strlen(const char *str, size_t len)
{
    if (str==nullptr) {
        return 0U;
    }
    return std::min(strlen(str), len);
}

class ignore_char_int_t : public std::ctype<char>
{
protected:
    mask my_table[table_size];

public:

    ignore_char_int_t(LookaheadMode mode, char c, size_t refs = 0)  
        : std::ctype<char>(&my_table[0], false, refs)
    {
        std::copy_n(classic_table(), table_size, my_table);
        if(mode==SKIP_NONE) {
            for (int loop=0; loop<table_size; ++loop) {
                my_table[loop] = digit;
            }
        }
        else if(mode==SKIP_ALL) {
            for (int loop=0; loop<table_size; ++loop) {
                my_table[loop] = space;
            }
            my_table['-'] = digit;
            my_table['0'] = digit;
            my_table['1'] = digit;
            my_table['2'] = digit;
            my_table['3'] = digit;
            my_table['4'] = digit;
            my_table['5'] = digit;
            my_table['6'] = digit;
            my_table['7'] = digit;
            my_table['8'] = digit;
            my_table['9'] = digit;
        } else {
            // mode==SKIP_WHITESPACE, default;
        }
        if (c!=NO_IGNORE_CHAR) {
            my_table[c] = (mask)digit;
        }
    }
};

class ignore_char_float_t : public ignore_char_int_t
{
public:

    ignore_char_float_t(LookaheadMode mode, char c, size_t refs = 0)  
        : ignore_char_int_t(mode, c, refs)
    {
        if(mode==SKIP_NONE) {
        }
        else if(mode==SKIP_ALL) {
            my_table['.'] = digit;
        } else {
            // mode==SKIP_WHITESPACE, default;
        }
    }
};

static std::string removeIgnoreChar(const std::string& s, char c) {
    if (c!=NO_IGNORE_CHAR) {
        std::string result(s);
        result.erase(std::remove(result.begin(), result.end(), c), result.end());
        return result;
    }
    return s;
}

void setupNativeFake(fakeit::Mock<StreamFake> &mock, std::istream &stream)
{
    // These are pure virtual in Stream, so don't implement here (they'll be implemented in fakes derived from StreamFake)
    // virtual int available() = 0;
    // virtual int read() = 0;
    // virtual int peek() = 0;

    static std::map<StreamFake*, unsigned long> timeOuts;
    When(Method(mock, setTimeout)).AlwaysDo([&mock](unsigned long timeOut) {
        timeOuts[&mock.get()] = timeOut;
    });
    When(Method(mock, getTimeout)).AlwaysDo([&mock](void) {
        return timeOuts[&mock.get()];
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
        std::string strTarget(target, max_strlen(target, targetLen));
        std::string strTerminate(terminate, max_strlen(terminate, termLen));

        std::string found;
        if (!strTarget.empty() && !strTerminate.empty())
        {
            std::array<std::string, 2> searchTargets = { strTarget, strTerminate };
            found = search(searchTargets.begin(), searchTargets.end(), stream);
        }
        else if (!strTarget.empty()) {
            found = search(strTarget, stream);
        }
        else if (!strTerminate.empty()) {
            found = search(strTerminate, stream);
        } else {
            // Deliberate - do nothing
        }
        return !found.empty();
    });
    
    When(OverloadedMethod(mock, parseInt, long(LookaheadMode, char))).AlwaysDo([&stream](LookaheadMode mode, char ignore){
        // Create a locale with the custom ignore facet and
        // imbue the stringstream with the custom locale
        auto oldLocale = stream.imbue(std::locale(std::locale::classic(), new ignore_char_int_t(mode, ignore)));
        std::string i;
        stream >> i;
        (void)stream.imbue(oldLocale);
        try { 
            return std::stol(removeIgnoreChar(i, ignore));
        }
        catch (std::invalid_argument &e)
        {
            return 0L;
        }
    });

    When(OverloadedMethod(mock, parseFloat, float(LookaheadMode, char))).AlwaysDo([&stream](LookaheadMode mode, char ignore){
        // Create a locale with the custom ignore facet and
        // imbue the stringstream with the custom locale
        auto oldLocale = stream.imbue(std::locale(std::locale::classic(), new ignore_char_float_t(mode, ignore)));
        std::string i;
        stream >> i;
        (void)stream.imbue(oldLocale);
        try { 
            return std::stof(removeIgnoreChar(i, ignore));
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