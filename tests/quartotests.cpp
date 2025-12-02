#include "../src/quarto_review.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

// NOLINTBEGIN
using namespace i18n_check;
using namespace Catch::Matchers;

TEST_CASE("Smart Quotes Extraction", "[quarto][malformed]")
    {
    SECTION("Simple apostrophe word aren’t")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
This is a test with aren’t in the middle.
)";

        qr(code, L"test.qmd");

        const auto& issues = qr.get_smart_quotes();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"aren’t");
        }

    SECTION("Rock’n’roll style multiple smart quotes")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Genre: rock’n’roll music.
)";

        qr(code, L"rock.qmd");

        const auto& issues = qr.get_smart_quotes();
        REQUIRE(issues.size() == 2);
        CHECK(issues[0].m_string == std::wstring{ L"rock’n" });
        CHECK(issues[1].m_string == std::wstring{ L"’roll" });
        }

    SECTION("Quoted entity “quoted-word”")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Here is “quoted-word” inside text.
)";

        qr(code, L"quoted.qmd");

        const auto& issues = qr.get_smart_quotes();
        REQUIRE(issues.size() == 2);
        CHECK(issues[0].m_string == std::wstring{ L"“quoted-word" });
        CHECK(issues[1].m_string == std::wstring{ L"”" });
        }

    SECTION("Ignore lonely smart quote inside straight quotes")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
"“"
'’'
But detect this one: “kept”
)";

        qr(code, L"ignore.qmd");

        const auto& issues = qr.get_smart_quotes();
        REQUIRE(issues.size() == 2);
        CHECK(issues[0].m_string == std::wstring{ L"“kept" });
        CHECK(issues[1].m_string == std::wstring{ L"”" });
        }

    SECTION("Mixed line with multiple entities")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Test “cat” and aren’t and “rock’n’roll”.
)";

        qr(code, L"multiple.qmd");

        const auto& issues = qr.get_smart_quotes();
        REQUIRE(issues.size() == 7);

        CHECK(issues[0].m_string == std::wstring{ L"“cat" });
        CHECK(issues[1].m_string == std::wstring{ L"”" });
        CHECK(issues[2].m_string == std::wstring{ L"aren’t" });
        CHECK(issues[3].m_string == std::wstring{ L"“rock" });
        CHECK(issues[4].m_string == std::wstring{ L"’n" });
        CHECK(issues[5].m_string == std::wstring{ L"’roll" });
        CHECK(issues[6].m_string == std::wstring{ L"”" });
        }

    SECTION("Code blocks should be ignored")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(```
This “should” not be detected.
```
Outside “yes”.
)";

        qr(code, L"code.qmd");

        const auto& issues = qr.get_smart_quotes();
        REQUIRE(issues.size() == 2);
        CHECK(issues[0].m_string == std::wstring{ L"“yes" });
        CHECK(issues[1].m_string == std::wstring{ L"”" });
        }

    SECTION("Suppression blocks should be ignored")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
<!-- quneiform-suppress-begin! -->
Ignore this “bad”.
<!-- quneiform-suppress-end! -->

But detect “good”.
)";

        qr(code, L"suppress.qmd");

        const auto& issues = qr.get_smart_quotes();
        REQUIRE(issues.size() == 2);
        CHECK(issues[0].m_string == L"“good");
        CHECK(issues[1].m_string == L"”");
        }
    }

// NOLINTEND
