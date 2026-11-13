#include "../src/cpp_i18n_review.h"
#include "../src/i18n_review.h"
#include "../src/i18n_string_util.h"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iomanip>
#include <set>
#include <sstream>

// NOLINTBEGIN
using namespace i18n_string_util;
using namespace i18n_check;
using namespace Catch::Matchers;

// clang-format off
TEST_CASE("Articles", "[i18nreview]")
    {
    i18n_check::cpp_i18n_review reviewer{ false };
    SECTION("Printf")
        {
        CHECK(reviewer.is_string_article_issue(L"Find the %d"));
        }
    SECTION("Modern formatting")
        {
        CHECK(reviewer.is_string_article_issue(L"Find the {0}"));
        }
    SECTION("Positionals")
        {
        CHECK(reviewer.is_string_article_issue(L"Find the %1"));
        }
    }

TEST_CASE("Pronouns", "[i18nreview]")
    {
    i18n_check::cpp_i18n_review reviewer{ false };
    SECTION("English")
        {
        CHECK(reviewer.is_string_pronoun(L"HE"));
        CHECK(reviewer.is_string_pronoun(L" she"));
        CHECK(reviewer.is_string_pronoun(L"they  "));
        }
    }

TEST_CASE("Ambiguous", "[i18nreview]")
    {
    i18n_check::cpp_i18n_review reviewer{ false };
    SECTION("Abbreviations")
        {
        CHECK(reviewer.is_string_ambiguous(L"Std. dev. and corr. matr"));
        CHECK_FALSE(reviewer.is_string_ambiguous(L"business.com and company.org."));
        }
    SECTION("Printf")
        {
        CHECK(reviewer.is_string_ambiguous(L"%s is not a %d"));
        // although meets criteria, this is generally self explanatory
        CHECK_FALSE(reviewer.is_string_ambiguous(L"%d of %lu"));
        CHECK_FALSE(reviewer.is_string_ambiguous(L"Page %lu"));
        CHECK_FALSE(reviewer.is_string_ambiguous(L"Row %d"));
        CHECK_FALSE(reviewer.is_string_ambiguous(L"..."));
        CHECK_FALSE(reviewer.is_string_ambiguous(L"Alternative:<br/>"));
        }
    SECTION("Positionals")
        {
        std::wstring str{ L"%1 of %2" };
        i18n_string_util::remove_positional_commands(str);
        CHECK(str ==  std::wstring{ L" of " });
        CHECK(reviewer.is_string_ambiguous(L"%1 is not a %2"));
        CHECK_FALSE(reviewer.is_string_ambiguous(L"%1 of %2"));
        }
    SECTION("Punctuation")
        {
        CHECK(reviewer.is_string_ambiguous(L"Z+X+S1-2"));
        // not enough to be a problem
        CHECK_FALSE(reviewer.is_string_ambiguous(L"&Redo\\tCtrl+Shift+Z"));
        }
    SECTION("Acronyms")
        {
        CHECK(reviewer.is_string_ambiguous(L"FORWARD"));
        // known abbreviations
        CHECK_FALSE(reviewer.is_string_ambiguous(L"URL:"));
        }
    }

