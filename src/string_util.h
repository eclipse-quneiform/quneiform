/********************************************************************************
 * Copyright (c) 2005-2024 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

/** @addtogroup Utilities
    @brief Utility classes.
@{*/

#ifndef QUNEIFORM_STRING_UTIL_H
#define QUNEIFORM_STRING_UTIL_H

#include <array>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>

/// @brief String helper functions and classes.
namespace string_util
    {
    /// tolower
    /// @private
    inline int tolower(char c) { return std::tolower(static_cast<unsigned char>(c)); }

    /// @private
    inline wchar_t tolower(wchar_t c) { return std::towlower(c); }

    // strlen
    /// @private
    inline size_t strlen(const char* text) { return std::strlen(text); }

    /// @private
    inline size_t strlen(const wchar_t* text) { return std::wcslen(text); }

    /// @brief Determines if a given value is either of two other given values.
    /// @param value The value to compare with.
    /// @param first The first value to compare against.
    /// @param second The second value to compare against.
    /// @returns @c true if value is either of the other values.
    template<typename T>
    [[nodiscard]]
    constexpr bool is_either(const T value, const T first, const T second) noexcept
        {
        return (value == first || value == second);
        }

    /// @brief Determines if a given value is neither of two other given values.
    /// @param value The value to compare with.
    /// @param first The first value to compare against.
    /// @param second The second value to compare against.
    /// @returns @c true if value is neither of the other values.
    template<typename T>
    [[nodiscard]]
    constexpr bool is_neither(const T value, const T first, const T second) noexcept
        {
        assert(first != second);
        return (value != first && value != second);
        }

    /// @param ch The character to review.
    /// @returns @c true if @c ch is a trademark, service mark, or registration symbol.
    [[nodiscard]]
    constexpr static bool is_trademark_or_registration(const wchar_t ch) noexcept
        {
        // clang-format off
        return (ch == 0x2122) || // (TM)
            (ch == 0x00A9) ||    // (C)
            (ch == 0x24B8) ||    // (C)
            (ch == 0x2117) ||    // (P) (sound recording)
            (ch == 0x2120) ||    // (SM)
            (ch == 0x00AE) ||    // (R)
            (ch == 0x24C2);      // (M)
        // clang-format on
        }

    /** @returns Whether a character is a number (0-9 characters only, narrow versions).
        @param ch The letter to be reviewed.*/
    [[nodiscard]]
    constexpr static bool is_numeric_8bit(const wchar_t ch) noexcept
        {
        return ch >= L'0' && ch <= L'9';
        }

    /// @returns @c true if @c ch is a subscript number.
    /// @param ch The character to review.
    [[nodiscard]]
    constexpr static bool is_subscript_number(const wchar_t ch) noexcept
        {
        return (ch >= 0x2080 && ch <= 0x2089);
        }

    /// @returns @c true if @c ch is a subscript.
    /// @param ch The character to review.
    [[nodiscard]]
    constexpr static bool is_subscript(const wchar_t ch) noexcept
        {
        // clang-format off
        return (ch >= 0x2080 && ch <= 0x2089) ||
            (ch == 0x208A) ||
            (ch == 0x208B) ||
            (ch == 0x208C) ||
            (ch == 0x208D) ||
            (ch == 0x208E) ||
            (ch == 0x2090) ||
            (ch == 0x2091) ||
            (ch == 0x2092) ||
            (ch == 0x2093) ||
            (ch == 0x2094) ||
            (ch == 0x2095) ||
            (ch == 0x2096) ||
            (ch == 0x2097) ||
            (ch == 0x2098) ||
            (ch == 0x2099) ||
            (ch == 0x209A) ||
            (ch == 0x209B) ||
            (ch == 0x209C);
        // clang-format on
        }

    /** @brief Converts a character into its subscript equivalent.
        @param ch The character to convert.
        @returns The character converted into its subscript equivalent, or
            the original value if it can't be converted.
        @note This only applies to numbers, simple math characters,
            and a few letters (e.g., 2 -> ²)*/
    [[nodiscard]]
    constexpr static wchar_t to_subscript(const wchar_t ch) noexcept
        {
        // clang-format off
        return (ch == L'0' || ch == 0xFF10) ? static_cast<wchar_t>(0x2080) :
            (ch == L'1' || ch == 0xFF11) ? static_cast<wchar_t>(0x2081) :
            (ch == L'2' || ch == 0xFF12) ? static_cast<wchar_t>(0x2082) :
            (ch == L'3' || ch == 0xFF13) ? static_cast<wchar_t>(0x2083) :
            (ch == L'4' || ch == 0xFF14) ? static_cast<wchar_t>(0x2084) :
            (ch == L'5' || ch == 0xFF15) ? static_cast<wchar_t>(0x2085) :
            (ch == L'6' || ch == 0xFF16) ? static_cast<wchar_t>(0x2086) :
            (ch == L'7' || ch == 0xFF17) ? static_cast<wchar_t>(0x2087) :
            (ch == L'8' || ch == 0xFF18) ? static_cast<wchar_t>(0x2088) :
            (ch == L'9' || ch == 0xFF19) ? static_cast<wchar_t>(0x2089) :
            // simple math characters and letters
            (ch == L'+') ? static_cast<wchar_t>(0x208A) :
            (ch == L'-') ? static_cast<wchar_t>(0x208B) :
            (ch == L'=') ? static_cast<wchar_t>(0x208C) :
            (ch == L'(') ? static_cast<wchar_t>(0x208D) :
            (ch == L')') ? static_cast<wchar_t>(0x208E) :
            (ch == L'a') ? static_cast<wchar_t>(0x2090) :
            (ch == L'e') ? static_cast<wchar_t>(0x2091) :
            (ch == L'o') ? static_cast<wchar_t>(0x2092) :
            (ch == L'x') ? static_cast<wchar_t>(0x2093) :
            // 0x2094 is upside down 'e', no real equivalent with this
            (ch == L'h') ? static_cast<wchar_t>(0x2095) :
            (ch == L'k') ? static_cast<wchar_t>(0x2096) :
            (ch == L'l') ? static_cast<wchar_t>(0x2097) :
            (ch == L'm') ? static_cast<wchar_t>(0x2098) :
            (ch == L'n') ? static_cast<wchar_t>(0x2099) :
            (ch == L'p') ? static_cast<wchar_t>(0x209A) :
            (ch == L's') ? static_cast<wchar_t>(0x209B) :
            (ch == L't') ? static_cast<wchar_t>(0x209C) :
            ch;
        // clang-format on
        }

    /// @param ch The character to review.
    /// @returns @c true if @c ch is a fraction symbol.
    [[nodiscard]]
    constexpr static bool is_fraction(const wchar_t ch) noexcept
        {
        return (ch == 0xBC || ch == 0xBD || ch == 0xBE);
        }

    /// @param ch The character to review.
    /// @note This includes lowercased Roman numeral letters.
    /// @returns @c true if @c ch is a superscript number.
    [[nodiscard]]
    constexpr static bool is_superscript_number(const wchar_t ch) noexcept
        {
        // clang-format off
        return (ch == 0x2070) ||
            (ch == 0xB9) ||
            (ch == 0xB2) ||
            (ch == 0xB3) ||
            (ch == 0x2074) ||
            (ch == 0x2075) ||
            (ch == 0x2076) ||
            (ch == 0x2077) ||
            (ch == 0x2078) ||
            (ch == 0x2079) ||
            // Roman numerals
            (ch == 0x1D9C) || // c
            (ch == 0x1D48) || // d
            (ch == 0x2071) || // i
            (ch == 0x1D50) || // m
            (ch == 0x1D5B) || // v
            (ch == 0x02E3);   // x
        // clang-format on
        }

    /// @returns @c true if @c ch is a lowercased superscript letter.
    /// @param ch The character to review.
    [[nodiscard]]
    constexpr static bool is_superscript_lowercase(const wchar_t ch) noexcept
        {
        // clang-format off
        return // a-z
            (ch == 0x207A) ||
            (ch == 0x207B) ||
            (ch == 0x207C) ||
            (ch == 0x207D) ||
            (ch == 0x207E) ||
            (ch == 0x1D43) ||
            (ch == 0x1D47) ||
            (ch == 0x1D9C) ||
            (ch == 0x1D48) ||
            (ch == 0x1d49) ||
            (ch == 0x1DA0) ||
            (ch == 0x1D4D) ||
            (ch == 0x02B0) ||
            (ch == 0x2071) ||
            (ch == 0x02B2) ||
            (ch == 0x1D4F) ||
            (ch == 0x02E1) ||
            (ch == 0x1D50) ||
            (ch == 0x207F) ||
            (ch == 0x1D52) ||
            (ch == 0x1D56) ||
            (ch == 0x02B3) ||
            (ch == 0x02E2) ||
            (ch == 0x1D57) ||
            (ch == 0x1D58) ||
            (ch == 0x1D5B) ||
            (ch == 0x02B7) ||
            (ch == 0x02E3) ||
            (ch == 0x02B8) ||
            (ch == 0x1DBB);
        // clang-format on
        }

    /// @returns @c true if @c ch is a superscript.
    /// @param ch The character to review.
    [[nodiscard]]
    inline constexpr static bool is_superscript(const wchar_t ch) noexcept
        {
        // clang-format off
        return (ch == 0x2070) ||
            (ch == 0xB9) ||
            (ch == 0xB2) ||
            (ch == 0xB3) ||
            (ch == 0x2074) ||
            (ch == 0x2075) ||
            (ch == 0x2076) ||
            (ch == 0x2077) ||
            (ch == 0x2078) ||
            (ch == 0x2079) ||
            // simple math characters
            (ch == 0x207A) ||
            (ch == 0x207B) ||
            (ch == 0x207C) ||
            (ch == 0x207D) ||
            (ch == 0x207E) ||
            is_superscript_lowercase(ch);
        // clang-format on
        }

    /** @brief Converts a character into its superscript equivalent.
        @param ch The character to convert.
        @returns The character converted into its superscript equivalent, or
            the original value if it can't be converted.
        @note This only applies to numbers, simple math characters,
            and a few letters (e.g., 2 -> ²).*/
    [[nodiscard]]
    inline constexpr static wchar_t to_superscript(const wchar_t ch) noexcept
        {
        // clang-format off
        return (ch == L'0' || ch == 0xFF10) ? 0x2070 :
            (ch == L'1' || ch == 0xFF11) ? 0xB9 :
            (ch == L'2' || ch == 0xFF12) ? 0xB2 :
            (ch == L'3' || ch == 0xFF13) ? 0xB3 :
            (ch == L'4' || ch == 0xFF14) ? 0x2074 :
            (ch == L'5' || ch == 0xFF15) ? 0x2075 :
            (ch == L'6' || ch == 0xFF16) ? 0x2076 :
            (ch == L'7' || ch == 0xFF17) ? 0x2077 :
            (ch == L'8' || ch == 0xFF18) ? 0x2078 :
            (ch == L'9' || ch == 0xFF19) ? 0x2079 :
            // simple math characters and letters
            (ch == L'+') ? 0x207A :
            (ch == L'-') ? 0x207B :
            (ch == L'=') ? 0x207C :
            (ch == L'(') ? 0x207D :
            (ch == L')') ? 0x207E :
            (ch == L'a') ? 0x1D43 :
            (ch == L'b') ? 0x1D47 :
            (ch == L'c') ? 0x1D9C :
            (ch == L'd') ? 0x1D48 :
            (ch == L'e') ? 0x1D49 :
            (ch == L'f') ? 0x1DA0 :
            (ch == L'g') ? 0x1D4D :
            (ch == L'h') ? 0x02B0 :
            (ch == L'i') ? 0x2071 :
            (ch == L'j') ? 0x02B2 :
            (ch == L'k') ? 0x1D4F :
            (ch == L'l') ? 0x02E1 :
            (ch == L'm') ? 0x1D50 :
            (ch == L'n') ? 0x207F :
            (ch == L'o') ? 0x1D52 :
            (ch == L'p') ? 0x1D56 :
            (ch == L'r') ? 0x02B3 :
            (ch == L's') ? 0x02E2 :
            (ch == L't') ? 0x1D57 :
            (ch == L'u') ? 0x1D58 :
            (ch == L'v') ? 0x1D5B :
            (ch == L'w') ? 0x02B7 :
            (ch == L'x') ? 0x02E3 :
            (ch == L'y') ? 0x02B8 :
            (ch == L'z') ? 0x1DBB :
            ch;
        // clang-format on
        }

    /** @brief Determines whether a character is a hexadecimal digit (0-9,A-F,a-f).
        @param ch The letter to be analyzed.
        @returns @c true if @c ch is a hex digit.*/
    [[nodiscard]]
    inline constexpr bool is_hex_digit(const wchar_t ch) noexcept
        {
        return (is_numeric_8bit(static_cast<wchar_t>(ch)) ||
                ((ch >= 0x61 /*'a'*/ && ch <= 0x66 /*'f'*/) ||
                 (ch >= 0x41 /*'A'*/ && ch <= 0x46 /*'F'*/)));
        }

    /** @brief Attempts to convert a string @c buffer to a double.
        @details Same as wcstod(), except it handles thousands separators as well.
        @param buffer The string buffer to read a number from.
        @param[out] endPtr The (optional) pointer in the buffer where the number text ends.
            (Will be the null terminator if the entire string is a number.)
        @returns The string's value converted to a number. (Will be zero if conversion failed.)
        @todo Need to support 0x (hex) strings.*/
    [[nodiscard]]
    inline double wcstod_thousands_separator(const wchar_t* buffer, wchar_t** endPtr)
        {
        if (buffer == nullptr)
            {
            return 0;
            }

        // vanilla version of reading as a number
        wchar_t* end(nullptr);
        auto value = std::wcstod(buffer, &end);

        // step over any space like wcstod would have done
        while (buffer < end && (std::iswspace(*buffer) != 0))
            {
            ++buffer;
            }

        // if wcstod ran into what appears to be a thousands separator,
        // then weed those out and try wcstod again
        if ( // wcstod actually read a little bit at least
            end > buffer &&
            // but stopped on a thousands separator
            (*end == ',' || *end == '.') &&
            // and what's after the thousands separator is a number
            *(end + 1) != 0 && is_numeric_8bit(*(end + 1)))
            {
            const auto thousandsSep = *end;
            const auto* realNumberStart = buffer;
            const auto* realNumberEnd = buffer;
            // scan past any numbers, +/-, and thousands & radix separators
            while (*realNumberEnd != 0 &&
                   (is_numeric_8bit(*realNumberEnd) || is_either(*realNumberEnd, L',', L'.') ||
                    is_either(*realNumberEnd, L'+', L'-')))
                {
                ++realNumberEnd;
                }
            // copy over the number text from the buffer, but skipping over the thousands separators
            constexpr size_t BUFFER_SIZE{ 64 };
            std::array<wchar_t, BUFFER_SIZE> realNumberStr{ 0 };
            size_t newNumBufferCounter{ 0 };
            while (realNumberStart < realNumberEnd && newNumBufferCounter + 1 < BUFFER_SIZE)
                {
                if (*realNumberStart != thousandsSep)
                    {
                    realNumberStr[newNumBufferCounter++] = *realNumberStart;
                    }
                ++realNumberStart;
                }
            end = const_cast<wchar_t*>(realNumberStart);
            // try wcstod again
            value = std::wcstod(realNumberStr.data(), nullptr);
            }
        // set the end to where we read, if caller asked for it
        if (endPtr != nullptr)
            {
            *endPtr = end;
            }

        return value;
        }

    /** Returns the number of characters in the string pointed to by @c str, not including the
            terminating '\0' character, but at most @c maxlen.\n
            In doing this, this looks only at the first @c maxlen characters in @c str and
            never beyond @c str+ @c maxlen.\n
            This function should be used for input that may not be null terminated.
        @param str The string to review.
        @param maxlen The maximum length of the string to scan.
        @returns The valid length of the string or maxlen, whichever is shorter.*/
    template<typename T>
    [[nodiscard]]
    size_t strnlen(const T* str, const size_t maxlen) noexcept
        {
        if (!str || maxlen == 0)
            {
            return 0;
            }
        size_t i = 0;
        for (i = 0; i < maxlen && str[i]; ++i)
            {
            /* no-op */;
            }
        return i;
        }

    /// @brief Search for substring in string (case-insensitive).
    /// @param string The string to search.
    /// @param strSearch The string to search for.
    /// @returns The pointer to where the substring was found, or @c nullptr if not found.
    template<typename T>
    [[nodiscard]]
    const T* stristr(const T* string, const T* strSearch) noexcept
        {
        if (!string || !strSearch || *strSearch == 0)
            {
            return nullptr;
            }
        while (*string)
            {
            // compare the characters one at a time
            size_t i = 0;
            for (i = 0; strSearch[i] != 0; ++i)
                {
                if (string[i] == 0)
                    {
                    return nullptr;
                    }
                if (string_util::tolower(strSearch[i]) != string_util::tolower(string[i]))
                    {
                    ++string;
                    break;
                    }
                }
            // if the substring loop completed then the substring was found
            if (strSearch[i] == 0)
                {
                return string;
                }
            }
        return nullptr;
        }

    /** @brief Searches for substring in a larger string (case-insensitively),
            limiting the search to a specified number of characters.
        @param string The string to review.
        @param strSearch The string to search for.
        @param charCount The number of characters to search for within @c string.
        @returns A pointer to the found string, or @c nullptr otherwise.*/
    template<typename T>
    [[nodiscard]]
    const T* strnistr(const T* string, const T* strSearch, const size_t charCount) noexcept
        {
        if (!string || !strSearch || charCount == 0 || *strSearch == 0)
            {
            return nullptr;
            }
        for (size_t i = 0; i < charCount; ++i)
            {
            // compare the characters one at a time
            size_t j = 0;
            for (j = 0; strSearch[j] != 0; ++j)
                {
                if ((i + j) >= charCount || string[i + j] == 0)
                    {
                    return nullptr;
                    }
                if (string_util::tolower(strSearch[j]) != string_util::tolower(string[i + j]))
                    {
                    break;
                    }
                }
            // if the substring loop completed then the substring was found
            if (strSearch[j] == 0)
                {
                return (string + i);
                }
            }
        return nullptr;
        }

    /// @brief Case-insensitive comparison by character count.
    /// @param first The first string to compare.
    /// @param second The second string to compare.
    /// @param charCount The max number of characters to compare.
    /// @returns The comparison result.
    template<typename T>
    [[nodiscard]]
    int strnicmp(const T* first, const T* second, size_t charCount) noexcept
        {
        // first check if either of the strings are null
        if (!first && !second)
            {
            return 0;
            }
        if (!first && second)
            {
            return -1;
            }
        if (first && !second)
            {
            return 1;
            }

        int result{ 0 };

        if (charCount > 0)
            {
            int f{ 0 }, l{ 0 };
            do
                {
                f = string_util::tolower(*(first++));
                l = string_util::tolower(*(second++));
                } while ((--charCount) && f && (f == l));
            result = static_cast<int>(f - l);
            }
        return result;
        }

    /// @brief Case-insensitive comparison by character count.
    /// @param first The first string to compare.
    /// @param second The second string to compare.
    /// @note The length of the @c string_view will be the max length to compare.
    /// @returns The comparison result.
    template<typename T>
    [[nodiscard]]
    int strnicmp(const std::basic_string_view<T> first,
                 const std::basic_string_view<T> second) noexcept
        {
        return strnicmp(first.data(), second.data(), second.length());
        }

    /// @brief Case-insensitive comparison.
    /// @param first The first string to compare.
    /// @param second The second string to compare.
    /// @returns The comparison result.
    template<typename T>
    [[nodiscard]]
    int stricmp(const T* first, const T* second) noexcept
        {
        // first check if either of the strings are null
        if (!first && !second)
            {
            return 0;
            }
        if (!first && second)
            {
            return -1;
            }
        if (first && !second)
            {
            return 1;
            }

        int f{ 0 }, l{ 0 };
        do
            {
            f = string_util::tolower(*(first++));
            l = string_util::tolower(*(second++));
            } while (f && (f == l));

        return static_cast<int>(f - l);
        }

    /** @brief Natural order comparison (recognizes numeric strings).
        @details This will see "2" as being less than "12".
        @param firstString The first string in the comparison.
        @param secondString The second string in the comparison.
        @param caseInsensitive Whether the comparison should be case-insensitive.
        @returns @c -1 if the first string is less, 1 if the first string is greater,
            or @c 0 if the strings are equal.*/
    template<typename T>
    [[nodiscard]]
    int strnatordcmp(const T* firstString, const T* secondString, bool caseInsensitive = false)
        {
        // first check if either of the strings are null
        if (!firstString && !secondString)
            {
            return 0;
            }
        if (!firstString && secondString)
            {
            return -1;
            }
        if (firstString && !secondString)
            {
            return 1;
            }

        size_t firstStringIndex = 0, secondStringIndex = 0;

        while (true)
            {
            T ch1 = firstString[firstStringIndex];
            T ch2 = secondString[secondStringIndex];

            // skip leading spaces
            while (std::iswspace(ch1))
                {
                ch1 = firstString[++firstStringIndex];
                }

            while (std::iswspace(ch2))
                {
                ch2 = secondString[++secondStringIndex];
                }

            // process run of digits
            if (is_numeric_8bit(ch1) && is_numeric_8bit(ch2))
                {
                T *firstEnd(nullptr), *secondEnd(nullptr);
                const double firstDouble =
                    wcstod_thousands_separator(firstString + firstStringIndex, &firstEnd);
                const double secondDouble =
                    wcstod_thousands_separator(secondString + secondStringIndex, &secondEnd);

                if (firstDouble < secondDouble)
                    {
                    return -1;
                    }
                if (firstDouble > secondDouble)
                    {
                    return 1;
                    }
                // numbers are equal
                // if this was the end of both strings then they are equal
                if (*firstEnd == 0 && *secondEnd == 0)
                    {
                    return 0;
                    }
                /* the first string is done, but there is more to the second string
                   after the number, so first is smaller*/
                if (*firstEnd == 0)
                    {
                    return -1;
                    }
                /* the second string is done, but there is more to the first string
                   after the number, so first is bigger*/
                if (*secondEnd == 0)
                    {
                    return 1;
                    }
                // there is more to both of them, so move the counter and move on

                // if wcstod_thousands_separator() didn't move the pointers,
                // then we are stuck, so return that they are equal
                if (static_cast<decltype(firstStringIndex)>(firstEnd - firstString) ==
                        firstStringIndex &&
                    static_cast<decltype(secondStringIndex)>(secondEnd - secondString) ==
                        secondStringIndex)
                    {
                    return 0;
                    }
                firstStringIndex = (firstEnd - firstString);
                secondStringIndex = (secondEnd - secondString);
                continue;
                }

            // if we are at the end of the strings then they are the same
            if (ch1 == 0 && ch2 == 0)
                {
                return 0;
                }

            if (caseInsensitive)
                {
                ch1 = string_util::tolower(ch1);
                ch2 = string_util::tolower(ch2);
                }

            if (ch1 < ch2)
                {
                return -1;
                }
            if (ch1 > ch2)
                {
                return 1;
                }

            ++firstStringIndex;
            ++secondStringIndex;
            }
        }

    /// @brief Compare, recognizing numeric strings and ignoring case.
    /// @param a The first string to compare.
    /// @param b The second string to compare.
    /// @returns The comparison result.
    template<typename T>
    [[nodiscard]]
    int strnatordncasecmp(const T* a, const T* b)
        {
        return strnatordcmp(a, b, true);
        }

    /// Indicates whether a larger strings ends with the specified suffix. Lengths are provided
    /// by the caller for efficiency. This function is case-sensitive.
    /// @param text The text to review.
    /// @param textLength The text's length.
    /// @param suffix The suffix to check for.
    /// @param suffixLength The length of @c suffix.
    /// @returns @c true if text contains the provided suffix.
    template<typename T>
    [[nodiscard]]
    bool has_suffix(const T* text, const size_t textLength, const T* suffix,
                    const size_t suffixLength) noexcept
        {
        if (text == nullptr || suffix == nullptr || textLength <= suffixLength)
            {
            return false;
            }
        return (string_util::strnicmp(text + (textLength - suffixLength), suffix, suffixLength) ==
                0);
        }

    /** @brief Searches for a matching tag, skipping an extra open/close pairs
            of symbols in between.
        @param stringToSearch The string to search in.
        @param openSymbol The opening symbol.
        @param closeSymbol The closing symbol that we are looking for.
        @param failOnOverlappingOpenSymbol Whether it should immediately return
            failure if an open symbol is found before a matching close symbol.\n
            Recommended to be @c false.
        @returns A pointer to where the closing tag is, or @c nullptr if one can't be found.*/
    [[nodiscard]]
    inline const wchar_t* find_matching_close_tag(const wchar_t* stringToSearch,
                                                  const wchar_t openSymbol,
                                                  const wchar_t closeSymbol,
                                                  const bool failOnOverlappingOpenSymbol) noexcept
        {
        if (stringToSearch == nullptr)
            {
            return nullptr;
            }
        long openStack = 0;
        while (*stringToSearch != 0)
            {
            if (stringToSearch[0] == openSymbol)
                {
                if (failOnOverlappingOpenSymbol)
                    {
                    return nullptr;
                    }
                ++openStack;
                }
            else if (stringToSearch[0] == closeSymbol)
                {
                if (openStack == 0)
                    {
                    return stringToSearch;
                    }
                --openStack;
                }
            ++stringToSearch;
            }
        return nullptr;
        }

    /// @brief Searches for a matching tag, skipping any extra open/close pairs
    ///     of symbols in between.
    /// @param stringToSearch The string to search.
    /// @param openSymbol The opening symbol.
    /// @param closeSymbol The closing symbol.
    /// @returns Pointer to where the closing tag is, or @c nullptr if not found.
    [[nodiscard]]
    inline const wchar_t* find_matching_close_tag(std::wstring_view stringToSearch,
                                                  std::wstring_view openSymbol,
                                                  std::wstring_view closeSymbol)
        {
        if (stringToSearch.empty() || openSymbol.empty() || closeSymbol.empty())
            {
            return nullptr;
            }

        long openStack{ 0 };
        while (!stringToSearch.empty())
            {
            if (stringToSearch.length() >= openSymbol.length() &&
                stringToSearch.starts_with(openSymbol))
                {
                ++openStack;
                stringToSearch.remove_prefix(openSymbol.length());
                continue;
                }
            if (stringToSearch.length() >= closeSymbol.length() &&
                stringToSearch.starts_with(closeSymbol))
                {
                if (openStack == 0)
                    {
                    return stringToSearch.data();
                    }
                --openStack;
                }
            stringToSearch.remove_prefix(1);
            }
        return nullptr;
        }

    /** @brief Searches for a matching tag, skipping any extra open/close pairs
            of symbols in between.
        @param stringToSearch The string to search in.
        @param openSymbol The opening symbol.
        @param closeSymbol The closing symbol.
        @returns A pointer in the string where the character was found,
            or @c nullptr if not found.*/
    template<typename T>
    [[nodiscard]]
    const T* find_unescaped_matching_close_tag(const T* stringToSearch, const T openSymbol,
                                               const T closeSymbol) noexcept
        {
        assert(openSymbol != closeSymbol);
        if (!stringToSearch || openSymbol == closeSymbol)
            {
            return nullptr;
            }
        const T* const originalStart = stringToSearch;
        long openStack = 0;
        while (*stringToSearch)
            {
            if (stringToSearch[0] == openSymbol &&
                ((stringToSearch == originalStart) || stringToSearch[-1] != L'\\'))
                {
                ++openStack;
                ++stringToSearch;
                continue;
                }
            if (stringToSearch[0] == closeSymbol &&
                ((stringToSearch == originalStart) || stringToSearch[-1] != L'\\'))
                {
                if (openStack == 0)
                    {
                    return stringToSearch;
                    }
                --openStack;
                }
            ++stringToSearch;
            }
        return nullptr;
        }

    /** @brief Searches for a matching tag, skipping any extra open/close
            pairs of symbols in between, but also constrained to the same line of text.
        @param stringToSearch The string to search in.
        @param openSymbol The opening symbol.
        @param closeSymbol The closing symbol.
        @returns A pointer in the string where the character was found,
            or @c nullptr if not found.*/
    template<typename T>
    [[nodiscard]]
    const T* find_unescaped_matching_close_tag_same_line(const T* stringToSearch,
                                                         const T openSymbol,
                                                         const T closeSymbol) noexcept
        {
        assert(openSymbol != closeSymbol);
        if (!stringToSearch || openSymbol == closeSymbol)
            {
            return nullptr;
            }
        const T* const originalStart = stringToSearch;
        long openStack = 0;
        while (*stringToSearch)
            {
            if (stringToSearch[0] == L'\n' || stringToSearch[0] == L'\r')
                {
                return nullptr;
                }
            if (stringToSearch[0] == openSymbol &&
                ((stringToSearch == originalStart) || stringToSearch[-1] != L'\\'))
                {
                ++openStack;
                ++stringToSearch;
                continue;
                }
            if (stringToSearch[0] == closeSymbol &&
                ((stringToSearch == originalStart) || stringToSearch[-1] != L'\\'))
                {
                if (openStack == 0)
                    {
                    return stringToSearch;
                    }
                --openStack;
                }
            ++stringToSearch;
            }
        return nullptr;
        }

    /** @brief Searches for a matching tag, skipping any extra open/close pairs
            of symbols in between, but also constrained to the same line of text.
        @param stringToSearch The string to search in.
        @param openSymbol The opening symbol.
        @param closeSymbol The closing symbol.
        @param numberOfCharacters The max number of characters to search through in the string.
        @returns A pointer in the string where the character was found,
            or @c nullptr if not found.*/
    [[nodiscard]]
    inline const wchar_t* find_unescaped_matching_close_tag_same_line_n(
        const wchar_t* stringToSearch, const wchar_t openSymbol, const wchar_t closeSymbol,
        int64_t numberOfCharacters) noexcept
        {
        assert(openSymbol != closeSymbol);
        if ((stringToSearch == nullptr) || openSymbol == closeSymbol)
            {
            return nullptr;
            }
        const wchar_t* const originalStart = stringToSearch;
        long openStack = 0;
        while ((*stringToSearch != 0) && numberOfCharacters > 0)
            {
            if (stringToSearch[0] == L'\n' || stringToSearch[0] == L'\r')
                {
                return nullptr;
                }
            if (stringToSearch[0] == openSymbol &&
                ((stringToSearch == originalStart) || stringToSearch[-1] != L'\\'))
                {
                ++openStack;
                }
            else if (stringToSearch[0] == closeSymbol &&
                     ((stringToSearch == originalStart) || stringToSearch[-1] != L'\\'))
                {
                if (openStack == 0)
                    {
                    return stringToSearch;
                    }
                --openStack;
                }
            ++stringToSearch;
            --numberOfCharacters;
            }
        return nullptr;
        }

    /** @brief Searches for a single character in a string that does not have a `\\` in front of it.
        @param stringToSearch The string to search in.
        @param ch The character to search for.
        @returns A pointer in the string where the character was found,
            or @c nullptr if not found.*/
    template<typename T>
    [[nodiscard]]
    const T* find_unescaped_char(const T* stringToSearch, const T ch) noexcept
        {
        if (!stringToSearch)
            {
            return nullptr;
            }
        while (*stringToSearch)
            {
            // if on an escape character, then step over that and whatever it's escaping
            if (*stringToSearch == L'\\')
                {
                stringToSearch += 2;
                continue;
                }
            if (*stringToSearch == ch)
                {
                break;
                }
            ++stringToSearch;
            }
        return (*stringToSearch == 0) ? nullptr : stringToSearch;
        }

    /** @brief Searches for a single character in a string that does not have a `\\` in front of it.
        @param stringToSearch The string to search in.
        @param ch The character to search for.
        @returns A pointer in the string where the character was found, or @c nullptr if not found.
        @param numberOfCharacters The max number of characters to search through in the string.*/
    template<typename T>
    [[nodiscard]]
    const T* find_unescaped_char_n(const T* stringToSearch, const T ch,
                                   int64_t numberOfCharacters) noexcept
        {
        if (!stringToSearch)
            {
            return nullptr;
            }
        while (*stringToSearch && numberOfCharacters > 0)
            {
            // if on an escape character, then step over that
            if (*stringToSearch == L'\\')
                {
                ++stringToSearch;
                --numberOfCharacters;
                if (numberOfCharacters == 0)
                    {
                    return nullptr;
                    }
                }
            else if (*stringToSearch == ch)
                {
                break;
                }
            ++stringToSearch;
            --numberOfCharacters;
            }
        return (*stringToSearch == 0 || numberOfCharacters <= 0) ? nullptr : stringToSearch;
        }

    /** @brief Searches for a single character in a string that does not have a `\\` in front of it,
            but also constrained to the same line of text.
        @param stringToSearch The string to search in.
        @param ch The character to search for.
        @returns A pointer in the string where the character was found, or @c nullptr if not found.
        @param numberOfCharacters The max number of characters to search through in the string.*/
    template<typename T>
    [[nodiscard]]
    const T* find_unescaped_char_same_line_n(const T* stringToSearch, const T ch,
                                             int64_t numberOfCharacters) noexcept
        {
        if (!stringToSearch)
            {
            return nullptr;
            }
        while (*stringToSearch && numberOfCharacters > 0)
            {
            if (stringToSearch[0] == L'\n' || stringToSearch[0] == L'\r')
                {
                return nullptr;
                }
            // if on an escape character, then step over that
            if (*stringToSearch == L'\\')
                {
                ++stringToSearch;
                --numberOfCharacters;
                if (numberOfCharacters == 0)
                    {
                    return nullptr;
                    }
                }
            else if (*stringToSearch == ch)
                {
                break;
                }
            ++stringToSearch;
            --numberOfCharacters;
            }
        return (*stringToSearch == 0 || numberOfCharacters <= 0) ? nullptr : stringToSearch;
        }

    /** @brief Searches for a single character in a string for n number of characters.
        @details Size argument should be less than or equal to the
            length of the string being searched.
        @param stringToSearch The string to search in.
        @param ch The character to search for.
        @param numberOfCharacters The max number of characters to search through in the string.
        @returns A pointer in the string where the character was found,
            or @c nullptr if not found.*/
    template<typename T>
    [[nodiscard]]
    const T* strnchr(const T* stringToSearch, const T ch, size_t numberOfCharacters) noexcept
        {
        if (!stringToSearch)
            {
            return nullptr;
            }
        size_t i = 0;
        for (i = 0; i < numberOfCharacters; ++i)
            {
            /* if string being searched is shorter than the size argument,
                then return failure (nullptr).*/
            if (stringToSearch[i] == 0)
                {
                return nullptr;
                }
            if (stringToSearch[i] == ch)
                {
                return stringToSearch + i;
                }
            }
        return nullptr;
        }

    /** @brief Searches in a string for a single character from a larger sequence
            and returns a pointer if found.
        @param stringToSearch The string to search in.
        @param searchSequence The sequence of characters to search for.
            If any character in this sequence is found in @c stringToSearch,
            then its position will be returned.
        @param searchSeqLength The length of the search sequence.
        @returns A pointer to where the character was found, or @c nullptr if not found.*/
    template<typename T>
    [[nodiscard]]
    const T* strcspn_pointer(const T* stringToSearch, const T* searchSequence,
                             const size_t searchSeqLength) noexcept
        {
        if (!stringToSearch || !searchSequence || searchSeqLength == 0)
            {
            return nullptr;
            }
        assert((string_util::strlen(searchSequence) == searchSeqLength) &&
               "Invalid length passed to strcspn_pointer().");
        while (stringToSearch)
            {
            if (stringToSearch[0] == 0)
                {
                return nullptr;
                }
            for (size_t i = 0; i < searchSeqLength; ++i)
                {
                if (stringToSearch[0] == searchSequence[i])
                    {
                    return stringToSearch;
                    }
                }
            ++stringToSearch;
            }
        return nullptr;
        }

    /// @brief Searches for a string in a larger string as a whole word.
    /// @param haystack The string to search in.
    /// @param needle The string to search for.
    /// @param startIndex The starting position within @c haystack to start the search.
    /// @returns The position of the found word, or @c npos if not found.
    template<typename T>
    [[nodiscard]]
    T::size_type find_whole_word(const T& haystack, const T& needle, size_t startIndex = 0)
        {
        if (needle.length() == 0 || haystack.length() == 0)
            {
            return T::npos;
            }

        auto start{ startIndex };

        while (start != T::npos)
            {
            start = haystack.find(needle, start);

            if (start == T::npos)
                {
                return T::npos;
                }
            // if at start of haystack
            if (start == 0)
                {
                if (needle.length() == haystack.length())
                    {
                    return start;
                    }
                if (std::iswspace(haystack[start + needle.length()]) ||
                    std::iswpunct(haystack[start + needle.length()]))
                    {
                    return start;
                    }

                ++start;
                continue;
                }
            // at end of haystack
            if ((start + needle.length()) == haystack.length())
                {
                if (needle.length() == haystack.length())
                    {
                    return start;
                    }
                if (std::iswspace(haystack[start - 1]) || std::iswpunct(haystack[start - 1]) ||
                    // after a hard coded newline or tab
                    (start >= 2 &&
                     (haystack[start - 1] == L'n' || haystack[start - 1] == L'r' ||
                      haystack[start - 1] == L't') &&
                     haystack[start - 2] == L'\\'))
                    {
                    return start;
                    }

                ++start;
                continue;
                }
            // inside of haystack

            if (needle.length() == haystack.length())
                {
                return start;
                }
            if ((std::iswspace(haystack[start + needle.length()]) ||
                 std::iswpunct(haystack[start + needle.length()])) &&
                (std::iswspace(haystack[start - 1]) || std::iswpunct(haystack[start - 1]) ||
                 // after a hard coded newline or tab
                 (start >= 2 &&
                  (haystack[start - 1] == L'n' || haystack[start - 1] == L'r' ||
                   haystack[start - 1] == L't') &&
                  haystack[start - 2] == L'\\')))
                {
                return start;
                }

            ++start;
            }
        return T::npos;
        }

    /// Case-insensitive predicate for comparing basic_string types against
    /// strings in a @c std::map (usually in conjunction with @c std::find_if()).
    template<typename TKey, typename TVal>
    class equal_basic_string_i_compare_map
        {
      public:
        /// @brief Constructor.
        /// @param key The value to compare against.
        explicit equal_basic_string_i_compare_map(TKey key) noexcept : m_key(std::move(key)) {}

        /// @returns @c true if initial value is the same the pair's key.
        /// @param val The pair to compare against.
        [[nodiscard]]
        bool operator()(const std::pair<TKey, TVal>& val) const noexcept
            {
            return (string_util::stricmp(val.first.c_str(), m_key.c_str()) == 0);
            }

      private:
        TKey m_key;
        };

    template<typename T>
    class less_basic_string_compare
        {
      public:
        [[nodiscard]]
        bool operator()(const T& left, const T& right) const
            {
            return (left.compare(right) < 0);
            }
        };

    template<typename T>
    class less_basic_string_i_compare
        {
      public:
        [[nodiscard]]
        bool operator()(const T& left, const T& right) const noexcept
            {
            return (string_util::stricmp(left.c_str(), right.c_str()) < 0);
            }
        };

    template<typename T>
    class less_string_natural_order_i_compare
        {
      public:
        [[nodiscard]]
        bool operator()(const T* left, const T* right) const
            {
            return (string_util::strnatordncasecmp(left, right) < 0);
            }
        };

    /** @brief Performs a heuristic check on a buffer to see if it's 7-bit or 8-bit ASCII.
        @param buffer The buffer to review (should be either char or unsigned char).
        @param buffSize The byte count of @c buffer.
        @returns @c true if @c buffer is some sort of ASCII, false if possibly UTF-16.
        @note The larger the buffer, the more accurate the check will be.*/
    template<typename T>
    [[nodiscard]]
    bool is_extended_ascii(const T* buffer, const size_t buffSize) noexcept
        {
        static_assert(std::is_same_v<T, char> || std::is_same_v<T, unsigned char>);
        if (!buffer || buffSize == 0)
            {
            return false;
            }

        size_t spaceCount{ 0 };

        // go up to last 2 characters to determine what we are looking at
        for (size_t i = 0; i < buffSize - 1; ++i)
            {
            // embedded NULL terminator midstream? More than likely that this is some variant of UTF
            // and this char sequence we are on is a Unicode char with a zero low or high bit
            if (buffer[i] == 0 && buffer[i + 1] != 0)
                {
                return false;
                }
            if (std::isspace(static_cast<unsigned char>(buffer[i])))
                {
                ++spaceCount;
                }
            }

        // No embedded zeros, so this likely is 7-bit/8-bit,
        // unless a large buffer with no ASCII spaces. No spaces
        // in a large block of text is not normal for non-CJK text.
        return (buffSize > 128) ? (spaceCount > 0) : true;
        }

    /// @brief Trims left side of @c str (in-place).
    /// @param[in,out] str The string to trim.
    /// @note This assumes the string type uses wchar_t as its datatype.
    template<typename string_typeT>
    void ltrim(string_typeT& str)
        {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](wchar_t ch) noexcept
                                            { return !std::iswspace(ch); }));
        }

    /// @brief Trims right side of @c str (in-place).
    /// @param[in,out] str The string to trim.
    /// @note This assumes the string type uses @c wchar_t as its datatype.
    template<typename string_typeT>
    void rtrim(string_typeT& str)
        {
        str.erase(std::find_if(str.rbegin(), str.rend(),
                               [](wchar_t ch) noexcept { return !std::iswspace(ch); })
                      .base(),
                  str.end());
        }

    /// @brief Trims left and right sides of @c str (in-place).
    /// @param[in,out] str The string to trim.
    /// @note This assumes the string type uses @c wchar_t as its datatype.
    template<typename string_typeT>
    void trim(string_typeT& str)
        {
        ltrim(str);
        rtrim(str);
        }

    /// @brief "Trims" left and right sides of a wstring_view.
    /// @param str The string to trim.
    /// @returns A subset of the view, with leading and trailing spaces excluded.
    inline std::wstring_view trim_view(std::wstring_view str)
        {
        if (str.empty())
            {
            return str;
            }
        auto left = str.cbegin();
        for (;; ++left)
            {
            if (left == str.cend())
                {
                return {};
                }
            if (std::iswspace(*left) == 0)
                {
                break;
                }
            }
        auto right = str.cend() - 1;
        for (; right > left && (std::iswspace(*right) != 0); --right)
            ;
        return str.substr(std::distance(str.cbegin(), left), std::distance(left, right) + 1);
        }

    /// @brief Trims punctuation from left side of @c str (in-place).
    /// @param[in,out] str The string to trim.
    /// @note This assumes the string type uses @c wchar_t as its datatype.
    template<typename string_typeT>
    void ltrim_punct(string_typeT& str)
        {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(),
                                            [](wchar_t ch) { return !std::iswpunct(ch); }));
        }

    /// @brief Trims punctuation from right side of @c str (in-place).
    /// @param[in,out] str The string to trim.
    /// @note This assumes the string type uses @c wchar_t as its datatype.
    template<typename string_typeT>
    void rtrim_punct(string_typeT& str)
        {
        str.erase(
            std::find_if(str.rbegin(), str.rend(), [](wchar_t ch) { return !std::iswpunct(ch); })
                .base(),
            str.end());
        }

    /// @brief Trims punctuation from left and right sides of @c str (in-place).
    /// @param[in,out] str The string to trim.
    /// @note This assumes the string type uses @c wchar_t as its datatype.
    template<typename string_typeT>
    void trim_punct(string_typeT& str)
        {
        ltrim_punct(str);
        rtrim_punct(str);
        }

    /** @brief Determines if a character is one of a list of characters.
        @param character The character to review.
        @param charString The list of characters to compare against.
        @returns @c true if the character of one of the list of characters.*/
    template<typename Tchar_type>
    [[nodiscard]]
    constexpr bool is_one_of(const Tchar_type character, const Tchar_type* charString) noexcept
        {
        if (charString == nullptr)
            {
            return false;
            }
        while (*charString)
            {
            if (character == charString[0])
                {
                return true;
                }
            ++charString;
            }
        return false;
        }

    /// @brief Removes all whitespace from a string
    /// @param text The text to review.
    /// @returns The text with all whitespace removed.
    template<typename T>
    [[nodiscard]]
    T remove_all_whitespace(const T& text)
        {
        T tempText = text;
        for (typename T::size_type i = 0; i < tempText.length(); /*in loop*/)
            {
            if (tempText[i] == 10 || tempText[i] == 13 || tempText[i] == 9)
                {
                tempText.erase(i, 1);
                }
            else
                {
                ++i;
                }
            }
        return tempText;
        }

    /// @brief Removes all instances of a character from a string.
    /// @param[in,out] text The text to remove the character from.
    /// @param charToRemove The character to remove.
    template<typename T>
    void remove_all(T& text, const typename T::traits_type::char_type charToRemove)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(charToRemove, start);
            if (start == T::npos)
                {
                return;
                }
            text.erase(start, 1);
            }
        }

    /** @brief Replace all instances of a character in a string.
        @param text The text to replace items in.
        @param charToReplace The character to replace.
        @param replacementChar The character to replace @c charToReplace with.*/
    template<typename T>
    void replace_all(T& text, const typename T::traits_type::char_type charToReplace,
                     const typename T::traits_type::char_type replacementChar)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(charToReplace, start);
            if (start == T::npos)
                {
                return;
                }
            text[start++] = replacementChar;
            }
        }

    /** @brief Replace all instances of a substring in a string.
        @param text The text to replace items in.
        @param textToReplace The text to replace.
        @param textToReplaceLength The length of @c textToReplace.
        @param replacementText The text to replace @c textToReplace with.*/
    template<typename T>
    void replace_all(T& text, const typename T::traits_type::char_type* textToReplace,
                     const size_t textToReplaceLength,
                     const typename T::traits_type::char_type* replacementText)
        {
        if (!textToReplace || !replacementText)
            {
            return;
            }
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(textToReplace, start);
            if (start == T::npos)
                {
                return;
                }
            text.replace(start, textToReplaceLength, replacementText);
            }
        }

    /** @brief Replace all instances of a substring in a string.
        @param text The text to replace items in.
        @param textToReplace The text to replace.
        @param replacementText The text to replace @c textToReplace with.*/
    template<typename T>
    void replace_all(T& text, const T& textToReplace, const T& replacementText)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(textToReplace, start);
            if (start == T::npos)
                {
                return;
                }
            text.replace(start, textToReplace.length(), replacementText);
            }
        }

    /** @brief Replace all instances of a substring in a string (searching by whole word).
        @param text The text to replace items in.
        @param textToReplace The text to replace.
        @param replacementText The text to replace @c textToReplace with.
        @param index Where to start the search within @c text.*/
    template<typename T>
    void replace_all_whole_word(T& text, const T& textToReplace, const T& replacementText,
                                const size_t index = 0)
        {
        if (textToReplace.length() == 0 || replacementText.length() == 0)
            {
            return;
            }
        auto start{ index };

        while (start != T::npos)
            {
            start = find_whole_word(text, textToReplace, start);
            if (start == T::npos)
                {
                break;
                }
            text.replace(start, textToReplace.length(), replacementText);
            start += replacementText.length();
            }
        }

    /** @brief Strips extraneous spaces/tabs/carriage returns from a block of text so
            that there isn't more than one space consecutively.
        @param[in,out] text The text to remove extra spaces from.
        @returns The number of removed spaces.*/
    template<typename string_typeT>
    size_t remove_extra_spaces(string_typeT& text)
        {
        size_t numberOfSpacesRemoved = 0;

        if (!text.length())
            {
            return 0;
            }
        bool alreadyHasSpace = true;
        // make sure that there is only a space between each word
        for (unsigned int i = 0; i < text.length(); ++i)
            {
            // if this is the first space found after the current
            // word then it's OK--just leave it
            if (std::iswspace(text[i]) && !alreadyHasSpace)
                {
                alreadyHasSpace = true;
                }
            // this is extra space right after another--get rid of it
            else if (std::iswspace(text[i]) && alreadyHasSpace)
                {
                // make sure it isn't a Windows \r\n
                if (i && !(text[i - 1] == 13 && text[i] == 10))
                    {
                    ++numberOfSpacesRemoved;
                    text.erase(i--, 1);
                    }
                }
            // we are starting another word--reset
            else if (!std::iswspace(text[i]))
                {
                alreadyHasSpace = false;
                }
            }

        return numberOfSpacesRemoved;
        }

    /** @brief Removes blank lines from a block of text (in-place).
        @param text The text to have blank lines removed from.
        @returns The number of characters (not lines) removed from the block.*/
    template<typename string_typeT>
    size_t remove_blank_lines(string_typeT& text)
        {
        size_t numberOfLinesRemoved = 0;

        if (!text.length())
            {
            return 0;
            }
        bool alreadyHasNewLine = true;
        // make sure that there is only a space between each word
        for (unsigned int i = 0; i < text.length(); ++i)
            {
            // if this is the first space found after the current
            // word then it's OK--just leave it
            if (is_either<wchar_t>(static_cast<wchar_t>(text[i]), 10, 13) && !alreadyHasNewLine)
                {
                alreadyHasNewLine = true;
                }
            // this is extra space right after another--get rid of it
            else if (is_either<wchar_t>(static_cast<wchar_t>(text[i]), 10, 13) && alreadyHasNewLine)
                {
                // make sure it isn't a Windows \r\n
                if (i && !(text[i - 1] == 13 && text[i] == 10))
                    {
                    ++numberOfLinesRemoved;
                    text.erase(i--, 1);
                    }
                }
            // we are starting another word--reset
            else if (!is_either<wchar_t>(static_cast<wchar_t>(text[i]), 10, 13))
                {
                alreadyHasNewLine = false;
                }
            }

        return numberOfLinesRemoved;
        }

    /// @brief `std::less`-like functor to compare @c std::wstring case insensitively.
    /// @details This is useful as a predicate for `std::map` and `std::set`.
    class string_no_case_less
        {
      public:
        /// @private
        [[nodiscard]]
        bool operator()(const std::wstring_view lhs, const std::wstring_view rhs) const noexcept
            {
            return string_util::stricmp(lhs.data(), rhs.data()) < 0;
            }

        /// @private
        [[nodiscard]]
        bool operator()(const std::wstring& lhs, const std::wstring& rhs) const noexcept
            {
            return string_util::stricmp(lhs.c_str(), rhs.c_str()) < 0;
            }
        };

    /** @brief Converts strings to double values, but also takes into account
            ranges (returning the average).
        @details For example, a string like "5-8" will return 6.5. Hyphens and colons are seen as
       range separators.
        @param nptr The string to parse.
        @param[out] endptr Where the parsing of @c nptr stopped.
        @returns The double value that was parsed.*/
    template<typename Tchar_type>
    [[nodiscard]]
    double strtod_ex(const Tchar_type* nptr, Tchar_type** endptr) noexcept
        {
        if (nptr == nullptr)
            {
            *endptr = nullptr;
            return 0.0;
            }
        const wchar_t sepStr[3] = { 0x2D, 0x3A, 0 };
        const Tchar_type* separator = string_util::strcspn_pointer<Tchar_type>(nptr, sepStr, 2);
        // if there is no hyphen or there is one, but it is at the end then just call strtod
        if (separator == nullptr || *(separator + 1) == 0)
            {
            return std::wcstod(nptr, endptr);
            }

        const double d1 = std::wcstod(nptr, endptr);
        std::advance(separator, 1);
        const double d2 = std::wcstod(separator, endptr);
        return (d1 + d2) / static_cast<double>(2);
        }

    /** @brief Converts a full-width number/English letter/various symbols
            into its "narrow" counterpart.
        @param ch The character to convert.
        @returns The narrow version of a character, or the character if not full-width.
        @todo Add support for wide Japanese/Korean characters.*/
    [[nodiscard]]
    inline constexpr wchar_t full_width_to_narrow(const wchar_t ch) noexcept
        {
        // clang-format off
        return
            // lower area of Unicode, most likely branch
            (ch < 65'000) ? ch :
            (ch >= 65'281 && ch <= 65'374) ? (ch - 65'248) :
            // cent and pound sterling
            (ch >= 65'504 && ch <= 65'505) ? (ch - 65'342) :
            // Yen
            (ch == 65'509) ? 165 :
            // Not
            (ch == 65'506) ? 172 :
            // macron
            (ch == 65'507) ? 175 :
            // broken bar
            (ch == 65'508) ? 166 :
            ch;
        // clang-format on
        }
    } // namespace string_util

/** @}*/

#endif // QUNEIFORM_STRING_UTIL_H
