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
#include <future>
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

        auto funcSameLine = std::async(std::launch::async, &quarto_review::LoadSentencesOnSameLine,
                                       this, std::cref(filteredContent));

        auto funcSplitSentences = std::async(std::launch::async, &quarto_review::LoadSplitSentences,
                                             this, std::cref(filteredContent));

        auto funcMalformed = std::async(std::launch::async, &quarto_review::LoadMalformedContent,
                                        this, std::cref(filteredContent));

        auto funcSmartQuotes = std::async(std::launch::async, &quarto_review::LoadSmartQuotes, this,
                                          std::cref(filteredContent));

        auto funcRanges = std::async(std::launch::async, &quarto_review::LoadRanges, this,
                                     std::cref(filteredContent));

        auto funcMalformedImages =
            std::async(std::launch::async, &quarto_review::LoadMalformedImages, this,
                       std::cref(filteredContent));

        auto funcAbsolutePaths = std::async(std::launch::async, &quarto_review::LoadAbsolutePaths,
                                            this, std::cref(filteredContent));

            {
            auto sameLineResults = funcSameLine.get();
            m_sentencesOnSameLine.insert(m_sentencesOnSameLine.end(),
                                         std::make_move_iterator(sameLineResults.begin()),
                                         std::make_move_iterator(sameLineResults.end()));
            }

            {
            auto splitSentenceResults = funcSplitSentences.get();
            m_sentencesSplitOnDifferentLines.insert(
                m_sentencesSplitOnDifferentLines.end(),
                std::make_move_iterator(splitSentenceResults.begin()),
                std::make_move_iterator(splitSentenceResults.end()));
            }

            {
            auto malformedContentResults = funcMalformed.get();
            m_malformedContent.insert(m_malformedContent.end(),
                                      std::make_move_iterator(malformedContentResults.begin()),
                                      std::make_move_iterator(malformedContentResults.end()));
            }

            {
            auto smartQuoteResults = funcSmartQuotes.get();
            m_smartQuotes.insert(m_smartQuotes.end(),
                                 std::make_move_iterator(smartQuoteResults.begin()),
                                 std::make_move_iterator(smartQuoteResults.end()));
            }

            {
            auto rangeDashResults = funcRanges.get();
            m_rangeDashIssues.insert(m_rangeDashIssues.end(),
                                     std::make_move_iterator(rangeDashResults.begin()),
                                     std::make_move_iterator(rangeDashResults.end()));
            }

            {
            auto malformedImageResults = funcMalformedImages.get();
            m_malformedImageLinks.insert(m_malformedImageLinks.end(),
                                         std::make_move_iterator(malformedImageResults.begin()),
                                         std::make_move_iterator(malformedImageResults.end()));
            }

            {
            auto absolutePathResults = funcAbsolutePaths.get();
            m_absolute_path_links.insert(m_absolute_path_links.end(),
                                         std::make_move_iterator(absolutePathResults.begin()),
                                         std::make_move_iterator(absolutePathResults.end()));
            }
        }

    //--------------------------------------------------
    std::vector<quarto_review::string_info>
    quarto_review::LoadAbsolutePaths(const std::wstring& filteredContent) const
        {
        std::vector<string_info> issues;

        if ((get_style() & check_malformed_strings) == 0U)
            {
            return issues;
            }

        const std::wstring& text = filteredContent;
        const size_t textLength = text.size();

        for (size_t scanIndex = 0; scanIndex < textLength; ++scanIndex)
            {
            // detect start of a Markdown link or image link
            bool isLinkStart = false;

            if (text[scanIndex] == L'[')
                {
                isLinkStart = true;
                }
            else if (text[scanIndex] == L'!' && scanIndex + 1 < textLength &&
                     text[scanIndex + 1] == L'[')
                {
                isLinkStart = true;
                }

            if (!isLinkStart)
                {
                continue;
                }

            // find closing ']'
            const size_t closingBracketIndex = text.find(L']', scanIndex + 1);

            if (closingBracketIndex == std::wstring::npos)
                {
                continue;
                }

            // must be followed by '('
            if (closingBracketIndex + 1 >= textLength || text[closingBracketIndex + 1] != L'(')
                {
                continue;
                }

            const size_t pathStartIndex = closingBracketIndex + 2;

            // find end of the path: ')' or line break
            const size_t pathEndIndex = text.find_first_of(L")\r\n", pathStartIndex);

            if (pathEndIndex == std::wstring::npos || pathEndIndex == pathStartIndex)
                {
                continue;
                }

            bool shouldFlag = false;

            // site-root relative paths: /foo/bar
            if (text[pathStartIndex] == L'/')
                {
                const bool isProtocolRelative =
                    (pathStartIndex + 1 < pathEndIndex && text[pathStartIndex + 1] == L'/');

                const bool isAnchor =
                    (pathStartIndex + 1 < pathEndIndex && text[pathStartIndex + 1] == L'#');

                if (!isProtocolRelative && !isAnchor)
                    {
                    shouldFlag = true;
                    }
                }
            // Windows absolute paths: C:/foo/bar
            else if (pathStartIndex + 2 < pathEndIndex &&
                     std::iswalpha(text[pathStartIndex]) != 0 && text[pathStartIndex + 1] == L':' &&
                     text[pathStartIndex + 2] == L'/')
                {
                shouldFlag = true;
                }

            if (shouldFlag)
                {
                const auto filePosition = get_line_and_column(scanIndex, text);

                const size_t matchEndIndex =
                    (pathEndIndex < textLength && text[pathEndIndex] == L')') ? pathEndIndex + 1 :
                                                                                pathEndIndex;

                issues.emplace_back(
                    text.substr(scanIndex, matchEndIndex - scanIndex),
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePosition);
                }

            // skip ahead so we don't rescan inside the same link
            scanIndex = pathEndIndex;
            }

        return issues;
        }

    //--------------------------------------------------
    std::vector<quarto_review::string_info>
    quarto_review::LoadMalformedImages(const std::wstring& filteredContent) const
        {
        std::vector<string_info> issues;

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
                issues.emplace_back(
                    imgEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }
        return issues;
        }

    //--------------------------------------------------
    std::vector<quarto_review::string_info>
    quarto_review::LoadRanges(const std::wstring& filteredContent) const
        {
        std::vector<string_info> issues;

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
                issues.emplace_back(
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
                issues.emplace_back(
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
                issues.emplace_back(
                    k12Entry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }
        return issues;
        }

    //--------------------------------------------------
    std::vector<quarto_review::string_info>
    quarto_review::LoadSmartQuotes(const std::wstring& filteredContent) const
        {
        std::vector<string_info> issues;

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
                issues.emplace_back(
                    sqEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }
        return issues;
        }

    //--------------------------------------------------
    std::vector<quarto_review::string_info>
    quarto_review::LoadMalformedContent(const std::wstring& filteredContent) const
        {
        std::vector<string_info> issues;

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
                issues.emplace_back(
                    mcEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }
        return issues;
        }

    //--------------------------------------------------
    std::vector<quarto_review::string_info>
    quarto_review::LoadSplitSentences(const std::wstring& filteredContent) const
        {
        std::vector<string_info> issues;

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
                issues.emplace_back(
                    ssEntry.second.length() > 32 ? ssEntry.second.substr(0, 29) + _DT(L"...") :
                                                   ssEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }
        return issues;
        }

    //--------------------------------------------------
    std::vector<quarto_review::string_info>
    quarto_review::LoadSentencesOnSameLine(const std::wstring& filteredContent) const
        {
        std::vector<string_info> issues;

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
                issues.emplace_back(
                    mlEntry.second.length() > 32 ? mlEntry.second.substr(0, 29) + _DT(L"...") :
                                                   mlEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }
        return issues;
        }
    } // namespace i18n_check