TEST_CASE("untranslatable web code", "[i18nreview]")
    {
    i18n_check::cpp_i18n_review reviewer{ false };
    SECTION("Long JavaScript")
        {
        // longer than 200 characters, with English words in comments
        CHECK(reviewer.is_untranslatable_string(LR"(
  // per-SVG state that is swapped in and out along with a page's filter choice
  const choiceKeys = ['svg', 'alt', 'prepared', 'scopes', 'spotBound', 'zoomBound', 'counters',
                      'countRun', 'inked'];
  function svgState(page, container) {
    const svgs = container.querySelectorAll('.page-svg');
    const state = { svg: svgs[0] || null };
    return state;
  })", true).first);
        CHECK(reviewer.is_untranslatable_string(LR"(
  function togglePresenting() {
    if (!document.fullscreenEnabled) return;
    if (document.fullscreenElement) {
      document.exitFullscreen();
    } else {
      root.requestFullscreen().catch(function() {});
    }
  })", true).first);
        }
    SECTION("Long CSS")
        {
        CHECK(reviewer.is_untranslatable_string(LR"(
.dash-toolbar { position: sticky; top: 0; display: flex; gap: 8px; padding: 4px 8px; }
.dash-card { border: 1px solid var(--border); border-radius: 6px; background: var(--card); }
.dash-card:hover { outline: 2px solid var(--accent); cursor: pointer; })", true).first);
        }
    SECTION("Long messages are still translatable")
        {
        CHECK_FALSE(reviewer.is_untranslatable_string(
            L"The dashboard could not be saved because the destination folder is read-only; "
            "choose another folder (or change its permissions) and then try again. If the "
            "problem continues, contact your system administrator for further assistance.", true).first);
        CHECK_FALSE(reviewer.is_untranslatable_string(
            L"Enter the name of the dataset in {0}; then select the columns to include in {1}; "
            "finally, choose the variables for {2}; and press OK to build the report with all "
            "of the selected options applied to the current project.", true).first);
        }
    SECTION("MIME type with suffix")
        {
        CHECK(reviewer.is_untranslatable_string(L"image/svg+xml", true).first);
        }
    SECTION("CSS font family lists")
        {
        CHECK(reviewer.is_untranslatable_string(
            L"system-ui, -apple-system, BlinkMacSystemFont, Roboto, 'Helvetica Neue', Arial, sans-serif", true).first);
        CHECK(reviewer.is_untranslatable_string(L"', sans-serif", true).first);
        CHECK(reviewer.is_untranslatable_string(L"Georgia, 'Times New Roman', serif", true).first);
        CHECK(reviewer.is_untranslatable_string(L"Consolas, 'Courier New', monospace", true).first);
        CHECK_FALSE(reviewer.is_untranslatable_string(L"Choose a font, such as serif", true).first);
        }
    SECTION("Namespaced XML attribute")
        {
        CHECK(reviewer.is_untranslatable_string(L"xlink:href", true).first);
        CHECK(reviewer.is_untranslatable_string(L"xml:space", true).first);
        CHECK_FALSE(reviewer.is_untranslatable_string(L"Note: file saved", true).first);
        }
    SECTION("HTML attribute fragments")
        {
        CHECK(reviewer.is_untranslatable_string(
            LR"( data-orient=\"%s\" style=\"--svg-w:%d;--svg-h:%d\")", true).first);
        CHECK(reviewer.is_untranslatable_string(
            LR"(class=\"dash-card\" aria-hidden=\"true\")", true).first);
        CHECK_FALSE(reviewer.is_untranslatable_string(
            LR"(Set the value to class=\"dash-card\" for the new card)", true).first);
        }
    SECTION("Keyboard keys in HTML markup")
        {
        CHECK(reviewer.is_untranslatable_string(
            LR"(<dt><kbd>&larr;</kbd> <kbd>&rarr;</kbd></dt><dd>%s</dd>\n<dt><kbd>Home</kbd> <kbd>End</kbd></dt><dd>%s</dd>\n<dt><kbd>1</kbd> &ndash; <kbd>9</kbd></dt><dd>%s</dd>\n<dt><kbd>G</kbd></dt><dd>%s</dd>\n)", true).first);
        CHECK(reviewer.is_untranslatable_string(
            LR"(<dt><kbd>Space</kbd> <kbd>Enter</kbd></dt><dd>%s</dd>\n<dt><kbd>Esc</kbd></dt><dd>%s</dd>\n)", true).first);
        // real text around the keys is still translatable
        CHECK_FALSE(reviewer.is_untranslatable_string(
            LR"(<p>Press <kbd>Ctrl</kbd>+<kbd>S</kbd> to save the current dashboard</p>)", true).first);
        }
    }

TEST_CASE("Raw JavaScript string returned from function", "[i18nreview]")
    {
    i18n_check::cpp_i18n_review cpp{ false };
    const wchar_t* code = LR"(wxString GetScript()
    {
    return LR"JS(
  // shows the version of a page matching its filter groups' choices
  function showPageChoice(page) {
    if (!page.filters) return;
    page.filters.forEach(function(filter) {
      if (!filter.group) return;
      filter.group.querySelectorAll('input[type="radio"]').forEach(function(radio, i) {
        radio.checked = i === filter.index;
      });
    });
  }
)JS";
    })";
    cpp(code, L"");
    cpp.review_strings([](size_t) {}, [](size_t, const std::filesystem::path&) { return true; });
    CHECK(cpp.get_not_available_for_localization_strings().empty());
    CHECK(cpp.get_internal_strings().size() == 1);
    }

TEST_CASE("untranslatable", "[i18nreview]")
    {
    i18n_check::cpp_i18n_review reviewer{ false };
    CHECK(reviewer.is_untranslatable_string(L"application/x-photoshop-style-library", false).first);
    CHECK(reviewer.is_untranslatable_string(L"image/webp", false).first);
    CHECK(reviewer.is_untranslatable_string(L"ms-appdata", false).first);
    CHECK(reviewer.is_untranslatable_string(L"&perms=write&frob=", false).first);
    CHECK(reviewer.is_untranslatable_string(L"Content-Type: multipart/form-data; boundary=EBA799EB-D9A2-472B-AE86-568D4645707E\r\n", false).first);
    CHECK(reviewer.is_untranslatable_string(L"Content-Disposition: form-data; name=\"photo\"; filename=\"", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"<No Name Specified>", false).first);
    // percentages can be translatable as the ordering of the number and % can be changed
    CHECK_FALSE(reviewer.is_untranslatable_string(L"100%", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"{n}%", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"%d%", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"%d%%", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"%d (%d%%)", false).first);
    // HTML anchors
    CHECK(reviewer.is_untranslatable_string(L"#something", false).first);
    CHECK(reviewer.is_untranslatable_string(L"#Something-else", false).first);
    CHECK(reviewer.is_untranslatable_string(L"#something-else", false).first);
    CHECK(reviewer.is_untranslatable_string(L"#somethingElse", false).first);
    // CSS
    CHECK(reviewer.is_untranslatable_string(L"height:%dpx;", false).first);
    // long, multiline embedded stylesheet (should not be dismissed just because
    // it is longer than the "probably a real message" length heuristic)
    CHECK(reviewer
              .is_untranslatable_string(
                  LR"(<style>
.hl-swatch { display: inline-block; width: 1.4em; height: 0.9em; margin-right: 0.3em;
    border: 1px solid var(--border-color); border-radius: 3px; vertical-align: middle; }
.legend-card { font-size: 0.9em; width: fit-content; max-width: 100%;
    border: 1px solid var(--border-color);
    transition: border-color 0.4s ease, box-shadow 0.4s ease; }
.legend-card:hover { border-color: color-mix(in srgb, var(--banner-color) 50%, CanvasText);
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1); }
.legend-card .report-banner-content { line-height: 1.5; }
)",
                  false)
              .first);
    // formatted percentage symbol should be translatable
    CHECK_FALSE(reviewer.is_untranslatable_string(L"%s%% (%s)", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"%s%%\n(%s)", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"%s%%\\n(%s)", false).first);
    // command line options
    CHECK(reviewer.is_untranslatable_string(L"--no-ext-diff", false).first);
    CHECK(reviewer.is_untranslatable_string(L"--no-ext", false).first);
    CHECK(reviewer.is_untranslatable_string(L"--date=raw", false).first);
    CHECK(reviewer.is_untranslatable_string(L"%1> rm -rf %2", false).first);
    CHECK(reviewer.is_untranslatable_string(L"rm -rf %2", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"dorm room", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"p-value", false).first);
    // JSON syntax
    CHECK(reviewer.is_untranslatable_string(L"ui.bold-header-row", false).first);
    CHECK(reviewer.is_untranslatable_string(L"last-bar-brackets[", false).first);
    CHECK(reviewer.is_untranslatable_string(L"].end-block", false).first);
    CHECK(reviewer.is_untranslatable_string(LR"(, \"question-brackets\": )", false).first);
    CHECK(reviewer.is_untranslatable_string(LR"(, \"label\": {\"text\": \")", false).first);
    CHECK(reviewer.is_untranslatable_string(LR"(color: rgb(%u, %u, %u);)", false).first);
    CHECK(reviewer.is_untranslatable_string(LR"({\"name\": \"%s\", "                 L"\"section\": {}, \"filter\": {}, \"filter-and\": [], \"filter-or\": [], "                 L"\"subsets\": [], \"pivots\": [], \"merges\": [], "                 L"\"recode-re\": [],")", false).first);
    CHECK(reviewer.is_untranslatable_string(LR"(\"categorical-columns\": [], \"date-columns\": [], )", false).first);
    CHECK(reviewer.is_untranslatable_string(LR"(\", \"canvas-margins\": [], \"padding\": [], \"outline\)", false).first);
    CHECK(reviewer.is_untranslatable_string(LR"({\"name\": \"%s\", "                 L"\"section\": {})", false).first);
    }

