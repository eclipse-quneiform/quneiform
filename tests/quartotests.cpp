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
<!-- quneiform-suppress-begin -->
Ignore this “bad”.
<!-- quneiform-suppress-end -->

But detect “good”.
)";

        qr(code, L"suppress.qmd");

        const auto& issues = qr.get_smart_quotes();
        REQUIRE(issues.size() == 2);
        CHECK(issues[0].m_string == L"“good");
        CHECK(issues[1].m_string == L"”");
        }
    }

TEST_CASE("Range dash issues (numeric and alphabetic)", "[quarto][style][ranges]")
    {
    SECTION("Numeric range uses hyphen instead of en dash")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
The score ranges from 0-100 on the scale.
)";

        qr(code, L"numeric.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"0-100");
        }

    SECTION("Page range shortened MLA style with hyphen")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
See pages 141-48 for details.
)";

        qr(code, L"pages.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"141-48");
        }

    SECTION("Alphabetic range single letters only")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Appendices A-Z are included.
)";

        qr(code, L"alpha.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"A-Z");
        }

    SECTION("Do not flag multi-letter hyphenated words")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
This is an X-ray machine and an E-mail address.
)";

        qr(code, L"hyphenated.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.empty());
        }

    SECTION("Multiple ranges in one line")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Ranges include 1-5, 10-20, and sections B-D.
)";

        qr(code, L"multiple.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.size() == 3);
        CHECK(issues[0].m_string == L"1-5");
        CHECK(issues[1].m_string == L"10-20");
        CHECK(issues[2].m_string == L"B-D");
        }

    SECTION("Ranges inside code blocks should be ignored")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
```
for (int i = 0-10; i++) {}
```
Outside range 5-7 should be flagged.
)";

        qr(code, L"code.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"5-7");
        }

    SECTION("Ranges inside suppression blocks should be ignored")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
<!-- quneiform-suppress-begin -->
Ignore 100-200 and A-Z here.
<!-- quneiform-suppress-end -->

But detect 3-4.
)";

        qr(code, L"suppress.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"3-4");
        }

    SECTION("Ranges inside math blocks should be ignored")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Inline math $x = 1-2$ should be ignored.
Display math:
$$
y = 3-4
$$
But text range 6-8 should be flagged.
)";

        qr(code, L"math.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"6-8");
        }

    SECTION("Correct en dash should not be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Valid ranges: 0–100 and A–Z.
)";

        qr(code, L"correct.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.empty());
        }

    SECTION("K-12 educational range should use en dash")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
This program serves K-12 students.
)";

        qr(code, L"k12.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"K-12");
        }

    SECTION("Correct K–12 with en dash should not be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
This program serves K–12 students.
)";

        qr(code, L"k12-ok.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.empty());
        }

    SECTION("Phone numbers like 555-4777 should NOT be flagged as ranges")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Call 555-4777 for information.
Also valid range 5-7 should be flagged.
)";

        qr(code, L"phone.qmd");

        const auto& issues = qr.get_range_dash_issues();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"5-7");
        }
    }


// NOLINTEND
