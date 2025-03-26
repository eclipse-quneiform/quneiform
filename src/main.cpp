/********************************************************************************
 * Copyright (c) 2021-2025 Blake Madden
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

#include "analyze.h"
#include "cxxopts/include/cxxopts.hpp"
#include "input.h"
#include <iostream>
#include <numeric>
#include <sstream>

namespace fs = std::filesystem;

//-------------------------------------------------
int main(int argc, char* argv[])
    {
    cxxopts::Options options("Eclipse Quneiform", "Eclipse Quneiform: Internationalization/localization analysis "
                                          "system, (c) 2021-2025 Blake Madden");
    // clang-format off
    options.add_options()
        ("input", "The folder (or file) to analyze",
         cxxopts::value<std::string>())
        ("enable",
         "Which checks to perform (any combination of: "
         "allI18N, allL10N, allCodeFormatting, suspectL10NString, suspectL10NUsage, suspectI18NUsage, "
         "urlInL10NString, notL10NAvailable, deprecatedMacro, nonUTF8File, transInconsistency, articleOrPronoun"
         "numberInconsistency, multipartString, lengthInconsistency, L10NStringNeedsContext, UTF8FileWithBOM, "
         "unencodedExtASCII, pluralization, printfSingleNumber, concatenatedStrings, excessiveNonL10NContent,"
         "numberAssignedToId, dupValAssignedToIds, malformedString, fontIssue, printfMismatch, literalL10NStringCompare, "
         "acceleratorMismatch, trailingSpaces, tabs, wideLine, commentMissingSpace)",
         cxxopts::value<std::vector<std::string>>())
        ("disable", "Which checks to not perform (same as the options for --enable)",
         cxxopts::value<std::vector<std::string>>())
        ("log-l10n-allowed", "Whether it is acceptable to pass translatable "
                            "strings to logging functions. (Default is true.)")
        ("punct-l10n-allowed",
         "Whether it is acceptable for punctuation only "
         "strings to be translatable. (Default is false.)",
         cxxopts::value<bool>()->default_value("false"))
        ("exceptions-l10n-required",
         "Whether to verify that exception messages are available for translation.")
        ("min-l10n-wordcount",
         "The minimum number of words that a string must have to be reviewed for whether it should "
         "be translatable.",
         cxxopts::value<int>())
        ("cpp-version",
         "The C++ standard that should be assumed when issuing deprecated macro warnings.",
         cxxopts::value<int>())
        ("fuzzy",
         "Whether to review fuzzy translations. (Default is false.)",
         cxxopts::value<bool>()->default_value("false"))
        ("i,ignore", "Folders and files to ignore (can be used multiple times)",
                               cxxopts::value<std::vector<std::string>>())
        ("o,output", "The output report path (tab-delimited format)",
         cxxopts::value<std::string>())
        ("q,quiet", "Only print errors and the final output",
         cxxopts::value<bool>()->default_value("false"))
        ("v,verbose", "Display debug information",
         cxxopts::value<bool>()->default_value("false"))("h,help", "Print usage");
    // clang-format on
    options.positional_help("inputFolder");
    options.show_positional_help();

    const auto startTime{ std::chrono::high_resolution_clock::now() };

    cxxopts::ParseResult result;
    try
        {
        options.parse_positional({ "input" });

        result = options.parse(argc, argv);

        if (result.count("help"))
            {
            std::wcout << i18n_string_util::lazy_string_to_wstring(options.help()) << L"\n";
            return 0;
            }
        }
    catch (const cxxopts::exceptions::exception& exp)
        {
        std::wcout << i18n_string_util::lazy_string_to_wstring(exp.what());
        return 0;
        }

    // helper to get a boolean option (option not being present returns default)
    const auto readBoolOption = [&result](const std::string& option, const bool defaultValue)
    {
        if (result.count(option) > 0)
            {
            return result[option].as<bool>();
            }
        else
            {
            return defaultValue;
            }
    };

    const auto readIntOption = [&result](const std::string& option, const int defaultValue)
    {
        if (result.count(option) > 0)
            {
            return result[option].as<int>();
            }
        else
            {
            return defaultValue;
            }
    };

    fs::path inputFolder;
    if (result.count("input"))
        {
        inputFolder = fs::path{ result["input"].as<std::string>(), fs::path::native_format };
        if (!fs::exists(inputFolder))
            {
            std::wcout << L"Input path does not exist: " << inputFolder;
            return 0;
            }
        }
    else
        {
        std::wcout << L"You must pass in at least one folder to analyze.\n\n";
        std::wcout << i18n_string_util::lazy_string_to_wstring(options.help()) << L"\n";
        return 0;
        }

    if (!readBoolOption("quiet", false))
        {
        std::wcout << L"\n###################################################\n# "
                   << i18n_string_util::lazy_string_to_wstring(options.program())
                   << L":\n# Internationalization/localization analysis system\n# (c) 2021-2025 "
                      L"Blake Madden\n"
                   << L"###################################################\n\n";
        std::wcout << L"Searching for files to analyze in " << inputFolder << L"...\n\n";
        }

    std::vector<std::string> providedIgnoredPaths{
        (result["ignore"].count() > 0) ? result["ignore"].as<std::vector<std::string>>() :
                                         std::vector<std::string>{}
    };

    std::vector<fs::path> providedIgnoredPathsWidened;
    for (const auto& iPath : providedIgnoredPaths)
        {
        providedIgnoredPathsWidened.push_back(i18n_string_util::lazy_string_to_wstring(iPath));
        }
    // paths being ignored
    const auto excludedInfo =
        i18n_check::get_paths_files_to_exclude(inputFolder, providedIgnoredPathsWidened);

    // input folder
    const auto filesToAnalyze = i18n_check::get_files_to_analyze(
        inputFolder, excludedInfo.m_excludedPaths, excludedInfo.m_excludedFiles);

    const auto setSourceParserInfo = [&readBoolOption, &readIntOption](auto& parser)
    {
        parser.log_messages_can_be_translatable(readBoolOption("log-l10n-allowed", true));
        parser.allow_translating_punctuation_only_strings(
            readBoolOption("punct-l10n-allowed", false));
        parser.exceptions_should_be_translatable(readBoolOption("exceptions-l10n-required", true));
        parser.set_min_words_for_classifying_unavailable_string(
            readIntOption("min-l10n-wordcount", 2));
        parser.set_min_cpp_version(readIntOption("cpp-version", 2014));
    };

    i18n_check::cpp_i18n_review cpp(readBoolOption("verbose", false));
    setSourceParserInfo(cpp);
    i18n_check::csharp_i18n_review csharp(readBoolOption("verbose", false));
    setSourceParserInfo(csharp);

    i18n_check::rc_file_review rc(readBoolOption("verbose", false));
    rc.allow_translating_punctuation_only_strings(readBoolOption("punct-l10n-allowed", false));

    i18n_check::po_file_review po(readBoolOption("verbose", false));
    po.review_fuzzy_translations(readBoolOption("fuzzy", false));

    i18n_check::info_plist_file_review infoPlist(readBoolOption("verbose", false));

    // see which checks are being performed
    if (result.count("enable"))
        {
        const auto& styles = result["enable"].as<std::vector<std::string>>();
        int64_t rs{ i18n_check::review_style::no_checks };
        for (const auto& r : styles)
            {
            if (r == "allI18N")
                {
                rs |= i18n_check::review_style::all_i18n_checks;
                }
            else if (r == "allL10N")
                {
                rs |= i18n_check::review_style::all_l10n_checks;
                }
            else if (r == "allCodeFormatting")
                {
                rs |= i18n_check::review_style::all_code_formatting_checks;
                }
            else if (r == "suspectL10NString")
                {
                rs |= i18n_check::review_style::check_l10n_strings;
                }
            else if (r == "suspectL10NUsage")
                {
                rs |= i18n_check::review_style::check_suspect_l10n_string_usage;
                }
            else if (r == "suspectI18NUsage")
                {
                rs |= i18n_check::review_style::check_suspect_i18n_usage;
                }
            else if (r == "printfMismatch")
                {
                rs |= i18n_check::review_style::check_mismatching_printf_commands;
                }
            else if (r == "acceleratorMismatch")
                {
                rs |= i18n_check::review_style::check_accelerators;
                }
            else if (r == "transInconsistency")
                {
                rs |= i18n_check::review_style::check_consistency;
                }
            else if (r == "halfWidth")
                {
                rs |= i18n_check::review_style::check_halfwidth;
                }
            else if (r == "numberInconsistency")
                {
                rs |= i18n_check::review_style::check_numbers;
                }
            else if (r == "lengthInconsistency")
                {
                rs |= i18n_check::review_style::check_length;
                }
            else if (r == "L10NStringNeedsContext")
                {
                rs |= i18n_check::review_style::check_needing_context;
                }
            else if (r == "urlInL10NString")
                {
                rs |= i18n_check::review_style::check_l10n_contains_url;
                }
            else if (r == "excessiveNonL10NContent")
                {
                rs |= i18n_check::review_style::check_l10n_contains_excessive_nonl10n_content;
                }
            else if (r == "multipartString")
                {
                rs |= i18n_check::review_style::check_multipart_strings;
                }
            else if (r == "pluralization")
                {
                rs |= i18n_check::review_style::check_pluaralization;
                }
            else if (r == "articleOrPronoun")
                {
                rs |= i18n_check::review_style::check_articles_proceeding_placeholder;
                }
            else if (r == "concatenatedStrings")
                {
                rs |= i18n_check::review_style::check_l10n_concatenated_strings;
                }
            else if (r == "literalL10NStringCompare")
                {
                rs |= i18n_check::review_style::check_literal_l10n_string_comparison;
                }
            else if (r == "notL10NAvailable")
                {
                rs |= i18n_check::review_style::check_not_available_for_l10n;
                }
            else if (r == "deprecatedMacro")
                {
                rs |= i18n_check::review_style::check_deprecated_macros;
                }
            else if (r == "nonUTF8File")
                {
                rs |= i18n_check::review_style::check_utf8_encoded;
                }
            else if (r == "UTF8FileWithBOM")
                {
                rs |= i18n_check::review_style::check_utf8_with_signature;
                }
            else if (r == "unencodedExtASCII")
                {
                rs |= i18n_check::review_style::check_unencoded_ext_ascii;
                }
            else if (r == "printfSingleNumber")
                {
                rs |= i18n_check::review_style::check_printf_single_number;
                }
            else if (r == "numberAssignedToId")
                {
                rs |= i18n_check::review_style::check_number_assigned_to_id;
                }
            else if (r == "dupValAssignedToIds")
                {
                rs |= i18n_check::review_style::check_duplicate_value_assigned_to_ids;
                }
            else if (r == "malformedString")
                {
                rs |= i18n_check::review_style::check_malformed_strings;
                }
            else if (r == "fontIssue")
                {
                rs |= i18n_check::review_style::check_fonts;
                }
            else if (r == "trailingSpaces")
                {
                rs |= i18n_check::review_style::check_trailing_spaces;
                }
            else if (r == "tabs")
                {
                rs |= i18n_check::review_style::check_tabs;
                }
            else if (r == "wideLine")
                {
                rs |= i18n_check::review_style::check_line_width;
                }
            else if (r == "commentMissingSpace")
                {
                rs |= i18n_check::review_style::check_space_after_comment;
                }
            else
                {
                std::wcout << L"Unknown option passed to --enable: "
                           << i18n_string_util::lazy_string_to_wstring(r) << L"\n\n"
                           << i18n_string_util::lazy_string_to_wstring(options.help()) << L"\n";
                return 1;
                }
            }
        cpp.set_style(static_cast<i18n_check::review_style>(rs));
        csharp.set_style(static_cast<i18n_check::review_style>(rs));
        po.set_style(static_cast<i18n_check::review_style>(rs));
        rc.set_style(static_cast<i18n_check::review_style>(rs));
        }
    // ...and if any checks are being excluded
    if (result.count("disable"))
        {
        const auto& styles = result["disable"].as<std::vector<std::string>>();
        int64_t rs{ static_cast<int64_t>(cpp.get_style()) };
        for (const auto& r : styles)
            {
            if (r == "allI18N")
                {
                rs = rs & ~i18n_check::review_style::all_i18n_checks;
                }
            else if (r == "allL10N")
                {
                rs = rs & ~i18n_check::review_style::all_l10n_checks;
                }
            else if (r == "allCodeFormatting")
                {
                rs = rs & ~i18n_check::review_style::all_code_formatting_checks;
                }
            else if (r == "suspectL10NString")
                {
                rs = rs & ~i18n_check::review_style::check_l10n_strings;
                }
            else if (r == "suspectL10NUsage")
                {
                rs = rs & ~i18n_check::review_style::check_suspect_l10n_string_usage;
                }
            else if (r == "suspectI18NUsage")
                {
                rs = rs & ~i18n_check::review_style::check_suspect_i18n_usage;
                }
            else if (r == "printfMismatch")
                {
                rs = rs & ~i18n_check::review_style::check_mismatching_printf_commands;
                }
            else if (r == "acceleratorMismatch")
                {
                rs = rs & ~i18n_check::review_style::check_accelerators;
                }
            else if (r == "transInconsistency")
                {
                rs = rs & ~i18n_check::review_style::check_consistency;
                }
            else if (r == "halfWidth")
                {
                rs = rs & ~i18n_check::review_style::check_halfwidth;
                }
            else if (r == "numberInconsistency")
                {
                rs = rs & ~i18n_check::review_style::check_numbers;
                }
            else if (r == "lengthInconsistency")
                {
                rs = rs & ~i18n_check::review_style::check_length;
                }
            else if (r == "L10NStringNeedsContext")
                {
                rs = rs & ~i18n_check::review_style::check_needing_context;
                }
            else if (r == "urlInL10NString")
                {
                rs = rs & ~i18n_check::review_style::check_l10n_contains_url;
                }
            else if (r == "excessiveNonL10NContent")
                {
                rs = rs & ~i18n_check::review_style::check_l10n_contains_excessive_nonl10n_content;
                }
            else if (r == "multipartString")
                {
                rs = rs & ~i18n_check::review_style::check_multipart_strings;
                }
            else if (r == "pluralization")
                {
                rs = rs & ~i18n_check::review_style::check_pluaralization;
                }
            else if (r == "articleOrPronoun")
                {
                rs = rs & ~i18n_check::review_style::check_articles_proceeding_placeholder;
                }
            else if (r == "concatenatedStrings")
                {
                rs = rs & ~i18n_check::review_style::check_l10n_concatenated_strings;
                }
            else if (r == "literalL10NStringCompare")
                {
                rs = rs & ~i18n_check::review_style::check_literal_l10n_string_comparison;
                }
            else if (r == "notL10NAvailable")
                {
                rs = rs & ~i18n_check::review_style::check_not_available_for_l10n;
                }
            else if (r == "deprecatedMacro")
                {
                rs = rs & ~i18n_check::review_style::check_deprecated_macros;
                }
            else if (r == "nonUTF8File")
                {
                rs = rs & ~i18n_check::review_style::check_utf8_encoded;
                }
            else if (r == "UTF8FileWithBOM")
                {
                rs = rs & ~i18n_check::review_style::check_utf8_with_signature;
                }
            else if (r == "unencodedExtASCII")
                {
                rs = rs & ~i18n_check::review_style::check_unencoded_ext_ascii;
                }
            else if (r == "printfSingleNumber")
                {
                rs = rs & ~i18n_check::review_style::check_printf_single_number;
                }
            else if (r == "numberAssignedToId")
                {
                rs = rs & ~i18n_check::review_style::check_number_assigned_to_id;
                }
            else if (r == "dupValAssignedToIds")
                {
                rs = rs & ~i18n_check::review_style::check_duplicate_value_assigned_to_ids;
                }
            else if (r == "malformedString")
                {
                rs = rs & ~i18n_check::review_style::check_malformed_strings;
                }
            else if (r == "fontIssue")
                {
                rs = rs & ~i18n_check::review_style::check_fonts;
                }
            else if (r == "trailingSpaces")
                {
                rs = rs & ~i18n_check::review_style::check_trailing_spaces;
                }
            else if (r == "tabs")
                {
                rs = rs & ~i18n_check::review_style::check_tabs;
                }
            else if (r == "wideLine")
                {
                rs = rs & ~i18n_check::review_style::check_line_width;
                }
            else if (r == "commentMissingSpace")
                {
                rs = rs & ~i18n_check::review_style::check_space_after_comment;
                }
            else
                {
                std::wcout << L"Unknown option passed to --disable: "
                           << i18n_string_util::lazy_string_to_wstring(r) << L"\n\n"
                           << i18n_string_util::lazy_string_to_wstring(options.help()) << L"\n";
                return 1;
                }
            }
        cpp.set_style(static_cast<i18n_check::review_style>(rs));
        csharp.set_style(static_cast<i18n_check::review_style>(rs));
        po.set_style(static_cast<i18n_check::review_style>(rs));
        rc.set_style(static_cast<i18n_check::review_style>(rs));
        }

    const bool isQuiet{ readBoolOption("quiet", false) };
    i18n_check::batch_analyze analyzer(&cpp, &rc, &po, &csharp, &infoPlist);
    analyzer.analyze(
        filesToAnalyze, [](const size_t) {},
        [&filesToAnalyze, isQuiet](const size_t currentFileIndex, const fs::path& file)
        {
            if (!isQuiet)
                {
                std::wcout << L"Examining " << currentFileIndex << L" of " << filesToAnalyze.size()
                           << L" files (" << file.filename() << L")\n";
                }
            return true;
        });

    const std::wstringstream report = analyzer.format_results(readBoolOption("verbose", false));

    // write the output to file (if requested)
    if (result.count("output"))
        {
        fs::path outPath{ result["output"].as<std::string>() };
        std::ofstream ofs(outPath);

        // write the results report in UTF-8
        std::string utf8Str;
        const auto resText = report.str();
        if constexpr (sizeof(wchar_t) == sizeof(uint16_t))
            {
            utf8::utf16to8(resText.cbegin(), resText.cend(), std::back_inserter(utf8Str));
            }
        else if constexpr (sizeof(wchar_t) == sizeof(uint32_t))
            {
            utf8::utf32to8(resText.cbegin(), resText.cend(), std::back_inserter(utf8Str));
            }

        if (outPath.extension().compare(L"csv") == 0)
            {
            string_util::replace_all<std::string>(utf8Str, "\t", ",");
            }

        ofs << utf8Str;
        }
    // ...otherwise, send it to the console
    else
        {
        std::wcout << report.str();
        }

    if (!readBoolOption("quiet", false))
        {
        const auto endTime{ std::chrono::high_resolution_clock::now() };

        if (std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() < 1)
            {
            std::wcout << L"\nFinished in "
                       << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
                              .count()
                       << L" milliseconds.\n\n";
            }
        else
            {
            std::wcout
                << L"\nFinished in "
                << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count()
                << L" seconds.\n\n";
            }

        std::wcout << analyzer.format_summary(true).str();
        }

    return 0;
    }