TEST_CASE("untranslatable code and formats", "[i18nreview]")
    {
    i18n_check::cpp_i18n_review reviewer{ false };
    SECTION("Date/time formats")
        {
        CHECK(reviewer.is_untranslatable_string(L"yyyy-MM-dd HH:mm:ss", false).first);
        CHECK(reviewer.is_untranslatable_string(L"dd/MM/yyyy", false).first);
        CHECK(reviewer.is_untranslatable_string(L"MMMM d, yyyy", false).first);
        CHECK(reviewer.is_untranslatable_string(L"hh:mm tt", false).first);
        CHECK(reviewer.is_untranslatable_string(L"HH:mm", false).first);
        CHECK(reviewer.is_untranslatable_string(L"yyyy-MM-ddTHH:mm:ss.fffZ", false).first);
        // already handled (printf/strftime commands are stripped, or too few letters)
        CHECK(reviewer.is_untranslatable_string(L"%Y-%m-%d", false).first);
        CHECK(reviewer.is_untranslatable_string(L"%H:%M:%S", false).first);
        CHECK(reviewer.is_untranslatable_string(L"N2", false).first);
        CHECK(reviewer.is_untranslatable_string(L"C0", false).first);
        CHECK(reviewer.is_untranslatable_string(L"o", false).first);
        // only one date-like token, this is a word
        CHECK_FALSE(reviewer.is_untranslatable_string(L"Hmm...", false).first);
        CHECK_FALSE(reviewer.is_untranslatable_string(L"Date format:", false).first);
        }
    SECTION("Reverse-DNS identifiers")
        {
        CHECK(reviewer.is_untranslatable_string(L"com.company.product", false).first);
        CHECK(reviewer.is_untranslatable_string(L"org.kde.konsole", false).first);
        CHECK(reviewer.is_untranslatable_string(L"com.apple.Safari", false).first);
        CHECK(reviewer.is_untranslatable_string(L"settings.window.width", false).first);
        CHECK_FALSE(reviewer.is_untranslatable_string(L"Settings saved. Restart the program.", false).first);
        }
    SECTION("Shader code")
        {
        CHECK(reviewer.is_untranslatable_string(L"#version 330 core", false).first);
        CHECK(reviewer.is_untranslatable_string(L"#version 330 core\\nlayout (location = 0) in vec3 aPos;\\n", false).first);
        CHECK(reviewer.is_untranslatable_string(L"#extension GL_ARB_separate_shader_objects : enable", false).first);
        CHECK(reviewer.is_untranslatable_string(L"uniform mat4 model;", false).first);
        CHECK(reviewer.is_untranslatable_string(L"uniform sampler2D screenTexture;", false).first);
        CHECK(reviewer.is_untranslatable_string(L"precision mediump float;", false).first);
        CHECK(reviewer.is_untranslatable_string(L"out vec4 FragColor;", false).first);
        CHECK(reviewer.is_untranslatable_string(L"layout (location = 0) in vec3 aPos;", false).first);
        CHECK(reviewer.is_untranslatable_string(L"gl_Position = projection * view * vec4(aPos, 1.0);", false).first);
        CHECK(reviewer.is_untranslatable_string(L"float4 main(float4 pos : POSITION) : SV_TARGET", false).first);
        CHECK_FALSE(reviewer.is_untranslatable_string(L"Uniform spacing between items", false).first);
        CHECK_FALSE(reviewer.is_untranslatable_string(L"Precision float values", false).first);
        CHECK_FALSE(reviewer.is_untranslatable_string(L"Version 3 is now available.", false).first);
        }
    SECTION("ANSI escape sequences")
        {
        // already handled (not enough consecutive letters)
        CHECK(reviewer.is_untranslatable_string(L"\\033[1;31m", false).first);
        CHECK(reviewer.is_untranslatable_string(L"\\x1b[2J\\x1b[H", false).first);
        CHECK(reviewer.is_untranslatable_string(L"\\033[38;5;208m", false).first);
        CHECK(reviewer.is_untranslatable_string(L"\\x1b[?25l", false).first);
        // visible text inside of the color codes is still user facing
        CHECK_FALSE(reviewer.is_untranslatable_string(L"\\033[1;31mThe file could not be opened.\\033[0m", false).first);
        }
    }

