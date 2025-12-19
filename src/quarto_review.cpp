/********************************************************************************
 * Copyright (c) 2025 Blake Madden
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

#include "quarto_review.h"
#include <utility>

namespace i18n_check
    {
    //--------------------------------------------------
    void quarto_review::operator()(std::wstring_view srcText, const std::filesystem::path& fileName)
        {
        m_file_name = fileName;
        m_file_start = nullptr;

        if (srcText.empty())
            {
            return;
            }

        std::wstring filteredContent{ srcText };

        // remove code blocks
        auto startPos = filteredContent.find(L"```");
        while (startPos != std::wstring::npos)
            {
            auto endPos = filteredContent.find(L"```", startPos + 3);
            if (endPos == std::wstring::npos)
                {
                break;
                }
            endPos += 3;
            clear_section(filteredContent, startPos, endPos);
            startPos = filteredContent.find(L"```", endPos);
            }

        // remove math blocks ($$ ... $$ first)
        auto mathStart = filteredContent.find(L"$$");
        while (mathStart != std::wstring::npos)
            {
            auto mathEnd = filteredContent.find(L"$$", mathStart + 2);
            if (mathEnd == std::wstring::npos)
                {
                break;
                }
            mathEnd += 2;
            clear_section(filteredContent, mathStart, mathEnd);
            mathStart = filteredContent.find(L"$$", mathEnd);
            }

        // remove inline math ($ ... $)
        mathStart = filteredContent.find(L'$');
        while (mathStart != std::wstring::npos)
            {
            // skip escaped \$
            if (mathStart > 0 && filteredContent[mathStart - 1] == L'\\')
                {
                mathStart = filteredContent.find(L'$', mathStart + 1);
                continue;
                }

            auto mathEnd = filteredContent.find(L'$', mathStart + 1);
            if (mathEnd == std::wstring::npos)
                {
                break;
                }

            clear_section(filteredContent, mathStart, mathEnd + 1);
            mathStart = filteredContent.find(L'$', mathEnd + 1);
            }

        // remove suppression blocks
        const std::wregex suppressBegin(L"<!--\\s*quneiform-suppress-begin\\s*-->",
                                        std::regex_constants::ECMAScript);
        const std::wregex suppressEnd(L"<!--\\s*quneiform-suppress-end\\s*-->",
                                      std::regex_constants::ECMAScript);

        std::size_t searchFrom{ 0 };
        std::wsmatch matchBegin, matchEnd;
        while (std::regex_search(filteredContent.cbegin() + searchFrom, filteredContent.cend(),
                                 matchBegin, suppressBegin))
            {
            const std::size_t beginStart =
                searchFrom + static_cast<std::size_t>(matchBegin.position(0));
            const std::size_t beginEnd =
                beginStart + static_cast<std::size_t>(matchBegin.length(0));

            if (!std::regex_search(filteredContent.cbegin() + beginEnd, filteredContent.cend(),
                                   matchEnd, suppressEnd))
                {
                break;
                }

            const std::size_t endStart = beginEnd + static_cast<std::size_t>(matchEnd.position(0));
            const std::size_t endEnd = endStart + static_cast<std::size_t>(matchEnd.length(0));

            clear_section(filteredContent, beginStart, endEnd);

            searchFrom = beginStart;
            }

        const std::wregex multiSentenceLineRE(
            LR"(([A-Za-zÀ-ÖØ-öø-ÿ]{2,}(?:\.[A-Za-zÀ-ÖØ-öø-ÿ]+)*\.)[”’"'»)\]]*[ \t]+[“"'(]*[A-ZÀ-ÖØ-Þ0-9])",
            std::regex_constants::ECMAScript);
        const std::wregex abbrevRE(
            L"\\b(?:(?:"
            // Country / organization abbreviations
            "(?:U\\.S\\.A|U\\.S|U\\.K|E\\.U|U\\.N|N\\.A\\.T\\.O|F\\.B\\.I|C\\.I\\.A|A\\.T\\.M)"
            "|"
            // English
            "(?:Mr|Mrs|Ms|Dr|Prof|Sr|Jr|St|Mt|vs|etc|e\\.g|i\\.e|Jan|Feb|Mar|Apr|"
            "Jun|Jul|Aug|Sep|Oct|Nov|Dec|a\\.m|p\\.m)"
            "|"
            // French
            "(?:Mme|Mlle|M\\.|p\\.ex|c\\.à\\.d|env|av|apr|approx|No|n°|vol|ch)"
            "|"
            // Spanish
            "(?:Sr|Sra|Srta|Ud|Uds|Dña|p\\.ej|etc)"
            "|"
            // German
            "(?:Hr|Fr|Dr|Prof|bzw|z\\.B|u\\.a|usw|ca)"
            ")\\.)",
            std::regex_constants::ECMAScript | std::regex_constants::icase);

        if ((get_style() & check_l10n_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> multiSentenceEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, multiSentenceLineRE))
                {
                currentBlockOffset += stPositions.position();

                const std::wstring lastWord = (stPositions.size() > 1) ? stPositions[1].str() : L"";

                // flag if not an abbreviation, meaning that this is really the end of a sentence
                if (!std::regex_match(lastWord, abbrevRE))
                    {
                    multiSentenceEntries.emplace_back(
                        currentBlockOffset,
                        currentTextBlock.substr(stPositions.position(), stPositions.length()));
                    }

                currentBlockOffset += stPositions.length();

                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& mlEntry : multiSentenceEntries)
                {
                const auto filePos = get_line_and_column(mlEntry.first, filteredContent);
                m_sentencesOnSameLine.emplace_back(
                    mlEntry.second.length() > 32 ? mlEntry.second.substr(0, 29) + _DT(L"...") :
                                                   mlEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }

        const std::wregex splitLineSentenceRE(LR"(([A-Za-z]+)\r?\n([A-Za-z])([A-Za-z]| ))",
                                              std::regex_constants::ECMAScript);

        if ((get_style() & check_l10n_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> splitSentenceEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, splitLineSentenceRE))
                {
                currentBlockOffset += stPositions.position();

                splitSentenceEntries.emplace_back(
                    currentBlockOffset,
                    currentTextBlock.substr(stPositions.position(), stPositions.length()));

                currentBlockOffset += stPositions.length();

                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& ssEntry : splitSentenceEntries)
                {
                const auto filePos = get_line_and_column(ssEntry.first, filteredContent);
                m_sentencesSplitOnDifferentLines.emplace_back(
                    ssEntry.second.length() > 32 ? ssEntry.second.substr(0, 29) + _DT(L"...") :
                                                   ssEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }

        const std::wregex malformedContentRE(
            LR"((\{\{\s*[<%]\s*(?:pagebreak|include|video|embed|placeholder|lipsum)\b[^}\r\n]*[>%]\s*\}\})(?=\r?\n(?!$)[^\S\r\n]*\S))",
            std::regex_constants::ECMAScript);

        if ((get_style() & check_malformed_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> malformedEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, malformedContentRE))
                {
                currentBlockOffset += stPositions.position();

                malformedEntries.emplace_back(
                    currentBlockOffset,
                    currentTextBlock.substr(stPositions.position(), stPositions.length()));

                currentBlockOffset += stPositions.length();

                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& mcEntry : malformedEntries)
                {
                const auto filePos = get_line_and_column(mcEntry.first, filteredContent);
                m_malformedContent.emplace_back(
                    mcEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }

        const std::wregex smartQuotesRE(
            LR"(([A-Za-z0-9_"'-]*[\u2018\u2019\u201C\u201D][A-Za-z0-9_"'-]*))",
            std::regex_constants::ECMAScript);

        if ((get_style() & check_malformed_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> smartQuotesEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, smartQuotesRE))
                {
                currentBlockOffset += stPositions.position();

                auto token{ currentTextBlock.substr(stPositions.position(), stPositions.length()) };

                const bool isSingleQuoteChar =
                    token.length() == 3 && (token.starts_with(L"'") || token.starts_with(L"\"")) &&
                    (token.ends_with(L"'") || token.ends_with(L"\""));

                if (!isSingleQuoteChar)
                    {
                    smartQuotesEntries.emplace_back(currentBlockOffset, token);
                    }

                currentBlockOffset += stPositions.length();

                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& sqEntry : smartQuotesEntries)
                {
                const auto filePos = get_line_and_column(sqEntry.first, filteredContent);
                m_smartQuotes.emplace_back(
                    sqEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }

        const std::wregex numericRangeHyphenRE(LR"(\b([0-9]{1,4})-([0-9]{1,4})\b)",
                                               std::regex_constants::ECMAScript);
        const std::wregex phoneRE(LR"(\b\d{3}-\d{4}\b)", std::regex_constants::ECMAScript);
        const std::wregex codePageRE(LR"(^8859-(?:[1-9]|1[0-6])$)",
                                     std::regex_constants::ECMAScript);

        if ((get_style() & check_malformed_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> numericRangeEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };

            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, numericRangeHyphenRE))
                {
                currentBlockOffset += stPositions.position();

                numericRangeEntries.emplace_back(
                    currentBlockOffset,
                    currentTextBlock.substr(stPositions.position(), stPositions.length()));

                currentBlockOffset += stPositions.length();

                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& nrEntry : numericRangeEntries)
                {
                // ignore phone numbers and code pages
                if (std::regex_match(nrEntry.second, phoneRE) ||
                    std::regex_match(nrEntry.second, codePageRE))
                    {
                    continue;
                    }
                const auto filePos = get_line_and_column(nrEntry.first, filteredContent);
                m_rangeDashIssues.emplace_back(
                    nrEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }

        const std::wregex alphaRangeHyphenRE(LR"(\b([A-Z])-([A-Z])\b)",
                                             std::regex_constants::ECMAScript);

        if ((get_style() & check_malformed_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> alphaRangeEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };

            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, alphaRangeHyphenRE))
                {
                currentBlockOffset += stPositions.position();

                alphaRangeEntries.emplace_back(
                    currentBlockOffset,
                    currentTextBlock.substr(stPositions.position(), stPositions.length()));

                currentBlockOffset += stPositions.length();

                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& arEntry : alphaRangeEntries)
                {
                const auto filePos = get_line_and_column(arEntry.first, filteredContent);
                m_rangeDashIssues.emplace_back(
                    arEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }

        const std::wregex k12RangeHyphenRE(LR"(\b(Pre-K|K)-(8|12)\b)",
                                           std::regex_constants::ECMAScript);

        if ((get_style() & check_malformed_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> k12RangeEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };

            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, k12RangeHyphenRE))
                {
                currentBlockOffset += stPositions.position();

                k12RangeEntries.emplace_back(
                    currentBlockOffset,
                    currentTextBlock.substr(stPositions.position(), stPositions.length()));

                currentBlockOffset += stPositions.length();

                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& k12Entry : k12RangeEntries)
                {
                const auto filePos = get_line_and_column(k12Entry.first, filteredContent);
                m_rangeDashIssues.emplace_back(
                    k12Entry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }

        const std::wregex malformedImageRE(
            LR"((!\[(?![^\]]*\]\[)[^\]]*$)|(!\[(?![^\]]*\]\[)[^\]]*\]\(\s*(['"])[^'\\"\r\n]+\3\s*\))|(!\[(?![^\]]*\]\[)[^\]]*\]\([^\)\r\n]*(?=\r?\n|$)))",
            std::regex_constants::ECMAScript);

        if ((get_style() & check_malformed_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> malformedImageEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };

            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, malformedImageRE))
                {
                currentBlockOffset += stPositions.position();

                const auto start = stPositions.position();
                auto length = stPositions.length();

                // stop at end-of-line
                const auto eol = currentTextBlock.find_first_of(L"\r\n", start);
                if (eol != std::wstring::npos && std::cmp_less(eol, start + length))
                    {
                    length = eol - start;
                    }

                malformedImageEntries.emplace_back(currentBlockOffset,
                                                   currentTextBlock.substr(start, length));

                currentBlockOffset += length;
                currentTextBlock = currentTextBlock.substr(start + length);
                }

            for (const auto& imgEntry : malformedImageEntries)
                {
                const auto filePos = get_line_and_column(imgEntry.first, filteredContent);
                m_malformedImageLinks.emplace_back(
                    imgEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }

        const std::wregex absolutePathRE(LR"(!?\[[^\]]*\]\((/[^)\r\n]*|[A-Za-z]:/[^)\r\n]*)\))",
                                         std::regex_constants::ECMAScript);

        if ((get_style() & check_malformed_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> absolutePathEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };

            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, absolutePathRE))
                {
                currentBlockOffset += stPositions.position();

                const auto fullMatch =
                    currentTextBlock.substr(stPositions.position(), stPositions.length());

                const auto path = stPositions[1].str(); // captured (/foo/bar)

                bool shouldFlag = false;

                // site-root relative paths
                if (!path.empty() && path[0] == L'/')
                    {
                    // protocol-relative URLs: //cdn.example.com
                    if (path.size() > 1 && path[1] == L'/')
                        {
                        shouldFlag = false;
                        }

                    // anchors: /#section
                    else if (path.size() > 1 && path[1] == L'#')
                        {
                        shouldFlag = false;
                        }

                    else
                        {
                        shouldFlag = true;
                        }
                    }

                // Windows drive paths: C:/...
                else if (path.size() > 2 && (std::iswalpha(path[0]) != 0) && path[1] == L':' &&
                         path[2] == L'/')
                    {
                    shouldFlag = true;
                    }

                if (shouldFlag)
                    {
                    absolutePathEntries.emplace_back(currentBlockOffset, fullMatch);
                    }

                currentBlockOffset += stPositions.length();
                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& entry : absolutePathEntries)
                {
                const auto filePos = get_line_and_column(entry.first, filteredContent);

                m_absolute_path_links.emplace_back(
                    entry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }
        }
    } // namespace i18n_check
