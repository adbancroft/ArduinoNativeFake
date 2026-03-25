#pragma once
#include <map>
#include <algorithm>
#include <SimpleArduinoFake.h>

namespace ArduinoNativeFake
{

namespace StreamDetail {

using namespace fakeit;

template <class Iterator>
static inline std::string find_begins_with(const Iterator begin, const Iterator end, const std::string &next)
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
static inline std::string find_initial_match(const Iterator begin, const Iterator end, std::istream &input)
{
    std::string match;
    do {
        match = find_begins_with(begin, end, std::string(1U, input.get()));
    } while (!input.eof() && match.empty());
    return match;
}

template <class Iterator>
static inline std::string search(const Iterator begin, const Iterator end, std::istream &input)
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

static inline std::string search(const std::string &searchTerm, std::istream &input)
{
    std::array<std::string, 1> targets = { searchTerm };
    return search(targets.begin(), targets.end(), input);
}

static inline size_t max_strlen(const char *str, size_t len)
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

static inline std::string removeIgnoreChar(const std::string& s, char c) {
    if (c!=NO_IGNORE_CHAR) {
        std::string result(s);
        result.erase(std::remove(result.begin(), result.end(), c), result.end());
        return result;
    }
    return s;
}

} // namespace StreamDetail

}