TEST_CASE("translatable", "[i18nreview]")
    {
    i18n_check::cpp_i18n_review reviewer{ false };
    CHECK_FALSE(reviewer.is_untranslatable_string(L"Q3FY%", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"Debug", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"...", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"x-axis", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"y-axis", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"z-axis", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"x-axes", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"y-axes", false).first);
    CHECK(reviewer.is_untranslatable_string(L"u-axes", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"Dot", false).first);
    CHECK_FALSE(reviewer.is_untranslatable_string(L"Color:", false).first);
    CHECK(reviewer.is_untranslatable_string(L"variables.positive-aggregate", false).first);
    CHECK(reviewer.is_untranslatable_string(L"row-suppression.label", false).first);
    }

TEST_CASE("i18n string utils", "[i18nstringutil]")
    {
    SECTION("Conversions to 7-bit")
        {
        CHECK(full_width_number_to_7bit(L'０') == L'0');
        CHECK(full_width_number_to_7bit(L'９') == L'9');
        CHECK(full_width_number_to_7bit(L'9') == L'9');
        CHECK(full_width_number_to_7bit(L'w') == L'w');

        CHECK(devanagari_number_to_7bit(L'०') == L'0');
        CHECK(devanagari_number_to_7bit(L'९') == L'9');

        // wrong converter will return unaltered value
        CHECK(full_width_number_to_7bit(L'९') == L'९');

        // pass through multiple converters
        CHECK(devanagari_number_to_7bit(full_width_number_to_7bit(L'९')) == L'9');

        // integration
        const auto numbers = i18n_review::load_numbers(L"८८५९-१");
        REQUIRE(numbers.size() == 2);
        CHECK(numbers[0] == std::wstring{ L"1" });
        CHECK(numbers[1] == std::wstring{ L"8859" });
        }

    SECTION("Conversions from 7-bit")
        {
        CHECK(seven_bit_number_to_full_width(L'0') == L'０');
        CHECK(seven_bit_number_to_full_width(L'9') == L'９');
        CHECK(seven_bit_number_to_full_width(L'９') == L'９');
        CHECK(seven_bit_number_to_full_width(L'w') == L'w');

        CHECK(seven_bit_number_to_devanagari(L'0') == L'०');
        CHECK(seven_bit_number_to_devanagari(L'9') == L'९');

        // wrong converter will return unaltered value
        CHECK(seven_bit_number_to_full_width(L'९') == L'९');
        }
    }

