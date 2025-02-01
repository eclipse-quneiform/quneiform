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
#include <iostream>
#ifdef wxVERSION_NUMBER
    #include <wx/file.h>
#endif

namespace i18n_check
    {
    //------------------------------------------------------
    bool valid_utf8_file(const std::filesystem::path& filePath, bool& startsWithBom)
        {
        startsWithBom = false;
        std::ifstream ifs(filePath);
        if (!ifs)
            {
            return false;
            }

        std::istreambuf_iterator<char> it(ifs.rdbuf());
        std::istreambuf_iterator<char> eos;

        if (utf8::starts_with_bom(it, eos))
            {
            startsWithBom = true;
            std::advance(it, std::size(utf8::bom));
            }

        return utf8::is_valid(it, eos);
        }

    //------------------------------------------------------
    std::pair<bool, std::wstring> read_utf16_file(const std::filesystem::path& filePath)
        {
        std::ifstream fs8(filePath);
        if (!fs8.is_open())
            {
            std::wcout << L"Could not open " << filePath << L"\n";
            return std::make_pair(false, std::wstring{});
            }

        std::string buffer;
        buffer.reserve(static_cast<size_t>(std::filesystem::file_size(filePath)));

        fs8.seekg(0, std::ios_base::end);
        const auto fileLength = fs8.tellg();
        fs8.seekg(0);
        std::vector<char> fileData(static_cast<size_t>(fileLength));
        fs8.read(&fileData[0], fileLength);

        if (lily_of_the_valley::unicode_extract_text::is_unicode(&fileData[0]))
            {
            lily_of_the_valley::unicode_extract_text uExtract;
            uExtract(&fileData[0], fileData.size(),
                     lily_of_the_valley::unicode_extract_text::is_little_endian(&fileData[0]));
            return std::make_pair(true, std::wstring{ uExtract.get_filtered_text() });
            }
        return std::make_pair(false, std::wstring{});
        }

    //------------------------------------------------------
    std::pair<bool, std::wstring> read_utf8_file(const std::filesystem::path& filePath,
                                                 bool& startsWithBom)
        {
        if (!valid_utf8_file(filePath, startsWithBom))
            {
            return std::make_pair(false, std::wstring{});
            }

        std::ifstream fs8(filePath);
        if (!fs8.is_open())
            {
            std::wcout << L"Could not open " << filePath << L"\n";
            return std::make_pair(false, std::wstring{});
            }

        unsigned line_count = 1;
        std::string line;
        std::wstring buffer;
        buffer.reserve(static_cast<size_t>(std::filesystem::file_size(filePath)));

        while (std::getline(fs8, line))
            {
            if (auto end_it = utf8::find_invalid(line.begin(), line.end()); end_it != line.cend())
                {
                std::cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
                std::cout << "This part is OK: " << std::string(line.begin(), end_it) << "\n";
                }

            // Convert it to utf-16
            std::u16string utf16line = utf8::utf8to16(line);
            for (const auto& ch : utf16line)
                {
                buffer += static_cast<wchar_t>(ch);
                }
            buffer += L"\n";

            ++line_count;
            }

        return std::make_pair(true, buffer);
        }

    //------------------------------------------------------
    void batch_analyze::pseudo_translate(const std::vector<std::filesystem::path>& filesToTranslate,
                                         i18n_check::pseudo_translation_method pseudoMethod,
                                         bool addSurroundingBrackets, int8_t widthChange,
                                         bool addTrackingIds, analyze_callback_reset resetCallback,
                                         analyze_callback callback)
        {
        size_t currentFileIndex{ 0 };

        const auto outputFile =
            [this](const std::filesystem::path filePath, const std::wstring& content)
        {
            std::u32string outBuffer;
            outBuffer.reserve(content.size());
            for (const auto& chr : content)
                {
                outBuffer += static_cast<char32_t>(chr);
                }
            std::string utfBuffer{ utf8::utf32to8(outBuffer) };

            std::ofstream out(filePath);
            if (out.is_open())
                {
                out.write(utfBuffer.c_str(), utfBuffer.size());
                m_logReport.append(_WXTRANS_WSTR(L"\nPseudo-translation catalog generated at: "))
                    .append(filePath.wstring());
                }
        };

        pseudo_translater trans;
        trans.set_pseudo_method(pseudoMethod);
        trans.add_surrounding_brackets(addSurroundingBrackets);
        trans.change_width(widthChange);
        trans.enable_tracking(addTrackingIds);

        resetCallback(filesToTranslate.size());
        // load file content into analyzers
        for (const auto& file : filesToTranslate)
            {
            if (!callback(++currentFileIndex, file))
                {
                return;
                }

            const file_review_type fileType = get_file_type(file);

            std::filesystem::path outPath = std::filesystem::path{ file };
            outPath.replace_filename(L"pseudo_" + outPath.filename().wstring());

            // change copy of PO template files to PO files
            if (outPath.extension().compare(L".pot") == 0)
                {
                outPath.replace_extension(L".po");
                }

            try
                {
                bool startsWithBom{ false };
                if (auto [readUtf8Ok, fileUtf8Text] = read_utf8_file(file, startsWithBom);
                    readUtf8Ok)
                    {
                    if (fileType == file_review_type::po)
                        {
                        trans.translate_po_file(fileUtf8Text);
                        outputFile(outPath, fileUtf8Text);
                        }
                    }
                else if (auto [readUtf16Ok, fileUtf16Text] = read_utf16_file(file); readUtf16Ok)
                    {
                    // UTF-16 may not be supported consistently on all platforms and compilers
                    if (fileType == file_review_type::po)
                        {
                        trans.translate_po_file(fileUtf16Text);
                        outputFile(outPath, fileUtf16Text);
                        }
                    }
                else
                    {
                    std::wifstream ifs(file);
                    std::wstring str((std::istreambuf_iterator<wchar_t>(ifs)),
                                     std::istreambuf_iterator<wchar_t>());
                    if (fileType == file_review_type::po)
                        {
                        trans.translate_po_file(str);
                        outputFile(outPath, str);
                        }
                    }
                }
            catch (const std::exception& expt)
                {
                m_logReport.append(i18n_string_util::lazy_string_to_wstring(expt.what()))
                    .append(L"\n");
                std::wcout << i18n_string_util::lazy_string_to_wstring(expt.what()) << L"\n";
                }
            }
        }

    //------------------------------------------------------
    void batch_analyze::analyze(const std::vector<std::filesystem::path>& filesToAnalyze,
                                analyze_callback_reset resetCallback, analyze_callback callback)
        {
        m_filesThatShouldBeConvertedToUTF8.clear();
        m_filesThatContainUTF8Signature.clear();
        m_cpp->clear_results();
        m_rc->clear_results();
        m_plist->clear_results();
        m_po->clear_results();
        m_csharp->clear_results();

        size_t currentFileIndex{ 0 };

        resetCallback(filesToAnalyze.size());
        // load file content into analyzers
        for (const auto& file : filesToAnalyze)
            {
            if (!callback(++currentFileIndex, file))
                {
                return;
                }

            const file_review_type fileType = get_file_type(file);

            try
                {
                bool startsWithBom{ false };
                if (const auto [readUtf8Ok, fileUtf8Text] = read_utf8_file(file, startsWithBom);
                    readUtf8Ok)
                    {
                    if (startsWithBom && m_cpp->get_style() & check_utf8_with_signature)
                        {
                        m_filesThatContainUTF8Signature.push_back(file);
                        }
                    if (fileType == file_review_type::rc)
                        {
                        (*m_rc)(fileUtf8Text, file);
                        }
                    else if (fileType == file_review_type::infoplist)
                        {
                        (*m_plist)(fileUtf8Text, file);
                        }
                    else if (fileType == file_review_type::po)
                        {
                        (*m_po)(fileUtf8Text, file);
                        }
                    else if (fileType == file_review_type::cs)
                        {
                        (*m_csharp)(fileUtf8Text, file);
                        }
                    else
                        {
                        (*m_cpp)(fileUtf8Text, file);
                        }
                    }
                else if (const auto [readUtf16Ok, fileUtf16Text] = read_utf16_file(file);
                         readUtf16Ok)
                    {
                    // UTF-16 or ANSI may not be supported consistently on
                    // all platforms and compilers.
                    // RC files are usually encoded in ANSI given their age,
                    // so don't check those files.
                    if (fileType != file_review_type::rc && m_cpp->get_style() & check_utf8_encoded)
                        {
                        m_filesThatShouldBeConvertedToUTF8.push_back(file);
                        }
                    if (fileType == file_review_type::rc)
                        {
                        (*m_rc)(fileUtf16Text, file);
                        }
                    else if (fileType == file_review_type::infoplist)
                        {
                        (*m_plist)(fileUtf16Text, file);
                        }
                    else if (fileType == file_review_type::po)
                        {
                        (*m_po)(fileUtf16Text, file);
                        }
                    else if (fileType == file_review_type::cs)
                        {
                        (*m_csharp)(fileUtf16Text, file);
                        }
                    else
                        {
                        (*m_cpp)(fileUtf16Text, file);
                        }
                    }
                else
                    {
                    if (fileType != file_review_type::rc && m_cpp->get_style() & check_utf8_encoded)
                        {
                        m_filesThatShouldBeConvertedToUTF8.push_back(file);
                        }
#ifdef wxVERSION_NUMBER
                    const std::wstring str = [&file]()
                    {
                        wxFile ifs(file.wstring());
                        wxString fileContents;
                        if (ifs.IsOpened())
                            {
                            if (ifs.ReadAll(&fileContents))
                                {
                                return std::wstring{ fileContents.wc_str() };
                                }
                            }
                        return std::wstring{};
                    }();
#else
                    std::wifstream ifs(file);
                    const std::wstring str((std::istreambuf_iterator<wchar_t>(ifs)),
                                           std::istreambuf_iterator<wchar_t>());
#endif
                    if (fileType == file_review_type::rc)
                        {
                        (*m_rc)(str, file);
                        }
                    else if (fileType == file_review_type::infoplist)
                        {
                        (*m_plist)(str, file);
                        }
                    else if (fileType == file_review_type::po)
                        {
                        (*m_po)(str, file);
                        }
                    else if (fileType == file_review_type::cs)
                        {
                        (*m_csharp)(str, file);
                        }
                    else
                        {
                        (*m_cpp)(str, file);
                        }
                    }
                }
            catch (const std::exception& expt)
                {
                m_logReport.append(i18n_string_util::lazy_string_to_wstring(expt.what()))
                    .append(L"\n");
                std::wcout << i18n_string_util::lazy_string_to_wstring(expt.what()) << L"\n";
                }
            }

        // analyze the content
        try
            {
            m_cpp->review_strings(resetCallback, callback);
            m_po->review_strings(resetCallback, callback);
            }
        catch (const std::exception& expt)
            {
            m_logReport.append(i18n_string_util::lazy_string_to_wstring(expt.what())).append(L"\n");
            std::wcout << i18n_string_util::lazy_string_to_wstring(expt.what()) << L"\n";
            }
        }

    //------------------------------------------------------
    std::wstringstream batch_analyze::format_summary(const bool verbose)
        {
        std::wstringstream report;
        if (verbose)
            {
            report
                << _(L"Checks Performed")
                << L"\n###################################################\n"
                << ((m_cpp->get_style() & check_l10n_strings) ? L"suspectL10NString\n" : L"")
                << ((m_cpp->get_style() & check_suspect_i18n_usage) ? L"suspectI18NUsage\n" : L"")
                << ((m_cpp->get_style() & check_suspect_l10n_string_usage) ? L"suspectL10NUsage\n" :
                                                                             L"")
                << ((m_cpp->get_style() & check_mismatching_printf_commands) ? L"printfMismatch\n" :
                                                                               L"")
                << ((m_cpp->get_style() & check_accelerators) ? L"acceleratorMismatch\n" : L"")
                << ((m_cpp->get_style() & check_consistency) ? L"transInconsistency\n" : L"")
                << ((m_cpp->get_style() & check_halfwidth) ? L"halfWidth\n" : L"")
                << ((m_cpp->get_style() & check_numbers) ? L"numberInconsistency\n" : L"")
                << ((m_cpp->get_style() & check_length) ? L"lengthInconsistency\n" : L"")
                << ((m_cpp->get_style() & check_needing_context) ? L"L10NStringNeedsContext\n" :
                                                                   L"")
                << ((m_cpp->get_style() & check_l10n_contains_url) ? L"urlInL10NString\n" : L"")
                << ((m_cpp->get_style() & check_multipart_strings) ? L"multipartString\n" : L"")
                << ((m_cpp->get_style() & check_pluaralization) ? L"pluralization\n" : L"")
                << ((m_cpp->get_style() & check_articles_proceeding_placeholder) ?
                        L"articleOrPronoun\n" :
                        L"")
                << ((m_cpp->get_style() & check_l10n_contains_excessive_nonl10n_content) ?
                        L"excessiveNonL10NContent\n" :
                        L"")
                << ((m_cpp->get_style() & check_l10n_concatenated_strings) ?
                        L"concatenatedStrings\n" :
                        L"")
                << ((m_cpp->get_style() & check_literal_l10n_string_comparison) ?
                        L"literalL10NStringCompare\n" :
                        L"")
                << ((m_cpp->get_style() & check_not_available_for_l10n) ? L"notL10NAvailable\n" :
                                                                          L"")
                << ((m_cpp->get_style() & check_deprecated_macros) ? L"deprecatedMacro\n" : L"")
                << ((m_cpp->get_style() & check_utf8_encoded) ? L"nonUTF8File\n" : L"")
                << ((m_cpp->get_style() & check_utf8_with_signature) ? L"UTF8FileWithBOM\n" : L"")
                << ((m_cpp->get_style() & check_unencoded_ext_ascii) ? L"unencodedExtASCII\n" : L"")
                << ((m_cpp->get_style() & check_printf_single_number) ? L"printfSingleNumber\n" :
                                                                        L"")
                << ((m_cpp->get_style() & check_number_assigned_to_id) ? L"numberAssignedToId\n" :
                                                                         L"")
                << ((m_cpp->get_style() & check_duplicate_value_assigned_to_ids) ?
                        L"dupValAssignedToIds\n" :
                        L"")
                << ((m_cpp->get_style() & check_malformed_strings) ? L"malformedString\n" : L"")
                << ((m_cpp->get_style() & check_fonts) ? L"fontIssue\n" : L"")
                << ((m_cpp->get_style() & check_trailing_spaces) ? L"trailingSpaces\n" : L"")
                << ((m_cpp->get_style() & check_tabs) ? L"tabs\n" : L"")
                << ((m_cpp->get_style() & check_line_width) ? L"wideLine\n" : L"")
                << ((m_cpp->get_style() & check_space_after_comment) ? L"commentMissingSpace\n" :
                                                                       L"")
                << L"\n";
            }
        report << _(L"Statistics") << L"\n###################################################\n"
               << _(L"Strings available for translation within C/C++ source files: ")
               << m_cpp->get_localizable_strings().size() << L"\n"
               << _(L"Strings available for translation within C# source files: ")
               << m_csharp->get_localizable_strings().size() << L"\n"
               << _(L"String table entries within Windows resource files: ")
               << m_rc->get_localizable_strings().size() << L"\n"
               << _(L"Translation entries within PO message catalog files: ")
               << m_po->get_catalog_entries().size() << L"\n";

        return report;
        }

    //------------------------------------------------------
    std::wstringstream batch_analyze::format_results(const bool verbose /*= false*/)
        {
        const auto replaceSpecialSpaces = [](const std::wstring& str)
        {
            auto newStr{ str };
            string_util::replace_all(newStr, L'\t', L' ');
            string_util::replace_all(newStr, L'\n', L' ');
            string_util::replace_all(newStr, L'\r', L' ');
            return newStr;
        };

        std::wstringstream report;
        report << // TRANSLATORS: Column header in output
            _("File") << L"\t" <<
            // TRANSLATORS: Column header in output
            _(L"Line") << L"\t" <<
            // TRANSLATORS: Column header in output
            _(L"Column") << L"\t" <<
            // TRANSLATORS: Column header in output
            _(L"Value") << L"\t" <<
            // TRANSLATORS: Column header in output
            _(L"Explanation") << L"\t" <<
            // TRANSLATORS: Column header in output
            _(L"Warning ID") << L"\n";

        // Windows resource file warnings
        for (const auto& val : m_rc->get_unsafe_localizable_strings())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                   << _(L"String available for translation that probably should not be.")
                   << L"\"\t[suspectL10NString]\n";
            }

        for (const auto& val : m_rc->get_localizable_strings_with_urls())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                   << _(L"String available for translation that contains an URL or email address.")
                   << L"\"\t[urlInL10NString]\n";
            }

        for (const auto& val : m_rc->get_localizable_strings_with_unlocalizable_content())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                   << _(L"String available for translation that contains a "
                        "large amount of non-translatable content")
                   << L"\"\t[excessiveNonL10NContent]\n";
            }

        for (const auto& val : m_rc->get_localizable_strings_being_concatenated())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                   << _(L"String available for translation that may be getting concatenated "
                        "at runtime with other content.")
                   << L"\"\t[concatenatedStrings]\n";
            }

        for (const auto& val : m_rc->get_multipart_strings())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                   << _(L"String available for translation may contain multiple sections "
                        "being sliced at runtime. Consider splitting each section into "
                        "a separate resource.")
                   << L"\"\t[multipartString]\n";
            }

        for (const auto& val : m_rc->get_faux_plural_strings())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                   << _(L"The same string is used for singular and plural situations. "
                        "Consider using a pluralizing function (if available) or "
                        "reword the message.")
                   << L"\"\t[pluralization]\n";
            }

        for (const auto& val : m_rc->get_article_issue_strings())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                   << _(L"An article is proceeding dynamic content, or a pronoun is being "
                        "formatted into a larger message. It is recommended to create "
                        "multiple versions of this string for all possible contexts instead "
                        "of using dynamic placeholders.")
                   << L"\"\t[articleOrPronoun]\n";
            }

        for (const auto& val : m_rc->get_localizable_strings_with_halfwidths())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                   << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
            report << _(L"String available for translation that contains halfwidth characters. "
                        "Fullwidth characters are recommended.");
            report << L"\"\t[halfWidth]\n";
            }

        for (const auto& val : m_rc->get_bad_dialog_font_sizes())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                   << replaceSpecialSpaces(val.m_string)
                   << L"\"\t\""
                   // this provides more detailed info about the issue
                   << replaceSpecialSpaces(val.m_usage.m_value) << L"\"\t[fontIssue]\n";
            }

        for (const auto& val : m_rc->get_non_system_dialog_fonts())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                   << replaceSpecialSpaces(val.m_usage.m_value) << L"\"\t[fontIssue]\n";
            }

        // macOS manifest files
        for (const auto& val : m_plist->get_missing_localization_bundles())
            {
            report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                   << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                   << _(L"No languages included in macOS Info.plist file. "
                        "This file should have a CFBundleLocalizations section with "
                        "an array of strings under it; otherwise, no translations will be loaded "
                        "at runtime.")
                   << L"\"\t[suspectI18NUsage]\n";
            }

        // gettext catalogs
        for (const auto& catEntry : m_po->get_catalog_entries())
            {
            for (const auto& issue : catEntry.second.m_issues)
                {
                if (issue.first == translation_issue::printf_issue)
                    {
                    report
                        << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                        << issue.second << L"\"\t\""
                        << _(L"Mismatching printf command between source and translation strings.")
                        << "\"\t[printfMismatch]\n";
                    }
                else if (issue.first == translation_issue::suspect_source_issue)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"String available for translation that probably should not be, or "
                                "contains a hard-coded URL or email address.")
                           << "\"\t[suspectL10NString]\n";
                    }
                else if (issue.first == translation_issue::excessive_nonl10n_content)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"String available for translation that contains a "
                                "large amount of non-translatable content.")
                           << "\"\t[excessiveNonL10NContent]\n";
                    }
                else if (issue.first == translation_issue::malformed_translation)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"String available for translation that is possibly malformed.")
                           << "\"\t[malformedString]\n";
                    }
                else if (issue.first == translation_issue::concatenation_issue)
                    {
                    report
                        << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                        << issue.second << L"\"\t\""
                        << _(L"String available for translation that may be getting concatenated "
                             "at runtime with other content.")
                        << "\"\t[concatenatedStrings]\n";
                    }
                else if (issue.first == translation_issue::source_needing_context_issue)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"Ambiguous string available for translation that is "
                                "lacking a translator comment.")
                           << "\"\t[L10NStringNeedsContext]\n";
                    }
                else if (issue.first == translation_issue::multipart_string)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"String available for translation may contain multiple sections "
                                "being sliced at runtime. Consider splitting each section into "
                                "a separate resource.")
                           << "\"\t[multipartString]\n";
                    }
                else if (issue.first == translation_issue::article_issue)
                    {
                    report
                        << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                        << issue.second << L"\"\t\""
                        << _(L"An article is proceeding dynamic content, or a pronoun is being "
                             "formatted into a larger message. It is recommended to create "
                             "multiple versions of this string for all possible contexts instead "
                             "of using dynamic placeholders.")
                        << "\"\t[articleOrPronoun]\n";
                    }
                else if (issue.first == translation_issue::pluralization)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"The same string is used for singular and plural situations. "
                                "Consider using a pluralizing function (if available) or "
                                "reword the message.")
                           << "\"\t[pluralization]\n";
                    }
                else if (issue.first == translation_issue::accelerator_issue)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"Mismatching keyboard accelerators between source "
                                "and translation strings.")
                           << "\"\t[acceleratorMismatch]\n";
                    }
                else if (issue.first == translation_issue::number_issue)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"Mismatching numbers between source "
                                "and translation strings.")
                           << "\"\t[numberInconsistency]\n";
                    }
                else if (issue.first == translation_issue::length_issue)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"Translation is suspiciously longer than the source string.")
                           << "\"\t[lengthInconsistency]\n";
                    }
                else if (issue.first == translation_issue::consistency_issue)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"Mismatching first character casing, mismatching trailing "
                                "punctuation/spaces/newlines, or mismatching number of tabs or "
                                "pipe symbols between source and translation strings.")
                           << "\"\t[transInconsistency]\n";
                    }
                else if (issue.first == translation_issue::halfwidth)
                    {
                    report << catEntry.first << L"\t" << catEntry.second.m_line << L"\t\t\""
                           << issue.second << L"\"\t\""
                           << _(L"Halfwidth characters detected. "
                                "Fullwidth characters are recommended.")
                           << "\"\t[halfWidth]\n";
                    }
                }
            }

        // Source code warnings
        const auto formatSourceFileWarnings =
            [&replaceSpecialSpaces, &report, &verbose](const auto* sourceParser)
        {
            for (const auto& val : sourceParser->get_unsafe_localizable_strings())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                if (val.m_usage.m_type ==
                    i18n_review::string_info::usage_info::usage_type::function)
                    {
                    report << _(L"String available for translation that probably "
                                "should not be in function call: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                else if (val.m_usage.m_type ==
                         i18n_review::string_info::usage_info::usage_type::variable)
                    {
                    report << _(L"String available for translation that probably "
                                "should not be assigned to variable: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                else
                    {
                    report << _(L"String available for translation that probably "
                                "should not be within: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                report << L"[suspectL10NString]\n";
                }

            for (const auto& val : sourceParser->get_localizable_strings_with_urls())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                if (val.m_usage.m_type ==
                    i18n_review::string_info::usage_info::usage_type::function)
                    {
                    report << _(L"String available for translation that contains an "
                                "URL or email address in function call: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                else if (val.m_usage.m_type ==
                         i18n_review::string_info::usage_info::usage_type::variable)
                    {
                    report << _(L"String available for translation that contains an "
                                "URL or email address assigned to variable: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                else
                    {
                    report << _(L"String available for translation that contains an "
                                "URL or email address within: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                report << L"[urlInL10NString]\n";
                }

            for (const auto& val :
                 sourceParser->get_localizable_strings_with_unlocalizable_content())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                report << _(L"String available for translation that contains a "
                            "large amount of non-translatable.");
                report << L"\t\"[excessiveNonL10NContent]\n";
                }

            for (const auto& val : sourceParser->get_suspect_i18n_usuage())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                report << val.m_usage.m_value;
                report << L"\t\"[suspectI18NUsage]\n";
                }

            for (const auto& val :
                 sourceParser->get_localizable_strings_ambiguous_needing_context())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                report << _(L"Ambiguous string available for translation that is "
                            "lacking a translator comment.");
                report << L"\t\"[L10NStringNeedsContext]\n";
                }

            for (const auto& val : sourceParser->get_localizable_strings_being_concatenated())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                report << _(L"String available for translation that may be getting concatenated "
                            "at runtime with other content.");
                report << L"\"\t[concatenatedStrings]\n";
                }

            for (const auto& val : sourceParser->get_literal_localizable_strings_being_compared())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                report << _(L"Literal string available for translation that is either being "
                            "searched for or compared against. It is recommended to make this "
                            "localizable string a variable and use that for "
                            "all locations that this string is referenced.");
                report << L"\"\t[literalL10NStringCompare]\n";
                }

            for (const auto& val : sourceParser->get_multipart_strings())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                report << _(L"String available for translation may contain multiple sections "
                            "being sliced at runtime. Consider splitting each section into "
                            "a separate resource.");
                report << L"\"\t[multipartString]\n";
                }

            for (const auto& val : sourceParser->get_faux_plural_strings())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                report << _(L"The same string is used for singular and plural situations. "
                            "Consider using a pluralizing function (if available) or "
                            "reword the message.");
                report << L"\"\t[pluralization]\n";
                }

            for (const auto& val : sourceParser->get_article_issue_strings())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t\t\""
                       << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                       << _(L"An article is proceeding dynamic content, or a pronoun is being "
                            "formatted into a larger message. It is recommended to create "
                            "multiple versions of this string for all possible contexts instead "
                            "of using dynamic placeholders.")
                       << L"\"\t[articleOrPronoun]\n";
                }

            for (const auto& val : sourceParser->get_localizable_strings_with_halfwidths())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                report << _(L"String available for translation that contains halfwidth characters. "
                            "Fullwidth characters are recommended.");
                report << L"\"\t[halfWidth]\n";
                }

            for (const auto& val : sourceParser->get_localizable_strings_in_internal_call())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                if (val.m_usage.m_type ==
                    i18n_review::string_info::usage_info::usage_type::function)
                    {
                    report << _(L"Localizable string being used within non-user"
                                " facing function call: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                else if (val.m_usage.m_type ==
                         i18n_review::string_info::usage_info::usage_type::variable)
                    {
                    report << _(L"Localizable string being assigned to non-user facing variable: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                else if (!val.m_usage.m_value.empty())
                    {
                    report << _(L"Localizable string being assigned to: ") << val.m_usage.m_value
                           << L"\"\t";
                    }
                else
                    {
                    report << _(L"Localizable string being used within non-user element.")
                           << L"\"\t";
                    }
                report << L"[suspectL10NUsage]\n";
                }

            for (const auto& val : sourceParser->get_not_available_for_localization_strings())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\"";
                if (val.m_usage.m_type ==
                    i18n_review::string_info::usage_info::usage_type::function)
                    {
                    report << _(L"String not available for translation in function call: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                else if (val.m_usage.m_type ==
                         i18n_review::string_info::usage_info::usage_type::variable)
                    {
                    report << _(L"String not available for translation assigned to variable: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                else if (!val.m_usage.m_value.empty())
                    {
                    report << _(L"String not available for translation assigned to: ")
                           << val.m_usage.m_value << L"\"\t";
                    }
                else
                    {
                    report << _(L"String not available for translation.") << L"\"\t";
                    }
                report << L"[notL10NAvailable]\n";
                }

            for (const auto& val : sourceParser->get_deprecated_macros())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                       << replaceSpecialSpaces(val.m_string) << L"\"\t\"" << val.m_usage.m_value
                       << L"\"\t[deprecatedMacro]\n";
                }

            for (const auto& val : sourceParser->get_printf_single_numbers())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                       << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                       << _(L"Prefer using std::to_[w]string() instead of printf() to "
                            "format a number.")
                       << L"\"\t[printfSingleNumber]\n";
                }

            for (const auto& val : sourceParser->get_duplicates_value_assigned_to_ids())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t\t"
                       << replaceSpecialSpaces(val.m_string) << L"\t\""
                       << _(L"Verify that duplicate assignment was intended. "
                            "If correct, consider assigning the first ID variable by name "
                            "to the second one to make this intention clear.")
                       << L"\"\t[dupValAssignedToIds]\n";
                }

            for (const auto& val : sourceParser->get_ids_assigned_number())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t\t"
                       << replaceSpecialSpaces(val.m_string) << L"\t\""
                       << _(L"Prefer using ID constants provided by your framework when "
                            "assigning values to an ID variable.")
                       << L"\"\t[numberAssignedToId]\n";
                }

            for (const auto& val : sourceParser->get_malformed_strings())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                       << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                       << _(L"Malformed syntax in string.") << L"\"\t[malformedString]\n";
                }

            for (const auto& val : sourceParser->get_unencoded_ext_ascii_strings())
                {
                std::wstringstream encodingRecommendations;
                auto untabbedStr{ replaceSpecialSpaces(val.m_string) };
                for (const auto& ch : untabbedStr)
                    {
                    if (ch > 127)
                        {
                        encodingRecommendations << LR"(\U)" << std::setfill(L'0') << std::setw(8)
                                                << std::uppercase << std::hex
                                                << static_cast<int>(ch);
                        }
                    else
                        {
                        encodingRecommendations << ch;
                        }
                    }

                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t"
                       << L"\"" << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                       << _(L"String contains extended ASCII characters that should be encoded. "
                            "Recommended change: '")
                       << encodingRecommendations.str() << L"'\"\t[unencodedExtASCII]\n";
                }

            for (const auto& val : sourceParser->get_trailing_spaces())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                       << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                       << _(L"Trailing spaces detected at end of line.")
                       << L"\"\t[trailingSpaces]\n";
                }

            for (const auto& val : sourceParser->get_tabs())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                       << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                       << _(L"Tab detected in file; prefer using spaces.") << L"\"\t[tabs]\n";
                }

            for (const auto& val : sourceParser->get_wide_lines())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                       << replaceSpecialSpaces(val.m_string) << L"\"\t\"" << _(L"Line length: ")
                       << val.m_usage.m_value << L"\"\t[wideLine]\n";
                }

            for (const auto& val : sourceParser->get_comments_missing_space())
                {
                report << val.m_file_name << L"\t" << val.m_line << L"\t" << val.m_column << L"\t\""
                       << replaceSpecialSpaces(val.m_string) << L"\"\t\""
                       << _(L"Space should be inserted between comment tag and comment.")
                       << L"\"\t[commentMissingSpace]\n";
                }

            if (verbose)
                {
                for (const auto& parseErr : sourceParser->get_error_log())
                    {
                    report << parseErr.m_file_name << L"\t";
                    if (parseErr.m_line != std::wstring::npos)
                        {
                        report << parseErr.m_line;
                        }
                    report << L"\t";
                    if (parseErr.m_column != std::wstring::npos)
                        {
                        report << parseErr.m_column;
                        }
                    report << L"\t\"" << parseErr.m_resourceString << L"\"\t\""
                           << parseErr.m_message << L"\"\t[debugParserInfo]\n";
                    }
                }
        };

        formatSourceFileWarnings(m_cpp);
        formatSourceFileWarnings(m_csharp);

        for (const auto& file : m_filesThatShouldBeConvertedToUTF8)
            {
            report << file << L"\t\t\t\t\""
                   << _(L"File contains extended ASCII characters, "
                        "but is not encoded as UTF-8.")
                   << L"\"\t[nonUTF8File]\n";
            }

        for (const auto& file : m_filesThatContainUTF8Signature)
            {
            report << file << L"\t\t\t\t\""
                   << _(L"File contains UTF-8 signature; "
                        "It is recommended to save without the file signature for "
                        "best compiler portability.")
                   << L"\"\t[UTF8FileWithBOM]\n";
            }

        return report;
        }
    } // namespace i18n_check
