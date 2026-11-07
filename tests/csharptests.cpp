#include "../src/csharp_i18n_review.h"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iomanip>
#include <set>
#include <sstream>

// NOLINTBEGIN
using namespace i18n_check;
using namespace Catch::Matchers;

// clang-format off
TEST_CASE("C# code", "[csharp][i18n]")
    {
    SECTION("Raw Strings")
        {
        csharp_i18n_review cs(false);
        const wchar_t* code = LR"(string val = @"This is your last ""chance""!"""; int level;)";
        cs(code, L"");
        cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
        REQUIRE(cs.get_not_available_for_localization_strings().size() == 1);
        CHECK(cs.get_not_available_for_localization_strings()[0].m_string == std::wstring{ LR"(This is your last "chance"!)" });

        cs.clear_results();

        code = LR"(string cs = @"This is your last chance!"; int level;)";
        cs(code, L"");
        cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
        REQUIRE(cs.get_not_available_for_localization_strings().size() == 1);
        CHECK(cs.get_not_available_for_localization_strings()[0].m_string == std::wstring{ LR"(This is your last chance!)" });

        cs.clear_results();

        code = LR"(string val = """This is your last ""chance""!"""; int level;)";
        cs(code, L"");
        cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
        REQUIRE(cs.get_not_available_for_localization_strings().size() == 1);
        CHECK(cs.get_not_available_for_localization_strings()[0].m_string == std::wstring{ LR"(This is your last "chance"!)" });
        }
    }

TEST_CASE("C# loggers and attributes", "[csharp][i18n]")
    {
    SECTION("Log Functions")
        {
        const std::vector<std::wstring> logFunctions{
            L"Log.Information", L"Log.Warning", L"Log.Error", L"Log.Fatal",
            L"Console.Write", L"Console.Error.WriteLine", L"Trace.WriteLine",
            L"Trace.TraceError", L"LogInformation", L"LogCritical" };
        for (const auto& logFunction : logFunctions)
            {
            csharp_i18n_review cs(false);
            const std::wstring code{ logFunction + LR"(("The file could not be opened.");)" };
            cs(code, L"");
            cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
            // Catch2 can only log narrow strings (these names are all ASCII)
            std::string narrowName;
            for (const auto ch : logFunction) { narrowName += static_cast<char>(ch); }
            INFO(narrowName);
            CHECK(cs.get_localizable_strings().size() == 0);
            CHECK(cs.get_not_available_for_localization_strings().size() == 0);
            }
        }

    SECTION("Attributes")
        {
        const std::vector<std::wstring> attributes{
            LR"([DllImport("user32.dll")])",
            LR"([System.Runtime.InteropServices.DllImport("user32.dll")])",
            LR"([JsonPropertyName("first name")])",
            LR"([JsonProperty("first name")])",
            LR"([XmlElement("Customer Name")])",
            LR"([XmlAttribute("display name")])",
            LR"([Route("api/orders/{id}")])",
            LR"([HttpGet("orders/recent items")])",
            LR"([Obsolete("Use the new loader instead of this one.")])",
            LR"([InternalsVisibleTo("My Assembly Tests")])",
            LR"([Guid("2D7F3E44-1B1B-4C3C-9C55-5F6D9B1E2A10")])" };
        for (const auto& attribute : attributes)
            {
            csharp_i18n_review cs(false);
            const std::wstring code{ attribute + L"\npublic class Foo {}" };
            cs(code, L"");
            cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
            // Catch2 can only log narrow strings (these attributes are all ASCII)
            std::string narrowAttribute;
            for (const auto ch : attribute) { narrowAttribute += static_cast<char>(ch); }
            INFO(narrowAttribute);
            CHECK(cs.get_not_available_for_localization_strings().size() == 0);
            }
        }

    SECTION(".NET internal functions")
        {
        const std::vector<std::wstring> snippets{
            LR"(Environment.GetEnvironmentVariable("The main window toolbar");)",
            LR"(Process.Start("The main window toolbar");)",
            LR"(typeof(Foo).GetMethod("The main window toolbar");)",
            LR"(assembly.GetManifestResourceStream("The main window toolbar");)",
            LR"(Regex.IsMatch(input, "The main window toolbar");)",
            LR"(DateTime.ParseExact(text, "The main window toolbar", CultureInfo.InvariantCulture);)",
            LR"(CultureInfo.GetCultureInfo("The main window toolbar");)" };
        for (const auto& snippet : snippets)
            {
            csharp_i18n_review cs(false);
            cs(snippet, L"");
            cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
            // Catch2 can only log narrow strings (these snippets are all ASCII)
            std::string narrowSnippet;
            for (const auto ch : snippet) { narrowSnippet += static_cast<char>(ch); }
            INFO(narrowSnippet);
            CHECK(cs.get_not_available_for_localization_strings().size() == 0);
            }
        }

    SECTION("User-facing attribute still flagged")
        {
        // Description is shown in property grids, so it should still be flagged
        csharp_i18n_review cs(false);
        const wchar_t* code = LR"([Description("The name shown to the customer.")]
public string Name { get; set; })";
        cs(code, L"");
        cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
        CHECK(cs.get_not_available_for_localization_strings().size() == 1);
        }

    SECTION("ILogger Member Call")
        {
        csharp_i18n_review cs(false);
        const wchar_t* code = LR"(_logger.LogInformation("The file could not be opened.");)";
        cs(code, L"");
        cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
        CHECK(cs.get_not_available_for_localization_strings().size() == 0);
        }
    }

TEST_CASE("C# subscript and lookup keys", "[csharp][i18n]")
    {
    const auto runCode = [](csharp_i18n_review& cs, const wchar_t* code)
        {
        cs.clear_results();
        cs(code, L"");
        cs.review_strings([](size_t){}, [](size_t, const std::filesystem::path&){ return true; });
        };

    SECTION("Subscript keys")
        {
        csharp_i18n_review cs(false);
        runCode(cs, LR"(var folder = ConfigurationManager.AppSettings["Last opened project folder"];)");
        CHECK(cs.get_not_available_for_localization_strings().size() == 0);
        CHECK(cs.get_internal_strings().size() == 1);

        runCode(cs, LR"(var folder = settings[@"Last opened project folder"];)");
        CHECK(cs.get_not_available_for_localization_strings().size() == 0);
        CHECK(cs.get_internal_strings().size() == 1);
        }

    SECTION("Lookup functions")
        {
        csharp_i18n_review cs(false);
        runCode(cs, LR"(if (settings.ContainsKey("Last opened project folder")) {})");
        CHECK(cs.get_not_available_for_localization_strings().size() == 0);
        CHECK(cs.get_internal_strings().size() == 1);

        runCode(cs, LR"(if (settings.TryGetValue("Last opened project folder", out var folder)) {})");
        CHECK(cs.get_not_available_for_localization_strings().size() == 0);
        CHECK(cs.get_internal_strings().size() == 1);
        }

    SECTION("Values are still reviewed")
        {
        csharp_i18n_review cs(false);
        runCode(cs, LR"(labels["open_prompt"] = "Please select a file to open.";)");
        REQUIRE(cs.get_not_available_for_localization_strings().size() == 1);
        CHECK(cs.get_not_available_for_localization_strings()[0].m_string ==
              L"Please select a file to open.");

        // collection expression, not a subscript
        runCode(cs, LR"(string[] items = ["Please select a file to open."];)");
        CHECK(cs.get_not_available_for_localization_strings().size() == 1);
        }
    }

// NOLINTEND
// clang-format on