TEST_CASE("Replace escaped control chars", "[i18nstringutil]")
    {
    SECTION("Simple")
        {
        // the backslash and the 'n' are each replaced with a space (not collapsed to one)
        std::wstring str{ LR"(Hello\nworld)" };
        replace_escaped_control_chars(str);
        CHECK(str == L"Hello  world");
        }
    SECTION("Escaped backslash before real escape")
        {
        // "\\\n" is an escaped backslash ("\\") followed by a real "\n" escape --
        // the "\n" must still be converted, even though it's directly preceded
        // by a backslash pair.
        std::wstring str{ LR"(Hello\\\nworld)" };
        replace_escaped_control_chars(str);
        CHECK(str == LR"(Hello\\  world)");
        }
    SECTION("Escaped backslash only")
        {
        // "\\" on its own is just an escaped backslash; there is no "\n" here to convert.
        std::wstring str{ LR"(Hello\\nworld)" };
        replace_escaped_control_chars(str);
        CHECK(str == LR"(Hello\\nworld)");
        }
    }

TEST_CASE("File Paths", "[i18nstringutil]")
    {
    SECTION("Null")
        {
        CHECK_FALSE(is_file_address(L""));
        }
    SECTION("Typo Not Really A File Path")
        {
        CHECK_FALSE(is_file_address(L"file.Rob"));
        CHECK(is_file_address(L"file.TIF"));
        CHECK(is_file_address(L"file.tif"));
        // should be the start of a new sentence with the word "Tiff"
        CHECK_FALSE(is_file_address(L"file.Tif"));
        }
    SECTION("Non Addresses")
        {
        CHECK_FALSE(is_file_address(L"text"));
        CHECK_FALSE(is_file_address(L"FTPs"));
        CHECK_FALSE(is_file_address(L"wwwhat!"));
        CHECK_FALSE(is_file_address(L"File Names: (.cpp/.h)"));
        CHECK_FALSE(is_file_address(L".cpp/.h)"));
        }
    SECTION("Domains")
        {
        CHECK(is_file_address(L"http://www.sales.mycompany.com") );
        CHECK(is_file_address(L"HTTPS://www.sales.mycompany.com") );
        CHECK(is_file_address(L"ftp://www.sales.mycompany.com") );
        CHECK(is_file_address(L"www.sales.mycompany.com") );
        }
    SECTION("Domains No Prefix")
        {
        CHECK(is_file_address(L"ibm.com/") );
        CHECK(is_file_address(L"ibm.com/software/data/infosphere/identity-insight") );
        CHECK_FALSE(is_file_address(L"123.578/72")); // avoid what is really an equation
        CHECK(is_file_address(L"go.businessmachines.com"));
        CHECK(is_file_address(L"go.businessmachines.com/"));
        }
    SECTION("Network Paths")
        {
        CHECK(is_file_address(L"\\\\server") );
        CHECK(is_file_address(L"file://blah") );
        }
    SECTION("Windows Paths")
        {
        CHECK(is_file_address(L"C:\\users") );
        CHECK(is_file_address(L"C:\\") );
        CHECK(is_file_address(L"C:/users") ); // wrong path separator should still work
        CHECK(is_file_address(L"D:\\users") );
        CHECK_FALSE(is_file_address(L"7:\\users")); // not a real drive letter
        CHECK_FALSE(is_file_address(L"C:")); // not long enough
        }
    SECTION("UNIX Paths")
        {
        CHECK(is_file_address(L"/Users/Share") );
        CHECK_FALSE(is_file_address(L"/Users")); // not enough folders to determine if a real path
        CHECK_FALSE(is_file_address(L"/a")); // not long enough
        }
    SECTION("Email Paths")
        {
        CHECK(is_file_address(L"mailto:me") );
        CHECK(is_file_address(L"blake@mail.com") );
        CHECK_FALSE(is_file_address({ L"blake@mail.com", 10 }) ); // doesn't read far enough to get to the dot
        CHECK_FALSE(is_file_address({ L"blake@mail.com", 11 }) ); // doesn't read far enough to get to the domain
        CHECK(is_file_address({ L"blake@mail.com", 12 })); // "c" will be the domain
        CHECK_FALSE(is_file_address({ L"blake@mail", 10 })); // missing domain
        CHECK_FALSE(is_file_address({ L"bl.ke@mail", 10 })); // missing domain
        }
    SECTION("File Names")
        {
        CHECK(is_file_address(L"file.bmp") );
        CHECK_FALSE(is_file_address(L"file.bm"));
        CHECK_FALSE(is_file_address(L"file.bmps"));
        CHECK_FALSE(is_file_address(L"file.787"));
        CHECK(is_file_address(L"file.h"));
        CHECK(is_file_address(L"file.c"));
        CHECK(is_file_address(L"stdafx.h"));
        CHECK_FALSE(is_file_address(L".h"));
        CHECK(is_file_address(L"a.docx"));
        CHECK(is_file_address(L"libreoffice.tar.xz"));
        }

    SECTION("Short Url")
        {
        CHECK(i18n_string_util::is_url(L"amazon.com"));
        CHECK(i18n_string_util::is_url(L"shop.amazon.com"));
        CHECK(i18n_string_util::is_url(L"amazon.com's"));
        CHECK(i18n_string_util::is_url(L"amazon.com’s"));
        CHECK(!i18n_string_util::is_url({ L"amazon.txt’s", 12 }));

        CHECK(i18n_string_util::is_url(L"amazon.au"));
        CHECK(!i18n_string_util::is_url(L"amazon."));
        CHECK(!i18n_string_util::is_url(L"amazon.a"));
        }

     SECTION("Bogus Url/File")
        {
        CHECK_FALSE(i18n_string_util::is_url(L"Return to this window after you have finished the authorization process on Flickr.com"));
        CHECK_FALSE(i18n_string_util::is_file_address(L"Insert .tga"));
        CHECK_FALSE(i18n_string_util::is_file_address(L"Insert .baml"));
        CHECK_FALSE(i18n_string_util::is_file_address(L"Insert .html"));
        }

    SECTION("Short File")
        {
        CHECK(i18n_string_util::is_file_address(L"stdafx.h"));
        CHECK(i18n_string_util::is_file_address(L"stdafx.h's"));
        CHECK(i18n_string_util::is_file_address(L"stdafx.h’s"));
        }
    }

