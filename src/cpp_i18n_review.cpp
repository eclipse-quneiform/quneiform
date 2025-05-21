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

#include "cpp_i18n_review.h"

namespace i18n_check
    {
    //--------------------------------------------------
    void cpp_i18n_review::operator()(std::wstring_view srcText,
                                     const std::filesystem::path& fileName)
        {
        m_file_name = fileName;
        m_file_start = nullptr;

        if (srcText.empty())
            {
            return;
            }

        std::wstring cppBuffer{ srcText };
        wchar_t* cppText = cppBuffer.data();

        m_file_start = cppText;
        const wchar_t* const endSentinel =
            std::next(cppText, static_cast<ptrdiff_t>(srcText.length()));

        while (cppText != nullptr && std::next(cppText) < endSentinel && *cppText != 0)
            {
            while (std::next(cppText) < endSentinel && *cppText == L' ')
                {
                std::advance(cppText, 1);
                }
            // if a possible comment, then scan past it
            if (*cppText == L'/')
                {
                // see if a block comment (/*comment*/)
                if (*std::next(cppText) == L'*')
                    {
                    // Qt translator comments are left open until the next
                    // tr-like function is encountered, so if that is what we
                    // are on then we are done for now
                    const bool isQtTransComment = is_qt_translator_comment(
                        { std::next(cppText, 2),
                          static_cast<size_t>(endSentinel - std::next(cppText, 2)) });

                    // gettext translator comments must be right in front of the _() call,
                    // so for those we need to scan ahead here
                    m_context_comment_active = is_gettext_translator_comment(
                        { std::next(cppText, 2),
                          static_cast<size_t>(endSentinel - std::next(cppText, 2)) });

                    const auto [isSuppressed, suppressionEnd] = i18n_review::is_block_suppressed(
                        { std::next(cppText, 2),
                          static_cast<size_t>(endSentinel - std::next(cppText, 2)) });
                    if (isSuppressed)
                        {
                        m_context_comment_active = false;
                        clear_section(
                            cppText, std::next(cppText, static_cast<ptrdiff_t>(suppressionEnd + 2)));
                        std::advance(cppText, suppressionEnd);
                        }
                    wchar_t* end = std::wcsstr(cppText, L"*/");
                    if (end != nullptr && end < endSentinel)
                        {
                        clear_section(cppText, std::next(end, 2));
                        cppText = std::next(end, 2);
                        if (cppText >= endSentinel)
                            {
                            break;
                            }
                        }
                    // can't find ending tag, so just read in the rest of the text
                    else
                        {
                        return;
                        }
                    // move to next character
                    while (std::next(cppText) < endSentinel && std::iswspace(*cppText))
                        {
                        std::advance(cppText, 1);
                        }
                    // look ahead and see if next function is a translation function
                    // if we have a translator comment to connect to it
                    if (m_context_comment_active && !isQtTransComment)
                        {
                        const wchar_t* openingParen = std::wcschr(cppText, L'(');
                        if (openingParen != nullptr && openingParen < endSentinel)
                            {
                            m_context_comment_active = is_i18n_function(
                                { cppText, static_cast<size_t>(openingParen - cppText) });
                            }
                        }
                    else if (isQtTransComment)
                        {
                        m_context_comment_active = isQtTransComment;
                        }
                    }
                // or a single line comment
                else if (*std::next(cppText) == L'/' && std::next(cppText, 2) < endSentinel)
                    {
                    const bool isQtTransComment = is_qt_translator_comment(
                        { std::next(cppText, 2),
                          static_cast<size_t>(endSentinel - std::next(cppText, 2)) });

                    m_context_comment_active = is_gettext_translator_comment(
                        { std::next(cppText, 2),
                          static_cast<size_t>(endSentinel - std::next(cppText, 2)) });

                    const auto [isSuppressed, suppressionEnd] = i18n_review::is_block_suppressed(
                        { std::next(cppText, 2),
                          static_cast<size_t>(endSentinel - std::next(cppText, 2)) });
                    if (isSuppressed)
                        {
                        m_context_comment_active = false;
                        clear_section(
                            cppText, std::next(cppText, static_cast<ptrdiff_t>(suppressionEnd + 2)));
                        std::advance(cppText, suppressionEnd);
                        }

                    if (static_cast<bool>(m_review_styles & check_space_after_comment) &&
                        std::next(cppText, 2) < endSentinel &&
                        static_cast<bool>(std::iswalnum(*std::next(cppText, 2))) &&
                        // something like "//--------" is OK
                        *std::next(cppText, 2) != L'-')
                        {
                        m_comments_missing_space.push_back(
                            string_info(std::wstring{}, string_info::usage_info{}, m_file_name,
                                        get_line_and_column((cppText - m_file_start))));
                        }
                    // move to the end of the line
                    if (const size_t endOfLine = std::wcscspn(cppText, L"\n\r");
                        std::next(cppText, static_cast<ptrdiff_t>(endOfLine)) < endSentinel)
                        {
                        clear_section(cppText,
                                      std::next(cppText, static_cast<ptrdiff_t>(endOfLine)));
                        std::advance(cppText, endOfLine);
                        }
                    // move to next character
                    while (std::next(cppText) < endSentinel && std::iswspace(*cppText))
                        {
                        std::advance(cppText, 1);
                        }
                    // look ahead and see if next function is a translation function
                    // if we have a translator comment to connect to it
                    if (m_context_comment_active && !isQtTransComment)
                        {
                        // in case there are multiple lines of '//' comments,
                        // then clear those now to maintain our context comment active state
                        while (std::next(cppText) < endSentinel && *cppText == L'/' &&
                               *std::next(cppText) == L'/')
                            {
                            if (static_cast<bool>(m_review_styles & check_space_after_comment) &&
                                std::next(cppText, 2) < endSentinel &&
                                static_cast<bool>(std::iswalnum(*std::next(cppText, 2))) &&
                                // something like "//--------" is OK
                                *std::next(cppText, 2) != L'-')
                                {
                                m_comments_missing_space.push_back(string_info(
                                    std::wstring{}, string_info::usage_info{}, m_file_name,
                                    get_line_and_column((cppText - m_file_start))));
                                }
                            // move to the end of the line
                            if (const size_t endOfLine = std::wcscspn(cppText, L"\n\r");
                                std::next(cppText, static_cast<ptrdiff_t>(endOfLine)) < endSentinel)
                                {
                                clear_section(
                                    cppText, std::next(cppText, static_cast<ptrdiff_t>(endOfLine)));
                                std::advance(cppText, endOfLine);
                                }
                            while (std::next(cppText) < endSentinel && std::iswspace(*cppText))
                                {
                                std::advance(cppText, 1);
                                }
                            }
                        const wchar_t* openingParen = std::wcschr(cppText, L'(');
                        if (openingParen != nullptr && openingParen < endSentinel)
                            {
                            m_context_comment_active = is_i18n_function(
                                { cppText, static_cast<size_t>(openingParen - cppText) });
                            }
                        }
                    else if (isQtTransComment)
                        {
                        m_context_comment_active = isQtTransComment;
                        }
                    }
                // not a comment
                else
                    {
                    std::advance(cppText, 1);
                    }
                }
            else if (*cppText == L'#')
                {
                cppText = process_preprocessor_directive(cppText, cppText - m_file_start);
                if (cppText == nullptr || cppText >= endSentinel)
                    {
                    break;
                    }
                }
            else if (((cppText == m_file_start) || !is_valid_name_char(*std::prev(cppText))) &&
                     is_assembly_block(cppText))
                {
                cppText = process_assembly_block(cppText);
                if (cppText == nullptr || cppText >= endSentinel)
                    {
                    break;
                    }
                }
            // ...or an actual quote
            else if (*cppText == L'\"')
                {
                // skip escaped quotes
                if (cppText > std::next(m_file_start) && *std::prev(cppText) == L'\\' &&
                    *std::prev(cppText, 2) != L'\\')
                    {
                    std::advance(cppText, 1);
                    continue;
                    }
                // skip quote symbol that is actually inside of single quote
                if (cppText > std::next(m_file_start) && *std::prev(cppText) == L'\'' &&
                    *std::next(cppText) == L'\'')
                    {
                    std::advance(cppText, 1);
                    continue;
                    }
                // see if this string is in a function call or is a direct variable assignment
                // and gauge whether it is meant to be translatable or not
                std::wstring functionName;
                variable_info variableInfo;
                std::wstring deprecatedMacroEncountered;
                size_t parameterPosition{ 0 };
                const wchar_t* startPos = std::prev(cppText, 1);
                const wchar_t* functionVarNamePos{ nullptr };
                bool isRawString{ false };
                wchar_t currentRawStringMarker{ L'R' };
                // if a raw string, step over 'R'
                if (is_raw_string_marker(*startPos))
                    {
                    currentRawStringMarker = *startPos;
                    isRawString = true;
                    std::advance(startPos, -1);
                    }
                // special case for triple quote (supported by languages like C#)
                if (std::wcsncmp(cppText, LR"(""")", 3) == 0)
                    {
                    currentRawStringMarker = *cppText;
                    isRawString = true;
                    std::advance(startPos, -1);
                    }
                // step back over double-byte, u16, or u32 prefixes
                if (*startPos == L'L' || *startPos == L'u' || *startPos == L'U')
                    {
                    std::advance(startPos, -1);
                    }
                // step back over UTF-8 'u8' symbol
                if (startPos > std::next(m_file_start) && *startPos == L'8' &&
                    *std::prev(startPos) != L'u')
                    {
                    std::advance(startPos, -2);
                    }
                // ...and spaces in front of quote
                while (startPos > m_file_start && static_cast<bool>(std::iswspace(*startPos)))
                    {
                    std::advance(startPos, -1);
                    }
                // if we are on a character that can be part of a variable
                // (and not punctuation) at this point,
                // then this might be a #defined variable
                if (is_valid_name_char(*startPos))
                    {
                    const wchar_t* directiveStart = startPos;
                    while (directiveStart > m_file_start && is_valid_name_char(*directiveStart))
                        {
                        std::advance(directiveStart, -1);
                        }
                    variableInfo.m_name.assign(std::next(directiveStart),
                                               startPos - directiveStart);
                    }
                else
                    {
                    functionVarNamePos = read_var_or_function_name(
                        startPos, m_file_start, functionName, variableInfo,
                        deprecatedMacroEncountered, parameterPosition);
                    }
                // find the end of the string now and feed it into the system
                std::advance(cppText, 1);
                wchar_t* end = cppText;
                if (isRawString)
                    {
                    cppText = raw_step_into_string(cppText, currentRawStringMarker);
                    end = find_raw_string_end(cppText, currentRawStringMarker);
                    }
                else
                    {
                    while (end != nullptr && end < endSentinel)
                        {
                        end = std::wcschr(end, L'\"');
                        if (end != nullptr)
                            {
                            // Watch out for escaped quotes.
                            // Note that we could have escaped slashes in front of a quote, so
                            // make sure that they slash in front of the quote (if there is one)
                            // is intended for it.
                            size_t proceedingSlashCount{ 0 };
                            const wchar_t* proceedingSlashes = std::prev(end);
                            while (proceedingSlashes >= m_file_start && *proceedingSlashes == L'\\')
                                {
                                std::advance(proceedingSlashes, -1);
                                ++proceedingSlashCount;
                                }
                            // something like "hello\\\" there" should be seen as "hello\" there".
                            if (proceedingSlashCount > 0 && ((proceedingSlashCount % 2) != 0))
                                {
                                std::advance(end, 1);
                                continue;
                                }

                            // see if there is more to this string on another line
                            wchar_t* connectedQuote = std::next(end);
                            while (connectedQuote < endSentinel &&
                                   static_cast<bool>(std::iswspace(*connectedQuote)))
                                {
                                std::advance(connectedQuote, 1);
                                }
                            // if a \ at the end of the line, then step over that and
                            // restart stepping over an more spaces on the next line
                            if (std::next(connectedQuote) < endSentinel &&
                                *connectedQuote == L'\\' &&
                                (*std::next(connectedQuote) == L'\r' ||
                                 *std::next(connectedQuote) == L'\n'))
                                {
                                clear_section(connectedQuote, std::next(connectedQuote));
                                end = std::next(connectedQuote, 2);
                                while (connectedQuote < endSentinel &&
                                       static_cast<bool>(std::iswspace(*connectedQuote)))
                                    {
                                    std::advance(connectedQuote, 1);
                                    }
                                }
                            // step over any comments at the end of the line
                            else if (std::next(connectedQuote) < endSentinel &&
                                *connectedQuote == L'/' &&
                                *std::next(connectedQuote) == L'/')
                                {
                                // move to the end of the line
                                if (const size_t endOfLine = std::wcscspn(connectedQuote, L"\n\r");
                                    std::next(connectedQuote, static_cast<ptrdiff_t>(endOfLine)) <
                                    endSentinel)
                                    {
                                    clear_section(
                                        connectedQuote,
                                                  std::next(connectedQuote,
                                                            static_cast<ptrdiff_t>(endOfLine)));
                                    std::advance(connectedQuote, endOfLine);
                                    while (connectedQuote < endSentinel &&
                                           static_cast<bool>(std::iswspace(*connectedQuote)))
                                        {
                                        std::advance(connectedQuote, 1);
                                        }
                                    }
                                }
                            else if (std::next(connectedQuote) < endSentinel &&
                                     *connectedQuote == L'/' && *std::next(connectedQuote) == L'*')
                                {
                                // move to the end of the line
                                wchar_t* endOfLine = std::wcsstr(connectedQuote, L"*/");
                                if (endOfLine != nullptr && endOfLine < endSentinel)
                                    {
                                    clear_section(connectedQuote, std::next(endOfLine, 2));
                                    connectedQuote = std::next(endOfLine, 2);
                                    while (connectedQuote < endSentinel &&
                                           static_cast<bool>(std::iswspace(*connectedQuote)))
                                        {
                                        std::advance(connectedQuote, 1);
                                        }
                                    }
                                }

                            if (connectedQuote < endSentinel && *connectedQuote == L'\"')
                                {
                                end = std::next(connectedQuote);
                                continue;
                                }
                            if (std::next(connectedQuote) < endSentinel &&
                                *connectedQuote == L'L' && *std::next(connectedQuote) == L'\"')
                                {
                                end = std::next(connectedQuote, 2);
                                continue;
                                }
                            // step over PRIu64 macro that appears between printf strings
                            // Format macros for the std::fprintf family of functions that may
                            // appear between quoted sections that will actually join the two quotes
                            const std::wregex intPrintfMacro{
                                LR"(PR[IN][uidoxX](8|16|32|64|FAST8|FAST16|FAST32|FAST64|LEAST8|LEAST16|LEAST32|LEAST64|MAX|PTR))"
                            };
                            constexpr size_t int64PrintfMacroLength{ 6 };
                            if (std::next(connectedQuote, int64PrintfMacroLength) < endSentinel &&
                                std::regex_match(
                                    std::wstring{ connectedQuote, int64PrintfMacroLength },
                                    intPrintfMacro))
                                {
                                clear_section(connectedQuote,
                                              std::next(connectedQuote, int64PrintfMacroLength));
                                std::advance(connectedQuote, int64PrintfMacroLength);
                                while (connectedQuote < endSentinel &&
                                       static_cast<bool>(std::iswspace(*connectedQuote)))
                                    {
                                    std::advance(connectedQuote, 1);
                                    }
                                if (connectedQuote < endSentinel && *connectedQuote == L'\"')
                                    {
                                    end = std::next(connectedQuote);
                                    continue;
                                    }
                                }
                            else if (connectedQuote < endSentinel && *connectedQuote == L'\\')
                                {
                                wchar_t* scanAhead{ std::next(connectedQuote) };
                                while (scanAhead < endSentinel &&
                                       string_util::is_either(*scanAhead, L'\n', L'\r'))
                                    {
                                    std::advance(scanAhead, 1);
                                    }
                                if (scanAhead < endSentinel && *scanAhead == L'\"')
                                    {
                                    clear_section(connectedQuote, std::next(connectedQuote, 1));
                                    connectedQuote = scanAhead;
                                    end = std::next(scanAhead);
                                    continue;
                                    }
                                }
                            break;
                            }
                        break;
                        }
                    }

                // if we found the end of the quote
                if (end != nullptr && end < endSentinel)
                    {
                    const wchar_t* nextChar = std::next(
                        end, (isRawString ? get_raw_step_size(currentRawStringMarker) + 1 : 1));
                    while (std::next(nextChar) < endSentinel && std::iswspace(*nextChar))
                        {
                        ++nextChar;
                        }
                    process_quote(cppText, end, functionVarNamePos, functionName, variableInfo,
                                  deprecatedMacroEncountered, parameterPosition, *nextChar == L',');
                    // closing quote was just cleared; now, clear the opening one
                    if (isRawString && std::prev(cppText, 2) >= m_file_start &&
                        *std::prev(cppText) == L'(' && *std::prev(cppText, 2) == L'\"')
                        {
                        *std::prev(cppText, 2) = L' ';
                        *std::prev(cppText) = L' ';
                        }
                    else if (std::prev(cppText) >= m_file_start && *std::prev(cppText) == L'\"')
                        {
                        *std::prev(cppText) = L' ';
                        }
                    cppText = std::next(
                        end, (isRawString ? get_raw_step_size(currentRawStringMarker) + 1 : 1));
                    if (cppText >= endSentinel)
                        {
                        break;
                        }
                    }
                }
            // ";}" should have a space or newline between them
            else if (*cppText == L';' && *std::next(cppText) == L'}')
                {
                log_message(L"MISSING SPACE",
                            L"Space or newline should be inserted between ';' and '}'.",
                            (cppText - m_file_start));
                std::advance(cppText, 1);
                }
            else
                {
                if (static_cast<bool>(m_review_styles & check_tabs) && *cppText == L'\t')
                    {
                    m_tabs.push_back(string_info(std::wstring{}, string_info::usage_info{},
                                                 m_file_name,
                                                 get_line_and_column((cppText - m_file_start))));
                    }
                else if (static_cast<bool>(m_review_styles & check_trailing_spaces) &&
                         *cppText == L' ' && std::next(cppText) < endSentinel &&
                         (*std::next(cppText) == L'\n' || *std::next(cppText) == L'\r'))
                    {
                    assert(cppText >= m_file_start);
                    auto prevLineStart = std::wstring_view{ m_file_start }.find_last_of(
                        L"\n\r", cppText - m_file_start);
                    if (prevLineStart == std::wstring::npos)
                        {
                        prevLineStart = 0;
                        }
                    // step forward to original line
                    ++prevLineStart;
                    std::wstring codeLine(
                        std::next(m_file_start, static_cast<ptrdiff_t>(prevLineStart)),
                        (cppText - std::next(m_file_start, static_cast<ptrdiff_t>(prevLineStart))));
                    string_util::ltrim(codeLine);
                    m_trailing_spaces.push_back(
                        string_info(codeLine, string_info::usage_info{}, m_file_name,
                                    get_line_and_column((cppText - m_file_start))));
                    }
                else if (static_cast<bool>(m_review_styles & check_line_width) &&
                         (*cppText == L'\n' || *cppText == L'\r') && cppText > m_file_start)
                    {
                    const auto currentPos{ (cppText - m_file_start) };
                    auto previousNewLine =
                        std::wstring_view{ m_file_start }.find_last_of(L"\n\r", currentPos - 1);
                    if (previousNewLine == std::wstring::npos)
                        {
                        previousNewLine = 0;
                        }
                    const size_t currentLineLength{ currentPos - (++previousNewLine) };
                    if (currentLineLength > m_max_line_length)
                        {
                        // ...also, only warn if the current line doesn't have a raw
                        // string in it--those can make it complicated to break a line
                        // into smaller lines.
                        // We will also ignore the line if it appears to be a long bitmask.
                        const std::wstring_view currentLine{
                            std::next(m_file_start, static_cast<ptrdiff_t>(previousNewLine)),
                            currentLineLength
                        };
                        if (currentLine.find(L"LR") == std::wstring::npos &&
                            currentLine.find(L'|') == std::wstring::npos)
                            {
                            m_wide_lines.emplace_back(
                                // truncate and add ellipsis
                                std::wstring{ currentLine.substr(
                                                  0, std::min<size_t>(32, currentLine.length())) }
                                    // NOLINT
                                    .append(L"..."),
                                string_info::usage_info{
                                    string_info::usage_info::usage_type::orphan,
                                    std::to_wstring(currentLineLength), std::wstring{},
                                    std::wstring{} },
                                m_file_name, get_line_and_column(currentPos));
                            }
                        }
                    }
                std::advance(cppText, 1);
                }
            }

        // review full content again once comments and preprocessor blocks are cleared
        if (m_wx_info.m_app_init_info.m_file_name.empty())
            {
            const size_t foundImplAppPos = cppBuffer.find(L"wxIMPLEMENT_APP");
            const size_t foundPos = cppBuffer.find(L"::OnInit()");
            if (foundImplAppPos != std::wstring::npos && foundPos != std::wstring::npos)
                {
                m_wx_info.m_app_init_info = string_info{
                    std::wstring{},
                    string_info::usage_info(string_info::usage_info::usage_type::function,
                                            L"OnInit()", std::wstring{}, std::wstring{}),
                    m_file_name, get_line_and_column(foundPos)
                };
                }
            }
        if (cppBuffer.find(L"wxUILocale::UseDefault()") != std::wstring::npos)
            {
            m_wx_info.m_wxuilocale_initialized = true;
            }
        if (cppBuffer.find(L"wxLocale") != std::wstring::npos)
            {
            m_wx_info.m_wxlocale_initialized = true;
            }

        load_id_assignments(cppBuffer, fileName);
        load_deprecated_functions(cppBuffer, fileName);
        load_suspect_i18n_ussage(cppBuffer, fileName);

        m_file_name.clear();
        m_file_start = nullptr;
        }

    //--------------------------------------------------
    void cpp_i18n_review::remove_decorations(std::wstring& str) const
        {
        while (str.length() > 0 && str.back() == L'&')
            {
            str.pop_back();
            }
        if (str.length() > 0 && str.back() == L'>')
            {
            const auto templateStart = str.find_last_of(L'<');
            if (templateStart != std::wstring::npos)
                {
                // if constructing a shared_ptr, then use the type that it is constructing
                if (std::wstring_view{ str }
                            .substr(0, templateStart)
                            .compare(L"std::make_shared") == 0 ||
                    std::wstring_view{ str }.substr(0, templateStart).compare(L"make_shared") ==
                        0 ||
                    std::wstring_view{ str }.substr(0, templateStart).compare(L"std::shared_ptr") ==
                        0 ||
                    std::wstring_view{ str }.substr(0, templateStart).compare(L"shared_ptr") == 0)
                    {
                    str.erase(0, templateStart + 1);
                    str.pop_back();
                    }
                // use the root type of the variable with template info stripped off
                else
                    {
                    str.erase(templateStart);
                    }
                }
            }
        // Strip off colons in front of string (e.g., the common practice of typing "::" in front
        // of items in the global namespace).
        // Also get rid of any accessors (e.g., '>' (from "->") or '.').
        if (str.length() > 0 && (str.front() == L':' || str.front() == L'>' || str.front() == L'.'))
            {
            const auto colonEnd = str.find_first_not_of(L":>.");
            str.erase(0, (colonEnd == std::wstring::npos) ? str.length() : colonEnd);
            }
        // lop off name of variable from member call (e.g., "str.Format" becomes "Format").
        const auto accessor = str.find_first_of(L">.");
        if (accessor != std::wstring::npos)
            {
            str.erase(0, accessor + 1);
            }
        }

    //--------------------------------------------------
    wchar_t* cpp_i18n_review::process_assembly_block(wchar_t* asmStart)
        {
        assert(asmStart);
        const std::wstring_view asmCommand1{ L"asm" };
        const std::wstring_view asmCommand2{ L"__asm__" };
        const std::wstring_view asmCommand3{ L"__asm" };
        wchar_t* const originalStart = asmStart;
        // GCC
        if (std::wcsncmp(asmStart, asmCommand1.data(), asmCommand1.length()) == 0 ||
            std::wcsncmp(asmStart, asmCommand2.data(), asmCommand2.length()) == 0)
            {
            std::advance(asmStart,
                         (std::wcsncmp(asmStart, asmCommand1.data(), asmCommand1.length()) == 0) ?
                             asmCommand1.length() :
                             asmCommand2.length());
            // step over spaces between __asm and its content
            while (*asmStart != 0 && static_cast<bool>(std::iswspace(*asmStart)))
                {
                std::advance(asmStart, 1);
                }
            const std::wstring_view volatileCommand1{ L"volatile" };
            const std::wstring_view volatileCommand2{ L"__volatile__" };
            // skip (optional) volatile modifier
            if (std::wcsncmp(asmStart, volatileCommand1.data(), volatileCommand1.length()) == 0)
                {
                std::advance(asmStart, volatileCommand1.length());
                while (*asmStart != 0 && static_cast<bool>(std::iswspace(*asmStart)))
                    {
                    std::advance(asmStart, 1);
                    }
                }
            else if (std::wcsncmp(asmStart, volatileCommand2.data(), volatileCommand2.length()) ==
                     0)
                {
                std::advance(asmStart, volatileCommand2.length());
                while (*asmStart != 0 && static_cast<bool>(std::iswspace(*asmStart)))
                    {
                    std::advance(asmStart, 1);
                    }
                }
            if (*asmStart == L'(')
                {
                const auto* end =
                    string_util::find_matching_close_tag(std::next(asmStart), L'(', L')', false);
                if (end == nullptr)
                    {
                    log_message(L"asm", L"Missing closing ')' in asm block.",
                                (asmStart - m_file_start));
                    return std::next(asmStart, 1);
                    }
                clear_section(originalStart, std::next(end));
                return std::next(const_cast<wchar_t*>(end));
                }
            if (*asmStart != 0)
                {
                const size_t end = std::wcscspn(asmStart, L"\n\r");
                clear_section(originalStart, std::next(asmStart, static_cast<ptrdiff_t>(end + 1)));
                return std::next(asmStart, static_cast<ptrdiff_t>(end + 1));
                }
            return nullptr;
            }
        // MSVC
        if (std::wcsncmp(asmStart, asmCommand3.data(), asmCommand3.length()) == 0)
            {
            std::advance(asmStart, asmCommand3.length());
            // step over spaces between __asm and its content
            while (*asmStart != 0 && static_cast<bool>(std::iswspace(*asmStart)))
                {
                std::advance(asmStart, 1);
                }
            if (*asmStart == L'{')
                {
                const auto* end =
                    string_util::find_matching_close_tag(std::next(asmStart), L'{', L'}', false);
                if (end == nullptr)
                    {
                    log_message(L"__asm", L"Missing closing '}' in __asm block.",
                                (asmStart - m_file_start));
                    return std::next(asmStart);
                    }
                clear_section(originalStart, std::next(end));
                return std::next(const_cast<wchar_t*>(end));
                }
            if (*asmStart != 0)
                {
                const size_t endPos = std::wcscspn(asmStart, L"\n\r");
                clear_section(originalStart,
                              std::next(asmStart, static_cast<ptrdiff_t>(endPos + 1)));
                return std::next(asmStart, static_cast<ptrdiff_t>(endPos + 1));
                }
            return nullptr;
            }
        return std::next(asmStart);
        }

    //--------------------------------------------------
    wchar_t* cpp_i18n_review::skip_preprocessor_define_block(wchar_t* directiveStart)
        {
        const std::wregex debugRE{ L"[_]*DEBUG[_]*" };
        const std::wregex debugLevelRE{ L"([a-zA-Z_]*DEBUG_LEVEL|0)" };
        const std::wregex releaseRE{ L"[_]*RELEASE[_]*" };
        const auto findSectionEnd = [](wchar_t* sectionStart) -> wchar_t*
        {
            const std::wstring_view elifCommand{ L"#elif" };
            const std::wstring_view endifCommand{ L"#endif" };
            const auto* closingElIf =
                string_util::find_matching_close_tag(sectionStart, L"#if", elifCommand);
            const auto* closingEndIf =
                string_util::find_matching_close_tag(sectionStart, L"#if", endifCommand);
            if (closingElIf != nullptr && closingEndIf != nullptr)
                {
                if (closingElIf < closingEndIf)
                    {
                    auto pDiff{ closingElIf - sectionStart };
                    return std::next(sectionStart,
                                     pDiff + static_cast<ptrdiff_t>(elifCommand.length()));
                    }
                auto pDiff{ closingEndIf - sectionStart };
                return std::next(sectionStart,
                                 pDiff + static_cast<ptrdiff_t>(endifCommand.length()));
                }
            if (closingElIf != nullptr)
                {
                auto pDiff{ closingElIf - sectionStart };
                return std::next(sectionStart,
                                 pDiff + static_cast<ptrdiff_t>(elifCommand.length()));
                }
            if (closingEndIf != nullptr)
                {
                auto pDiff{ closingEndIf - sectionStart };
                return std::next(sectionStart,
                                 pDiff + static_cast<ptrdiff_t>(endifCommand.length()));
                }
            return nullptr;
        };

        const std::wstring_view ifndefCommand{ L"ifndef" };
        if (std::wstring_view{ directiveStart }.starts_with(ifndefCommand))
            {
            std::advance(directiveStart, ifndefCommand.length());
            while (static_cast<bool>(std::iswspace(*directiveStart)))
                {
                std::advance(directiveStart, 1);
                }
            auto* defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                {
                std::advance(defSymbolEnd, 1);
                }
            const std::wstring defSymbol{ directiveStart,
                                          static_cast<size_t>(defSymbolEnd - directiveStart) };
            // NDEBUG (i.e., release) is a standard symbol;
            // if not defined, then this is a debug preprocessor section
            return (defSymbol == L"NDEBUG" || std::regex_match(defSymbol, releaseRE)) ?
                       findSectionEnd(defSymbolEnd) :
                       nullptr;
            }
        const std::wstring_view ifdefCommand{ L"ifdef" };
        if (std::wstring_view{ directiveStart }.starts_with(ifdefCommand))
            {
            std::advance(directiveStart, ifdefCommand.length());
            while (static_cast<bool>(std::iswspace(*directiveStart)))
                {
                std::advance(directiveStart, 1);
                }
            auto* defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                {
                std::advance(defSymbolEnd, 1);
                }
            const std::wstring defSymbol{ directiveStart,
                                          static_cast<size_t>(defSymbolEnd - directiveStart) };
            return (std::regex_match(defSymbol, debugRE)) ? findSectionEnd(defSymbolEnd) : nullptr;
            }
        const std::wstring_view ifdefinedCommand{ _DT(L"if defined") };
        if (std::wstring_view{ directiveStart }.starts_with(ifdefinedCommand))
            {
            std::advance(directiveStart, ifdefinedCommand.length());
            while (static_cast<bool>(std::iswspace(*directiveStart)))
                {
                std::advance(directiveStart, 1);
                }
            auto* defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                {
                std::advance(defSymbolEnd, 1);
                }
            const std::wstring defSymbol{ directiveStart,
                                          static_cast<size_t>(defSymbolEnd - directiveStart) };
            return (std::regex_match(defSymbol, debugRE)) ? findSectionEnd(defSymbolEnd) : nullptr;
            }
        const std::wstring_view ifCommand{ L"if" };
        if (std::wstring_view{ directiveStart }.starts_with(ifCommand))
            {
            std::advance(directiveStart, ifCommand.length());
            while (static_cast<bool>(std::iswspace(*directiveStart)))
                {
                std::advance(directiveStart, 1);
                }
            auto* defSymbolEnd{ directiveStart };
            while (is_valid_name_char(*defSymbolEnd))
                {
                std::advance(defSymbolEnd, 1);
                }
            const std::wstring defSymbol{ directiveStart,
                                          static_cast<size_t>(defSymbolEnd - directiveStart) };
            return (std::regex_match(defSymbol, debugLevelRE)) ? findSectionEnd(defSymbolEnd) :
                                                                 nullptr;
            }
        return nullptr;
        }

    //--------------------------------------------------
    wchar_t* cpp_i18n_review::process_preprocessor_directive(wchar_t* directiveStart,
                                                             const size_t directivePos)
        {
        assert(directiveStart);
        if (directiveStart == nullptr)
            {
            return nullptr;
            }
        wchar_t* const originalStart = directiveStart;

        if (*directiveStart == L'#')
            {
            std::advance(directiveStart, 1);
            }
        // step over spaces between '#' and its directive (e.g., pragma)
        while (string_util::is_either(*directiveStart, L' ', L'\t'))
            {
            std::advance(directiveStart, 1);
            }

        auto* blockEnd = skip_preprocessor_define_block(directiveStart);
        if (blockEnd != nullptr)
            {
            clear_section(directiveStart, blockEnd);
            return blockEnd;
            }

        // skip directives
        const std::wstring_view directive{ directiveStart };
        if (directive.starts_with(L"include") || directive.starts_with(L"if") ||
            directive.starts_with(L"ifdef") || directive.starts_with(L"ifndef") ||
            directive.starts_with(L"else") || directive.starts_with(L"elif") ||
            directive.starts_with(L"endif") || directive.starts_with(L"undef") ||
            directive.starts_with(L"define") || directive.starts_with(L"error") ||
            directive.starts_with(L"warning") || directive.starts_with(L"pragma"))
            {
            wchar_t* end = directiveStart;
            while (*end != 0)
                {
                if (*end == L'\n' || *end == L'\r')
                    {
                    // At end of line?
                    // Make sure this isn't a multi-line directive before stopping.
                    bool multiLine = false;
                    wchar_t* backTrace = end;
                    while (backTrace > directiveStart)
                        {
                        if (static_cast<bool>(std::iswspace(*backTrace)))
                            {
                            std::advance(backTrace, -1);
                            continue;
                            }
                        if (*backTrace == L'\\')
                            {
                            multiLine = true;
                            break;
                            }
                        break;
                        }
                    // Just a single-line (or at the last line)? Then we are done.
                    if (!multiLine)
                        {
                        break;
                        }
                    }
                std::advance(end, 1);
                }

            bool shouldClearSection{ true };
            // special parsing logic for #define sections
            // (try to review strings in here as best we can)
            const std::wstring_view defineCommand{ L"define" };
            if (std::wstring_view{ directiveStart }.starts_with(defineCommand))
                {
                std::advance(directiveStart, defineCommand.length());
                while (*directiveStart != 0 && string_util::is_either(*directiveStart, L' ', L'\t'))
                    {
                    std::advance(directiveStart, 1);
                    }
                if (*directiveStart == 0)
                    {
                    return directiveStart;
                    }
                wchar_t* endOfDefinedTerm = directiveStart;
                while (endOfDefinedTerm < end && *endOfDefinedTerm != 0 &&
                       is_valid_name_char(*endOfDefinedTerm))
                    {
                    std::advance(endOfDefinedTerm, 1);
                    }
                const std::wstring definedTerm =
                    std::wstring(directiveStart, endOfDefinedTerm - directiveStart);

                directiveStart = std::next(endOfDefinedTerm);
                while (*directiveStart != 0 &&
                       (*directiveStart == L' ' || *directiveStart == L'\t' ||
                        *directiveStart == L'('))
                    {
                    std::advance(directiveStart, 1);
                    }
                if (*directiveStart == 0)
                    {
                    return directiveStart;
                    }
                auto* endOfPossibleFuncName = directiveStart;
                while (is_valid_name_char(*endOfPossibleFuncName))
                    {
                    std::advance(endOfPossibleFuncName, 1);
                    }
                if (*endOfPossibleFuncName == L'(' && *directiveStart != 0 &&
                    m_ctors_to_ignore.find(
                        std::wstring(directiveStart, endOfPossibleFuncName - directiveStart)) !=
                        m_ctors_to_ignore.cend())
                    {
                    directiveStart = std::next(endOfPossibleFuncName);
                    }
                // #define'd variable followed by a quote? Process as a string variable then.
                if (*directiveStart != 0 &&
                    (*directiveStart == L'\"' || (*std::next(directiveStart) == L'\"')))
                    {
                    const auto* quoteEnd =
                        string_util::find_unescaped_char(std::next(directiveStart), L'\"');
                    if (quoteEnd != nullptr && (quoteEnd - directiveStart) > 0)
                        {
                        const std::wstring definedValue = std::wstring(
                            std::next(directiveStart), (quoteEnd - directiveStart) - 1);
                        process_variable(
                            variable_info{ definedTerm, std::wstring{}, std::wstring{} },
                            definedValue, directivePos + (directiveStart - originalStart));
                        }
                    }
                // example: #define VALUE height, #define VALUE 0x5
                // No open parentheses after the defined value--then not a function.
                // Just leave the end marker where it is (EOL), but leave the value
                // assignment as-is for variable assignment checks later.
                else if ((end > directiveStart) &&
                         std::wstring_view{ directiveStart,
                                            static_cast<size_t>(end - directiveStart) }
                                 .find(L'(') == std::wstring_view::npos)
                    {
                    shouldClearSection = false;
                    }
                // ...or more like a #defined function, so let main parser deal with it
                // (just strip out the preprocessor junk here)
                else
                    {
                    end = directiveStart;
                    }
                }
            if (shouldClearSection)
                {
                clear_section(originalStart, end);
                }
            return end;
            }
        // unknown preprocessor, just skip the '#'
        return std::next(directiveStart);
        }
    } // namespace i18n_check
