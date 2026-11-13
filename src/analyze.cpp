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
#include "csharp_i18n_review.h"
#include "i18n_string_util.h"
#include "unicode_extract_text.h"
#include "utfcpp/source/utf8.h"
#include <cstdint>
#include <iostream>
#include <iterator>
#include <optional>
#ifdef wxVERSION_NUMBER
    #include <wx/file.h>
#endif

namespace i18n_check
    {
    //------------------------------------------------------
    bool valid_utf8_file(const std::filesystem::path& filePath, bool& startsWithBom)
        {
        startsWithBom = false;
        // read as binary so that the entire file is validated
        // (text mode on Windows stops reading at a 0x1A byte)
        const std::ifstream ifs(filePath, std::ios::binary);
        if (!ifs)
            {
            return false;
            }

        std::istreambuf_iterator<char> it(ifs.rdbuf());
        const std::istreambuf_iterator<char> eos;

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
        // Must be read as binary: in text mode, the Microsoft C runtime converts CRLF byte pairs to
        // LF and treats 0x1A as EOF, both of which can legitimately appear inside of UTF-16 text
        // (e.g., 'К' [U+041A] is stored as "1A 04" in UTF-16LE).
        std::ifstream fs8(filePath, std::ios::binary);
        if (!fs8.is_open())
            {
            std::wcout << L"Could not open " << filePath << L"\n";
            return std::make_pair(false, std::wstring{});
            }

        fs8.seekg(0, std::ios_base::end);
        const auto fileLength = fs8.tellg();
        fs8.seekg(0);
        // need at least a 2-byte BOM to be UTF-16
        if (fileLength < 2)
            {
            return std::make_pair(false, std::wstring{});
            }
        std::vector<char> fileData(static_cast<size_t>(fileLength));
        fs8.read(fileData.data(), fileLength);
        fileData.resize(static_cast<size_t>(fs8.gcount()));
        if (fileData.size() < 2)
            {
            return std::make_pair(false, std::wstring{});
            }

        if (lily_of_the_valley::unicode_extract_text::is_unicode(fileData.data()))
            {
            lily_of_the_valley::unicode_extract_text uExtract;
            uExtract(fileData.data(), fileData.size(),
                     lily_of_the_valley::unicode_extract_text::is_little_endian(fileData.data()));
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

        unsigned lineCount = 1;
        std::string line;
        std::wstring buffer;
        buffer.reserve(static_cast<size_t>(std::filesystem::file_size(filePath)));

        while (std::getline(fs8, line))
            {
            if (auto endIt = utf8::find_invalid(line.begin(), line.end()); endIt != line.cend())
                {
                std::cout << "Invalid UTF-8 encoding detected at line " << lineCount << "\n";
                std::cout << "This part is OK: " << std::string(line.begin(), endIt) << "\n";
                }

            // Convert it to utf-16
            const std::u16string utf16line = utf8::utf8to16(line);
            for (const auto& ch : utf16line)
                {
                buffer += static_cast<wchar_t>(ch);
                }
            buffer += L"\n";

            ++lineCount;
            }

        return std::make_pair(true, buffer);
        }

    //------------------------------------------------------
    void batch_analyze::pseudo_translate(const std::vector<std::filesystem::path>& filesToTranslate,
                                         i18n_check::pseudo_translation_method pseudoMethod,
                                         bool addSurroundingBrackets, int8_t widthChange,
                                         bool addTrackingIds,
                                         const analyze_callback_reset& resetCallback,
                                         const analyze_callback& callback)
        {
        size_t currentFileIndex{ 0 };

        const auto outputFile =
            [this](const std::filesystem::path& filePath, const std::wstring& content)
        {
            // wchar_t is UTF-16 on Windows (where characters outside of the BMP are
            // surrogate pairs), but UTF-32 on most other platforms.
            std::string utfBuffer;
            utfBuffer.reserve(content.size());
            if constexpr (sizeof(wchar_t) == 2)
                {
                utf8::utf16to8(content.cbegin(), content.cend(), std::back_inserter(utfBuffer));
                }
            else
                {
                utf8::utf32to8(content.cbegin(), content.cend(), std::back_inserter(utfBuffer));
                }

            if (std::ofstream out(filePath); out.is_open())
                {
                out.write(utfBuffer.c_str(), utfBuffer.size());
                m_logReport.append(_WXTRANS_WSTR(L"\nPseudo-translation catalog generated at: "))
                    .append(filePath.wstring());
                }
        };

        pseudo_translator trans;
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

            // only PO (and POT) files are pseudo-translated, so don't bother reading anything else
            if (get_file_type(file) != file_review_type::po)
                {
                continue;
                }

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
                    trans.translate_po_file(fileUtf8Text);
                    outputFile(outPath, fileUtf8Text);
                    }
                else if (auto [readUtf16Ok, fileUtf16Text] = read_utf16_file(file); readUtf16Ok)
                    {
                    // UTF-16 may not be supported consistently on all platforms and compilers
                    trans.translate_po_file(fileUtf16Text);
                    outputFile(outPath, fileUtf16Text);
                    }
                else
                    {
                    std::wifstream ifs(file);
                    std::wstring str((std::istreambuf_iterator<wchar_t>(ifs)),
                                     std::istreambuf_iterator<wchar_t>());
                    trans.translate_po_file(str);
                    outputFile(outPath, str);
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
                                const analyze_callback_reset& resetCallback,
                                const analyze_callback& callback)
        {
        m_filesThatShouldBeConvertedToUTF8.clear();
        m_filesThatContainUTF8Signature.clear();
        m_cpp->clear_results();
        m_rc->clear_results();
        m_plist->clear_results();
        m_po->clear_results();
        m_csharp->clear_results();
        m_quarto->clear_results();
        m_lastBatchSize = filesToAnalyze.size();

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

            // sends the file's text to the analyzer for its file type
            const auto loadIntoAnalyzer = [this, fileType, &file](const std::wstring_view text)
            {
                switch (fileType)
                    {
                case file_review_type::rc:
                    (*m_rc)(text, file);
                    break;
                case file_review_type::infoplist:
                    (*m_plist)(text, file);
                    break;
                case file_review_type::po:
                    (*m_po)(text, file);
                    break;
                case file_review_type::cs:
                    (*m_csharp)(text, file);
                    break;
                case file_review_type::quarto:
                    (*m_quarto)(text, file);
                    break;
                default:
                    (*m_cpp)(text, file);
                    break;
                    }
            };

            try
                {
                bool startsWithBom{ false };
                if (const auto [readUtf8Ok, fileUtf8Text] = read_utf8_file(file, startsWithBom);
                    readUtf8Ok)
                    {
                    if (startsWithBom && ((m_cpp->get_style() & check_utf8_with_signature) != 0))
                        {
                        m_filesThatContainUTF8Signature.push_back(file);
                        }
                    loadIntoAnalyzer(fileUtf8Text);
                    }
                else if (const auto [readUtf16Ok, fileUtf16Text] = read_utf16_file(file);
                         readUtf16Ok)
                    {
                    // UTF-16 or ANSI may not be supported consistently on
                    // all platforms and compilers.
                    // RC files are usually encoded in ANSI given their age,
                    // so don't check those files.
                    if (fileType != file_review_type::rc &&
                        ((m_cpp->get_style() & check_utf8_encoded) != 0))
                        {
                        m_filesThatShouldBeConvertedToUTF8.push_back(file);
                        }
                    loadIntoAnalyzer(fileUtf16Text);
                    }
                else
                    {
                    if (fileType != file_review_type::rc &&
                        ((m_cpp->get_style() & check_utf8_encoded) != 0))
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
                    loadIntoAnalyzer(str);
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
            m_csharp->review_strings(resetCallback, callback);
            m_po->review_strings(resetCallback, callback);
            }
        catch (const std::exception& expt)
            {
            m_logReport.append(i18n_string_util::lazy_string_to_wstring(expt.what())).append(L"\n");
            std::wcout << i18n_string_util::lazy_string_to_wstring(expt.what()) << L"\n";
            }
        }

    //------------------------------------------------------
    std::wstringstream batch_analyze::format_summary(const bool verbose) const
        {
        std::wstringstream report;
        if (verbose)
            {
            report
                << _(L"Checks Performed")
                << L"\n###################################################\n"
                << (((m_cpp->get_style() & check_l10n_strings) != 0) ? L"suspectL10NString\n" : L"")
                << (((m_cpp->get_style() & check_suspect_i18n_usage) != 0) ? L"suspectI18NUsage\n" :
                                                                             L"")
                << (((m_cpp->get_style() & check_suspect_l10n_string_usage) != 0) ?
                        L"suspectL10NUsage\n" :
                        L"")
                << (((m_cpp->get_style() & check_mismatching_printf_commands) != 0) ?
                        L"printfMismatch\n" :
                        L"")
                << (((m_cpp->get_style() & check_accelerators) != 0) ? L"acceleratorMismatch\n" :
                                                                       L"")
                << (((m_cpp->get_style() & check_consistency) != 0) ? L"transInconsistency\n" : L"")
                << (((m_cpp->get_style() & check_halfwidth) != 0) ? L"halfWidth\n" : L"")
                << (((m_cpp->get_style() & check_numbers) != 0) ? L"numberInconsistency\n" : L"")
                << (((m_cpp->get_style() & check_length) != 0) ? L"lengthInconsistency\n" : L"")
                << (((m_cpp->get_style() & check_needing_context) != 0) ?
                        L"L10NStringNeedsContext\n" :
                        L"")
                << (((m_cpp->get_style() & check_l10n_contains_url) != 0) ? L"urlInL10NString\n" :
                                                                            L"")
                << (((m_cpp->get_style() & check_multipart_strings) != 0) ? L"multipartString\n" :
                                                                            L"")
                << (((m_cpp->get_style() & check_pluralization) != 0) ? L"pluralization\n" : L"")
                << (((m_cpp->get_style() & check_articles_proceeding_placeholder) != 0) ?
                        L"articleOrPronoun\n" :
                        L"")
                << (((m_cpp->get_style() & check_l10n_contains_excessive_nonl10n_content) != 0) ?
                        L"excessiveNonL10NContent\n" :
                        L"")
                << (((m_cpp->get_style() & check_l10n_concatenated_strings) != 0) ?
                        L"concatenatedStrings\n" :
                        L"")
                << (((m_cpp->get_style() & check_literal_l10n_string_comparison) != 0) ?
                        L"literalL10NStringCompare\n" :
                        L"")
                << (((m_cpp->get_style() & check_not_available_for_l10n) != 0) ?
                        L"notL10NAvailable\n" :
                        L"")
                << (((m_cpp->get_style() & check_deprecated_macros) != 0) ? L"deprecatedMacro\n" :
                                                                            L"")
                << (((m_cpp->get_style() & check_utf8_encoded) != 0) ? L"nonUTF8File\n" : L"")
                << (((m_cpp->get_style() & check_utf8_with_signature) != 0) ? L"UTF8FileWithBOM\n" :
                                                                              L"")
                << (((m_cpp->get_style() & check_unencoded_ext_ascii) != 0) ?
                        L"unencodedExtASCII\n" :
                        L"")
                << (((m_cpp->get_style() & check_unencoded_ext_ascii) == 0) ? L"escapedUnicode\n" :
                                                                              L"")
                << (((m_cpp->get_style() & check_printf_single_number) != 0) ?
                        L"printfSingleNumber\n" :
                        L"")
                << (((m_cpp->get_style() & check_number_assigned_to_id) != 0) ?
                        L"numberAssignedToId\n" :
                        L"")
                << (((m_cpp->get_style() & check_duplicate_value_assigned_to_ids) != 0) ?
                        L"dupValAssignedToIds\n" :
                        L"")
                << (((m_cpp->get_style() & check_malformed_strings) != 0) ? L"malformedString\n" :
                                                                            L"")
                << (((m_cpp->get_style() & check_fonts) != 0) ? L"fontIssue\n" : L"")
                << (((m_cpp->get_style() & check_trailing_spaces) != 0) ? L"trailingSpaces\n" : L"")
                << (((m_cpp->get_style() & check_tabs) != 0) ? L"tabs\n" : L"")
                << (((m_cpp->get_style() & check_line_width) != 0) ? L"wideLine\n" : L"")
                << (((m_cpp->get_style() & check_space_after_comment) != 0) ?
                        L"commentMissingSpace\n" :
                        L"")
                << L"\n";
            }
        report << _(L"Statistics") << L"\n###################################################\n"
               << _(L"# of files reviewed: ") << get_last_batch_size() << L"\n"
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
    std::wstringstream batch_analyze::format_results(const bool verbose /*= false*/) const
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

        // Writes a row in the format:
        // File\tLine\tColumn\t"Value"\t"Explanation"\t[ID]
        // An empty line, column, or value is written as an empty field.
        const auto writeRow = [&report](const auto& fileName, const std::optional<size_t> line,
                                        const std::optional<size_t> column,
                                        const std::wstring_view value, const auto& explanation,
                                        const std::wstring_view warningId)
        {
            report << fileName << L"\t";
            if (line)
                {
                report << *line;
                }
            report << L"\t";
            if (column)
                {
                report << *column;
                }
            report << L"\t";
            if (!value.empty())
                {
                report << L"\"" << value << L"\"";
                }
            report << L"\t\"" << explanation << L"\"\t[" << warningId << L"]\n";
        };

        // Writes a row for a string_info result.
        const auto writeInfoRow = [&writeRow, &replaceSpecialSpaces](
                                      const auto& val, const auto& explanation,
                                      const std::wstring_view warningId, const bool withColumn)
        {
            writeRow(val.m_file_name, val.m_line,
                     withColumn ? std::optional<size_t>{ val.m_column } : std::nullopt,
                     replaceSpecialSpaces(val.m_string), explanation, warningId);
        };

        // Windows resource file warnings
        for (const auto& val : m_rc->get_unsafe_localizable_strings())
            {
            writeInfoRow(val, _(L"String available for translation that probably should not be."),
                         L"suspectL10NString", false);
            }

        for (const auto& val : m_rc->get_localizable_strings_with_urls())
            {
            writeInfoRow(
                val, _(L"String available for translation that contains an URL or email address."),
                L"urlInL10NString", false);
            }

        for (const auto& val : m_rc->get_localizable_strings_with_unlocalizable_content())
            {
            writeInfoRow(val,
                         _(L"String available for translation that contains a "
                           "large amount of non-translatable content"),
                         L"excessiveNonL10NContent", false);
            }

        for (const auto& val : m_rc->get_localizable_strings_being_concatenated())
            {
            writeInfoRow(val,
                         _(L"String available for translation that may be getting concatenated "
                           "at runtime with other content."),
                         L"concatenatedStrings", false);
            }

        for (const auto& val : m_rc->get_multipart_strings())
            {
            writeInfoRow(val,
                         _(L"String available for translation may contain multiple sections "
                           "being sliced at runtime. Consider splitting each section into "
                           "a separate resource."),
                         L"multipartString", false);
            }

        for (const auto& val : m_rc->get_faux_plural_strings())
            {
            writeInfoRow(val,
                         _(L"The same string is used for singular and plural situations. "
                           "Consider using a pluralizing function (if available) or "
                           "reword the message."),
                         L"pluralization", false);
            }

        for (const auto& val : m_rc->get_article_issue_strings())
            {
            writeInfoRow(val,
                         _(L"An article is proceeding dynamic content, or a pronoun is being "
                           "formatted into a larger message. It is recommended to create "
                           "multiple versions of this string for all possible contexts instead "
                           "of using dynamic placeholders."),
                         L"articleOrPronoun", false);
            }

        for (const auto& val : m_rc->get_localizable_strings_with_halfwidths())
            {
            writeInfoRow(val,
                         _(L"String available for translation that contains halfwidth characters. "
                           "Fullwidth characters are recommended."),
                         L"halfWidth", true);
            }

        // the usage value provides more detailed info about the font issue
        for (const auto& val : m_rc->get_bad_dialog_font_sizes())
            {
            writeInfoRow(val, replaceSpecialSpaces(val.m_usage.m_value), L"fontIssue", false);
            }

        for (const auto& val : m_rc->get_non_system_dialog_fonts())
            {
            writeInfoRow(val, replaceSpecialSpaces(val.m_usage.m_value), L"fontIssue", false);
            }

        // macOS manifest files
        for (const auto& val : m_plist->get_missing_localization_bundles())
            {
            writeInfoRow(
                val,
                _(L"No languages included in macOS Info.plist file. "
                  "This file should have a CFBundleLocalizations section with "
                  "an array of strings under it; otherwise, no translations will be loaded "
                  "at runtime."),
                L"suspectI18NUsage", false);
            }

        // Quarto files
        for (const auto& val : m_quarto->get_multi_sentence_lines())
            {
            writeInfoRow(val,
                         _(L"Line contains more than one sentence. Consider placing each sentence "
                           "on its own line to simplify resource extraction."),
                         L"suspectL10NString", true);
            }

        for (const auto& val : m_quarto->get_multi_line_sentences())
            {
            writeInfoRow(val,
                         _(L"Sentence is split across multiple lines. Consider placing each "
                           "sentence on its own line to simplify resource extraction."),
                         L"suspectL10NString", true);
            }

        for (const auto& val : m_quarto->get_malformed_content())
            {
            writeInfoRow(
                val,
                _(L"Shortcodes should have a blank line between them and any following content."),
                L"malformedString", true);
            }

        for (const auto& val : m_quarto->get_smart_quotes())
            {
            writeInfoRow(val,
                         _(L"Prefer straight quotes instead of smart quotes. "
                           "Pandoc will automatically convert them to "
                           "language-appropriate quotation marks."),
                         L"malformedString", true);
            }

        for (const auto& val : m_quarto->get_range_dash_issues())
            {
            writeInfoRow(val, _(L"Use an en dash (–) for ranges (e.g., 141–48, A–Z)."),
                         L"malformedString", true);
            }

        for (const auto& val : m_quarto->get_malformed_image_links())
            {
            writeInfoRow(val, _(L"Image link is malformed."), L"malformedString", true);
            }

        for (const auto& val : m_quarto->get_absolute_path_links())
            {
            writeInfoRow(val, _(L"Use relative paths instead of absolute or site-root paths."),
                         L"malformedString", true);
            }

        // gettext catalogs
        for (const auto& catEntry : m_po->get_catalog_entries())
            {
            const auto writePoRow = [&writeRow, &catEntry](const std::wstring_view value,
                                                           const auto& explanation,
                                                           const std::wstring_view warningId)
            {
                writeRow(catEntry.first, catEntry.second.m_line, std::nullopt, value, explanation,
                         warningId);
            };

            for (const auto& issue : catEntry.second.m_issues)
                {
                if (issue.first == translation_issue::printf_issue)
                    {
                    writePoRow(
                        issue.second,
                        _(L"Mismatching printf command between source and translation strings."),
                        L"printfMismatch");
                    }
                else if (issue.first == translation_issue::suspect_source_issue)
                    {
                    writePoRow(issue.second,
                               _(L"String available for translation that probably should not be, "
                                 "or contains a hard-coded URL or email address."),
                               L"suspectL10NString");
                    }
                else if (issue.first == translation_issue::excessive_nonl10n_content)
                    {
                    writePoRow(issue.second,
                               _(L"String available for translation that contains a "
                                 "large amount of non-translatable content."),
                               L"excessiveNonL10NContent");
                    }
                else if (issue.first == translation_issue::malformed_translation)
                    {
                    writePoRow(issue.second,
                               _(L"String available for translation that is possibly malformed."),
                               L"malformedString");
                    }
                else if (issue.first == translation_issue::concatenation_issue)
                    {
                    writePoRow(
                        issue.second,
                        _(L"String available for translation that may be getting concatenated "
                          "at runtime with other content."),
                        L"concatenatedStrings");
                    }
                else if (issue.first == translation_issue::source_needing_context_issue)
                    {
                    writePoRow(issue.second,
                               _(L"Ambiguous string available for translation that is "
                                 "lacking a translator comment."),
                               L"L10NStringNeedsContext");
                    }
                else if (issue.first == translation_issue::multipart_string)
                    {
                    writePoRow(issue.second,
                               _(L"String available for translation may contain multiple sections "
                                 "being sliced at runtime. Consider splitting each section into "
                                 "a separate resource."),
                               L"multipartString");
                    }
                else if (issue.first == translation_issue::article_issue)
                    {
                    writePoRow(
                        issue.second,
                        _(L"An article is proceeding dynamic content, or a pronoun is being "
                          "formatted into a larger message. It is recommended to create "
                          "multiple versions of this string for all possible contexts instead "
                          "of using dynamic placeholders."),
                        L"articleOrPronoun");
                    }
                else if (issue.first == translation_issue::pluralization)
                    {
                    writePoRow(issue.second,
                               _(L"The same string is used for singular and plural situations. "
                                 "Consider using a pluralizing function (if available) or "
                                 "reword the message."),
                               L"pluralization");
                    }
                else if (issue.first == translation_issue::accelerator_issue)
                    {
                    writePoRow(issue.second,
                               _(L"Mismatching keyboard accelerators between source "
                                 "and translation strings."),
                               L"acceleratorMismatch");
                    }
                else if (issue.first == translation_issue::number_issue)
                    {
                    writePoRow(issue.second,
                               _(L"Mismatching numbers between source "
                                 "and translation strings."),
                               L"numberInconsistency");
                    }
                else if (issue.first == translation_issue::length_issue)
                    {
                    writePoRow(issue.second,
                               _(L"Translation is suspiciously longer than the source string."),
                               L"lengthInconsistency");
                    }
                else if (issue.first == translation_issue::consistency_issue)
                    {
                    // the issue text is the explanation, there is no value
                    writePoRow(std::wstring_view{}, issue.second, L"transInconsistency");
                    }
                else if (issue.first == translation_issue::halfwidth)
                    {
                    writePoRow(issue.second,
                               _(L"Halfwidth characters detected. "
                                 "Fullwidth characters are recommended."),
                               L"halfWidth");
                    }
                }
            }

        // Source code warnings
        const auto formatSourceFileWarnings =
            [&replaceSpecialSpaces, &writeRow, &writeInfoRow, &verbose](const auto* sourceParser)
        {
            using usage_type = i18n_review::string_info::usage_info::usage_type;

            for (const auto& val : sourceParser->get_unsafe_localizable_strings())
                {
                const std::wstring explanation =
                    (val.m_usage.m_type == usage_type::function) ?
                        _WXTRANS_WSTR(L"String available for translation that probably "
                                      "should not be in function call: ") :
                    (val.m_usage.m_type == usage_type::variable) ?
                        _WXTRANS_WSTR(L"String available for translation that probably "
                                      "should not be assigned to variable: ") :
                        _WXTRANS_WSTR(L"String available for translation that probably "
                                      "should not be within: ");
                writeInfoRow(val, explanation + val.m_usage.m_value, L"suspectL10NString", true);
                }

            for (const auto& val : sourceParser->get_localizable_strings_with_urls())
                {
                const std::wstring explanation =
                    (val.m_usage.m_type == usage_type::function) ?
                        _WXTRANS_WSTR(L"String available for translation that contains an "
                                      "URL or email address in function call: ") :
                    (val.m_usage.m_type == usage_type::variable) ?
                        _WXTRANS_WSTR(L"String available for translation that contains an "
                                      "URL or email address assigned to variable: ") :
                        _WXTRANS_WSTR(L"String available for translation that contains an "
                                      "URL or email address within: ");
                writeInfoRow(val, explanation + val.m_usage.m_value, L"urlInL10NString", true);
                }

            for (const auto& val :
                 sourceParser->get_localizable_strings_with_unlocalizable_content())
                {
                writeInfoRow(val,
                             _(L"String available for translation that contains a "
                               "large amount of non-translatable content."),
                             L"excessiveNonL10NContent", true);
                }

            for (const auto& val : sourceParser->get_suspect_i18n_usage())
                {
                writeInfoRow(val, val.m_usage.m_value, L"suspectI18NUsage", true);
                }

            for (const auto& val :
                 sourceParser->get_localizable_strings_ambiguous_needing_context())
                {
                writeInfoRow(val,
                             _(L"Ambiguous string available for translation that is "
                               "lacking a translator comment."),
                             L"L10NStringNeedsContext", true);
                }

            for (const auto& val : sourceParser->get_localizable_strings_being_concatenated())
                {
                writeInfoRow(val,
                             _(L"String available for translation that may be getting "
                               "concatenated at runtime with other content."),
                             L"concatenatedStrings", true);
                }

            for (const auto& val : sourceParser->get_literal_localizable_strings_being_compared())
                {
                writeInfoRow(val,
                             _(L"Literal string available for translation that is either being "
                               "searched for or compared against. It is recommended to make this "
                               "localizable string a variable and use that for "
                               "all locations that this string is referenced."),
                             L"literalL10NStringCompare", true);
                }

            for (const auto& val : sourceParser->get_multipart_strings())
                {
                writeInfoRow(val,
                             _(L"String available for translation may contain multiple sections "
                               "being sliced at runtime. Consider splitting each section into "
                               "a separate resource."),
                             L"multipartString", true);
                }

            for (const auto& val : sourceParser->get_faux_plural_strings())
                {
                writeInfoRow(val,
                             _(L"The same string is used for singular and plural situations. "
                               "Consider using a pluralizing function (if available) or "
                               "reword the message."),
                             L"pluralization", true);
                }

            for (const auto& val : sourceParser->get_article_issue_strings())
                {
                writeInfoRow(val,
                             _(L"An article is proceeding dynamic content, or a pronoun is being "
                               "formatted into a larger message. It is recommended to create "
                               "multiple versions of this string for all possible contexts "
                               "instead of using dynamic placeholders."),
                             L"articleOrPronoun", true);
                }

            for (const auto& val : sourceParser->get_localizable_strings_with_halfwidths())
                {
                writeInfoRow(val,
                             _(L"String available for translation that contains halfwidth "
                               "characters. Fullwidth characters are recommended."),
                             L"halfWidth", true);
                }

            for (const auto& val : sourceParser->get_localizable_strings_in_internal_call())
                {
                const std::wstring explanation =
                    (val.m_usage.m_type == usage_type::function) ?
                        _WXTRANS_WSTR(L"Localizable string being used within non-user"
                                      " facing function call: ") +
                            val.m_usage.m_value :
                    (val.m_usage.m_type == usage_type::variable) ?
                        _WXTRANS_WSTR(
                            L"Localizable string being assigned to non-user facing variable: ") +
                            val.m_usage.m_value :
                    (!val.m_usage.m_value.empty()) ?
                        _WXTRANS_WSTR(L"Localizable string being assigned to: ") +
                            val.m_usage.m_value :
                        std::wstring{ _WXTRANS_WSTR(
                            L"Localizable string being used within non-user element.") };
                writeInfoRow(val, explanation, L"suspectL10NUsage", true);
                }

            for (const auto& val : sourceParser->get_not_available_for_localization_strings())
                {
                const std::wstring explanation =
                    (val.m_usage.m_type == usage_type::function) ?
                        _WXTRANS_WSTR(L"String not available for translation in function call: ") +
                            val.m_usage.m_value :
                    (val.m_usage.m_type == usage_type::variable) ?
                        _WXTRANS_WSTR(
                            L"String not available for translation assigned to variable: ") +
                            val.m_usage.m_value :
                    (!val.m_usage.m_value.empty()) ?
                        _WXTRANS_WSTR(L"String not available for translation assigned to: ") +
                            val.m_usage.m_value :
                        std::wstring{ _WXTRANS_WSTR(L"String not available for translation.") };
                writeInfoRow(val, explanation, L"notL10NAvailable", true);
                }

            for (const auto& val : sourceParser->get_deprecated_macros())
                {
                writeInfoRow(val, val.m_usage.m_value, L"deprecatedMacro", true);
                }

            for (const auto& val : sourceParser->get_printf_single_numbers())
                {
                writeInfoRow(val,
                             _(L"Prefer using std::to_[w]string() instead of printf() to "
                               "format a number."),
                             L"printfSingleNumber", true);
                }

            for (const auto& val : sourceParser->get_printf_plural_mismatches())
                {
                writeInfoRow(val,
                             _(L"Mismatching printf commands between singular and plural forms."),
                             L"printfMismatch", true);
                }

            for (const auto& val : sourceParser->get_duplicates_value_assigned_to_ids())
                {
                writeInfoRow(val,
                             _(L"Verify that duplicate assignment was intended. "
                               "If correct, consider assigning the first ID variable by name "
                               "to the second one to make this intention clear."),
                             L"dupValAssignedToIds", false);
                }

            for (const auto& val : sourceParser->get_ids_assigned_number())
                {
                writeInfoRow(val,
                             _(L"Prefer using ID constants provided by your framework when "
                               "assigning values to an ID variable."),
                             L"numberAssignedToId", false);
                }

            for (const auto& val : sourceParser->get_malformed_strings())
                {
                writeInfoRow(val, _(L"Malformed syntax in string."), L"malformedString", true);
                }

            for (const auto& val : sourceParser->get_unencoded_ext_ascii_strings())
                {
                std::wstringstream encodingRecommendations;
                auto untabbedStr{ replaceSpecialSpaces(val.m_string) };
                for (size_t i = 0; i < untabbedStr.length(); ++i)
                    {
                    auto codePoint{ static_cast<uint32_t>(untabbedStr[i]) };
                    if (codePoint > 127)
                        {
                        // On Windows, wchar_t is UTF-16, so characters outside of the BMP
                        // are stored as surrogate pairs. Combine them into a single code point,
                        // as a \U value of a lone surrogate is not valid C++.
                        if (codePoint >= 0xD800 && codePoint <= 0xDBFF &&
                            i + 1 < untabbedStr.length())
                            {
                            const auto lowSurrogate{ static_cast<uint32_t>(untabbedStr[i + 1]) };
                            if (lowSurrogate >= 0xDC00 && lowSurrogate <= 0xDFFF)
                                {
                                codePoint = 0x10000 + ((codePoint - 0xD800) << 10) +
                                            (lowSurrogate - 0xDC00);
                                ++i;
                                }
                            }
                        encodingRecommendations << LR"(\U)" << std::setfill(L'0') << std::setw(8)
                                                << std::uppercase << std::hex << codePoint;
                        }
                    else
                        {
                        encodingRecommendations << untabbedStr[i];
                        }
                    }

                writeInfoRow(val,
                             _WXTRANS_WSTR(L"String contains extended ASCII characters that "
                                           "should be encoded. Recommended change: '") +
                                 encodingRecommendations.str() + L"'",
                             L"unencodedExtASCII", true);
                }

            for (const auto& val : sourceParser->get_escaped_unicode_strings())
                {
                writeInfoRow(val,
                             _(L"String contains an escaped Unicode character sequence. "
                               "Prefer using the literal UTF-8 character instead."),
                             L"escapedUnicode", true);
                }

            for (const auto& val : sourceParser->get_trailing_spaces())
                {
                writeInfoRow(val, _(L"Trailing spaces detected at end of line."), L"trailingSpaces",
                             true);
                }

            for (const auto& val : sourceParser->get_tabs())
                {
                writeInfoRow(val, _(L"Tab detected in file; prefer using spaces."), L"tabs", true);
                }

            for (const auto& val : sourceParser->get_wide_lines())
                {
                writeInfoRow(val, _WXTRANS_WSTR(L"Line length: ") + val.m_usage.m_value,
                             L"wideLine", true);
                }

            for (const auto& val : sourceParser->get_comments_missing_space())
                {
                writeInfoRow(val, _(L"Space should be inserted between comment tag and comment."),
                             L"commentMissingSpace", true);
                }

            if (verbose)
                {
                for (const auto& parseErr : sourceParser->get_error_log())
                    {
                    writeRow(parseErr.m_file_name,
                             (parseErr.m_line != std::wstring::npos) ?
                                 std::optional<size_t>{ parseErr.m_line } :
                                 std::nullopt,
                             (parseErr.m_column != std::wstring::npos) ?
                                 std::optional<size_t>{ parseErr.m_column } :
                                 std::nullopt,
                             parseErr.m_resourceString, parseErr.m_message, L"debugParserInfo");
                    }
                }
        };

        formatSourceFileWarnings(m_cpp);
        formatSourceFileWarnings(m_csharp);

        // file-level warnings
        for (const auto& file : m_filesThatShouldBeConvertedToUTF8)
            {
            writeRow(file, std::nullopt, std::nullopt, std::wstring_view{},
                     _(L"File contains extended ASCII characters, "
                       "but is not encoded as UTF-8."),
                     L"nonUTF8File");
            }

        for (const auto& file : m_filesThatContainUTF8Signature)
            {
            writeRow(file, std::nullopt, std::nullopt, std::wstring_view{},
                     _(L"File contains UTF-8 signature; "
                       "It is recommended to save without the file signature for "
                       "best compiler portability."),
                     L"UTF8FileWithBOM");
            }

        return report;
        }
    } // namespace i18n_check
