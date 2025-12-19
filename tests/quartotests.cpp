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

TEST_CASE("Malformed image links", "[quarto][malformed][images]")
    {
    SECTION("Quoted image path should be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Here is an image:
![Alt text]("images/cat.png")
)";

        qr(code, L"quoted-img.qmd");

        const auto& issues = qr.get_malformed_image_links();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"![Alt text](\"images/cat.png\")");
        }

    SECTION("Missing closing bracket in alt text")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Broken image:
![Alt text(images/cat.png)
)";

        qr(code, L"alt-missing.qmd");

        const auto& issues = qr.get_malformed_image_links();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == std::wstring{ L"![Alt text(images/cat.png)" });
        }

    SECTION("Missing closing parenthesis in image path")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Broken image:
![Alt text](images/cat.png
)";

        qr(code, L"path-missing.qmd");

        const auto& issues = qr.get_malformed_image_links();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"![Alt text](images/cat.png");
        }

    SECTION("Multiple malformed images in one document")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
First:
![One]("a.png")

Second:
![Two](b.png

Third:
![Three]('c.png')
)";

        qr(code, L"multi.qmd");

        const auto& issues = qr.get_malformed_image_links();
        REQUIRE(issues.size() == 3);
        CHECK(issues[0].m_string == L"![One](\"a.png\")");
        CHECK(issues[1].m_string == L"![Two](b.png");
        CHECK(issues[2].m_string == L"![Three]('c.png')");
        }

    SECTION("Valid image syntax should not be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
![Alt text](images/cat.png)
)";

        qr(code, L"valid.qmd");

        const auto& issues = qr.get_malformed_image_links();
        REQUIRE(issues.empty());
        }

    SECTION("Images inside code blocks should be ignored")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(

Outside:
![Good](good.png)
)";

        qr(code, L"code.qmd");

        const auto& issues = qr.get_malformed_image_links();
        REQUIRE(issues.empty());
        }

    SECTION("Images inside suppression blocks should be ignored")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
<!-- quneiform-suppress-begin -->
![Alt]("ignored.png")
<!-- quneiform-suppress-end -->

![Alt]("flagged.png")
)";

        qr(code, L"suppress.qmd");

        const auto& issues = qr.get_malformed_image_links();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"![Alt](\"flagged.png\")");
        }

    SECTION("Images inside math blocks should be ignored")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
$$
![Alt]("math.png")
$$

Text image:
![Alt]("text.png")
)";

        qr(code, L"math.qmd");

        const auto& issues = qr.get_malformed_image_links();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"![Alt](\"text.png\")");
        }

    SECTION("Reference-style images should not be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
Here is a reference-style image:

![Alt text][fig-cat]

And its definition:
[fig-cat]: images/cat.png

Collapsed reference form:
![Another alt][]
[Another alt]: images/dog.png
)";

        qr(code, L"reference-image.qmd");

        const auto& issues = qr.get_malformed_image_links();
        REQUIRE(issues.empty());
        }
    }

TEST_CASE("Suspicious absolute paths in Markdown destinations",
          "[quarto][portability][absolute-paths]")
    {
    SECTION("Site-root relative image path should be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
![Alt](/images/cat.png)
)";

        qr(code, L"root-image.qmd");

        const auto& issues = qr.get_absolute_path_links();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"![Alt](/images/cat.png)");
        }

    SECTION("Site-root relative link path should be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
[Doc](/docs/intro.qmd)
)";

        qr(code, L"root-link.qmd");

        const auto& issues = qr.get_absolute_path_links();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"[Doc](/docs/intro.qmd)");
        }

    SECTION("Windows drive path using C:/ should be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
[Help](C:/projects/help.md)
)";

        qr(code, L"win-drive.qmd");

        const auto& issues = qr.get_absolute_path_links();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"[Help](C:/projects/help.md)");
        }

    SECTION("Windows drive path prefixed with slash should be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
[Help](/C:/projects/help.md)
)";

        qr(code, L"win-drive-slash.qmd");

        const auto& issues = qr.get_absolute_path_links();
        REQUIRE(issues.size() == 1);
        CHECK(issues[0].m_string == L"[Help](/C:/projects/help.md)");
        }

    SECTION("Protocol-relative URLs should NOT be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
![Alt](//cdn.example.com/image.png)
)";

        qr(code, L"cdn.qmd");

        const auto& issues = qr.get_absolute_path_links();
        REQUIRE(issues.empty());
        }

    SECTION("Anchor-only root paths should NOT be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
[Jump](/#section)
)";

        qr(code, L"anchor.qmd");

        const auto& issues = qr.get_absolute_path_links();
        REQUIRE(issues.empty());
        }

    SECTION("Relative paths should NOT be flagged")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
![Alt](images/cat.png)
[Doc](../docs/intro.qmd)
)";

        qr(code, L"relative.qmd");

        const auto& issues = qr.get_absolute_path_links();
        REQUIRE(issues.empty());
        }

    SECTION("Mixed document flags only absolute paths")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
![Good](images/cat.png)
![Bad](/images/dog.png)
[AlsoBad](C:/docs/help.md)
)";

        qr(code, L"mixed.qmd");

        const auto& issues = qr.get_absolute_path_links();
        REQUIRE(issues.size() == 2);
        CHECK(issues[0].m_string == L"![Bad](/images/dog.png)");
        CHECK(issues[1].m_string == L"[AlsoBad](C:/docs/help.md)");
        }

    SECTION("Absolute paths inside code blocks should be ignored")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"qmd(
```
![Alt](/images/cat.png)
[Help](C:/docs/help.md)
```
)qmd";

        qr(code, L"code.qmd");

        const auto& issues = qr.get_absolute_path_links();
        REQUIRE(issues.empty());
        }

    SECTION("Absolute paths inside suppression blocks should be ignored")
        {
        quarto_review qr;
        qr.set_style(check_malformed_strings);

        const wchar_t* code = LR"(
<!-- quneiform-suppress-begin -->
![Alt](/images/cat.png)
[Help](C:/docs/help.md)
<!-- quneiform-suppress-end -->
)";

        qr(code, L"suppress.qmd");

        const auto& issues = qr.get_absolute_path_links();
        REQUIRE(issues.empty());
        }
    }

// NOLINTEND