TEST_CASE("Printf remove", "printf")
    {
    SECTION("Empty")
        {
        std::wstring str = L"";
        remove_printf_commands(str);
        CHECK(str == L"");
        }

    SECTION("Numbers")
        {
        std::wstring str = L"Number %d and another %lu, and another %-5.06f and another %#x.";
        remove_printf_commands(str);
        CHECK(str == L"Number  and another , and another  and another .");
        }

    SECTION("Espcaped")
        {
        std::wstring str = L"A percent 5%% and another \\% one.";
        remove_printf_commands(str);
        CHECK(str == L"A percent 5%% and another \\% one.");
        }

    SECTION("Simple")
        {
        std::wstring str = L"This is simple %s, this is too %c.";
        remove_printf_commands(str);
        CHECK(str == L"This is simple , this is too .");
        }

    SECTION("Boundary Check")
        {
        std::wstring str = L"%s%i%c";
        remove_printf_commands(str);
        CHECK(str == L"");
        }

    SECTION("Bogus Specifiers")
        {
        std::wstring str = L"This isn't correct %y, neither is this %.5q. This is out of order %5#f.";
        remove_printf_commands(str);
        CHECK(str == L"This isn't correct %y, neither is this %.5q. This is out of order %5#f.");
        }
    }

TEST_CASE("Hex color remove", "[hexcolor]")
    {
    SECTION("Empty")
        {
        std::wstring str = L"";
        remove_hex_color_values(str);
        CHECK(str == L"");
        }

    SECTION("Boundary Check")
        {
        std::wstring str = L"#FF01Ab#ab993D";
        remove_hex_color_values(str);
        CHECK(str == L"");
        }

    SECTION("Colors")
        {
        std::wstring str = L"My color is #FF01Ab and #ab993D.";
        remove_hex_color_values(str);
        CHECK(str == L"My color is  and .");
        }

    SECTION("Bogus Specifiers")
        {
        std::wstring str = L"My color is #ZF01Ab and #ab993.";
        remove_hex_color_values(str);
        CHECK(str == L"My color is #ZF01Ab and #ab993.");
        }
    }

// NOLINTEND
// clang-format on
